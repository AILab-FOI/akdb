

#ifndef CONFIGURATION
#define CONFIGURATION



#include "ini_parser/iniparser.h"
/**
 * @def DB_NAME
 * @brief Constant declaring the name oof database file
*/
#define DB_FILE "kalashnikov.db"
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
  * @def MAX_OBSERVABLE_OBSERVERS
  * @brief Constant for declaring maximum number of observers objects for some observable type
 */
#define MAX_OBSERVABLE_OBSERVERS 10
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
#define DB_FILE_SIZE 40
/**
  * @def MAX_DB_FILE_BLOCKS
  * @brief Constant declaring total blocks in DB file (for the given DB_FILE size)
 */
#define DB_FILE_BLOCKS_NUM (1024 * 1024 * DB_FILE_SIZE / sizeof(AK_block))
/**
  * @def INITIAL_EXTENT_SIZE
  * @brief Constant declaring initial extent size in blocks
 */
#define INITIAL_EXTENT_SIZE 15
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
 * @def NUMBER_OF_KEYS
 * @brief Constant declaring number of buckets in hash table
*/
#define NUMBER_OF_KEYS 512

/**
 * @def NUMBER_OF_THREADS
 * @brief Constant declaring maximum number of threads that an application can acquire
*/
#define NUMBER_OF_THREADS 42

/**
  * @def MAX_EXTENTS
  * @brief Constant declaring maximum number of extents for a given segment
 */
//#define MAX_EXTENTS 20


#endif