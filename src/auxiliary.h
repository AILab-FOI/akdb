/**
@file auxiliary.h Provides data structures for the auxiliary functions
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

#ifndef AUXILIARY
#define AUXILIARY

#include "assert.h"
#include "time.h"
#include "string.h"
#include "ctype.h"
#include "debug.h"
#include "configuration.h"

/**
 * \struct list_elem
 * \brief defines a list element
 */
struct list_elem {
    ///data type
    int type;
    //data size in list element
    int size;
    ///loaded data
    char data[ MAX_VARCHAR_LENGTH ];
    ///point to next element
    struct list_elem *next;
};

typedef struct list_elem AK_list;
typedef struct list_elem *AK_list_elem;

/**
 * @author Matija Novak 
 * @struct list_structure_op
 * @brief defines a list (obsolete)
 */
typedef struct {
    ///START row_element_op
    ///structure that reperesents one attribute which is used for projection or nat_join ect.

    ///table name
    char table[ MAX_ATT_NAME ];
    ///needed atribute in the table
    char attribute_name[ MAX_ATT_NAME ];
    ///next element in list
    struct list_structure_op *next;
} list_structure_op;

typedef list_structure_op *element_op;
typedef list_structure_op list_op;

#define TBL_BOX_OFFSET 1
//#define TBL_COL_DELIMITER '|'
//#define TBL_ROW_DELIMITER '-'

/*
enum tbl_valalign {
        ALIGN_LEFT  0,
        ALIGN_RIGHT 1;
};*/

/**
 * @author Matija Novak, moved to auxiliary.h from fileio.h by Tomislav Mikulček to avoid circular inclusions
 */
typedef struct list_structure_ {
    ///START row_element
    ///structure that reperesents one row of table that is inserted, updated, or deleted
    int type; //type of data
    unsigned char data[MAX_VARCHAR_LENGTH]; //data
    char table[ MAX_ATT_NAME ]; //table name
    char attribute_name[ MAX_ATT_NAME ]; //attribute name
    int constraint; //if ist 0 then its new data to be inserted in, when is it 1 then its a constraint on which update and delete searches the data
    ///END row_element
    struct list_structure_ *next; //next element in the list
} list_structure;

typedef list_structure *element;
typedef list_structure list;

int AK_strcmp(const void *a, const void *b);

#endif

