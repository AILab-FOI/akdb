/**
@file function.h Header file that provides data structures functions
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

#ifndef VIEW
#define VIEW

#include "table.h"

#endif

int AK_get_view_obj_id(char *name);
int AK_view_add(char *name, char *query, int set_id);
int AK_view_remove_by_obj_id(int obj_id);
int AK_view_remove_by_name(char *name);
int AK_view_rename(char *name, char *new_name);
int AK_view_change_query(char *name, char *query);
void AK_view_test();
