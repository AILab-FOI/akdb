/**
@file sequence.c Provides functions for sequences
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

#include "sequence.h"

/**
 * @author Boris Kišić
 * @brief Function for adding sequence.
 * @param name name of the sequence
 * @param start_value start value of the sequence
 * @param increment increment of the sequence
 * @param max_value maximium value of the sequence
 * @param min_value minimum value of the sequence
 * @param cycle 0:non-cyclic sequence, 1:cyclic sequence
 * @return sequence_id or EXIT_ERROR
 */
int AK_sequence_add(char *name, int start_value, int increment, int max_value, int min_value, int cycle){
    AK_PRO;
    printf("\n***Add sequence***");
   /*
    AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L3(&row_root);
    */
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof(struct list_node));
    Ak_Init_L3(&row_root);
    
    int sequence_id = AK_get_id(); //get sequence_id
    Ak_Insert_New_Element(TYPE_INT, &sequence_id, "AK_sequence", "obj_id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, name, "AK_sequence", "name", row_root);
    Ak_Insert_New_Element(TYPE_INT, &start_value, "AK_sequence", "current_value", row_root);
    Ak_Insert_New_Element(TYPE_INT, &increment, "AK_sequence", "increment", row_root);
    Ak_Insert_New_Element(TYPE_INT, &max_value, "AK_sequence", "max", row_root);
    Ak_Insert_New_Element(TYPE_INT, &min_value, "AK_sequence", "min", row_root);
    Ak_Insert_New_Element(TYPE_INT, &cycle, "AK_sequence", "cycle", row_root);
    
    int result = Ak_insert_row(row_root); 
    
    if (result == EXIT_ERROR || sequence_id == EXIT_ERROR) {
      Ak_dbg_messg(HIGH, SEQUENCES, "AK_sequence_add: Could not add sequence.\n");
      AK_EPI;
      return EXIT_ERROR;
    }
    
    Ak_DeleteAll_L3(&row_root);
    
    AK_free(row_root);
    AK_EPI;
    return sequence_id;
}

/**
 * @author Boris Kišić
 * @brief Function for removing sequence.
 * @param name name of the sequence
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_sequence_remove(char *name){
    AK_PRO;
    printf("\n***Remove sequence***");
   /* AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L3(&row_root);
    */
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof(struct list_node));
    Ak_Init_L3(&row_root);
    
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, name, "AK_sequence", "name", row_root, 1);
    int result =  Ak_delete_row(row_root); 
   
    if (result == EXIT_ERROR) {
      Ak_dbg_messg(HIGH, SEQUENCES, "AK_sequence_delete: Could not delete sequence.\n");
      AK_EPI;
      return EXIT_ERROR;
    }
    
    Ak_DeleteAll_L3(&row_root);
    AK_free(row_root);
    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Boris Kišić
 * @brief Function returns the current value of the sequence.
 * @param name name of the sequence
 * @return current_value or EXIT_ERROR
 */
int AK_sequence_current_value(char *name){
    int i = 0;
    int current_value = -1;
    int num_rows = AK_get_num_records("AK_sequence");
    
    struct list_node *row;
    AK_PRO;

    
    //while ((row = (AK_list *)AK_get_row(i, "AK_sequence")) != NULL){
    while ((row = (struct list_node *)AK_get_row(i, "AK_sequence")) != NULL){
        if (strcmp(get_row_attr_data(1,row), name) == 0) {
            memcpy(&current_value, get_row_attr_data(2,row), sizeof (int));
	    break;
        }
        i++;
    }
    
    if (current_value == -1){
	AK_EPI;
        return EXIT_ERROR;
    }
    AK_EPI;
    return current_value;
}

/**
 * @author Boris Kišić
 * @brief Function returns the next value of the sequence and writes it in a system table as current value.
 * @param name name of the sequence
 * @return next_value or EXIT_ERROR
 */
int AK_sequence_next_value(char *name){
    int next_value ;
    int i = 0;
    int obj_id;
    int current_value = -1;
    int increment;
    int max_value;
    int min_value;
    int cycle;
    
    struct list_node *row;
    AK_PRO;
    //while ((row = (AK_list *)AK_get_row(i, "AK_sequence")) != NULL){
    while ((row = (struct list_node *)AK_get_row(i, "AK_sequence")) != NULL){
        if(strcmp( get_row_attr_data(1,row) ,name) == 0) {
        memcpy(&obj_id, get_row_attr_data(0,row), sizeof (int));
        memcpy(&current_value, get_row_attr_data(2,row), sizeof (int));
        memcpy(&increment, get_row_attr_data(3,row), sizeof (int));
        memcpy(&max_value, get_row_attr_data(4,row), sizeof (int));
        memcpy(&min_value, get_row_attr_data(5,row), sizeof (int));
        memcpy(&cycle, get_row_attr_data(6,row), sizeof (int));
        break;
        }
        i++;
    }
   

    if (current_value == -1){
    AK_EPI;
        return EXIT_ERROR;
    }
 
    next_value = current_value + increment;
    
    if(next_value < min_value){
    if(cycle==0){
        printf("\nNon-cyclic sequence can not go below its minimum value.");
            AK_EPI;
        return EXIT_ERROR;
    }
    else{
        next_value=max_value;
    } 
    }
    
    if(next_value > max_value){
    if(cycle==0){
        printf("\nNon-cyclic sequence can not go over its maximum value.");
        AK_EPI;
        return EXIT_ERROR;
    }
    else{
        next_value=min_value;
    } 
    }
    
 
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof(struct list_node));
    Ak_Init_L3(&row_root);
    
    Ak_Insert_New_Element_For_Update(TYPE_INT, &obj_id, "AK_sequence", "obj_id", row_root, 1);
    Ak_Insert_New_Element_For_Update(TYPE_INT, &next_value, "AK_sequence", "current_value", row_root, 0);
    int result =  Ak_update_row(row_root); 
    Ak_DeleteAll_L3(&row_root);
    AK_free(row_root);
   
   if (result == EXIT_ERROR) {
      Ak_dbg_messg(HIGH, SEQUENCES, "AK_sequence_next_value: Could not retrieve next value.\n");
      AK_EPI;
      return EXIT_ERROR;
    }
   
    AK_EPI;
    return next_value;
	//return 100;
}

