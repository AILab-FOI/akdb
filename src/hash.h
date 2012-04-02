/**
@file hash.h Provides data structures for Hash indices
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
#include "configuration.h"

/**
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
 * @struct main_bucket
 * @brief Main bucket for table hashing
 */
typedef struct {
	/// main bucket array of bucket_elem elements
    bucket_elem element[MAIN_BUCKET_SIZE];
} main_bucket;

/**
 * @struct hash_bucket
 * @brief Hash bucket for table hashing
 */
typedef struct {
	/// hash bucket level
    int bucket_level;
    /// hash bucket array of bucket_elem elements
    bucket_elem element[HASH_BUCKET_SIZE];
} hash_bucket;

#endif
