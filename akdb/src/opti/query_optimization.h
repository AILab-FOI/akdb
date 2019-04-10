/**
@file query_optimization.h Header file that provides data structure, functions and defines for general query optimization
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

#include "../auxi/test.h"
#include "rel_eq_comut.h"
#include "rel_eq_assoc.h"
#include "rel_eq_projection.h"
#include "rel_eq_selection.h"

#include "../auxi/mempro.h"
#include "../sql/view.h"

/**
 * @def MAX_PERMUTATION
 * @brief Constant declaring maximum number of permutations 
*/
#define MAX_PERMUTATION 24


void AK_print_optimized_query(struct list_node *list_query);
struct list_node *AK_execute_rel_eq(struct list_node *list_query, const char rel_eq, const char *FLAGS);
struct list_node *AK_query_optimization(struct list_node *list_query, const char *FLAGS, const int DIFF_PLANS);
TestResult AK_query_optimization_test() ; // (struct list_node *list_query)

#endif
