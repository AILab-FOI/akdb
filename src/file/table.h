/** @file table.h Header file that provides data structures for table abstraction
 *
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

#ifndef RENAME
#define RENAME

#endif

#ifndef TABLE
#define TABLE

#include "../mm/memoman.h"
#include <time.h>


struct AK_create_table {
	char name[MAX_ATT_NAME];
	int type;
};

typedef struct AK_create_table AK_create_table_parameter;



AK_create_table_parameter* AK_create_create_table_parameter(int type, char* name);
void AK_create_table(char* tblName, AK_create_table_parameter* parameters, int attribute_count) ;
int AK_num_attr(char * tblName);
int AK_get_num_records(char *tblName);
AK_header *AK_get_header(char *tblName);
char *AK_get_attr_name(char *tblName, int index);
int AK_get_attr_index(char *tblName, char *attrName) ;
AK_list *AK_get_column(int num, char *tblName);
AK_list * AK_get_row(int num, char * tblName);
AK_list_elem AK_get_tuple(int row, int column, char *tblName);
char * AK_tuple_to_string(AK_list *tuple);
void AK_print_row_spacer(int col_len[], int length);
void AK_print_row(int col_len[], AK_list *row);
void AK_print_table(char *tblName);
void AK_print_row_spacer_to_file(int col_len[], int length);
void AK_print_row_to_file(int col_len[], AK_list *row);
void AK_print_table_to_file(char *tblName);
int AK_table_empty(char *tblName);
int AK_get_table_obj_id(char *table);
int AK_check_tables_scheme(AK_mem_block *tbl1_temp_block, AK_mem_block *tbl2_temp_block, char *operator_name);
void AK_table_test() ;

int AK_rename(char *old_table_name, char *old_attr, char *new_table_name, char *new_attr);
void AK_op_rename_test() ;

#endif
