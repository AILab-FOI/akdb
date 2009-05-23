/**
@file fileio.h Provides data structures for file input/output
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "configuration.h"



typedef struct {
	///START row_element
	///structure that reperesents one row of table that is inserted, updated, or deleted
	int type;
	unsigned char data[200];
	char table[ MAX_ATT_NAME ];
	char attribute_name[ MAX_ATT_NAME ];
	int constraint;
//možda dodati dali je atribut ograničenje ili nova vrijednost treba kod update-a
	///END row_element
	//you can add here your own attributes, but then you must implement your own special functions
	struct list_structure *next;
}list_structure;

typedef list_structure *element;
typedef list_structure list;

