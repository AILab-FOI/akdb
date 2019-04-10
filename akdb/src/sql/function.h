/**
@file function.h Header file that provides functions and defines for functions
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

#ifndef FUNCTION
#define FUNCTION

#include "../auxi/test.h"
#include "../file/table.h"
#include "../file/fileio.h"
#include "../auxi/mempro.h"
#include "../auxi/auxiliary.h"

int AK_get_function_obj_id(char* function, struct list_node *arguments_list);
int AK_check_function_arguments(int function_id, struct list_node *arguments_list);
int AK_check_function_arguments_type(int function_id, struct list_node *args);
int AK_function_add(char *name, int return_type, struct list_node *arguments_list);
int AK_function_arguments_add(int function_id, int arg_number, int arg_type, char *argname);
int AK_function_remove_by_obj_id(int obj_id);
int AK_function_arguments_remove_by_obj_id(int *obj_id);
int AK_function_remove_by_name(char *name, struct list_node *arguments_list);
int AK_function_rename(char *name, struct list_node *arguments_list, char *new_name);
int AK_function_change_return_type(char *name, struct list_node *arguments_list, int new_return_type);
TestResult AK_function_test();

#endif
