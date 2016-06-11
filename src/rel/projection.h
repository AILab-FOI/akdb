/**
@file projection.h Header file that provides data structures for relational projection operation
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
 
#ifndef PROJECTION
#define PROJECTION

#include "expression_check.h"
#include "../file/table.h"
#include "../file/fileio.h"
#include "../auxi/mempro.h"

 struct AK_operand {
	char value[MAX_VARCHAR_LENGTH];
	int type;
};


void AK_temp_create_table(char *table, AK_header *header, int type_segment);
/*
void AK_create_block_header(int old_block, char *dstTable, AK_list *att);
void AK_copy_block_projection(AK_block *old_block, AK_list *att, char *dstTable);
int AK_projection(char *srcTable, char *dstTable, AK_list *att);
*/
void AK_create_block_header(int old_block, char *dstTable, struct list_node *att);
void AK_copy_block_projection(AK_block *old_block, struct list_node *att, char *dstTable, struct list_node *expr);
int AK_projection(char *srcTable, char *dstTable, struct list_node *att, struct list_node *expr);
int AK_determine_header_type(int a,int b);
char *AK_create_header_name(char * first,char *operator, char * second);
char *AK_perform_operatrion(char *op,struct AK_operand *a, struct AK_operand *b,int type);
void removeSubstring(char *s,const char *toremove);
void AK_op_projection_test();
char *AK_get_operator(char *exp);

#endif
