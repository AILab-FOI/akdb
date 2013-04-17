/**
@file query_optimization.h Header file that provides functions for general query optimization
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

#ifndef QUERY_OPTIMIZATION
#define QUERY_OPTIMIZATION

#include "rel_eq_comut.h"
#include "rel_eq_assoc.h"
#include "rel_eq_projection.h"
#include "rel_eq_selection.h"
/**
 * @def MAX_PERMUTATION
 * @brief Constant declaring maximum number of permutations 
*/
#define MAX_PERMUTATION 24

#endif

void AK_print_optimized_query(AK_list *list_query);
AK_list *AK_execute_rel_eq(AK_list *list_query, const char rel_eq, const char *FLAGS);
AK_list *AK_query_optimization(AK_list *list_query, const char *FLAGS, const int DIFF_PLANS);
void AK_query_optimization_test(AK_list *list_query) ;
