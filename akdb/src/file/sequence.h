/**
@file sequence.h Header file that provides data structures for sequences
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

#ifndef SEQUENCE
#define SEQUENCE

#include "table.h"
#include "id.h"
#include "fileio.h"
#include "../auxi/mempro.h"

int AK_sequence_add(char *name, int start_value, int increment, int max_value, int min_value, int cycle);
int AK_sequence_remove(char *name);
int AK_sequence_current_value(char *name);
int AK_sequence_next_value(char *name);
int AK_sequence_rename(char *old_name, char *new_name);
int AK_sequence_modify(char *name, int start_value, int increment, int max_value, int min_value, int cycle);
int AK_sequence_get_id(char *name);
void AK_sequence_test();

#endif
