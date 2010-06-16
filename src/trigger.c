/**
@file trigger.c Provides functions for triggers
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

#include "trigger.h"



int AK_trigger_add(char *name, char* event, char* condition, char* table, char* function) {
    int i;
    int funk_id = -1, table_id = -1, trigg_id;

    AK_list *row;

    i = 0;
    while ((row = AK_get_row(i,"AK_relation")) != NULL ) {
        if (strcmp(row->next->next->data, table) == 0) {
            memcpy(&table_id, row->next->data, sizeof(int));
            break;
        }
        i++;
    }

    if (table_id == -1) {
        if (DEBUG)
            printf("AK_trigger_add: No such table upon which to create a trigger.\n");
        return EXIT_ERROR;
    }

    i = 0;
    while ((row = AK_get_row(i, "AK_function")) != NULL) {
        if (strcmp(row->next->next->data, function) == 0) {
            memcpy(&funk_id, row->next->data, sizeof(int));
            break;
        }
        i++;
    }

    if (funk_id == -1) {
        if (DEBUG)
            printf("AK_trigger_add: No such function to execute upon activation of trigger.\n");
        return EXIT_ERROR;
    }

    element row_root =  (element) malloc( sizeof(list) );
    InitializeList(row_root);

    trigg_id = AK_get_id();
    InsertNewElement( TYPE_INT, &trigg_id, "AK_trigger", "obj_id", row_root );
    InsertNewElement( TYPE_VARCHAR, name, "AK_trigger", "name", row_root );
    InsertNewElement( TYPE_VARCHAR, event, "AK_trigger", "event", row_root );
    InsertNewElement( TYPE_VARCHAR, condition, "AK_trigger", "condition", row_root );
    InsertNewElement( TYPE_INT, &funk_id, "AK_trigger", "action", row_root );
    InsertNewElement( TYPE_INT, &table_id, "AK_trigger", "on", row_root );
    insert_row( row_root );


    free(row_root);
    free(row);

    return trigg_id;
}

int AK_trigger_remove_by_name(char *name) {
    element row_root =  (element) malloc( sizeof(list) );
    InitializeList(row_root);
    
    InsertNewElementForUpdate(TYPE_VARCHAR, name, "AK_trigger", "name", row_root, 1);

    return delete_row(row_root);
}

int AK_trigger_remove_by_table(char *table) {
    AK_list *row;
    int i, table_id=-1;

    i = 0;
    while ((row = AK_get_row(i,"AK_relation")) != NULL ) {
        if (strcmp(row->next->next->data, table) == 0) {
            memcpy(&table_id, row->next->data, sizeof(int));
            break;
        }
        i++;
    }

    if (table_id == -1) {
        if (DEBUG)
            printf("AK_trigger_remove_by_table: No such table.\n");
        return EXIT_ERROR;
    }

    element row_root =  (element) malloc( sizeof(list) );
    InitializeList(row_root);

    InsertNewElementForUpdate(TYPE_INT, table_id, "AK_trigger", "on", row_root, 1);

    return delete_row(row_root);
}

int AK_trigger_remove_by_function(char *function) {
    AK_list *row;
    int i, funk_id=-1;

    i = 0;
    while ((row = AK_get_row(i, "AK_function")) != NULL) {
        if (strcmp(row->next->next->data, function) == 0) {
            memcpy(&funk_id, row->next->data, sizeof(int));
            break;
        }
        i++;
    }

    if (funk_id == -1) {
        if (DEBUG)
            printf("AK_trigger_remove_by_table: No such table.\n");
        return EXIT_ERROR;
    }

    element row_root =  (element) malloc( sizeof(list) );
    InitializeList(row_root);

    InsertNewElementForUpdate(TYPE_INT, funk_id, "AK_trigger", "action", row_root, 1);

    return delete_row(row_root);
}

int AK_trigger_edit(int *obj_id, char *name, char* event, char* condition, char* table, char* function) {
    AK_list *row;
    int id,i;

    if (obj_id == NULL || (name != NULL && strcmp(name,"") != 0 && table != NULL && strcmp(table, "") != 0)) {
        if (DEBUG)
            printf("AK_trigger_edit: Not enough data to identify the trigger.\n");
        return EXIT_ERROR;
    }

    element row_root =  (element) malloc( sizeof(list) );
    InitializeList(row_root);

    if (obj_id != NULL)
        InsertNewElementForUpdate(TYPE_INT, obj_id, "AK_trigger", "obj_id", row_root, 1);
    if (name != NULL && strcmp(name,"") != 0)
        InsertNewElementForUpdate(TYPE_VARCHAR, name, "AK_trigger", "name", row_root, 1);
    if (event != NULL && strcmp(event, "") != 0)
        InsertNewElementForUpdate(TYPE_VARCHAR, event, "AK_trigger", "event", row_root, 0);
    if (condition != NULL && strcmp(condition, "") != 0)
        InsertNewElementForUpdate(TYPE_VARCHAR, condition, "AK_trigger", "condition", row_root, 0);
    if (table != NULL && strcmp(table, "") != 0) {
        id = -1;
        i = 0;
        while ((row = AK_get_row(i,"AK_relation")) != NULL ) {
            if (strcmp(row->next->next->data, table) == 0) {
                memcpy(&id, row->next->data, sizeof(int));
                break;
            }
            i++;
        }

        if (id == -1) {
            if (DEBUG)
                printf("AK_trigger_edit: Could not update trigger. Table does not exist.\n");
            return EXIT_ERROR;
        }
        InsertNewElementForUpdate(TYPE_INT, &id, "AK_trigger", "on", row_root, 1);
    }
    if (function != NULL && strcmp(function, "") != 0) {
        i = 0;
        id = -1;
        while ((row = AK_get_row(i, "AK_function")) != NULL) {
            if (strcmp(row->next->next->data, function) == 0) {
                memcpy(&id, row->next->data, sizeof(int));
                break;
            }
            i++;
        }

        if (id == -1) {
            if (DEBUG)
                printf("AK_trigger_edit: Could not update trigger. Function does not exist.\n");
            return EXIT_ERROR;
        }
        InsertNewElementForUpdate(TYPE_INT, &id, "AK_trigger", "action", row_root, 0);
    }

    update_row(row_root);
    free(row_root);

    return EXIT_SUCCESS;
}


void trigger_test() {
    element row_root =  (element) malloc( sizeof(list) );
    InitializeList(row_root);

    int i = AK_get_id();
    InsertNewElement( TYPE_INT, &i, "AK_function", "obj_id", row_root );
    InsertNewElement( TYPE_VARCHAR, "dummy_funk_1", "AK_function", "name", row_root );
    i = 0;
    InsertNewElement( TYPE_INT, &i, "AK_function", "arg_num", row_root );
    InsertNewElement( TYPE_INT, i, "AK_function", "return_type", row_root );
    insert_row( row_root );

    i = AK_get_id();
    InsertNewElement( TYPE_INT, &i, "AK_function", "obj_id", row_root );
    InsertNewElement( TYPE_VARCHAR, "dummy_funk_2", "AK_function", "name", row_root );
    i = 0;
    InsertNewElement( TYPE_INT, &i, "AK_function", "arg_num", row_root );
    InsertNewElement( TYPE_INT, i, "AK_function", "return_type", row_root );
    insert_row( row_root );

    AK_print_table("AK_function");

    AK_trigger_add("trigg1", "insert", "a > 1", "AK_reference", "dummy_funk_1");
    AK_trigger_add("trigg2", "update", "a < 2", "dummy_table", "dummy_funk_1");
    AK_trigger_add("trigg3", "delete", "", "AK_reference", "dummy_funk");
    //(int *obj_id, char *name, char* event, char* condition, char* table, char* function)
    AK_trigger_edit(NULL, "trigg1", "drop", NULL, "AK_reference", "dummy_funk_2");
    AK_trigger_edit(NULL, NULL, "drop", NULL, "AK_reference", "dummy_funk_2");
    AK_trigger_edit(NULL, "trigg1", NULL, NULL, "AK_reference", "dummy_funk");

    AK_print_table("AK_trigger");
}