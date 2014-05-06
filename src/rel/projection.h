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

#include "../file/table.h"
#include "../file/fileio.h"

void AK_temp_create_table(char *table, AK_header *header, int type_segment);
void AK_create_block_header(int old_block, char *dstTable, AK_list *att);
void AK_copy_block_projection(AK_block *old_block, AK_list *att, char *dstTable);
int AK_projection(char *srcTable, char *dstTable, AK_list *att);
void AK_op_projection_test();

#endif
