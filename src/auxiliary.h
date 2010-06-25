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

#include "time.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
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
    char data[ MAX_VARCHAR_LENGHT ];
    ///point to next element
    struct list_elem *next;
};

typedef struct list_elem AK_list;
typedef struct list_elem* AK_list_elem;

/**
 * @author Matija Novak 
 * @struct list_structure_op
 * @brief defines a list (obsolete)
 */
typedef struct {
    ///START row_element_op
    ///structure that reperesents one attribute which is used for projection or to nat_join ect.

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

#endif

