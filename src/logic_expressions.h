/*
   Copyright (C) Cfengine AS

   This file is part of Cfengine 3 - written and maintained by Cfengine AS.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; version 3.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA

  To the extent this program is licensed as part of the Enterprise
  versions of Cfengine, the applicable Commerical Open Source License
  (COSL) may apply to this file if you as a licensee so wish it. See
  included file COSL.txt.
*/

#ifndef CFENIGNE_LOGIC_EXPRESSIONS_H
#define CFENGINE_LOGIC_EXPRESSIONS_H

#include "bool.h"
#include "string_expressions.h"

/*
   Logic expressions grammar:

   <expr> ::= <or-expr>

   <or-expr> ::= <and-expr>
                 <and-expr> | <and-expr>

   <and-expr> ::= <not-expr>
                  <not-expr> . <not-expr>
                  <not-expr> & <not-expr>

   <not-expr> ::= ! <primary>
                  <primary>

   <primary> ::= ( <expr> )
                 <name>

   Basis of logic evaluation is <name> values which are provided by
   StringExpression and suitable string->bool evaluator.
*/

typedef enum LogicalOp
   {
   OR,
   AND,
   NOT,
   EVAL,
   } LogicalOp;

typedef struct Expression
   {
   LogicalOp op;
   union
      {
      struct AndOrExpression
         {
         struct Expression *lhs;
         struct Expression *rhs;
         } andor;

      struct NotExpression
         {
         struct Expression *arg;
         } not;

      struct EvalExpression
         {
         StringExpression *name;
         } eval;
      } val;
   } Expression;

/* Parsing and evaluation */

/*
 * Result of parsing.
 *
 * if succeeded, then result is the result of parsing and position is last
 * character consumed.
 *
 * if not succeded, then result is NULL and position is last character consumed
 * before the error.
 */
typedef struct ParseResult
   {
   Expression *result;
   int position;
   } ParseResult;

ParseResult ParseExpression(const char *expr, int start, int end);

typedef enum ExpressionValue
   {
   EXP_ERROR = -1,
   EXP_FALSE = false,
   EXP_TRUE = true,
   } ExpressionValue;

/*
 * Evaluator should return FALSE, TRUE or ERROR if unable to parse result.  In
 * later case evaluation will be aborted and ERROR will be returned from
 * EvalExpression.
 */
typedef ExpressionValue (*NameEvaluator)(const char *name, void *param);

/*
 * Result is heap-allocated. In case evalfn() returns ERROR whole
 * EvalExpression returns ERROR as well.
 */
ExpressionValue EvalExpression(const Expression *expr,
                               NameEvaluator nameevalfn,
                               VarRefEvaluator varrefevalfn,
                               void *param);

/*
 * Frees Expression produced by ParseExpression. NULL-safe.
 */
void FreeExpression(Expression *expr);

#endif
