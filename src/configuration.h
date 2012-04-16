/**
@file configuration.h Header file that provides global macros, constants and variables
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

#ifndef CONFIGURATION
#define CONFIGURATION

/**
 * @def MAX_NUM_OF_BLOCKS
 * @brief Constant declaring maximum number of blocks in a segment 
*/
#define MAX_NUM_OF_BLOCKS 200
/**
  * @def MAX_VARCHAR_LENGTH
  * @brief Constant declaring maximum length of varchar data value
 */
#define MAX_VARCHAR_LENGTH 200
/**
  * @def MAX_EXTENTS_IN_SEGMENT
  * @brief Constant declaring maximum number of extents in segment
*/
#define MAX_EXTENTS_IN_SEGMENT 200
/**
  * @def MAX_FREE_SPACE_SIZE
  * @brief Constant declaring maximum free space in block
*/
#define MAX_FREE_SPACE_SIZE 4000
/**
  * @def MAX_LAST_TUPLE_DICT_SIZE_TO_USE
  * @brief Constant declaring maximum size od last tuple in dictionary
*/
#define MAX_LAST_TUPLE_DICT_SIZE_TO_USE 470

/**
  * @def EXIT_SUCCESS
  * @brief Constant declaring successfull exit
 */
#define EXIT_SUCCESS 0

/**
  * @def EXIT_ERROR
  * @brief Constant declaring unsuccesfull exit
 */
#define EXIT_ERROR -1

/**
  * @def BLOCK_TYPE_FREE
  * @brief Constant declaring free block type (used in AK_block->type)
 */
#define BLOCK_TYPE_FREE -1

/**
  * @def BLOCK_TYPE_NORMAL
  * @brief Constant declaring normal block type e.g. used by some extent (used in AK_block->type)
 */
#define BLOCK_TYPE_NORMAL 0

/**
  * @def BLOCK_TYPE_CHAINED
  * @brief Constant declaring chained block type e.g. used if the block is chained with another (used in AK_block->type)
 */
#define BLOCK_TYPE_CHAINED 1

/**
  * @def NOT_CHAINED
  * @brief Constant used in AK_block->chained_with if the block isn't chained
 */
#define NOT_CHAINED -1

/**
  * @def FREE_INT
  * @brief Constant declaring dummy data for empty integers
 */
#define FREE_INT -10

/**
  * @def FREE_CHAR
  * @brief Constant declaring dummy data for empty chars
 */
#define FREE_CHAR '\0'

/**
  * @def DB_FILE
  * @brief Constant declaring name of the main DB file
 */
#define DB_FILE "kalashnikov.db"

/**
  * @def MAX_EXTENTS
  * @brief Constant declaring maximum number of extents for a given segment
 */
//#define MAX_EXTENTS 20

/**
  * @def MAX_ATTRIBUTES
  * @brief Constant declaring maximum number of attributes per block
 */
#define MAX_ATTRIBUTES 10

/**
  * @def MAX_ATT_NAME
  * @brief Constant declaring maximum length of attribute name string (used in AK_header->att_name)
 */
#define MAX_ATT_NAME 255

/**
  * @def MAX_CONSTRAINTS
  * @brief Constant declaring maximum number of constraints per attribute
 */
#define MAX_CONSTRAINTS 5

/**
  * @def MAX_CONSTR_NAME
  * @brief Constant declaring maximum length of constraint name string (used in AK_header->constr_name)
 */
#define MAX_CONSTR_NAME 255

/**
  * @def MAX_CONSTR_CODE
  * @brief Constant declaring maximum lenght of constraint code string
 */
#define MAX_CONSTR_CODE 255

/**
  * @def DATA_BLOCK_SIZE
  * @brief Constant declaring length of data block size (used in AK_block->data)
 */
#define DATA_BLOCK_SIZE 500

/**
  * @def DATA_ENTRY_SIZE
  * @brief Constant declaring lenght of data entry in sizeof( int )
 */
#define DATA_ENTRY_SIZE 10

/**
  * @def DB_FILE_SIZE
  * @brief Constant declaring size of DB file in MB
 */
#define DB_FILE_SIZE 20 //before 40

