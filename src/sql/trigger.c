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
 * @author Unknown, updated by Mario Peroković, check if data is TYPE_INT
 * @brief Saves conditions for a trigger.
 * @param trigger obj_id of the trigger in question
 * @param *condition AK_list list of conditions
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_trigger_save_conditions(int trigger, struct list_node *condition) {
    int i = 0;
    char tempData[MAX_VARCHAR_LENGTH];
    AK_PRO;

    struct list_node *temp = Ak_First_L2(condition);
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);

    Ak_Insert_New_Element_For_Update(TYPE_INT, &trigger, "AK_trigger_conditions", "trigger", row_root, SEARCH_CONSTRAINT);
    if (Ak_delete_row(row_root) == EXIT_ERROR){
	AK_EPI;
        return EXIT_ERROR;
    }

    Ak_DeleteAll_L3(&row_root);
    while (temp != NULL) {
        memcpy(tempData, temp->data, temp->size);
        tempData[temp->size] = '\0';
        Ak_Insert_New_Element(TYPE_INT, &trigger, "AK_trigger_conditions", "trigger", row_root);
        Ak_Insert_New_Element(TYPE_INT, &i, "AK_trigger_conditions", "id", row_root);
        if(temp->type == TYPE_INT){
            Ak_Insert_New_Element(TYPE_INT, &temp->data, "AK_trigger_conditions", "data", row_root);
        } else{
            Ak_Insert_New_Element(TYPE_VARCHAR, tempData, "AK_trigger_conditions", "data", row_root);
        }
        Ak_Insert_New_Element(TYPE_INT, &temp->type, "AK_trigger_conditions", "type", row_root);
        if (Ak_insert_row(row_root) == EXIT_ERROR) {
            AK_free(row_root);
	    AK_EPI;
            return EXIT_ERROR;
        }

	temp = Ak_Next_L2(temp);
        i++;
    }

    AK_free(row_root);
    AK_EPI;
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
int AK_trigger_add(char *name, char* event, struct list_node *condition, char* table, char* function) {
    int funk_id = -1, table_id = -1, trigg_id;
    AK_PRO;
    table_id = AK_get_table_obj_id(table);

    if (table_id == EXIT_ERROR) {
        Ak_dbg_messg(HIGH, TRIGGERS, "AK_trigger_add: No such table upon which to create a trigger.\n");
	AK_EPI;
        return EXIT_ERROR;
    }

    funk_id = AK_get_function_obj_id(function, NULL);

    if (funk_id == EXIT_ERROR) {
        Ak_dbg_messg(HIGH, TRIGGERS, "AK_trigger_add: No such function to execute upon activation of trigger.\n");
	AK_EPI;
        return EXIT_ERROR;
    }

    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);

    trigg_id = AK_get_id();
    Ak_Insert_New_Element(TYPE_INT, &trigg_id, "AK_trigger", "obj_id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, name, "AK_trigger", "name", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, event, "AK_trigger", "event", row_root);
    if (condition == NULL || Ak_IsEmpty_L2(condition) == 1) 
        Ak_Insert_New_Element(0, "", "AK_trigger", "condition", row_root);
    else
        Ak_Insert_New_Element(TYPE_VARCHAR, "T", "AK_trigger", "condition", row_root);
    Ak_Insert_New_Element(TYPE_INT, &funk_id, "AK_trigger", "action", row_root);
    Ak_Insert_New_Element(TYPE_INT, &table_id, "AK_trigger", "on", row_root);
    Ak_insert_row(row_root);

    AK_free(row_root);

    if (condition != NULL && Ak_IsEmpty_L2(condition) == 0)
        AK_trigger_save_conditions(trigg_id, condition);
    AK_EPI;
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
    
    struct list_node *row;
    AK_PRO;
    table_id = AK_get_table_obj_id(table);
    if (table_id == EXIT_ERROR){
	AK_EPI;
        return EXIT_ERROR;
    }

    //while ((row = (AK_list *)AK_get_row(i, "AK_trigger")) != NULL) {
    while ((row = (struct list_node *)AK_get_row(i, "AK_trigger")) != NULL) {
        struct list_node *name_elem = Ak_GetNth_L2(2,row);
        struct list_node *table_elem = Ak_GetNth_L2(5,row);
        if (strcmp(name_elem->data, name) == 0 && table_id == (int) * table_elem->data) {
            i = (int) * row->next->data;
            AK_free(row);
	    AK_EPI;
            return i;
        }
        i++;
    }

    AK_free(row);
    AK_EPI;
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
    AK_PRO;
    int trigg_id = AK_trigger_get_id(name, table);
    /*
    AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L3(&row_root);
    */

    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);
    
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, name, "AK_trigger", "name", row_root, SEARCH_CONSTRAINT);

    int result = Ak_delete_row(row_root);

    if (result == EXIT_ERROR) {
        Ak_dbg_messg(HIGH, TRIGGERS, "AK_trigger_remove_by_name: Could not delete trigger.\n");
	AK_EPI;
        return EXIT_ERROR;
    }

    // the following can be avoided if foreign key is declared...
    
    Ak_DeleteAll_L3(&row_root);
    Ak_Insert_New_Element_For_Update(TYPE_INT, &trigg_id, "AK_trigger_conditions", "trigger", row_root, SEARCH_CONSTRAINT);
    AK_EPI;
    return Ak_delete_row(row_root);
}

