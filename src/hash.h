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

#include "auxiliary.h"
#include "dbman.h"
#include "memoman.h"
#include "index.h"
#include "table.h"

#define MAX_MAIN_BUCKETS 512
#define MAIN_BUCKET_SIZE 4 //only the power of 2 (ie: 2, 4, 8, 16, 32, 64, 128 etc.)
#define HASH_BUCKET_SIZE 4
#define INFO_BUCKET 0
#define MAIN_BUCKET 1
#define HASH_BUCKET 2

typedef struct {
    int modulo;
    int main_bucket_num;
    int hash_bucket_num;
} hash_info;

typedef struct {
    unsigned int value;
    struct_add add;
} bucket_elem;

typedef struct {
    bucket_elem element[MAIN_BUCKET_SIZE];
} main_bucket;

typedef struct {
    int bucket_level;
    bucket_elem element[HASH_BUCKET_SIZE];
} hash_bucket;

#endif