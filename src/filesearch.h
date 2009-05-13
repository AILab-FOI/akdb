/**
@file filesearch.h Provides data structures for file searching
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

#ifndef FILESEARCH
#define FILESEARCH

/**
\struct search_params
\brief Attribute, lower and upper data value, type - input for KK_equisearch_unsorted and KK_rangesearch_unsorted
*/
typedef struct {
    char *szAttribute;
    void *pData_lower;
    void *pData_upper;
    int   iType;
} search_params;

/**
\struct search_result
\brief Search result of KK_equisearch_unsorted and KK_rangesearch_unsorted
*/
typedef struct {
    /// array of tuple addresses
    int *aiTuple_addresses;
    /// number of tuple addresses in array
    int iNum_tuple_addresses;
    /// array of indexes of searched-for attributes
    int *aiSearch_attributes;
    /// number of searched-for attributes in array
    int iNum_search_attributes;
    /// number of attributes in tuple
    int iNum_tuple_attributes;
} search_result;

#endif
