/**
@file nnull.h Header file that provides functions and defines for not null constraint
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

#ifndef NNULL
#define NNULL

#include "../../auxi/test.h"
#include "../../file/table.h"
#include "../../file/fileio.h"
#include "../../auxi/mempro.h"
#include "constraint_names.h"

int AK_set_constraint_not_null(char* tableName, char* attName, char* constraintName);
int AK_read_constraint_not_null(char* tableName, char* attName, char* newValue);
int AK_check_constraint_not_null(char* tableName, char* attName, char* newValue);
int AK_delete_constraint_not_null(char* tableName, char attName[], char constraintName[]);
TestResult AK_nnull_constraint_test();

#endif