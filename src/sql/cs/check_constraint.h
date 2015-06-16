/**
@file between.h Header file that provides data structures for between constaint
 */
/*
 * This program is free software; you can redistribute it and/or modify
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

#ifndef CHECK_CONSTRAINT
#define CHECK_CONSTRAINT

#include "../../file/table.h"
#include "../../file/fileio.h"
#include "../../rel/expression_check.h"
#include "../../auxi/mempro.h"

//int AK_check_constraint(char *srcTable, AK_list *expr);
int AK_check_constraint(char *srcTable, list_node *expr);
void AK_check_constraint_test();

#endif
