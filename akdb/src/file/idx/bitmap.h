/**
@file bitmap.h Header file that provides data structures for bitmap index
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

#ifndef BITMAP
#define BITMAP

#include "../../auxi/test.h"
#include "../../mm/memoman.h"
#include "index.h"
#include "../../file/table.h"
#include "../../file/fileio.h"
#include "../../file/files.h"
#include "../../auxi/mempro.h"


int Ak_If_ExistOp(struct list_node *L, char *ele);
void AK_create_Index_Table(char *tblName, struct list_node *attributes);
void Ak_print_Header_Test(char* tblName);
void Ak_create_Index(char *tblName, char *tblNameIndex, char *attributeName, int positionTbl, int numAtributes, AK_header *headerIndex);
list_ad* Ak_get_Attribute(char *indexName, char *attribute);
void Ak_create_List_Address_Test();
void Ak_print_Att_Test(list_ad *list);
list_ad* AK_get_Attribute(char *tableName, char *attributeName, char *attributeValue);
void AK_update(int addBlock, int addTd, char *tableName, char *attributeName, char *attributeValue, char *newAttributeValue);
int Ak_write_block(AK_block * block);
TestResult Ak_bitmap_test();

void AK_delete_bitmap_index(char *indexName);
void AK_add_to_bitmap_index(char *tableName, char *attributeName);

#endif
