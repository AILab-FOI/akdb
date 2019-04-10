/**
 * @file check_constraint.h Header file that provides data structures for check constraint
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

#include "../../auxi/test.h"
#include "../../file/table.h"
#include "../../file/fileio.h"
#include "../../rel/expression_check.h"
#include "../../auxi/mempro.h"

int condition_passed(char *condition, int type, void *value, void *row_data);
int AK_set_check_constraint(char *table_name, char *constraint_name, char *attribute_name, char *condition, int type, void *value);
int AK_check_constraint(char *table, char *attribute, void *value);
TestResult AK_check_constraint_test();

#endif
