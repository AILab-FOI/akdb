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

#include "table.h"

#endif

int AK_rel_eq_is_subset(AK_list_elem list_elem_set, AK_list_elem list_elem_subset);
int AK_rel_eq_can_commute(AK_list_elem list_elem_attribs, AK_list_elem list_elem_conds);
AK_list *AK_rel_eq_get_attributes(char *tblName);
char *AK_rel_eq_projection_attributes(char *attribs, char *tblName);
char *AK_rel_eq_collect_cond_attributes(AK_list_elem list_elem) ;
char *AK_rel_eq_remove_duplicates(char *attribs);
AK_list *AK_rel_eq_projection(AK_list *list_rel_eq) ;
void AK_print_rel_eq_projection(AK_list *list_rel_eq) ;
void AK_rel_eq_projection_test();
