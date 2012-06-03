/**
@file reference.h Provides data structures for referential integrity
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
 
#ifndef REFERENCE
#define REFERENCE

#include "dbman.h"
/**
 * @def REF_TYPE_NONE
 * @brief Constant declaring none reference type 
*/
#define REF_TYPE_NONE -1
/**
 * @def REF_TYPE_SET_NULL
 * @brief Constant declaring set null reference type 
*/
#define REF_TYPE_SET_NULL 1
/**
 * @def REF_TYPE_NO_ACTION
 * @brief Constant declaring no action reference type 
*/
#define REF_TYPE_NO_ACTION 2
/**
 * @def REF_TYPE_NO_ACTION
 * @brief Constant declaring cascade reference type 
*/
#define REF_TYPE_CASCADE 3
/**
 * @def REF_TYPE_RESTRICT
 * @brief Constant declaring restrict reference type 
*/
#define REF_TYPE_RESTRICT 4
/**
 * @def REF_TYPE_SET_DEFAULT
 * @brief Constant declaring set default reference type 
*/
#define REF_TYPE_SET_DEFAULT 5 // reserved for future use
/**
 * @def MAX_REFERENCE_ATTRIBUTES
 * @brief Constant declaring maximum number of reference attributes
*/
#define MAX_REFERENCE_ATTRIBUTES 10 //how many attributes can be define in a single foreign key constraint
/**
 * @def MAX_CHILD_CONSTRAINTS
 * @brief Constant declaring maximum number of child constraints
*/
#define MAX_CHILD_CONSTRAINTS 20 // how many references can be made on one parent table, this will be deprecated by the use of a list
/**
 * @author Dejan Franković
 * @struct AK_ref_item
 * @brief Structure that represents reference item. It contains of table, attributes, parent table and it's attributes, number of attributes, constraint
	  and type of reference.
 */
typedef struct {
    char table[MAX_ATT_NAME];
    char attributes[MAX_REFERENCE_ATTRIBUTES][MAX_ATT_NAME];
    char parent[MAX_ATT_NAME];
    char parent_attributes[MAX_REFERENCE_ATTRIBUTES][MAX_ATT_NAME];
    int attributes_number;
    char constraint[MAX_VARCHAR_LENGTH];
    int type;
} AK_ref_item;

#endif

int AK_add_reference(char *childTable, char *childAttNames[], char *parentTable, char *parentAttNames[], int attNum, char *constraintName, int type) ;
AK_ref_item AK_get_reference(char *tableName, char *constraintName) ;
int AK_reference_check_attribute(char *tableName, char *attribute, char *value) ;
int AK_reference_check_if_update_needed(AK_list *lista, int action) ;
int AK_reference_check_restricion(AK_list *lista, int action) ;
int AK_reference_update(AK_list *lista, int action) ;
int AK_reference_check_entry(AK_list *lista) ;
void AK_reference_test();

