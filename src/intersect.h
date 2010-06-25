/**
@file intersect.h Provides data structures for header atributes renaming
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
#ifndef INTERSECT
#define INTERSECT

#include "auxiliary.h"
#include "memoman.h"
#include "dbman.h"
#include "table.h"

typedef struct {
    /// type of attribute
    int type;
    /// attribute name
    char att_name[ MAX_ATT_NAME ];
} intersect_attr;

#endif

