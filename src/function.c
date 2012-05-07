/**
@file function.c Provides functions for functions
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
 * 
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include "function.h"

/**
 * @author Unknown
 * @brief Function that gets obj_id of a function by name (transferred from trigger.c).
 * @param *function name of the function
 * @return obj_id of the function or EXIT_ERROR
 */
int AK_get_function_obj_id(char* function) {
    int i = 0;
    int id = -1;
    AK_list *row;
    while ((row = (AK_list *)AK_get_row(i, "AK_function")) != NULL) {
        if (strcmp(row->next->next->data, function) == 0) {
            memcpy(&id, row->next->data, sizeof (int));
            break;
        }
        i++;
    }
    if (id == -1)
        return EXIT_ERROR;
    return id;
}

/**
 * @author Boris Kišić
 * @brief Function that adds a function to system table.
 * @param *name name of the function
 * @param *return_type data type returned from a function - values from 0 to 13 - defined in configuration.h
 * @param *arguments_list list of function arguments
 * @return function id or EXIT_ERROR
 */
int AK_function_add(char *name, int return_type, AK_list *arguments_list) {
    if (return_type<0 || return_type>13){
	Ak_dbg_messg(HIGH, FUNCTIONS, "AK_function_add: Invalid parameter value for return type.\n");
        return EXIT_ERROR;
    }
    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);
    int function_id = AK_get_id(); //get function_id
    int num_args = Ak_Size_L(arguments_list)/2; //get number of arguments
    Ak_Insert_New_Element(TYPE_INT, &function_id, "AK_function", "obj_id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, name, "AK_function", "name", row_root);
    Ak_Insert_New_Element(TYPE_INT, &num_args, "AK_function", "arg_num", row_root);
    Ak_Insert_New_Element(TYPE_INT, &return_type, "AK_function", "return_type", row_root);
    Ak_insert_row(row_root);
    
    int i=1;
    AK_list_elem current_elem = Ak_First_L(arguments_list); //set current_elem to first element in a list
    //add arguments list to arguments system table
    for(i; i<=num_args; i++){
      char argname = Ak_Retrieve_L(current_elem, arguments_list); // to do: adding argname to system catalog
      current_elem = Ak_Next_L(current_elem);
      char* argtype = Ak_Retrieve_L(current_elem, arguments_list);
      int argtype_int = atoi(argtype);
      current_elem = Ak_Next_L(current_elem);
      AK_function_arguments_add (function_id, i, argtype_int);
    }
    
    return function_id;
}

/**
 * @author Boris Kišić
 * @brief Function that adds a function argument to system table.
 * @param *function_id id of the function to which the argument belongs
 * @param *arg_number number of the argument
 * @param *arg_type data type of the argument
 * @return function argument id or EXIT_ERROR
 */
int AK_function_arguments_add(int function_id, int arg_number, int arg_type){
    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &function_id, "AK_function_arguments", "func_id", row_root);
    Ak_Insert_New_Element(TYPE_INT, &arg_number, "AK_function_arguments", "att_num", row_root);
    Ak_Insert_New_Element(TYPE_INT, &arg_type, "AK_function_arguments", "att_type", row_root);
    Ak_insert_row(row_root);
    
    return function_id;
}


/**
 * @author Boris Kišić
 * @brief Function removes a function and its arguments by its obj_id.
 * @param obj_id obj_id of the function
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_function_remove_by_obj_id(int obj_id) {
    printf("***Remove function***\n");
    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);

    Ak_Insert_New_Element_For_Update(TYPE_INT, &obj_id, "AK_function", "obj_id", row_root, 1);
    int result = Ak_delete_row(row_root);

    if (result == EXIT_ERROR) {
        Ak_dbg_messg(HIGH, FUNCTIONS, "AK_function_remove_by_obj_id: Could not delete function.\n");
        return EXIT_ERROR;
    }
    
    //delete function arguments
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element_For_Update(TYPE_INT, &obj_id, "AK_function_arguments", "func_id", row_root, 1);
    Ak_delete_row(row_root);
    return EXIT_SUCCESS;
}


/**
 * @author Boris Kišić
 * @brief Function that removes a function from system table by name.
 * @param *name name of the function
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_function_remove_by_name(char *name) {
    int func_id = AK_get_function_obj_id(name);
    int result = AK_function_remove_by_obj_id(func_id);

    if (result == EXIT_ERROR) {
        Ak_dbg_messg(HIGH, FUNCTIONS, "AK_function_remove_by_name: Could not delete function.\n");
        return EXIT_ERROR;
    }
   
    return EXIT_SUCCESS;
}

/**
 * @author Boris Kišić
 * @brief Function for functions testing.
 * @return No return value
 */
void AK_function_test() {
    printf("function.c: Present!\n");
    
    AK_list *arguments_list1 = (AK_list *) malloc(sizeof (AK_list));
    Ak_Init_L(arguments_list1);
    Ak_InsertAtEnd_L(TYPE_VARCHAR, "argument1", sizeof ("argument1"), arguments_list1);
    Ak_InsertAtEnd_L(TYPE_INT, "5", sizeof (int), arguments_list1);
    Ak_InsertAtEnd_L(TYPE_VARCHAR, "argument2", sizeof ("argument1"), arguments_list1);
    Ak_InsertAtEnd_L(TYPE_INT, "3", sizeof (int), arguments_list1);
    AK_function_add("test_funkcija", 1, arguments_list1);
    AK_function_add("test_funkcija2", 3, arguments_list1);
    AK_print_table("AK_function");
    AK_print_table("AK_function_arguments");
    Ak_DeleteAll_L(arguments_list1);
    free(arguments_list1);
    //int id=109;
    //AK_function_remove_by_obj_id(id);
    AK_function_remove_by_name("test_funkcija2");
    AK_print_table("AK_function");
    AK_print_table("AK_function_arguments");
}