/**
 * @author Unknown
 * @brief Function removes a trigger by its obj_id
 * @param obj_id obj_id of the trigger
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_trigger_remove_by_obj_id(int obj_id) {
    AK_PRO;
    /*
    AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L3(&row_root);
    */

    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);

    Ak_Insert_New_Element_For_Update(TYPE_INT, &obj_id, "AK_trigger", "obj_id", row_root, SEARCH_CONSTRAINT);

    int result = Ak_delete_row(row_root);

    if (result == EXIT_ERROR) {
        Ak_dbg_messg(HIGH, TRIGGERS, "AK_trigger_remove_by_name: Could not delete trigger.\n");
	AK_EPI;
        return EXIT_ERROR;
    }

    // the following can be avoided if foreign key is declared...
    
    Ak_DeleteAll_L3(&row_root);
    Ak_Insert_New_Element_For_Update(TYPE_INT, &obj_id, "AK_trigger_conditions", "trigger", row_root, SEARCH_CONSTRAINT);
    AK_EPI;
    return Ak_delete_row(row_root);
}


/**
 * @author Unknown
 * @brief Function edits information about the trigger in system table. In order to identify the trigger,
 *	  either obj_id or table and name parameters should be defined. The other options should be set to NULL.
 *   	  Values of parameters that aren't changing can be left NULL. If conditions are to be removed,
 * 	  condition parameter should hold an empty list.
 * @param *name name of the trigger (or NULL if using obj_id)
 * @param *event event of the trigger (or NULL if it isn't changing)
 * @param *condition list of conditions for trigger (or NULL if it isn't changing; empty list if all conditions are to be removed)
 * @param *table name of the connected table (or NULL id using obj_id)
 * @param *function name of the connected function (or NULL if it isn't changing)
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
//int AK_trigger_edit(char *name, char* event, AK_list* condition, char* table, char* function) {
int AK_trigger_edit(char *name, char* event, struct list_node *condition, char* table, char* function) {  
    AK_PRO;
    if (name == NULL || table == NULL) {
        Ak_dbg_messg(HIGH, TRIGGERS, "AK_trigger_edit: Not enough data to identify the trigger.\n");
	AK_EPI;
        return EXIT_ERROR;
    }
    int table_id = AK_get_table_obj_id(table);
    int trigger_id = AK_trigger_get_id(name, table);
    int function_id = AK_get_function_obj_id(function, NULL);
    if (function_id == EXIT_ERROR) {
        Ak_dbg_messg(HIGH, TRIGGERS, "AK_trigger_edit: Could not update trigger. Function does not exist.\n");
	AK_EPI;
        return EXIT_ERROR;
    }

    if(table_id == -1 || trigger_id == -1){
        Ak_dbg_messg(HIGH, TRIGGERS, "AK_trigger_edit: Could not update trigger. Table or trigger does not exist.\n");
	AK_EPI;
        return EXIT_ERROR;
    }

    /*
    AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L3(&row_root);
    */

    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);

    Ak_Insert_New_Element_For_Update(TYPE_INT, &trigger_id, "AK_trigger", "obj_id", row_root, SEARCH_CONSTRAINT);

    if (event != NULL)
        Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, event, "AK_trigger", "event", row_root, NEW_VALUE);
    if (condition != NULL){
        Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "T", "AK_trigger", "condition", row_root, NEW_VALUE);
        AK_trigger_save_conditions(trigger_id, condition);
    }
    else
        Ak_Insert_New_Element_For_Update(0, "", "AK_trigger", "condition", row_root, NEW_VALUE);
    if (function != NULL)
        Ak_Insert_New_Element_For_Update(TYPE_INT, &function_id, "AK_trigger", "action", row_root, NEW_VALUE);

    int result = Ak_update_row(row_root);
    AK_free(row_root);

    if (result == EXIT_ERROR) {
        Ak_dbg_messg(HIGH, TRIGGERS, "AK_trigger_edit: Could not update trigger.\n");
	AK_EPI;
        return EXIT_ERROR;
    }
    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Unknown, updated by Mario Peroković
 * @brief Function gets postfix list of conditions for the trigger (compatible with selection)
 * @param trigger obj_id of the trigger
 * @return list of conditions for the trigger
 */