/**
  * @def MAX_DB_FILE_BLOCKS
  * @brief Constant declaring total blocks in DB file (for the given DB_FILE size)
 */
#define DB_FILE_BLOCKS_NUM (1024 * 1024 * DB_FILE_SIZE / sizeof(AK_block))

/**
  * @def INITIAL_EXTENT_SIZE
  * @brief Constant declaring initial extent size in blocks
 */
#define INITIAL_EXTENT_SIZE 15 //before 20

/**
  * @def EXTENT_GROWTH_TABLE
  * @brief Constant declaring extent growth factor for tables
 */
#define EXTENT_GROWTH_TABLE 0.5

/**
  * @def EXTENT_GROWTH_INDEX
  * @brief Constant declaring extent growth factor for indices
 */
#define EXTENT_GROWTH_INDEX 0.2

/**
  * @def EXTENT_GROWTH_TRANSACTION
  * @brief Constant declaring extent growth factor for transaction segments
 */
#define EXTENT_GROWTH_TRANSACTION 0.2

/**
  * @def EXTENT_GROWTH_TEMP
  * @brief Constant declaring extent growth factor for temporary segments
 */
#define EXTENT_GROWTH_TEMP 0.5

/**
  * @def SEGMENT_TYPE_SYSTEM_TABLE
  * @brief Constant declaring system table segment type (used in system catalog)
 */
#define SEGMENT_TYPE_SYSTEM_TABLE 0

/**
  * @def SEGMENT_TYPE_TABLE
  * @brief Constant declaring table segment type (used in system catalog)
 */
#define SEGMENT_TYPE_TABLE 1

/**
  * @def SEGMENT_TYPE_INDEX
  * @brief Constant declaring index segment type (used in system catalog)
 */
#define SEGMENT_TYPE_INDEX 2

/**
   * @def SEGMENT_TYPE_TRANSACTION
   * @brief Constant declaring transaction segment type (used in system catalog)
 */
#define SEGMENT_TYPE_TRANSACTION 3

/**
  * @def SEGMENT_TYPE_TEMP
  * @brief Constant declaring temporary segment type (used in system catalog)
 */
#define SEGMENT_TYPE_TEMP 4

/**
  * @def TYPE_INTERNAL
  * @brief Constant declaring internal data type (used in AK_header->type and AK_tuple_dict->type)
 */
#define TYPE_INTERNAL 0

/**
\def TYPE_INT
\brief integer data type (used in AK_header->type and AK_tuple_dict->type)
 */
#define TYPE_INT 1

/**
  * @def TYPE_FLOAT
  * @brief Constant declaring float data type (used in AK_header->type and AK_tuple_dict->type)
 */
#define TYPE_FLOAT 2

/**
  * @def TYPE_NUMBER
  * @brief Constant declaring number data type (used in AK_header->type and AK_tuple_dict->type)
 */
#define TYPE_NUMBER 3

/**
  * @def TYPE_VARCHAR
  * @brief Constant declaring varchar data type (used in AK_header->type and AK_tuple_dict->type)
 */

#define TYPE_VARCHAR 4

/**
  * @def TYPE_DATE
  * @brief Constant declaring date data type (used in AK_header->type and AK_tuple_dict->type)
 */
#define TYPE_DATE 5

/**
  * @def TYPE_DATETIME
  * @brief datetime data type (used in AK_header->type and AK_tuple_dict->type)
 */
#define TYPE_DATETIME 6

/**
  * @def TYPE_TIME
  * @brief Constant declaring time data type (used in AK_header->type and AK_tuple_dict->type)
 */
#define TYPE_TIME 7

/**
  * @def TYPE_BLOB
  * @brief blob data type (used in AK_header->type and AK_tuple_dict->type)
 */
#define TYPE_BLOB 8

/**
  * @def TYPE_BOOL
  * @brief Constant declaring boolean data type (used in AK_header->type and AK_tuple_dict->type)
 */
#define TYPE_BOOL 9

/**
 * @def TYPE_OPERAND
 * @brief Constant indicating operand in AK_list
 */
#define TYPE_OPERAND 10

/**
  * @def TYPE_OPERATOR
  * @brief indicates operator in AK_list
 */
