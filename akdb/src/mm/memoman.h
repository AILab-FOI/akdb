/**
@file memoman.h Header file that contains data structures, defines and functions for the memory manager
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
#ifndef MEMOMAN
#define MEMOMAN

#include "../auxi/test.h"
#include "../dm/dbman.h"
#include "../auxi/mempro.h"

/**
  * @author Unknown
  * @struct AK_mem_block
  * @brief Structure that defines a block of data in memory
 */
typedef struct {
    /// pointer to block from DB file
    AK_block * block;
    /// dirty bit (BLOCK_CLEAN if unchanged; BLOCK_DIRTY if changed but not yet written to file)
    int dirty;
    /// timestamp when the block has lastly been read
    unsigned long timestamp_read;
    /// timestamp when the block has lastly been changed
    unsigned long timestamp_last_change;
} AK_mem_block;

/**
  * @author Unknown
  * @struct AK_db_cache
  * @brief Structure that defines global cache memory
 */
typedef struct {
    /// last recently read blocks
    AK_mem_block * cache[ MAX_CACHE_MEMORY ];
    /// next cached block to be replaced (0 - MAX_CACHE_MEMORY-1); depends on caching algorithm
    int next_replace;
} AK_db_cache;

/**
 * Structure that contains all vital information for the command
 * that is about to execute. It is defined by the operation (INSERT,
 * UPDATE, DELETE that are defined inside the const.c file), table
 * where the data is stored, and certain data that will be stored.
 * Updated can be used to save select operation
 * @author Tomislav Turek updated by Danko Bukovac
 * @struct AK_command_recovery_struct
 * @brief recovery structure used to recover commands from binary file
 */
typedef struct {
    int operation;
    char table_name[MAX_VARCHAR_LENGTH];
    char arguments[MAX_ATTRIBUTES][MAX_VARCHAR_LENGTH];
    char condition[MAX_ATTRIBUTES][MAX_VARCHAR_LENGTH];
    int finished;
} AK_command_recovery_struct;

/**
 * The structure defines an array of commands being executed at the moment.
 * If and when commands fail to execute, the rest of the commands that did not
 * execute will be stored inside a binary file and the system will try recovery
 * and execution for those commands. With the array, we also store a number
 * that defines the number of commands that failed to execute (length of 
 * command_recovery array).
 * @author Dražen Bandić, updated by Tomislav Turek
 * @struct AK_redo_log
 * @brief Structure that defines global redo log
 */
typedef struct {
    AK_command_recovery_struct command_recovery[MAX_REDO_LOG_ENTRIES];
    int number;
} AK_redo_log;

/**
  * @author Unkown
  * @struct AK_query_mem_lib
  * @brief Structure that defines global query memory for libraries
 */
typedef struct {
    /// last parsed queries; to be changed to more adequate data structure
    char parsed[ MAX_QUERY_LIB_MEMORY ];
    /// next query to be replaced (0 - MAX_QUERY_LIB_MEMORY-1); field pointer (LIFO)
    int next_replace;
} AK_query_mem_lib;

/**
  * @author Unkown
  * @struct AK_query_mem_dict
  * @brief Structure that defines global query memory for data dictionaries
 */
typedef struct {
    /// last used data dictionaries
    AK_tuple_dict * dictionary[ MAX_QUERY_DICT_MEMORY ];
    /// next dictionary to be replaced (0 - MAX_QUERY_DICT_MEMORY-1); field pointer (LIFO)
    int next_replace;
} AK_query_mem_dict;


/**
  * @author Mario Novoselec
  * @struct AK_results
  * @brief Structure used for in-memory result caching
 */

typedef struct{
	unsigned long result_id;
	int result_size;
	char date_created[80];
	short free;
	char *source_table;
	AK_block *result_block;
	AK_header header[MAX_ATTRIBUTES];
}AK_results;

/**
  * @author Unknown
  * @struct AK_query_mem_result
  * @brief Structure that defines global query memory for results
 */

typedef struct {
    AK_results *results;
    /// next result to be replaced (0 - MAX_QUERY_RESULT_MEMORY-1); field pointer (LIFO)
    int next_replace;
} AK_query_mem_result;
/**
  * @author Unknown
  * @struct AK_query_mem
  * @brief Structure that defines global query memory
 */
typedef struct {
    /// parsed queries
    AK_query_mem_lib * parsed;
    /// obtained data dictionaries
    AK_query_mem_dict * dictionary;
    /// obtained query results
    AK_query_mem_result * result;
} AK_query_mem;

/**
 * @var db_cache
 * @brief Variable that defines the db cache
 */
AK_db_cache * db_cache;
/**
 * @var redo_log
 * @brief Variable that defines the global redo log
 */
AK_redo_log * redo_log;
/**
 * @var query_mem
 * @brief Variable that defines the global query memory
 */
AK_query_mem * query_mem;

void AK_cache_result(char *srcTable,AK_block *temp_block,AK_header header[]);
int AK_find_available_result_block();
unsigned long AK_generate_result_id(unsigned char *str);
int AK_cache_block(int num, AK_mem_block *mem_block);
int AK_cache_AK_malloc();
int AK_redo_log_AK_malloc();
int AK_query_mem_AK_malloc();

void AK_query_mem_AK_free();

int AK_memoman_init();
//int AK_memoman_AK_free();
AK_mem_block *AK_get_block(int num);
int AK_release_oldest_cache_block();
int AK_mem_block_modify(AK_mem_block* mem_block, int dirty);
int AK_refresh_cache();

table_addresses *AK_get_segment_addresses_internal(char *tableName, char *segmentName);
table_addresses *AK_get_segment_addresses(char * segmentName);
table_addresses *AK_get_index_segment_addresses(char * segmentName);

table_addresses *AK_get_table_addresses(char *table);
table_addresses *AK_get_index_addresses(char * index);
int AK_find_AK_free_space(table_addresses * addresses);
int AK_init_new_extent(char *table_name, int extent_type);
int AK_flush_cache();
TestResult AK_memoman_test();
TestResult AK_memoman_test2();

#endif