//AK_list *AK_trigger_get_conditions(int trigger) {
struct list_node *AK_trigger_get_conditions(int trigger) {
    char *endPtr;
    AK_PRO;
    printf("\nid triggera: %d\n", trigger);

    struct list_node *expr = (struct list_node *) AK_malloc(sizeof(struct list_node));
    Ak_Init_L3(&expr);
    Ak_InsertAtEnd_L3(TYPE_ATTRIBS, "trigger", strlen("trigger"), expr);
    Ak_InsertAtEnd_L3(TYPE_INT, (char*)(&trigger), sizeof (int), expr);
    Ak_InsertAtEnd_L3(TYPE_OPERATOR, "=", 1, expr);
    
    AK_selection("AK_trigger_conditions", "AK_trigger_conditions_temp", expr);
    /*
    AK_list *result = AK_malloc(sizeof(AK_list));
    Ak_Init_L3(&result);
    */
    
    struct list_node *result = (struct list_node *) AK_malloc(sizeof(struct list_node));
    Ak_Init_L3(&result);
    
    int i = 0;
//    AK_list *row;
    struct list_node *row;
    //while((row = (AK_list *)AK_get_row(i, "AK_trigger_conditions_temp")) != NULL){
    while((row = (struct list_node *)AK_get_row(i, "AK_trigger_conditions_temp")) != NULL){
        struct list_node *first_arg_elem = Ak_GetNth_L2(4,row);
        struct list_node *second_arg_elem = Ak_GetNth_L2(3,row);
        Ak_InsertAtEnd_L3(strtol(first_arg_elem->data, &endPtr, 10), second_arg_elem->data, second_arg_elem->size, result);
        i++;
    }

    AK_delete_segment("AK_trigger_conditions_temp", SEGMENT_TYPE_TABLE);
    AK_free(row);
    AK_EPI;
    return result;
}

/**
* @author Ljubo Barać
* @brief Function renames the trigger
* @param old_name Name of the trigger to be renamed
* @param new_name New name of the trigger
* @return EXIT_SUCCESS or EXIT_ERROR
*/
int AK_trigger_rename(char *old_name, char *new_name, char *table){
	AK_PRO;
	printf("\n***Rename trigger***\n");

	int trig_id = AK_trigger_get_id(old_name, table);

	/*
	AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
	Ak_Init_L3(&row_root);
	*/

	struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
	Ak_Init_L3(&row_root);


	Ak_Insert_New_Element_For_Update(TYPE_INT, &trig_id, "AK_trigger", "obj_id", row_root, 1);
	Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, new_name, "AK_trigger", "name", row_root, 0);

	int result =  Ak_update_row(row_root);
	
	Ak_DeleteAll_L3(&row_root);
	AK_free(row_root);

	if (result == EXIT_ERROR) {
	   Ak_dbg_messg(HIGH, SEQUENCES, "AK_trigger_rename: Could not rename trigger.\n");
	   AK_EPI;
	   return EXIT_ERROR;
	   }
	AK_EPI;
	return EXIT_SUCCESS;
}



