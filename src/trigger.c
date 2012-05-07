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

/**
 * @author Unknown
 * @brief Saves conditions for a trigger.
 * @param trigger obj_id of the trigger in question
 * @param *condition AK_list list of conditions
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_trigger_save_conditions(int trigger, AK_list* condition) {
    int i = 0;
    char tempData[MAX_VARCHAR_LENGTH];
    AK_list_elem temp = (AK_list_elem)Ak_First_L(condition);
    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);

    Ak_Insert_New_Element_For_Update(TYPE_INT, &trigger, "AK_trigger_conditions", "trigger", row_root, 1);
    if (Ak_delete_row(row_root) == EXIT_ERROR)
        return EXIT_ERROR;

    Ak_DeleteAll_L(row_root);
    while (temp != NULL) {
        memcpy(tempData, temp->data, temp->size);
        tempData[temp->size] = '\0';
        Ak_Insert_New_Element(TYPE_INT, &trigger, "AK_trigger_conditions", "trigger", row_root);
        Ak_Insert_New_Element(TYPE_INT, &i, "AK_trigger_conditions", "id", row_root);
        Ak_Insert_New_Element(TYPE_VARCHAR, tempData, "AK_trigger_conditions", "data", row_root);
        Ak_Insert_New_Element(TYPE_INT, &temp->type, "AK_trigger_conditions", "type", row_root);
        if (Ak_insert_row(row_root) == EXIT_ERROR) {
            free(row_root);
            return EXIT_ERROR;
        }
        temp = (AK_list_elem)Ak_Next_L(temp);
        i++;
    }

    free(row_root);
    return EXIT_SUCCESS;
}

/**
 * @author Unknown
 * @brief Function that adds a trigger to system table.
 * @param *name name of the trigger
 * @param *event event that calls the trigger - this should perhaps be an integer with defined constants...
 * @param *condition AK_list list of conditions in postfix
 * @param *table name of the table trigger is hooked on
 * @param *function function that is being called by the trigger
 * @return trigger id or EXIT_ERROR
 */
int AK_trigger_add(char *name, char* event, AK_list *condition, char* table, char* function) {
    int i;
    int funk_id = -1, table_id = -1, trigg_id;

    table_id = AK_get_table_obj_id(table);

    if (table_id == EXIT_ERROR) {
        Ak_dbg_messg(HIGH, TRIGGERS, "AK_trigger_add: No such table upon which to create a trigger.\n");
        return EXIT_ERROR;
    }

    funk_id = AK_get_function_obj_id(function);

    if (funk_id == EXIT_ERROR) {
        Ak_dbg_messg(HIGH, TRIGGERS, "AK_trigger_add: No such function to execute upon activation of trigger.\n");
        return EXIT_ERROR;
    }

    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);

    trigg_id = AK_get_id();
    Ak_Insert_New_Element(TYPE_INT, &trigg_id, "AK_trigger", "obj_id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, name, "AK_trigger", "name", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, event, "AK_trigger", "event", row_root);
    if (condition == NULL || Ak_IsEmpty_L(condition) == 1)
        Ak_Insert_New_Element(0, "", "AK_trigger", "condition", row_root);
    else
        Ak_Insert_New_Element(TYPE_VARCHAR, "T", "AK_trigger", "condition", row_root);
    Ak_Insert_New_Element(TYPE_INT, &funk_id, "AK_trigger", "action", row_root);
    Ak_Insert_New_Element(TYPE_INT, &table_id, "AK_trigger", "on", row_root);
    Ak_insert_row(row_root);

    free(row_root);
    
    AK_archive_log("AK_trigger_add", name, event, condition, table, function); //ARCHIVE_LOG
    
    if (condition != NULL && Ak_IsEmpty_L(condition) == 0)
        AK_trigger_save_conditions(trigg_id, condition);

    return trigg_id;
}

/**
 * @author
 * @brief Function that gets obj_id of a trigger defined by name and table.
 * @param *name name of the trigger
 * @param *table name of the table on which the trigger is hooked
 * @return obj_id of the trigger or EXIT_ERROR
 */
int AK_trigger_get_id(char *name, char *table) {
    int i = 0, table_id = -1;
    AK_list *row;

    table_id = AK_get_table_obj_id(table);
    if (table_id == EXIT_ERROR)
        return EXIT_ERROR;

    while ((row = (AK_list *)AK_get_row(i, "AK_trigger")) != NULL) {
        if (strcmp(row->next->next->data, name) == 0 && table_id == (int) * row->next->next->next->next->next->next->data) {
            i = (int) * row->next->data;
            free(row);
            return i;
        }
        i++;
    }

    free(row);
    return EXIT_ERROR;
}

