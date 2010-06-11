/**
@file op_projekcija.h Provides data structures for relational projection operation
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
#ifndef PROJECTION
#define PROJECTION
#include "auxiliary.h"
#include "dbman.h"
#include "memoman.h"
#include "fileio.h"


///@author Matija Novak
typedef struct {
	///START row_element_op
	///structure that reperesents one attribute which is used for projection or to nat_join ect.
	
	///table name
	char table[ MAX_ATT_NAME ];
	///needed atribute in the table
	char attribute_name[ MAX_ATT_NAME ];
	///next element in list
	struct list_structure_op *next;
}list_structure_op;

typedef list_structure_op *element_op;
typedef list_structure_op list_op;

#endif