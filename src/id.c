/**
@file between.c Provides functions for creating id of objects
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

#include "id.h"


/**
 * @author Mislav Čakarić.
 * @brief  Get unique ID for any object, stored in sequence
 * @return objectID
 */
int AK_get_id(){
    char temp_data[MAX_VARCHAR_LENGHT];
    element row_root = (element) malloc(sizeof (list));
    InitializeList(row_root);
    AK_list *row = (AK_list*) malloc(sizeof (AK_list));
    int i, num_rec = AK_get_num_records("AK_sequence");
    int exists=0;
    int current_value=ID_START_VALUE;
    for (i = 0; i < num_rec; i++) {
        row = AK_get_row(i, "AK_sequence");
        AK_list_elem value = GetNthL(1, row);
        memcpy(temp_data,&value->data,value->size);
        if(strcmp(temp_data,"objectID")){
            exists=1;
            value = GetNthL(2, row);
            memcpy(&current_value,&value->data,value->size);
            break;
        }
    }
    if(exists){
        DeleteAllElements(row_root);
        InsertNewElementForUpdate(TYPE_VARCHAR, "objectID", "AK_sequence", "name", row_root, 1);
        delete_row(row_root);
        current_value++;
    }
    DeleteAllElements(row_root);
    int value=0;
    InsertNewElement(TYPE_INT, &value, "AK_sequence", "obj_id", row_root);
    InsertNewElement(TYPE_VARCHAR, "objectID", "AK_sequence", "name", row_root);
    InsertNewElement(TYPE_INT, &current_value, "AK_sequence", "current_value", row_root);
    InsertNewElement(TYPE_INT, &value, "AK_sequence", "max", row_root);
    InsertNewElement(TYPE_INT, &value, "AK_sequence", "min", row_root);
    InsertNewElement(TYPE_BOOL, &value, "AK_sequence", "tezina", row_root);
    value=1;
    InsertNewElement(TYPE_INT, &value, "AK_sequence", "increment", row_root);
    insert_row(row_root);
    return current_value;
}

/**
 * @author Mislav Čakarić.
 * @brief  function for testing getting ID's
 */
void id_test()
{
    printf("ID: %i\n",AK_get_id());
    AK_print_table("AK_sequence");
    printf("ID: %i\n",AK_get_id());
    AK_print_table("AK_sequence");
    printf("ID: %i\n",AK_get_id());
    AK_print_table("AK_sequence");
    printf("ID: %i\n",AK_get_id());
    AK_print_table("AK_sequence");
    printf("ID: %i\n",AK_get_id());
    AK_print_table("AK_sequence");
}