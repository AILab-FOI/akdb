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
 * You should have received a copy of the GNU General Public Licens
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
 * @brief  Function which gets user id
 * @param *username username of user whose id we are looking for
 * @return user_id, otherwise EXIT_ERROR
 */
int AK_user_get_id(char *username) {
	int i = 0;
	AK_list *row;
	while ((row = (AK_list *)AK_get_row(i, "AK_user")) != NULL) {
		if (strcmp(row->next->next->data, username) == 0) {
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
 * @author Kristina Takač, updated by Mario Peroković, inserting user id instead of username in AK_user_right
 * @brief Function that grants privilege to user
 * @param *username username of user to whom we want to grant privilege
 * @param *table name of table on which privilege will be granted to user
 * @param *right type of privilege which will be granted to user on given table
 * @return privilege_id or EXIT_ERROR if table or user aren't correct
*/
int AK_grant_privilege_user(char *username, char *table, char *right){
	int privilege_id, table_id;
	table_id = AK_get_table_obj_id(table);
	int user_id = AK_user_get_id(username);

	if(table_id == -1 || user_id == -1){
		return EXIT_ERROR;
	}
	
	if(strcmp(right, "ALL") == 0) {
		char rights[100];
		strcpy(&rights[0], "UPDATE\0");
		strcpy(&rights[10], "DELETE\0");
		strcpy(&rights[20], "INSERT\0");
		strcpy(&rights[30], "SELECT\0");
		int i;
		for(i=0; i< 4; i++) {
			AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
			Ak_Init_L(row_root);
			privilege_id = AK_get_id();

			Ak_Insert_New_Element(TYPE_INT, &privilege_id, "AK_user_right", "obj_id", row_root);
			Ak_Insert_New_Element(TYPE_INT, &user_id, "AK_user_right", "user_id", row_root);
			Ak_Insert_New_Element(TYPE_INT, &table_id, "AK_user_right", "artifact_id", row_root);
			Ak_Insert_New_Element(TYPE_VARCHAR, &rights[i*10], "AK_user_right", "right_type", row_root);
			Ak_insert_row(row_root);
			
			free(row_root);
		}
	}
	else{
		AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
		Ak_Init_L(row_root);
		
		privilege_id = AK_get_id();
		Ak_Insert_New_Element(TYPE_INT, &privilege_id, "AK_user_right", "obj_id", row_root);
		Ak_Insert_New_Element(TYPE_INT, &user_id, "AK_user_right", "user_id", row_root);
		Ak_Insert_New_Element(TYPE_INT, &table_id, "AK_user_right", "artifact_id", row_root);
		Ak_Insert_New_Element(TYPE_VARCHAR, right, "AK_user_right", "right_type", row_root);
		Ak_insert_row(row_root);
		free(row_root);
	}

	AK_archive_log("AK_grant_privilege_user", username, table, right); //ARCHIVE_LOG
	return privilege_id;
}

/**
 * @author Kristina Takač, updated by Mario Peroković - added comparing by table id, and use of user_id in AK_user_right
 * @brief Function that revokes privilege from user on given table
 * @param *username username of user to whom we want to grant privilege
 * @param *table name of table on which privilege will be revoked from user
 * @param *right type of privilege which will be revoked from user on given table
 * @return EXIT_SUCCESS if privilege is revoked, EXIT_ERROR if it isn't
*/
void AK_revoke_privilege_user(char *username, char *table, char *right){
	printf("Revokanje: %s %s %s\n", username, table, right);
	int table_id = AK_get_table_obj_id(table);
	int user_id = AK_user_get_id(username);
	int result;
	if(strcmp(right, "ALL") == 0) {
		AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
		Ak_Init_L(row_root);
		int i = 0;
		AK_list *row;
		while ((row = (AK_list *)AK_get_row(i, "AK_user_right")) != NULL) {
			if (((int) *row->next->next->data == user_id) && (table_id != -1) &&  ((int) *row->next->next->next->data == table_id)) {
				Ak_Insert_New_Element_For_Update(TYPE_INT, &user_id, "AK_user_right", "user_id", row_root, 1);
				result = Ak_delete_row(row_root);
			}
			i++;
			Ak_DeleteAll_L(row_root);
			free(row);
		}
	}
	else {
		AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
		Ak_Init_L(row_root);
		int i = 0;
		AK_list *row; 

		while ((row = (AK_list *)AK_get_row(i, "AK_user_right")) != NULL) {
			if (((int) *row->next->next->data == user_id) && (table_id ==  (int) * row->next->next->next->data) && (table_id != -1) && (strcmp(row->next->next->next->next->data,right) == 0)) {			
				int id = (int) * row->next->data;
				// strange, but it works! :)
				id = id + 1;
				Ak_Insert_New_Element_For_Update(TYPE_INT, &id, "AK_user_right", "obj_id", row_root, 1);
				result = Ak_delete_row(row_root);
			}//if
			i++;
			Ak_DeleteAll_L(row_root);
			free(row);
		}//while 
	}//else

	if (result == EXIT_ERROR) return EXIT_ERROR;

	return EXIT_SUCCESS;
}

/**
 * @author Kristina Takač.
 * @brief Function that grants privilege to given group on given table
 * @param *groupname  name of group to which we want to grant privilege
 * @param *table name of table on which privilege will be granted to user
 * @param *right type of privilege which will be granted to user on given table
 * @return privilege_id or EXIT_ERROR if table or user aren't correct
*/
int AK_grant_privilege_group(char *groupname, char *table, char *right){
	int privilege_id, table_id, group_id;
	table_id = AK_get_table_obj_id(table);
	group_id = AK_group_get_id(groupname);

	if(table_id == -1 || group_id == -1){
		return EXIT_ERROR;
	}

	if(strcmp(right, "ALL")==0) {
		char rights[100];
		strcpy(&rights[0], "UPDATE\0");
		strcpy(&rights[10], "DELETE\0");
		strcpy(&rights[20], "INSERT\0");
		strcpy(&rights[30], "SELECT\0");
		int i;
		for(i = 0; i < 4; i++) {
			AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
			Ak_Init_L(row_root);
			privilege_id = AK_get_id();

			Ak_Insert_New_Element(TYPE_INT, &privilege_id, "AK_group_right", "obj_id", row_root);
			Ak_Insert_New_Element(TYPE_INT, &group_id, "AK_group_right", "group_id", row_root);
			Ak_Insert_New_Element(TYPE_INT, &table_id, "AK_group_right", "artifact_id", row_root);
			Ak_Insert_New_Element(TYPE_VARCHAR, &rights[i*10], "AK_group_right", "right_type", row_root);
			Ak_insert_row(row_root);

			free(row_root);
		}
	}
	else{
		AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
		Ak_Init_L(row_root);
		
			
		privilege_id = AK_get_id();
		Ak_Insert_New_Element(TYPE_INT, &privilege_id, "AK_group_right", "obj_id", row_root);
		Ak_Insert_New_Element(TYPE_INT, &group_id, "AK_group_right", "group_id", row_root);
		Ak_Insert_New_Element(TYPE_INT, &table_id, "AK_group_right", "artifact_id", row_root);
		Ak_Insert_New_Element(TYPE_VARCHAR, right, "AK_group_right", "right_type", row_root);
		Ak_insert_row(row_root);
		free(row_root);
	}
		
	AK_archive_log("AK_grant_privilege_group", groupname, table, right); //ARCHIVE_LOG
	return privilege_id;
}
/**
 * @author Kristina Takač, updated by Mario Peroković - added comparing by table id
 * @brief Function that revokes privilege from group on given table 
 * @param *grounamep name of group which user belongs to
 * @param *table name of table on which privilege will be granted to group
 * @param *right type of privilege which will be granted to group on given table
 * @return EXIT_SUCCESS if privilege is revoked, EXIT_ERROR if it isn't
*/
void AK_revoke_privilege_group(char *groupname, char *table, char *right){
	int table_id, group_id;
	group_id = AK_group_get_id(groupname);
	table_id = AK_get_table_obj_id(table);
	int result;

	if(strcmp(right, "ALL") == 0) {
		printf("Revokanje  %s %s %s\n", groupname, table, right);
		AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
		Ak_Init_L(row_root);
		int i = 0;
		AK_list *row;
		while ((row = (AK_list *)AK_get_row(i, "AK_group_right")) != NULL) {
			if ((int ) * row->next->next->data == group_id && (int) * row->next->next->next->data == table_id) {
				Ak_Insert_New_Element_For_Update(TYPE_INT, &group_id, "AK_group_right", "group_id", row_root, 1);
				result = Ak_delete_row(row_root);
			}
			i++;
			Ak_DeleteAll_L(row_root);
			free(row);
		}
	}
	else {
		AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
		Ak_Init_L(row_root);
		int i = 0;
		AK_list *row; 
		while ((row = (AK_list *)AK_get_row(i, "AK_group_right")) != NULL) {
			if (((int ) * row->next->next->data == group_id) && (table_id ==  (int) * row->next->next->next->data) && (strcmp(row->next->next->next->next->data,right))) {
				int id = (int) * row->next->data;
				Ak_Insert_New_Element_For_Update(TYPE_INT, &id, "AK_group_right", "obj_id", row_root, 1);
				result = Ak_delete_row(row_root);
			}
		i++;
		Ak_DeleteAll_L(row_root);
		free(row);
		}
	}

	if (result == EXIT_ERROR) return EXIT_ERROR;

	return EXIT_SUCCESS;
}
/**
 * @author Kristina Takač, updated by Mario Peroković, added verifying the existence of user in the group
 * @brief Function that puts user in given group
 * @param *user username of user which will be put in group
 * @param *group name of group in which user will be put
 * @return EXIT_SUCCESS or EXIT_ERROR if user is already in the group
*/       
int AK_add_user_to_group(char *user, char *group){
	int user_id = AK_user_get_id(user);
	int group_id = AK_group_get_id(group);

	int i = 0;
	AK_list *row; 
	while ((row = (AK_list *)AK_get_row(i, "AK_user_group")) != NULL) {
		// if user is already in group, return error
		if (user_id == (int) *row->next->data) {
			int id = (int) * row->next->data;
			return EXIT_ERROR;
		}
		i++;
	}

	AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
	Ak_Init_L(row_root);

	Ak_Insert_New_Element(TYPE_INT, &user_id, "AK_user_group", "user_id", row_root);
	Ak_Insert_New_Element(TYPE_INT, &group_id, "AK_user_group", "group_id", row_root);
	Ak_insert_row(row_root);

	free(row_root);
	AK_archive_log("AK_add_user_to_group", user, group); //ARCHIVE_LOG
	return EXIT_SUCCESS;
}
/**
 * @author Kristina Takač.
 * @brief Function that checks whether given user has right for given operation on given table
 * @param *user username for which we want check privileges
 * @param *table name of table for which we want to check whether user has right on
 * @param *privilege privilege for which we want to check whether user has right for
 * @return EXIT_SUCCESS if user has right, EXIT_ERROR if user has no right
*/
int AK_check_privilege(char *username, char *table, char *privilege){
	int user_id = AK_user_get_id(username);
	int table_id = AK_get_table_obj_id(table);
	int i = 0;
	int number_of_groups = 0;
	int has_right = 0;
	int groups[100];	
	AK_list *row;

	if(strcmp(privilege, "ALL") == 0) {
		int checking_privileges[4] = {0, 0, 0, 0};
		char found_privilege[10];
		while ((row = (AK_list *)AK_get_row(i, "AK_user_right")) != NULL) {
			if ((strcmp(row->next->next->data,username) == 0) && (table_id == (int) * row->next->next->next->data)) {
				strcpy(found_privilege, row->next->next->next->next->data);
				if(strcmp(found_privilege, "UPDATE")==0)
					checking_privileges[0] = 1;
				if(strcmp(found_privilege, "DELETE")==0)
					checking_privileges[1] = 1;
				if(strcmp(found_privilege, "INSERT")==0)
					checking_privileges[2] = 1;
				if(strcmp(found_privilege, "SELECT")==0)
					checking_privileges[3] = 1;
			}
			i++;
			free(row);
		}
		for(i = 0; i < 4; i++) {
			if(checking_privileges[i] == 1) {
				has_right = 1;
			}
			else {
				has_right = 0;
				break;
			}
		}
		if(has_right == 1){
			printf("User %s has right to %s on %s", username, privilege, table);
			return EXIT_SUCCESS;
		}
		i = 0;
		while ((row = (AK_list *)AK_get_row(i, "AK_user_group")) != NULL) {
			if (user_id == (int) * row->next->data) {
				groups[number_of_groups] = (int) * row->next->next->data;
				number_of_groups++;	
			}
			i++;
			free(row);
		}
		// set "flags" to 0
		checking_privileges[0] = 0;
		checking_privileges[1] = 0;
		checking_privileges[2] = 0;
		checking_privileges[3] = 0;
		for(i = 0; i < number_of_groups; i++){
			int j=0;
			while ((row = (AK_list *)AK_get_row(j, "AK_group_right")) != NULL) {
				if ((groups[i] == (int) * row->next->next->data) && (table_id == (int) * row->next->next->next->data)) {
					strcpy(found_privilege, row->next->next->next->next->data);
					if(strcmp(found_privilege, "UPDATE")==0)
						checking_privileges[0] = 1;
					if(strcmp(found_privilege, "DELETE")==0)
						checking_privileges[1] = 1;		
					if(strcmp(found_privilege, "INSERT")==0)
						checking_privileges[2] = 1;			
					if(strcmp(found_privilege, "SELECT")==0)
						checking_privileges[3] = 1;		
				}
				j++;
				free(row);
			}
		}
		for(i = 0; i < 4; i++) {
			if(checking_privileges[i] == 1) {
				has_right = 1;
			}
			else {
				has_right = 0;
				break;
			}
		}
		if(has_right == 1){
			printf("User %s has right to %s on %s", username, privilege, table);
			return EXIT_SUCCESS;
		}
	}
	// if privilege is not ALL
	else {
		while ((row = (AK_list *)AK_get_row(i, "AK_user_right")) != NULL) {
			if ((strcmp(row->next->next->data,username) == 0) && (table_id == (int) * row->next->next->next->data) && (strcmp(row->next->next->next->next->data, privilege)==0)) {
				has_right = 1;
				printf("User %s has right to %s on %s", username, privilege, table);
				return EXIT_SUCCESS;
			}
			i++;
			free(row);
		}
		i = 0;
		while ((row = (AK_list *)AK_get_row(i, "AK_user_group")) != NULL) {
			if (user_id == (int) * row->next->data) {
				groups[number_of_groups] = (int) * row->next->next->data;
				number_of_groups++;	
			}
			i++;
			free(row);
		}
		for(i = 0; i < number_of_groups; i++){
			int j=0;
			while ((row = (AK_list *)AK_get_row(j, "AK_group_right")) != NULL) {
				if ((groups[i] == (int) * row->next->next->data) && (table_id == (int) * row->next->next->next->data) && (strcmp(row->next->next->next->next->data,privilege)==0)) {
					has_right = 1;
					printf("User %s has right to %s on %s", username, privilege, table);
					return EXIT_SUCCESS;
				}
				j++;
				free(row);
			}
		}
	}
	
	printf("User %s has no right to %s on %s", username, privilege, table);
	return EXIT_ERROR;
}

/**
 * @author Kristina Takač.
 * @brief Function that tests functions above for privileges
 * @return no return value                                                     
*/
void AK_privileges_test(){
	printf("\nThis is PRIVILEGES test!\n\n");
    AK_user_add("proba", 123);
    AK_user_add("kritakac", 321);
    AK_print_table("AK_user");
    
    AK_group_add("grupa1");
    AK_group_add("grupa2");
    AK_print_table("AK_group");

    AK_add_user_to_group("kritakac", "grupa1");
    AK_add_user_to_group("proba", "grupa2");
    AK_add_user_to_group("proba", "grupa2");
    AK_print_table("AK_user_group");
   
    AK_grant_privilege_group("grupa2", "professor", "DELETE");
    AK_grant_privilege_group("grupa1", "student", "ALL");
    
    AK_print_table("AK_group_right");
    AK_revoke_privilege_group("grupa2", "student", "ALL");
    AK_print_table("AK_group_right");
    
    AK_grant_privilege_user("kritakac", "student1", "DELETE");
    AK_grant_privilege_user("proba", "student1", "UPDATE");
    AK_grant_privilege_user("kritakac", "professor", "SELECT");
    
    AK_print_table("AK_user_right");
    AK_revoke_privilege_user("proba", "student1", "UPDATE");
    AK_print_table("AK_user_right");

    printf("\nCheck if user kritakac has right ALL on table student!\n");
    AK_check_privilege("kritakac", "student", "ALL");

    printf("\nCheck if user proba has right UPDATE on table student!\n");
    AK_check_privilege("proba", "student", "UPDATE");
}