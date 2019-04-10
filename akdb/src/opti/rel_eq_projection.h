/**
@file rel_eq_projection.h Header file that provides data structures for relational equivalences in projection
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

#ifndef REL_EQ_PROJECTION
#define REL_EQ_PROJECTION

#include "../auxi/test.h"
#include "../file/table.h"
#include "../auxi/mempro.h"

int AK_rel_eq_is_subset(struct list_node *list_elem_set, struct list_node *list_elem_subset);
int AK_rel_eq_can_commute(struct list_node *list_elem_attribs, struct list_node *list_elem_conds);
struct list_node *AK_rel_eq_get_attributes(char *tblName);
char *AK_rel_eq_projection_attributes(char *attribs, char *tblName);
char *AK_rel_eq_collect_cond_attributes(struct list_node *list_elem) ;
char *AK_rel_eq_remove_duplicates(char *attribs);
struct list_node *AK_rel_eq_projection(struct list_node *list_rel_eq) ;
void AK_print_rel_eq_projection(struct list_node *list_rel_eq) ;
TestResult AK_rel_eq_projection_test();

#endif