#define TYPE_OPERATOR 11

/**
  * @def TYPE_ATTRIBS
  * @brief Constant indicating attribute/s in AK_list
 */
#define TYPE_ATTRIBS 12

/**
  * @def TYPE_CONDITION
  * @brief Constant indicating condition in AK_list
 */
#define TYPE_CONDITION 13

/**
 * @def BLOCK_CLEAN
 * @brief Constant indicating block cleaning (not changed since read from disk)
 */
#define BLOCK_CLEAN 0

/**
 * @def BLOCK_DIRTY
 * @brief Constant indicating dirty block (changed since read from disk, has to be written)
 */
#define BLOCK_DIRTY 1

/**
  * @def MAX_CACHE_MEMORY
  * @brief Constant declaring maximum size of DB cache memory
 */
#define MAX_CACHE_MEMORY 255

/**
 * @def MAX_REDO_LOG_MEMORY
 * @brief maximum size of REDO log memory
 */
#define MAX_REDO_LOG_MEMORY 255

/**
  * @def MAX_QUERY_LIB_MEMORY
  * @brief Constant declaring maximum size of query lib memory
 */
#define MAX_QUERY_LIB_MEMORY 255

/**
 * @def MAX_QUERY_DICT_MEMORY
 * @brief Constant declaring maximum size of query dictionary memory
 */
#define MAX_QUERY_DICT_MEMORY 255

/**
 * @def MAX_QUERY_RESULT_MEMORY
 * @brief Constant declaring maximum size of query result cache memory
 */
#define MAX_QUERY_RESULT_MEMORY 255

/**
 * @def MAX_TOKENS
 * @brief Constant declaring maximum number of attributes to handle in relation equivalence function
 */
#define MAX_TOKENS 255

/**
 * @def ATTR_DELIMITER
 * @brief Constant declaring attributes delimiter
 */
#define ATTR_DELIMITER ";"

/**
  * @def ATTR_ESCAPE
  * @brief Constant indicating attributes escape section
 */
#define ATTR_ESCAPE '`'

/**
  * @def NULLL
  * @brief Constant declaring null value for tables
 */
#define NULLL "asdfgXYZ"

/**
  * @def relational operators
 */
#define RO_SELECTION 's'
#define RO_PROJECTION 'p'
#define RO_NAT_JOIN 'n'
#define RO_RENAME 'r'
#define RO_UNION 'u'
#define RO_INTERSECT 'i'
#define RO_EXCEPT 'e'
#define RO_THETA_JOIN 't'

/**
  * @def NEW_VALUE
  * @brief Constant indicating that data is new value
 */
#define NEW_VALUE 0

/**
  * @def SEARCH_CONSTRAINT
  * @brief Constant indicating that data is constraint to search for
 */
#define SEARCH_CONSTRAINT 1

/**
  * @def UPDATE
  * @brief Constant indicating that operation to be performed is update
 */
#define UPDATE 0

/**
 * @def DELETE
 * @brief Constant indicating that operation to be performed is delete
 */
#define DELETE 1

/**
 * @def FIND
 * @brief Constant indicating that operation to be performed is searching
 */
#define FIND 2

/**
 * @def MAX_MAIN_BUCKETS
 * @brief Constant declaring the maximum number of main buckets
*/
#define MAX_MAIN_BUCKETS 512

/**
 * @def MAIN_BUCKET_SIZE
 * @brief Constant declaring the size of main buckets
*/
#define MAIN_BUCKET_SIZE 4

/**
 * @def HASH_BUCKET_SIZE
 * @brief Constant declaring the size of hash buckets
*/
#define HASH_BUCKET_SIZE 4

/**
 * @def INFO_BUCKET
 * @brief Constant declaring type of bucket as "info bucket" when inserting bucket to block
*/
#define INFO_BUCKET 0

/**
 * @def MAIN_BUCKET
 * @brief Constant declaring type of bucket as "main bucket" when inserting bucket to block
*/
#define MAIN_BUCKET 1

/**
 * @def HASH_BUCKET
 * @brief Constant declaring type of bucket as "hash bucket" when inserting bucket to block
*/
#define HASH_BUCKET 2

#endif