/**
 * @author Ljubo Barać
 * @brief Function gets sequence id
 * @param name Name of the sequence
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_sequence_get_id(char *name){
    int i = 0;
    
    struct list_node * row;
    AK_PRO;
	
	while ((row = (struct list_node *)AK_get_row(i, "AK_sequence")) != NULL) {
		if (strcmp( get_row_attr_data(1,row) ,name) == 0) {
			i = (int) * get_row_attr_data(0,row);
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
 * @author Boris Kišić
 * @brief Function renames the sequence.
 * @param old_name Name of the sequence to be renamed
 * @param new_name New name of the sequence
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_sequence_rename(char *old_name, char *new_name){
    AK_PRO;
    printf("\n***Rename sequence***");
    int i = 0;
    
    int seq_id = AK_sequence_get_id(old_name);
    /*
    AK_list_elem row_root= (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L3(&row_root); 
*/
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);
    
    Ak_Insert_New_Element_For_Update(TYPE_INT, &seq_id, "AK_sequence", "obj_id", row_root, 1);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, new_name, "AK_sequence", "name", row_root, 0);
    int result =  Ak_update_row(row_root); 
    
    Ak_DeleteAll_L3(&row_root);
    AK_free(row_root);
    
    if (result == EXIT_ERROR || seq_id == -1) {
      Ak_dbg_messg(HIGH, SEQUENCES, "AK_sequence_rename: Could not rename sequence.\n");
      AK_EPI;
      return EXIT_ERROR;
    }
    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Boris Kišić fixed by Ljubo Barać
 * @brief Function for modifying sequence
 * @param name Name of the sequence
 * @param start_value start value of the sequence
 * @param increment increment of the sequence
 * @param max_value maximium value of the sequence
 * @param min_value minimum value of the sequence
 * @param cycle 0:non-cyclic sequence, 1:cyclic sequence
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_sequence_modify(char *name, int start_value, int increment, int max_value, int min_value, int cycle){
    AK_PRO;
    printf("\n***Edit sequence***");
    int i = 0;
    int seq_id = -1;
    
    struct list_node *row;
    //while ((row = (AK_list *)AK_get_row(i, "AK_sequence")) != NULL){
    while ((row = (struct list_node *)AK_get_row(i, "AK_sequence")) != NULL){
        if (strcmp(get_row_attr_data(1,row) ,name) == 0){
            memcpy(&seq_id, get_row_attr_data(0,row), sizeof (int));
	    break;
        }
        i++;
    }
    /*
    AK_list_elem row_root= (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L3(&row_root); 
*/
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof(struct list_node));
    Ak_Init_L3(&row_root);
    
    Ak_Insert_New_Element_For_Update(TYPE_INT, &seq_id, "AK_sequence", "obj_id", row_root, 1);
    Ak_Insert_New_Element_For_Update(TYPE_INT, &start_value, "AK_sequence", "current_value", row_root, 0);
    Ak_Insert_New_Element_For_Update(TYPE_INT, &increment, "AK_sequence", "increment", row_root, 0);
    Ak_Insert_New_Element_For_Update(TYPE_INT, &max_value, "AK_sequence", "max", row_root, 0);
    Ak_Insert_New_Element_For_Update(TYPE_INT, &min_value, "AK_sequence", "min", row_root, 0);
    Ak_Insert_New_Element_For_Update(TYPE_INT, &cycle, "AK_sequence", "cycle", row_root, 0);
    int result =  Ak_update_row(row_root); 
    
    Ak_DeleteAll_L3(&row_root);
    AK_free(row_root);
    
    if (result == EXIT_ERROR || seq_id == -1) {
      Ak_dbg_messg(HIGH, SEQUENCES, "AK_sequence_modify: Could not modify sequence.\n");
      AK_EPI;
      return EXIT_ERROR;
    }
    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Boris Kišić fixed by Ljubo Barać
 * @brief Function for sequences testing.
 * @return No return value
 */
void AK_sequence_test() {
    AK_PRO;
    printf("sequence.c: Present!\n");
    AK_sequence_add("sekvenca1", 100, 5, 200, 100, 1);     
    AK_sequence_add("sekvenca2", 200, 10, 205, 100, 1);
    AK_print_table("AK_sequence");
    int currval = AK_sequence_current_value("sekvenca1");
    printf("\nCurrent value of sequence sekvenca1: %d\n", currval);
    int nextval = AK_sequence_next_value("sekvenca1");
    printf("\nNext value of sequence sekvenca1: %d\n", nextval);
    
    AK_sequence_rename("sekvenca1", "sekvenca3");
    AK_print_table("AK_sequence"); 

    AK_sequence_modify("sekvenca3", 200, 20, 300, 150, 0);
    AK_sequence_remove("sekvenca1");
    AK_print_table("AK_sequence");
    
    printf("\n Test is successful :) \n");
    AK_EPI
}
