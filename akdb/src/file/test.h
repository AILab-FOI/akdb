/**
@file test.h Header file that provides functions for testing purposes
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

#ifndef TEST
#define TEST

#include "files.h"
#include "../auxi/mempro.h"

char* AK_get_table_atribute_types(char* tblName);
int create_header_test(char* tbl_name, char** attr_name, int _num, int* _type);
int insert_data_test(char* tbl_name, char** attr_name, char** attr_value, int _num, int* _type);
int selection_test(char* src_table, char* dest_table, char** sel_query, int _num, int* _type);
int get_column_test(int num, char* tbl);
int get_row_test(int num, char* tbl);
void AK_create_test_tables();

#endif

