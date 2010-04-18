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
#ifndef FILEIO
#define FILEIO

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dbman.h"
#include "memoman.h"
#include "string.h"
#include "configuration.h"
#include "auxiliary.h"


///@author Matija Novak
typedef struct list_structure_ {
	///START row_element
	///structure that reperesents one row of table that is inserted, updated, or deleted
	int type; //type of data
	unsigned char data[MAX_VARCHAR_LENGHT]; //data
	char table[ MAX_ATT_NAME ]; //table name
	char attribute_name[ MAX_ATT_NAME ]; //attribute name
	int constraint; //if ist 0 then its new data to be inserted in, when is it 1 then its a constraint on which update and delete searches the data
	///END row_element
	struct list_structure_ *next;//next element in the list
}list_structure;

typedef list_structure *element;
typedef list_structure list;

#endif