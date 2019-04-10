/**
@file nat_join.h Header file that provides data structures for relational natural join operation
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
#ifndef NATJOIN
#define NATJOIN

#include "../auxi/test.h"
#include "../file/table.h"
#include "../file/fileio.h"
#include "../rel/projection.h"
#include "../auxi/mempro.h"
#include "../sql/drop.h"
/*
void AK_create_join_block_header(int table_address1, int table_address2, char *new_table, AK_list *att);
void AK_merge_block_join(AK_list *row_root, AK_list *row_root_insert, AK_block *temp_block, char *new_table);
void AK_copy_blocks_join(AK_block *tbl1_temp_block, AK_block *tbl2_temp_block, AK_list *att, char *new_table);*/
void AK_create_join_block_header(int table_address1, int table_address2, char *new_table, struct list_node *att);
void AK_merge_block_join(struct list_node *row_root, struct list_node *row_root_insert, AK_block *temp_block, char *new_table);
void AK_copy_blocks_join(AK_block *tbl1_temp_block, AK_block *tbl2_temp_block, struct list_node *att, char *new_table);
//int AK_join(char *srcTable1, char * srcTable2, char * dstTable, AK_list *att);
int AK_join(char *srcTable1, char * srcTable2, char * dstTable, struct list_node *att);
TestResult AK_op_join_test();

#endif
