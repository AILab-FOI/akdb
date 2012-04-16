/**
@file dbman.h Header file that defines includes and datastructures for the disk manager
 of Kalashnikov DB
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

#ifndef DBMAN
#define DBMAN

#include "auxiliary.h"

/**
 * @author Markus Schatten
 * @struct AK_header
 * @brief Structure that represents header structure of blocks (describes an attribute inside an object). It contains
          type, attribute name, integrity, constraint name and constraint code.
 */
typedef struct {
    /// type of attribute
    int type;
    /// attribute name
    char att_name[ MAX_ATT_NAME ];
    /// standard integrity costraints
    int integrity[ MAX_CONSTRAINTS ];
    /// extra integrity constraint names
    char constr_name[ MAX_CONSTRAINTS ][ MAX_CONSTR_NAME ];
    /// extra integrity costraint codes
    char constr_code[ MAX_CONSTRAINTS ][ MAX_CONSTR_CODE ];
} AK_header;

/**
 * @author Markus Schatten
 * @struct AK_tuple_dict
 * @brief Structure that defines a mapping in a header of an object to the actual entries (data). It contains type, address
          and size.
 */
typedef struct {
    /// data entry type
    int type;
    /// data entry address (in AK_block->data)
    int address;
    /// data entry size (using sizeof( *** ) )
    int size;
} AK_tuple_dict;

/**
  * @author Markus Schatten
  * @struct AK_block
  * @brief Structure that defines a block of data inside a DB file. It contains address, type, chained_with, free space,
           last_tuple_dict_id, header and tuple_dict and data.
 */
typedef struct {
    /// block number (address) in DB file
    int address;
    /// block type (can be BLOCK_TYPE_FREE, BLOCK_TYPE_NORMAL or BLOCK_TYPE_CHAINED)
    int type;
    /// address of chained block; NOT_CHAINED otherwise
    int chained_with;
    /// free space in block
    int free_space;
    int last_tuple_dict_id;
    /// attribute definitions
    AK_header header[ MAX_ATTRIBUTES ];
    /// dictionary of data entries
    AK_tuple_dict tuple_dict[ DATA_BLOCK_SIZE ];
    /// actual data entries
    unsigned char data[ DATA_BLOCK_SIZE * DATA_ENTRY_SIZE ];
} AK_block;

/**
 * @author Markus Schatten
 * @var db
 * @brief Variable that defines the DB file file handle
 */
FILE * db;

/**
 * @author Markus Schatten
 * @var db_file_size
 * @brief Variable that defines the size of the DB file (in blocks)
 */
unsigned int db_file_size;

/**
  * @author Matija Novak
  * @struct table_addresses
  * @brief Structure that defines start and end address of extent
  */
typedef struct {
    ///sturcture for extents start end stop adresses
    int address_from[ MAX_EXTENTS_IN_SEGMENT ]; //start adress of the extent
    int address_to[ MAX_EXTENTS_IN_SEGMENT ]; //end adress of the extent
} table_addresses;

#endif
