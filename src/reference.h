/**
@file reference.h Provides data structures for referential integrity
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
 
#ifndef REFERENCE
#define REFERENCE

#include "auxiliary.h"
#include "dbman.h"
#include "memoman.h"
#include "fileio.h"
#include "files.h"
#include "table.h"

#define REF_TYPE_NONE -1
#define REF_TYPE_SET_NULL 1
#define REF_TYPE_NO_ACTION 2
#define REF_TYPE_CASCADE 3
#define REF_TYPE_RESTRICT 4
#define REF_TYPE_SET_DEFAULT 5 // reserved for future use

#define MAX_REFERENCE_ATTRIBUTES 10 //how many attributes can be define in a single foreign key constraint
#define MAX_CHILD_CONSTRAINTS 20 // how many references can be made on one parent table, this will be deprecated by the use of a list

typedef struct {
    char table[MAX_ATT_NAME];
    char attributes[MAX_REFERENCE_ATTRIBUTES][MAX_ATT_NAME];
    char parent[MAX_ATT_NAME];
    char parent_attributes[MAX_REFERENCE_ATTRIBUTES][MAX_ATT_NAME];
    int attributes_number;
    char constraint[MAX_VARCHAR_LENGTH];
    int type;
} AK_ref_item;

#endif