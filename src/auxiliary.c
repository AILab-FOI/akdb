/**
@file auxiliary.c Provides auxiliary functions
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

#include "auxiliary.h"

/** 
 @author Miroslav Policki
 
 Returns size in bytes of the provided database type

 @param iDB_type database data type (defined in configuration.h)
 @param szVarchar if iDB_type == TYPE_VARCHAR, pointer to the string, otherwise unused

 @return size of provided data type in bytes if provided data type is valid, else 0
*/
size_t AK_type_size(int iDB_type, char *szVarchar) {
    switch(iDB_type) {
        case TYPE_INTERNAL:
            return sizeof(int);
        case TYPE_INT:
            return sizeof(int);
        case TYPE_FLOAT:
            return sizeof(double);
        case TYPE_NUMBER:
            return sizeof(double);
        case TYPE_VARCHAR:
            return (size_t) strlen(szVarchar);
        case TYPE_DATE:
            return sizeof(int);
        case TYPE_DATETIME:
            return sizeof(int);
        case TYPE_TIME:
            return sizeof(int);
        case TYPE_BLOB:
            return sizeof(void *);
        case TYPE_BOOL:
            return (size_t) 1;
        default:
            return (size_t) 0;
    }
}



