/**
@file filesort.h Header file that provides functions and defines for file sorting
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

#include "../auxi/test.h"
#include "../mm/memoman.h"
#include "table.h"
#include "files.h"
#include "fileio.h"
#include "../auxi/mempro.h"
/**
  * @def DATA_ROW_SIZE
  * @brief Constatnt declaring size of data to be compared
  */

#define DATA_ROW_SIZE 200
/**
  * @def DATA_TUPLE_SIZE
  * @brief Constant declaring size of data to be copied
  */

#define DATA_TUPLE_SIZE 500



int AK_get_total_headers(AK_block *iBlock);
int AK_get_header_number(AK_block *iBlock, char *attribute_name);
int AK_get_num_of_tuples(AK_block *iBlock);
int AK_sort_segment(char *srcTable, char *destTable, struct list_node* attributes);
void AK_reset_block(AK_block * block);
void AK_block_sort(AK_block * iBlock, char * atr_name);
TestResult AK_filesort_test();

#endif

