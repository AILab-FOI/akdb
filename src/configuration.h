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

#ifndef CONFIGURATION
#define CONFIGURATION

#define EXIT_SUCCESS 0
#define EXIT_ERROR 1

#define BLOCK_TYPE_FREE -1
#define BLOCK_TYPE_NORMAL 0
#define BLOCK_TYPE_CHAINED 1
#define NOT_CHAINED -1
#define BLOCK_DATA_BEGIN 0
#define FREE_INT -1
#define FREE_CHAR '\0'

#define DB_FILE "kalashnikov.db"

#define MAX_OBJECT_NAME 255
#define MAX_EXTENTS 20
#define MAX_ATTRIBUTES 10
#define MAX_ATT_NAME 255
#define MAX_CONSTRAINTS 5
#define MAX_CONSTR_NAME 255
#define MAX_CONSTR_CODE 255
#define DATA_BLOCK_SIZE 500

#define DB_FILE_SIZE 20

#endif
 