/**
 * @author Unknown
 * @brief Function for trigger testing
 */
void AK_trigger_test() {
    AK_PRO;

    printf("trigger.c: Present!\n");
   

    struct list_node *arguments_list1 = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&arguments_list1);

  
    
    Ak_InsertAtEnd_L3(TYPE_VARCHAR, "argument1", sizeof ("argument1"), arguments_list1);
    Ak_InsertAtEnd_L3(TYPE_INT, "5", sizeof (int), arguments_list1);
    Ak_InsertAtEnd_L3(TYPE_VARCHAR, "argument2", sizeof ("argument1"), arguments_list1);
    Ak_InsertAtEnd_L3(TYPE_INT, "3", sizeof (int), arguments_list1);
    
    AK_function_add("dummy_funk_1", 1, arguments_list1);
    
    Ak_DeleteAll_L3(&arguments_list1);
    Ak_InsertAtEnd_L3(TYPE_VARCHAR, "argument7", sizeof ("argument1"), arguments_list1);
    Ak_InsertAtEnd_L3(TYPE_INT, "5", sizeof (int), arguments_list1);
    AK_function_add("dummy_funk_2", 1, arguments_list1);

    
    Ak_DeleteAll_L3(&arguments_list1);
    AK_free(arguments_list1);
    AK_print_table("AK_function");
    
    
    struct list_node *expr = (struct list_node *) AK_malloc(sizeof (struct list_node));
    struct list_node *dummyExpression = (struct list_node *) AK_malloc(sizeof(struct list_node));
    strcpy(dummyExpression->data, "");
    
    Ak_Init_L3(&expr);
    char *num = "2002";
   
    Ak_InsertAtEnd_L3(TYPE_ATTRIBS, "year", 4, expr);
    Ak_InsertAtEnd_L3(TYPE_INT, num, 4, expr);
    Ak_InsertAtEnd_L3(TYPE_OPERATOR, ">", 2, expr);
    Ak_InsertAtEnd_L3(TYPE_ATTRIBS, "firstname", 9, expr);
    Ak_InsertAtEnd_L3(TYPE_VARCHAR, "Matija", 6, expr);
    Ak_InsertAtEnd_L3(TYPE_OPERATOR, "=", 2, expr);
    Ak_InsertAtEnd_L3(TYPE_OPERATOR, "OR", 2, expr);

    AK_trigger_add("trigg1", "insert", expr, "AK_reference", "dummy_funk_1");
    AK_trigger_add("trigg2", "update", expr, "dummy_table", "dummy_funk_1");
    AK_trigger_add("trigg3", "delete", dummyExpression, "AK_reference", "dummy_funk");
    AK_trigger_add("trigg4", "insert", NULL, "AK_reference", "dummy_funk_2");
    AK_print_table("AK_trigger");
    AK_print_table("AK_trigger_conditions");

    printf("\n\nAK_trigger_get_conditions test!\n");
    AK_trigger_get_conditions(109);

    Ak_DeleteAll_L3(&expr);

    Ak_InsertAtEnd_L3(TYPE_ATTRIBS, "not null", 8, expr);

    printf("\n\nupdate trigger\n");


    //look for 'trigg1' on table 'AK_reference' and change it's event to drop and it's function to 'dummy_funk_1'
    AK_trigger_edit("trigg1", "drop", expr, "AK_reference", "dummy_funk_2");

    AK_print_table("AK_trigger");
    AK_print_table("AK_trigger_conditions");

    AK_trigger_remove_by_name("trigg1", "AK_reference");
    AK_print_table("AK_trigger");
    AK_print_table("AK_trigger_conditions");

    AK_free(dummyExpression);
    AK_free(expr);
    AK_EPI;
}
