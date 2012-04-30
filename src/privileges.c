/**
@file privileges.c Provides functions for privileges
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
#include "privileges.h"

/**
 * @author Kristina Takač.
 * @brief  Function which inserts user in table AK_user
 * @param *username username of user to be added
 * @param password password of user to be added
 * @return user_id
 */
int AK_user_add(char *username, int password){	
    int user_id;

    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);
	
    user_id = AK_get_id();
    Ak_Insert_New_Element(TYPE_INT, &user_id, "AK_user", "obj_id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, username, "AK_user", "username", row_root);
    Ak_Insert_New_Element(TYPE_INT, &password, "AK_user", "password", row_root);
 
    Ak_insert_row(row_root);

    free(row_root);
    AK_archive_log("AK_user_add", username); //ARCHIVE_LOG
    return user_id;
}

/**
 * @author Kristina Takač.
 * @brief Function that inserts group in table AK_group
 * @param *name name of group to be added
 * @return id of group                                                      
*/
int AK_group_add(char *name){	
   
    int group_id;

    
    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);
	
    group_id = AK_get_id();
    Ak_Insert_New_Element(TYPE_INT, &group_id, "AK_group", "obj_id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, name, "AK_group", "name", row_root);
    
 
    Ak_insert_row(row_root);

    free(row_root);
    AK_archive_log("AK_group_add", name); //ARCHIVE_LOG
    return group_id;
}
/**
 * @author Kristina Takač.
 * @brief Function that gets id of group with given name
 * @param *name name of group whose id we are looking for
 * @return id of group, otherwise EXIT_ERROR                                                      
*/
int AK_group_get_id(char *name) {
    int i = 0;
    AK_list *row;
    while ((row = (AK_list *)AK_get_row(i, "AK_group")) != NULL) {
        if (strcmp(row->next->next->data, name) == 0) {
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
 * @author Kristina Takač.
 * @brief Function that inserts privilege in table AK_right
 * @param *username username of user to whom we want to grant privilege
 * @param *group name of group which user belongs to
 * @param *table name of table on which privilege will be granted to user
 * @param *right type of privilege which will be granted to user on given table
 * @return privilege_id                                                     
*/
int AK_privilege_add(char *username, char *group, char *table, char *right){
	int privilege_id, group_id, table_id, right_type;
	group_id = AK_group_get_id(group);
	table_id = AK_get_table_obj_id(table);
	

	privilege_id = AK_get_id();
	
	if(strcmp(right, "ALL")==0){
		int rights[4] = {0, 1, 2, 3};
		int i;
		for(i=0; i< 4; i++){
			AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
			Ak_Init_L(row_root);
		
			

		        Ak_Insert_New_Element(TYPE_INT, &privilege_id, "AK_right", "obj_id", row_root);
			Ak_Insert_New_Element(TYPE_VARCHAR, username, "AK_right", "name", row_root);
			Ak_Insert_New_Element(TYPE_INT, &group_id, "AK_right", "group_id", row_root);
			Ak_Insert_New_Element(TYPE_INT, &table_id, "AK_right", "artifact_id", row_root);
			Ak_Insert_New_Element(TYPE_INT, &rights[i], "AK_right", "right_type", row_root);
			Ak_insert_row(row_root);

			free(row_root);
		}

	}else{
		if(strcmp(right, "UPDATE")==0){
			right_type = UPDATE;
		}
		if(strcmp(right, "DELETE")==0){
			right_type = DELETE;
		}
		if(strcmp(right, "INSERT")==0){
			right_type = INSERT;
		}
		if(strcmp(right, "SELECT")==0){
			right_type = SELECT;
		}
    		AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
		Ak_Init_L(row_root);
		
			

		Ak_Insert_New_Element(TYPE_INT, &privilege_id, "AK_right", "obj_id", row_root);
	        Ak_Insert_New_Element(TYPE_VARCHAR, username, "AK_right", "name", row_root);
		Ak_Insert_New_Element(TYPE_INT, &group_id, "AK_right", "group_id", row_root);
		Ak_Insert_New_Element(TYPE_INT, &table_id, "AK_right", "artifact_id", row_root);
		Ak_Insert_New_Element(TYPE_INT, &right_type, "AK_right", "right_type", row_root);
		Ak_insert_row(row_root);
		free(row_root);
	}
		
    		AK_archive_log("AK_privilege_add", username, group, table, right); //ARCHIVE_LOG
		return privilege_id;
}

void AK_revoke_privilege(char *username, char *table, char *right){
	int table_id;
	if(strcmp(right, "ALL")==0){
		AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    	        Ak_Init_L(row_root);
		int i = 0;
                AK_list *row;
                while ((row = (AK_list *)AK_get_row(i, "AK_right")) != NULL) {
                    if (strcmp(row->next->next->data, username) == 0) {
	
                        Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, username, "AK_right", "name", row_root, 1);
	                Ak_delete_row(row_root);
	            }
                    i++;
	           Ak_DeleteAll_L(row_root);
	           free(row);
                }

                
       
        }else{
	    table_id = AK_get_table_obj_id(table);
	    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    	    Ak_Init_L(row_root);
	    int i = 0;
            AK_list *row; 
            int right_type;
  	    if(strcmp(right, "UPDATE")==0){
			right_type = UPDATE;
		}
		if(strcmp(right, "DELETE")==0){
			right_type = DELETE;
		}
		if(strcmp(right, "INSERT")==0){
			right_type = INSERT;
		}
		if(strcmp(right, "SELECT")==0){
			right_type = SELECT;
		}
            while ((row = (AK_list *)AK_get_row(i, "AK_right")) != NULL) {
                    if ((strcmp(row->next->next->data, username) == 0) && (table_id ==  (int) * row->next->next->next->next->data) && 
			(right_type ==  (int) * row->next->next->next->next->next->data)) {
                        Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, username, "AK_right", "name", row_root, 1);
	                Ak_delete_row(row_root);
	            }
                    i++;
	           Ak_DeleteAll_L(row_root);
	           free(row);
                }

        }
		             
}       


void AK_privileges_test(){
    AK_user_add("proba", 123);
    AK_user_add("kritakac", 321);

    AK_print_table("AK_user");
    
    
    AK_group_add("grupa1");
    AK_group_add("grupa2");
    AK_print_table("AK_group");
    AK_privilege_add("proba", "grupa2", "student1", "ALL");
    AK_privilege_add("kritakac", "grupa1", "student1", "DELETE");
    
    AK_print_table("AK_right");
    AK_revoke_privilege("proba", "student1", "ALL");
    AK_print_table("AK_right");
   
   
}
