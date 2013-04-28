/**
@file filesort.h Header filr provides data structures for file sorting
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

#ifndef FILESORT
#define FILESORT

#include "../mm/memoman.h"
#include "table.h"
#include "files.h"
/**
  * @def DATA_ROW_SIZE
  * @brief Constatnt declaring size of data to be compared
  */

#define DATA_ROW_SIZE 200
/**
  * @def DATA_TUPLE_SIZE
  * @brief Costant declaring size of data to be copied
  */

#define DATA_TUPLE_SIZE 500


#endif

int Ak_get_total_headers(AK_block *iBlock);
int Ak_get_header_number(AK_block *iBlock, char *attribute_name);
int Ak_get_num_of_tuples(AK_block *iBlock);
void AK_sort_segment(char *table_name, char *attr);
void Ak_reset_block(AK_block * block);
void AK_block_sort(AK_block * iBlock, char * atr_name);
void Ak_filesort_test();
