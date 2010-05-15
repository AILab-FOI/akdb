/**
@file index.h Provides data structures for bitmap index
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


#ifndef INDEX
#define	INDEX

typedef struct {
    int addTd;
    int addBlock;
    int sizeTd;
} struct_add;

typedef struct {
    char *attName;
    struct_add add;
    struct list_structure_ad *next;
} list_structure_ad;

typedef list_structure_ad *element_ad;
typedef list_structure_ad list_ad;

#endif

