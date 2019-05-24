/**
@file index.h Header file that provides data structures for bitmap index
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

#include "../../auxi/mempro.h"
#include "../../file/table.h"
#include "../../file/fileio.h"
#include "../../file/files.h"

/**
  * @author Unknown
  * @struct struct_add
  * @brief Structure defining node address
 */
typedef struct {
	/// block address
    int addBlock;
    /// index table destination
    int indexTd;
} struct_add;

/**
  * @struct list_structure_add
  * @brief Structure that defines linked list node for index
 */
typedef struct list_structure_ad {
	/// attribute name
    char *attName;
    /// addresses
    struct_add add;
    /// next node pointer
    struct list_structure_ad *next;
} list_structure_ad;

typedef list_structure_ad *element_ad;
typedef list_structure_ad list_ad;

//printing index table
int AK_index_table_exist(char *indexTblName);
void AK_print_index_table(char *indexTblName);
struct list_node *AK_get_index_tuple(int row, int column, char *indexTblName);
int AK_get_index_num_records(char *indexTblName);
int AK_num_index_attr(char *indexTblName);

struct list_node *AK_get_index_tuple(int row, int column, char *indexTblName);


void AK_InitializelistAd(list_ad *L);
element_ad AK_Get_First_elementAd(list_ad *L);
element_ad AK_Get_Last_elementAd(list_ad *L);
element_ad AK_Get_Next_elementAd(element_ad Currentelement_op);
element_ad AK_Get_Previous_elementAd(element_ad Currentelement_op, element_ad L) ;
int AK_Get_Position_Of_elementAd(element_ad Searchedelement_op, list_ad *L);
void AK_Delete_elementAd(element_ad Deletedelement_op, list_ad *L) ;
void AK_Delete_All_elementsAd(list_ad *L) ;
void AK_Insert_NewelementAd(int addBlock, int indexTd, char *attName, element_ad elementBefore);

void AK_index_test();


#endif