/**
 * @author Unknown
 * @brief Function that removes a trigger from system table by name
 * @param *name name of the trigger
 * @param *table name of the table
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_trigger_remove_by_name(char *name, char *table) {
    int trigg_id = AK_trigger_get_id(name, table);

    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);

    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, name, "AK_trigger", "name", row_root, 1);

    int result = Ak_delete_row(row_root);

    if (result == EXIT_ERROR) {
        Ak_dbg_messg(HIGH, TRIGGERS, "AK_trigger_remove_by_name: Could not delete trigger.\n");
        return EXIT_ERROR;
    }

    // the following can be avoided if foreign key is declared...
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element_For_Update(TYPE_INT, &trigg_id, "AK_trigger_conditions", "trigger", row_root, 1);
    AK_archive_log("AK_trigger_remove_by_name", name, table); //ARCHIVE_LOG
    return Ak_delete_row(row_root);
}

/**
 * @author Unknown
 * @brief Function removes a trigger by its obj_id
 * @param obj_id obj_id of the trigger
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_trigger_remove_by_obj_id(int obj_id) {
    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);

    Ak_Insert_New_Element_For_Update(TYPE_INT, &obj_id, "AK_trigger", "obj_id", row_root, 1);

    int result = Ak_delete_row(row_root);

    if (result == EXIT_ERROR) {
        Ak_dbg_messg(HIGH, TRIGGERS, "AK_trigger_remove_by_name: Could not delete trigger.\n");
        return EXIT_ERROR;
    }

    // the following can be avoided if foreign key is declared...
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element_For_Update(TYPE_INT, &obj_id, "AK_trigger_conditions", "trigger", row_root, 1);
    return Ak_delete_row(row_root);
}


/**
 * @author Unknown
 * @brief Function edits information about the trigger in system table. In order to identify the trigger,
 *	  either obj_id or table and name parameters should be defined. The other options should be set to NULL.
 *   	  Values of parameters that aren't changing can be left NULL. If conditions are to be removed,
 * 	  condition parameter should hold an empty list. 
 * @param *obj_id obj_id of the trigger (or NULL if using name and table)
 * @param *name name of the trigger (or NULL if using obj_id)
 * @param *event event of the trigger (or NULL if it isn't changing)
 * @param *condition list of conditions for trigger (or NULL if it isn't changing; empty list if all conditions are to be removed)
 * @param *table name of the connected table (or NULL id using obj_id)
 * @param *function name of the connected function (or NULL if it isn't changing)
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_trigger_edit(int *obj_id, char *name, char* event, AK_list* condition, char* table, char* function) {
    AK_list *row;
    int id, i;

    if (obj_id == NULL && (name == NULL || table == NULL)) {
        Ak_dbg_messg(HIGH, TRIGGERS, "AK_trigger_edit: Not enough data to identify the trigger.\n");
        return EXIT_ERROR;
    }

    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);

    if (obj_id != NULL)
        Ak_Insert_New_Element_For_Update(TYPE_INT, obj_id, "AK_trigger", "obj_id", row_root, 1);
    if (name != NULL && strcmp(name, "") != 0)
        Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, name, "AK_trigger", "name", row_root, 1);
    if (event != NULL && strcmp(event, "") != 0)
        Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, event, "AK_trigger", "event", row_root, 0);
    if (condition != NULL && Ak_IsEmpty_L(condition) == 0)
        Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "T", "AK_trigger", "condition", row_root, 0);
    else
        Ak_Insert_New_Element_For_Update(0, "", "AK_trigger", "condition", row_root, 0);
    if (table != NULL && strcmp(table, "") != 0) {
        id = AK_get_table_obj_id(table);
        if (id == EXIT_ERROR) {
            Ak_dbg_messg(HIGH, TRIGGERS, "AK_trigger_edit: Could not update trigger. Table does not exist.\n");
            return EXIT_ERROR;
        }
        Ak_Insert_New_Element_For_Update(TYPE_INT, &id, "AK_trigger", "on", row_root, 1);
    }
    if (function != NULL && strcmp(function, "") != 0) {
        id = AK_get_function_obj_id(function);
        if (id == EXIT_ERROR) {
            Ak_dbg_messg(HIGH, TRIGGERS, "AK_trigger_edit: Could not update trigger. Function does not exist.\n");
            return EXIT_ERROR;
        }
        Ak_Insert_New_Element_For_Update(TYPE_INT, &id, "AK_trigger", "action", row_root, 0);
    }
    Ak_update_row(row_root);
    free(row_root);

    if (condition != NULL && Ak_IsEmpty_L(condition) == 0) {
        if (obj_id == NULL) {
            id = AK_trigger_get_id(name, table);
        } else id = obj_id;
        AK_trigger_save_conditions(id, condition);
    }

    return EXIT_SUCCESS;
}

/**
 * @author Unknown
 * @brief Function gets postfix list of conditions for the trigger (compatible with selection)
 * @param trigger obj_id of the trigger
 * @return list of conditions for the trigger
 */
