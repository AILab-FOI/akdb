/**
@file between.h Provides data structures for creating id of objects
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

#ifndef ID
#define ID

#include "table.h"
#include "fileio.h"
/**
 * @def ID_START_VALUE
 * @brief Constant declaring start value of id 
*/
#define ID_START_VALUE 100


int AK_get_id();
void Ak_id_test();

#endif
