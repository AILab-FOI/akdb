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
 * @author Saša Vukšić, updated by Mislav Čakarić, changed by Mario Peroković, now uses Ak_update_row
 * @brief Function for getting unique ID for any object, stored in sequence
 * @return objectID
 */
int AK_get_id() {
    int i;
    int exists = 0;
    int seq_id = -1;
    int current_value = ID_START_VALUE;
    char temp_data[MAX_VARCHAR_LENGTH];

    AK_list_elem row_root= (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root); 
    AK_list *row = (AK_list*) malloc(sizeof (AK_list));

    int num_rec = AK_get_num_records("AK_sequence");
    for (i = 0; i < num_rec; i++) {
        row = (AK_list *)AK_get_row(i, "AK_sequence");
        AK_list_elem value = Ak_GetNth_L(1, row);
        memcpy(temp_data, &value->data, value->size);
        temp_data[value->size] = 0; //terminate string
        
        if (strcmp(temp_data, "objectID") == 0) {
            exists = 1;
            value = Ak_GetNth_L(2, row);
            memcpy(&current_value, &value->data, value->size);
            value = Ak_GetNth_L(0, row);
            memcpy(&seq_id, &value->data, value->size);
            break;
        }
    }

    if(exists){
        current_value = current_value + 1;
        Ak_Insert_New_Element_For_Update(TYPE_INT, &seq_id, "AK_sequence", "obj_id", row_root, SEARCH_CONSTRAINT);
        Ak_Insert_New_Element_For_Update(TYPE_INT, &current_value, "AK_sequence", "current_value", row_root, NEW_VALUE);
        int result = Ak_update_row(row_root);
        Ak_DeleteAll_L(row_root);
        free(row_root);

        if(result != EXIT_SUCCESS){
            return EXIT_ERROR;
        }
        return current_value;
    }

    int value = 0;
    Ak_Insert_New_Element(TYPE_INT, &value, "AK_sequence", "obj_id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "objectID", "AK_sequence", "name", row_root);
    Ak_Insert_New_Element(TYPE_INT, &current_value, "AK_sequence", "current_value", row_root);
    value = 1;
    Ak_Insert_New_Element(TYPE_INT, &value, "AK_sequence", "increment", row_root);
    Ak_insert_row(row_root);
    Ak_DeleteAll_L(row_root);
    free(row_root);
    return current_value;
}

/**
 * @author Mislav Čakarić.
 * @brief  Function for testing getting ID's
 */
void Ak_id_test() {
    printf("ID: %i\n", AK_get_id());
    AK_print_table("AK_sequence");
    printf("ID: %i\n", AK_get_id());
    AK_print_table("AK_sequence");
    printf("ID: %i\n", AK_get_id());
    AK_print_table("AK_sequence");
    printf("ID: %i\n", AK_get_id());
    AK_print_table("AK_sequence");
    printf("ID: %i\n", AK_get_id());
    AK_print_table("AK_sequence");
}
