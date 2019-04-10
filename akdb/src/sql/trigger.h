/**
@file trigger.h Header file that provides functions and defines for  trigger.c
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

#ifndef TRIGGER
#define TRIGGER

#include "../auxi/test.h"
#include "../rec/archive_log.h"
#include "../file/table.h"
#include "../file/fileio.h"
#include "../file/id.h"
#include "../sql/function.h"
#include "../rel/selection.h"
#include "../auxi/mempro.h"

int AK_trigger_save_conditions(int trigger, struct list_node *condition);
int AK_trigger_add(char *name, char* event, struct list_node *condition, char* table, char* function, struct list_node *arguments_list);
int AK_trigger_get_id(char *name, char *table);
int AK_trigger_remove_by_name(char *name, char *table) ;
int AK_trigger_remove_by_obj_id(int obj_id);
int AK_trigger_edit(char *name, char* event, struct list_node* condition, char* table, char* function);
struct list_node *AK_trigger_get_conditions(int trigger);
int AK_trigger_rename(char *old_name, char *new_name, char *table);
TestResult AK_trigger_test();

#endif
