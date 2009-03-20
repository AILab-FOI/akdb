/**
@file dbman.h Defines includes and datastructures for the disk manager
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
\struct KK_header
\brief Header structure of blocks (describes an attribute inside an object)
*/
typedef struct {
	int type;
	char att_name[ MAX_ATT_NAME ];
	int integrity[ MAX_CONSTRAINTS ];
	char constr_name[ MAX_CONSTRAINTS ][ MAX_CONSTR_NAME ];
	char constr_code[ MAX_CONSTRAINTS ][ MAX_CONSTR_CODE ];
} KK_header;


/**
\struct KK_tuple_dict
\brief Defines a mapping in a header of an object to the actual entries (data)
*/
typedef struct {
	int type;
	int address;
	int size;
} KK_tuple_dict;


/**
\struct KK_block
\brief Defines a block of data inside a DB file
*/
typedef struct {
	int address;
	int type;
	int chained_with;
	int free_space;
	KK_header header[ MAX_ATTRIBUTES ];
	KK_tuple_dict tuple_dict[ DATA_BLOCK_SIZE ];
	int data[ DATA_BLOCK_SIZE ][ DATA_ENTRY_SIZE ];
} KK_block;

/**
\var db
\brief Defines the DB file file handle
*/
FILE * db;

/**
\var db_file_size
\brief Defines the size of the DB file (in blocks)
*/
unsigned int db_file_size;
