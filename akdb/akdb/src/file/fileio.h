/**
@file fileio.h Header file provides data structures for file input/output
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
#ifndef FILEIO
#define FILEIO

#include "../auxi/constants.h"
#include "../sql/cs/reference.h"
#include "../mm/memoman.h"
#include "../rec/recovery.h"
#include "../rec/archive_log.h"
#include "../rec/redo_log.h"
#include "files.h"
#include "../auxi/mempro.h"

void Ak_Insert_New_Element_For_Update(int newtype, void * data, char * table, char * attribute_name, struct list_node * ElementBefore, int newconstraint);
void Ak_Insert_New_Element(int newtype, void * data, char * table, char * attribute_name, struct list_node * ElementBefore);
int Ak_insert_row_to_block(struct list_node *row_root, AK_block *temp_block);
int Ak_insert_row(struct list_node *row_root);
void Ak_update_row_from_block(AK_block *temp_block, struct list_node *row_root);
void Ak_delete_row_from_block(AK_block *temp_block, struct list_node *row_root);
int Ak_delete_update_segment(struct list_node *row_root, int del);
int Ak_delete_row(struct list_node *row_root) ;
int Ak_update_row(struct list_node *row_root);
void Ak_fileio_test();
void Ak_delete_row_by_id(int id, char* tableName);

#endif
