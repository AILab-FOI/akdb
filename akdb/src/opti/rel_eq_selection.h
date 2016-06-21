/**
@file rel_eq_selection.h Header file that provides data structures for relational equivalences in selection
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

#ifndef REL_EQ_SELECTION
#define REL_EQ_SELECTION

#include "../file/table.h"
#include "../auxi/mempro.h"

int AK_rel_eq_is_attr_subset(char *set, char *subset) ;
char *AK_rel_eq_get_atrributes_char(char *tblName) ;
char *AK_rel_eq_cond_attributes(char *cond) ;
int AK_rel_eq_share_attributes(char *set, char *subset) ;
struct list_node *AK_rel_eq_split_condition(char *cond) ;
struct list_node *AK_rel_eq_selection(struct list_node *list_rel_eq);
void AK_print_rel_eq_selection(struct list_node *list_rel_eq) ;
void AK_rel_eq_selection_test();

#endif

