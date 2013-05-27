/**
@file drop.h
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

#ifndef DROP
#define DROP

#include "../file/table.h"
#include "../file/fileio.h"

struct drop_arguments {
    void* value;
    void* next;
};

typedef struct drop_arguments AK_drop_arguments;
#endif

void AK_drop_test();
int AK_if_exist(char *tblName, char *sys_table);
void AK_drop_helper_function(char *tblName, char *sys_table);


