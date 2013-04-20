/**
@file rel_eq_comut.h Header file that provides data structures for relational equivalences regarding comutativity
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

#ifndef REL_EQ_COMUT
#define REL_EQ_COMUT

#include "../file/table.h"
#include "./rel_eq_selection.h"

#endif

char *AK_rel_eq_commute_with_theta_join(char *cond, char *tblName);
void rel_eq_comut_test();