AK_list *AK_trigger_get_conditions(int trigger) {
    AK_list expr;
    Ak_Init_L(&expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "trigger", strlen("trigger"), &expr);
    Ak_InsertAtEnd_L(TYPE_INT, &trigger, sizeof (int), &expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "=", 1, &expr);
    AK_selection("AK_trigger_conditions", "AK_trigger_conditions_temp", &expr);
    printf("::::::::::: %d\n", trigger);
    AK_print_table("AK_trigger_conditions_temp");

    AK_list *result = malloc(sizeof (AK_list));
    Ak_Init_L(result);
    int i = 0;
    AK_list *row;
    int a, b;
    AK_list_elem temp;
    while ((row = (AK_list *)AK_get_row(i, "AK_trigger_conditions_temp")) != NULL) {
        temp = Ak_GetNth_L(3, row);
        memcpy(&a, temp->data, sizeof (int));
        temp = Ak_GetNth_L(1, row);
        memcpy(&b, temp->data, sizeof (int));
        temp = Ak_GetNth_L(2, row);
        Ak_InsertBefore_L(a, temp->data, temp->size, Ak_GetNth_L(b, row), result);
        i++;
    }

    AK_delete_segment("AK_trigger_conditions_temp", SEGMENT_TYPE_TABLE);
    free(row);
    return result;
}
/**
 * @author Unknown
 * @brief Function for trigger testing
 * @return No return value
 */
void AK_trigger_test() {
    printf("trigger.c: Present!\n");

    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);

    int i = AK_get_id();
    Ak_Insert_New_Element(TYPE_INT, &i, "AK_function", "obj_id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "dummy_funk_1", "AK_function", "name", row_root);
    i = 0;
    Ak_Insert_New_Element(TYPE_INT, &i, "AK_function", "arg_num", row_root);
    Ak_Insert_New_Element(TYPE_INT, &i, "AK_function", "return_type", row_root);
    Ak_insert_row(row_root);

    i = AK_get_id();
    Ak_Insert_New_Element(TYPE_INT, &i, "AK_function", "obj_id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "dummy_funk_2", "AK_function", "name", row_root);
    i = 0;
    Ak_Insert_New_Element(TYPE_INT, &i, "AK_function", "arg_num", row_root);
    Ak_Insert_New_Element(TYPE_INT, &i, "AK_function", "return_type", row_root);
    Ak_insert_row(row_root);

    AK_print_table("AK_function");

    AK_list *expr = (AK_list *) malloc(sizeof (AK_list));
    Ak_Init_L(expr);
    int num = 2002;
    Ak_InsertAtEnd_L(TYPE_ATTRIBS, "year", 4, expr);
    Ak_InsertAtEnd_L(TYPE_INT, &num, sizeof (int), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, ">", 2, expr);
    Ak_InsertAtEnd_L(TYPE_ATTRIBS, "firstname", 9, expr);
    Ak_InsertAtEnd_L(TYPE_VARCHAR, "Matija", 6, expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "=", 2, expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "OR", 2, expr);

    AK_trigger_add("trigg1", "insert", expr, "AK_reference", "dummy_funk_1");
    AK_trigger_add("trigg2", "update", expr, "dummy_table", "dummy_funk_1");
    AK_trigger_add("trigg3", "delete", "", "AK_reference", "dummy_funk");
    AK_trigger_add("trigg4", "insert", NULL, "AK_reference", "dummy_funk_2");
    AK_print_table("AK_trigger");
    AK_print_table("AK_trigger_conditions");

    Ak_DeleteAll_L(expr);
    Ak_InsertAtEnd_L(TYPE_ATTRIBS, "not null", 4, expr);

    //look for 'trigg1' on table 'AK_reference' and change it's event to drop and it's function to 'dummy_funk_2'
    AK_trigger_edit(NULL, "trigg1", "drop", expr, "AK_reference", "dummy_funk_2");
    AK_trigger_edit(NULL, NULL, "drop", NULL, "AK_reference", "dummy_funk_2");
    AK_trigger_edit(NULL, "trigg1", NULL, NULL, "AK_reference", "dummy_funk");

    AK_print_table("AK_trigger");
    AK_print_table("AK_trigger_conditions");

    AK_trigger_remove_by_name("trigg1", "AK_reference");
    AK_print_table("AK_trigger");
    AK_print_table("AK_trigger_conditions");
}
