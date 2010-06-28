/**
@file debug.h Defines global macros, constants and variables for debuging
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

#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"

typedef enum debug_level {
	LOW = 0,
	MIDDLE = 0,
	HIGH
} DEBUG_LEVEL;

typedef enum debug_type {
	GLOBAL = 0,
	DB_MAN = 0,
	FILE_MAN = 0,
	MEMO_MAN = 0,
	INDICES = 0,
	TABLES = 0,
	REL_OP,
	REL_EQ = 0,
	CONSTRAINTS = 0
} DEBUG_TYPE;