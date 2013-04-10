/**
@file hash.h Header file that provides data structures for Hash indices
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

#ifndef HASH
#define HASH

#include "index.h"
#include "table.h"
#include "constants.h"
#include "configuration.h"

/**
 * @author Unknown
 * @struct hash_info
 * @brief Structure for defining a hash info element
*/
typedef struct {
	/// modulo value for hash function
    int modulo;
    /// bucket number
    int main_bucket_num;
    /// hash bucket number
    int hash_bucket_num;
} hash_info;

/**
 * @author Unknown
 * @struct bucket_elem
 * @brief Structure for defining a single bucket element
 */
typedef struct {
	/// bucket element hash value
    unsigned int value;
    /// bucket element address values
    struct_add add;
} bucket_elem;

/**
 * @author Unknown
 * @struct main_bucket
 * @brief Structure for defining main bucket for table hashing
 */
typedef struct {
	/// main bucket array of bucket_elem elements
    bucket_elem element[MAIN_BUCKET_SIZE];
} main_bucket;

/**
 * @author Unknown
 * @struct hash_bucket
 * @brief Structure for hash bucket for table hashing
 */
typedef struct {
	/// hash bucket level
    int bucket_level;
    /// hash bucket array of bucket_elem elements
    bucket_elem element[HASH_BUCKET_SIZE];
} hash_bucket;

#endif

int AK_elem_hash_value(AK_list_elem elem);
struct_add* Ak_insert_bucket_to_block(char *indexName, char *data, int type);
void Ak_update_bucket_in_block(struct_add *add, char *data);
void AK_change_hash_info(char *indexName, int modulo, int main_bucket_num, int hash_bucket_num);
hash_info* AK_get_hash_info(char *indexName);
struct_add* Ak_get_nth_main_bucket_add(char *indexName, int n);
int AK_insert_in_hash_index(char *indexName, int hashValue, struct_add *add);
struct_add *AK_find_delete_in_hash_index(char *indexName, AK_list *values, int delete);
struct_add * AK_find_in_hash_index(char *indexName, AK_list *values);
void AK_delete_in_hash_index(char *indexName, AK_list *values);
int AK_create_hash_index(char *tblName, AK_list *attributes, char *indexName);
int AK_delete_hash_index(char *indexName) ;
void Ak_hash_test() ;
