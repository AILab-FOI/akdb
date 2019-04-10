/**
@file expression_check.h Header file that provides data structures for expression ckecking
 */
/*
 * This program is free software; you can redistribute it and/or modifys
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#ifndef EXPRESSION_CHECK
#define EXPRESSION_CHECK

#include "../auxi/test.h"
#include "../file/table.h"
#include "../file/fileio.h"
#include "../auxi/mempro.h"
#include <regex.h>
/*
int AK_check_arithmetic_statement(AK_list_elem el, const char *op, const char *a, const char *b);
int AK_check_if_row_satisfies_expression(AK_list_elem row_root, AK_list *expr);
*/

int AK_check_arithmetic_statement(struct list_node *el, const char *op, const char *a, const char *b);
int AK_check_if_row_satisfies_expression(struct list_node *row_root, struct list_node *expr);
int Ak_check_regex_expression(const char *value,const char *expression,int sensitive,int checkWildCard);
int Ak_check_regex_operator_expression(const char *value,const char *expression);
TestResult Ak_expression_check_test();

#endif /* CONSTRAINT_CHECKER_H_ */
