/**
@file selection.h Provides data structures for relational selection operation
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

#ifndef SELECTION
#define SELECTION

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "configuration.h"
#include "projection.h"
#include "dbman.h"
#include "memoman.h"

struct list_elem{
    int type;
    int size;
    char data[ MAX_VARCHAR_LENGHT ];
    struct list_elem *next;
};

typedef struct list_elem AK_list;
typedef struct list_elem* AK_list_elem;


#endif
