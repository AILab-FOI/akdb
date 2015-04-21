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
int AK_user_add(char *username, int *password, int set_id) {

    char *tblName = "AK_user";
    AK_PRO;
    /*AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L(row_root);
    */
    
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);
    
    int user_id = AK_get_id();
    if (set_id != 0) user_id = set_id;
    Ak_Insert_New_Element(TYPE_INT, &user_id, tblName, "obj_id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, username, tblName, "username", row_root);
    Ak_Insert_New_Element(TYPE_INT, &password, tblName, "password", row_root);
    Ak_insert_row(row_root);
    AK_EPI;
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
    //AK_list *row;
    struct list_node *row;
    AK_PRO;
    //while ((row = (AK_list *) AK_get_row(i, "AK_user")) != NULL) {
    while ((row = (struct list_node *) AK_get_row(i, "AK_user")) != NULL) {
        if (strcmp(row->next->next->data, username) == 0) {
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
 * @author Ljubo Barać
 * @brief Function removes the user.
 * @param name Name of the user to be removed
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_user_remove_by_name(char *name) {
    AK_PRO;
    /*
    AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L((AK_list_elem) row_root);*/
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, name, "AK_user", "username", row_root, 1);
    int result = Ak_delete_row(row_root);
    AK_EPI;
    return result;
}

/**
 * @author Ljubo Barać
 * @brief Function renames the user.
 * @param old_name Name of the user to be renamed
 * @param new_name New name of the user
 * @param password Password of the user to be renamed (should be provided)
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_user_rename(char *old_name, char *new_name, int *password) {
    AK_PRO;
    printf("\n***Rename group***");
    int result = 0;
    int user_id = AK_user_get_id(old_name);

    result = AK_user_remove_by_name(old_name);
    result = AK_user_remove_by_name(old_name);
    result = AK_user_add(new_name, password, user_id);
    AK_EPI;
    return result;

}

/**
 * @author Kristina Takač, edited by Ljubo Barać
 * @brief Function that inserts group in table AK_group
 * @param *name name of group to be added
 * @param set_id non default id to be passed
 * @return id of group                                                      
 */
int AK_group_add(char *name, int set_id) {
    char *tblName = "AK_group";
    AK_PRO;
    /*
    AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L(row_root);*/
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);
    int group_id = AK_get_id();
    if (set_id != 0) group_id = set_id;
    Ak_Insert_New_Element(TYPE_INT, &group_id, tblName, "obj_id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, name, tblName, "name", row_root);
    Ak_insert_row(row_root);
    AK_EPI;
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
//    AK_list *row;
    struct list_node *row;
    AK_PRO;
    //while ((row = (AK_list *) AK_get_row(i, "AK_group")) != NULL) {
    while ((row = (struct list_node *) AK_get_row(i, "AK_group")) != NULL) {
        if (strcmp(row->next->next->data, name) == 0) {
            i = (int) * row->next->data;
            AK_free(row);
            return i;
        }
        i++;
    }
    AK_free(row);
    AK_EPI;
    return EXIT_ERROR;
}

/**
 * @author Ljubo Barać
 * @brief Function removes the group.
 * @param name Name of the group to be removed
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_group_remove_by_name(char *name) {
    AK_PRO;
    AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L((AK_list_elem) row_root);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, name, "AK_group", "name", row_root, 1);
    int result = Ak_delete_row(row_root);
    AK_EPI;
    return result;
}

/**
 * @author Ljubo Barać
 * @brief Function renames the group.
 * @param old_name Name of the group to be renamed
 * @param new_name New name of the group
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_group_rename(char *old_name, char *new_name) {
    AK_PRO;
    printf("\n***Rename group***");
    int result = 0;
    int view_id = AK_group_get_id(old_name);

    result = AK_group_remove_by_name(old_name);
    result = AK_group_remove_by_name(old_name);
    result = AK_group_add(new_name, view_id);
    AK_EPI;
    return result;
}

/**
 * @author Kristina Takač, updated by Mario Peroković, inserting user id instead of username in AK_user_right
 * @brief Function that grants privilege to user
 * @param *username username of user to whom we want to grant privilege
 * @param *table name of table on which privilege will be granted to user
 * @param *right type of privilege which will be granted to user on given table
 * @return privilege_id or EXIT_ERROR if table or user aren't correct
 */
int AK_grant_privilege_user(char *username, char *table, char *right) {
    int privilege_id, table_id;
    AK_PRO;
    table_id = AK_get_table_obj_id(table);
    int user_id = AK_user_get_id(username);

    if (table_id == -1 || user_id == -1) {
	AK_EPI;
        return EXIT_ERROR;
    }

    if (strcmp(right, "ALL") == 0) {
        char rights[100];
        strcpy(&rights[0], "UPDATE\0");
        strcpy(&rights[10], "DELETE\0");
        strcpy(&rights[20], "INSERT\0");
        strcpy(&rights[30], "SELECT\0");
        int i;
        for (i = 0; i < 4; i++) {
            /*AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
            Ak_Init_L(row_root);*/
	    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
            Ak_Init_L3(&row_root);
            privilege_id = AK_get_id();

            Ak_Insert_New_Element(TYPE_INT, &privilege_id, "AK_user_right", "obj_id", row_root);
            Ak_Insert_New_Element(TYPE_INT, &user_id, "AK_user_right", "user_id", row_root);
            Ak_Insert_New_Element(TYPE_INT, &table_id, "AK_user_right", "artifact_id", row_root);
            Ak_Insert_New_Element(TYPE_VARCHAR, &rights[i * 10], "AK_user_right", "right_type", row_root);
            Ak_insert_row(row_root);

            AK_free(row_root);
        }
    } else {
        /*AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
        Ak_Init_L(row_root);*/
	struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
        Ak_Init_L3(&row_root);

        privilege_id = AK_get_id();
        Ak_Insert_New_Element(TYPE_INT, &privilege_id, "AK_user_right", "obj_id", row_root);
        Ak_Insert_New_Element(TYPE_INT, &user_id, "AK_user_right", "user_id", row_root);
        Ak_Insert_New_Element(TYPE_INT, &table_id, "AK_user_right", "artifact_id", row_root);
        Ak_Insert_New_Element(TYPE_VARCHAR, right, "AK_user_right", "right_type", row_root);
        Ak_insert_row(row_root);
        AK_free(row_root);
    }
    AK_EPI;
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
int AK_revoke_privilege_user(char *username, char *table, char *right) {
    AK_PRO;
    printf("Revokanje: %s %s %s\n", username, table, right);
    int table_id = AK_get_table_obj_id(table);
    int user_id = AK_user_get_id(username);
    int result;
    if (strcmp(right, "ALL") == 0) {
        /*AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
        Ak_Init_L(row_root);*/
	struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
        Ak_Init_L3(&row_root);
        int i = 0;
        //AK_list *row;
        struct list_node *row;
	//while ((row = (AK_list *) AK_get_row(i, "AK_user_right")) != NULL) {
	while ((row = (struct list_node *) AK_get_row(i, "AK_user_right")) != NULL) {
            if (((int) *row->next->next->data == user_id) && (table_id != -1) && ((int) *row->next->next->next->data == table_id)) {
                Ak_Insert_New_Element_For_Update(TYPE_INT, &user_id, "AK_user_right", "user_id", row_root, 1);
                result = Ak_delete_row(row_root);
            }
            i++;
            //Ak_DeleteAll_L(row_root);
	    Ak_DeleteAll_L3(&row_root);
            AK_free(row);
        }
    } else {
        /*AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
        Ak_Init_L(row_root);*/
	struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
        Ak_Init_L3(&row_root);
        int i = 0;
         //AK_list *row;
        struct list_node *row;

        while ((row = (struct list_node *) AK_get_row(i, "AK_user_right")) != NULL) {
	//while ((row = (AK_list *) AK_get_row(i, "AK_user_right")) != NULL) {
            if (((int) *row->next->next->data == user_id) && (table_id == (int) * row->next->next->next->data) && (table_id != -1) && (strcmp(row->next->next->next->next->data, right) == 0)) {
                int id = (int) * row->next->data;
                // strange, but it works! :)
                id = id + 1;
                Ak_Insert_New_Element_For_Update(TYPE_INT, &id, "AK_user_right", "obj_id", row_root, 1);
                result = Ak_delete_row(row_root);
            }//if
            i++;
            //Ak_DeleteAll_L(row_root);
	    Ak_DeleteAll_L3(&row_root);
            AK_free(row);
        }//while 
    }//else

    if (result == EXIT_ERROR){
	AK_EPI;
	return EXIT_ERROR;
    }
    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Jurica Hlevnjak
 * @brief Function that revokes ALL privileges from user on ALL tables (for DROP user)
 * @param username name of user from whom we want to revoke all privileges
 * @return EXIT_SUCCESS if privilege is revoked, EXIT_ERROR if it isn't
 */
int AK_revoke_all_privileges_user(char *username) {
    AK_PRO;
    int user_id = AK_user_get_id(username);
    int result;
    /*AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L(row_root);*/
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);
    int i = 0;
    //AK_list *row;
    struct list_node *row;
    
    while ((row = (struct list_node *) AK_get_row(i, "AK_user_right")) != NULL) {
    //while ((row = (AK_list *) AK_get_row(i, "AK_user_right")) != NULL) {
        if ((int) *row->next->next->data == user_id) {
            Ak_Insert_New_Element_For_Update(TYPE_INT, &user_id, "AK_user_right", "user_id", row_root, 1);
            result = Ak_delete_row(row_root);
        }
        i++;
        //Ak_DeleteAll_L(row_root);
	Ak_DeleteAll_L3(&row_root);
        AK_free(row);
    }

    if (result == EXIT_ERROR){
	AK_EPI;
	return EXIT_ERROR;
    }
    AK_EPI;
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
int AK_grant_privilege_group(char *groupname, char *table, char *right) {
    int privilege_id, table_id, group_id;
    AK_PRO;
    table_id = AK_get_table_obj_id(table);
    group_id = AK_group_get_id(groupname);

    if (table_id == -1 || group_id == -1) {
	AK_EPI;
        return EXIT_ERROR;
    }

    if (strcmp(right, "ALL") == 0) {
        char rights[100];
        strcpy(&rights[0], "UPDATE\0");
        strcpy(&rights[10], "DELETE\0");
        strcpy(&rights[20], "INSERT\0");
        strcpy(&rights[30], "SELECT\0");
        int i;
        for (i = 0; i < 4; i++) {
            /*AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
	    Ak_Init_L(row_root);*/
	    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
	    Ak_Init_L3(&row_root);
            privilege_id = AK_get_id();

            Ak_Insert_New_Element(TYPE_INT, &privilege_id, "AK_group_right", "obj_id", row_root);
            Ak_Insert_New_Element(TYPE_INT, &group_id, "AK_group_right", "group_id", row_root);
            Ak_Insert_New_Element(TYPE_INT, &table_id, "AK_group_right", "artifact_id", row_root);
            Ak_Insert_New_Element(TYPE_VARCHAR, &rights[i * 10], "AK_group_right", "right_type", row_root);
            Ak_insert_row(row_root);

            AK_free(row_root);
        }
    } else {
        /*AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
	Ak_Init_L(row_root);*/
	struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
	Ak_Init_L3(&row_root);


        privilege_id = AK_get_id();
        Ak_Insert_New_Element(TYPE_INT, &privilege_id, "AK_group_right", "obj_id", row_root);
        Ak_Insert_New_Element(TYPE_INT, &group_id, "AK_group_right", "group_id", row_root);
        Ak_Insert_New_Element(TYPE_INT, &table_id, "AK_group_right", "artifact_id", row_root);
        Ak_Insert_New_Element(TYPE_VARCHAR, right, "AK_group_right", "right_type", row_root);
        Ak_insert_row(row_root);
        AK_free(row_root);
    }

    AK_EPI;
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
int AK_revoke_privilege_group(char *groupname, char *table, char *right) {
    int table_id, group_id;
    AK_PRO;
    group_id = AK_group_get_id(groupname);
    table_id = AK_get_table_obj_id(table);
    int result;

    if (strcmp(right, "ALL") == 0) {
        printf("Revokanje  %s %s %s\n", groupname, table, right);
        /*AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
	Ak_Init_L(row_root);*/
	struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
	Ak_Init_L3(&row_root);
        int i = 0;
        //AK_list *row;
        struct list_node *row;
	
	while ((row = (struct list_node *) AK_get_row(i, "AK_group_right")) != NULL) {
	//while ((row = (AK_list *) AK_get_row(i, "AK_group_right")) != NULL) {
            if ((int) * row->next->next->data == group_id && (int) * row->next->next->next->data == table_id) {
                Ak_Insert_New_Element_For_Update(TYPE_INT, &group_id, "AK_group_right", "group_id", row_root, 1);
                result = Ak_delete_row(row_root);
            }
            i++;
            //Ak_DeleteAll_L(row_root);
	    Ak_DeleteAll_L3(&row_root);
            AK_free(row);
        }
    } else {
        /*AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
	Ak_Init_L(row_root);*/
	struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
	Ak_Init_L3(&row_root);
        int i = 0;
        //AK_list *row;
        struct list_node *row;
	
	while ((row = (struct list_node *) AK_get_row(i, "AK_group_right")) != NULL) {
	//while ((row = (AK_list *) AK_get_row(i, "AK_group_right")) != NULL) {
            if (((int) * row->next->next->data == group_id) && (table_id == (int) * row->next->next->next->data) && (strcmp(row->next->next->next->next->data, right))) {
                int id = (int) * row->next->data;
                Ak_Insert_New_Element_For_Update(TYPE_INT, &id, "AK_group_right", "obj_id", row_root, 1);
                result = Ak_delete_row(row_root);
            }
            i++;
            //Ak_DeleteAll_L(row_root);
	    Ak_DeleteAll_L3(&row_root);
            AK_free(row);
        }
    }

    if (result == EXIT_ERROR){
	AK_EPI;
	return EXIT_ERROR;
    }
    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Jurica Hlevnjak
 * @brief Function that revokes ALL privileges from group on ALL tables (needed for DROP group)
 * @param groupname name of group from which we want to revoke all privileges
 * @return EXIT_SUCCESS if privilege is revoked, EXIT_ERROR if it isn't
 */
int AK_revoke_all_privileges_group(char *groupname) {
    AK_PRO;
    int group_id = AK_group_get_id(groupname);
    int result;
    /*AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L(row_root);*/
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);
    int i = 0;
    //AK_list *row;
    struct list_node *row;
    
    while ((row = (struct list_node *) AK_get_row(i, "AK_group_right")) != NULL) {
    //while ((row = (AK_list *) AK_get_row(i, "AK_group_right")) != NULL) {
        if ((int) *row->next->next->data == group_id) {
            Ak_Insert_New_Element_For_Update(TYPE_INT, &group_id, "AK_group_right", "group_id", row_root, 1);
            result = Ak_delete_row(row_root);
        }
        i++;
        //Ak_DeleteAll_L(row_root);
	Ak_DeleteAll_L3(&row_root);
        AK_free(row);
    }

    if (result == EXIT_ERROR){
	AK_EPI;
	return EXIT_ERROR;
    }
    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Kristina Takač, updated by Mario Peroković, added verifying the existence of user in the group
 * @brief Function that puts user in given group
 * @param *user username of user which will be put in group
 * @param *group name of group in which user will be put
 * @return EXIT_SUCCESS or EXIT_ERROR if user is already in the group
 */
int AK_add_user_to_group(char *user, char *group) {
    AK_PRO;
    int user_id = AK_user_get_id(user);
    int group_id = AK_group_get_id(group);

    int i = 0;
    //AK_list *row;
    struct list_node *row;
    
    while ((row = (struct list_node *) AK_get_row(i, "AK_user_group")) != NULL) {
    //while ((row = (AK_list *) AK_get_row(i, "AK_user_group")) != NULL) {
        // if user is already in group, return error
        // ovo nije dobro--provjerava samo korinika ne i grupu
        if (user_id == (int) *row->next->data) {
	    AK_EPI;
            return EXIT_ERROR;
        }
        i++;
    }

    /*AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L(row_root);*/
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);

    Ak_Insert_New_Element(TYPE_INT, &user_id, "AK_user_group", "user_id", row_root);
    Ak_Insert_New_Element(TYPE_INT, &group_id, "AK_user_group", "group_id", row_root);
    Ak_insert_row(row_root);

    AK_free(row_root);

    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Jurica Hlevnjak
 * @brief Function removes user from all groups. Used for DROP user.
 * @param user name of user
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_remove_user_from_all_groups(char *user) {
    AK_PRO;
    /*AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L(row_root);*/
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);

    int user_id = AK_user_get_id(user);
    int i = 0;
    int result;
    //AK_list *row;
    struct list_node *row;
    
    while ((row = (struct list_node *) AK_get_row(i, "AK_user_group")) != NULL) {
    //while ((row = (AK_list *) AK_get_row(i, "AK_user_group")) != NULL) {
        if (user_id == (int) *row->next->data) {
            Ak_Insert_New_Element_For_Update(TYPE_INT, &user_id, "AK_user_group", "user_id", row_root, 1);
            result = Ak_delete_row(row_root);
            if (result == EXIT_ERROR){
		AK_EPI;
		return EXIT_ERROR;
	    }
        }
        i++;
//        Ak_DeleteAll_L(row_root);
	Ak_DeleteAll_L3(&row_root);
        AK_free(row);
    }

    //if (result == EXIT_ERROR) return EXIT_ERROR;
    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Jurica Hlevnjak
 * @brief Function removes all users from group. Used for DROP group.
 * @param group name of group
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_remove_all_users_from_group(char *group) {
    AK_PRO;
    /*AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L(row_root);*/
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);

    int group_id = AK_group_get_id(group);
    int i = 0;
    int result;
    //AK_list *row;
    struct list_node *row;
    
    while ((row = (struct list_node *) AK_get_row(i, "AK_user_group")) != NULL) {
    //while ((row = (AK_list *) AK_get_row(i, "AK_user_group")) != NULL) {
        if (group_id == (int) *row->next->next->data) {
            Ak_Insert_New_Element_For_Update(TYPE_INT, &group_id, "AK_user_group", "group_id", row_root, 1);
            result = Ak_delete_row(row_root);
            if (result == EXIT_ERROR){
		AK_EPI;
		return EXIT_ERROR;
	    }
        }
        i++;
        //Ak_DeleteAll_L(row_root);
	Ak_DeleteAll_L3(&row_root);
        AK_free(row);
    }

    // if (result == EXIT_ERROR) return EXIT_ERROR;
    AK_EPI;
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
int AK_check_privilege(char *username, char *table, char *privilege) {
    AK_PRO;
    int user_id = AK_user_get_id(username);
    int table_id = AK_get_table_obj_id(table);
    int i = 0;
    int number_of_groups = 0;
    int has_right = 0;
    int groups[100];
    //AK_list *row;
    struct list_node *row;
    
    if (strcmp(privilege, "ALL") == 0) {
        int checking_privileges[4] = {0, 0, 0, 0};
        char found_privilege[10];
        while ((row = (struct list_node *) AK_get_row(i, "AK_user_right")) != NULL) {
	//while ((row = (AK_list *) AK_get_row(i, "AK_user_right")) != NULL) {
            if ((strcmp(row->next->next->data, username) == 0) && (table_id == (int) * row->next->next->next->data)) {
                strcpy(found_privilege, row->next->next->next->next->data);
                if (strcmp(found_privilege, "UPDATE") == 0)
                    checking_privileges[0] = 1;
                if (strcmp(found_privilege, "DELETE") == 0)
                    checking_privileges[1] = 1;
                if (strcmp(found_privilege, "INSERT") == 0)
                    checking_privileges[2] = 1;
                if (strcmp(found_privilege, "SELECT") == 0)
                    checking_privileges[3] = 1;
            }
            i++;
            AK_free(row);
        }
        for (i = 0; i < 4; i++) {
            if (checking_privileges[i] == 1) {
                has_right = 1;
            } else {
                has_right = 0;
                break;
            }
        }
        if (has_right == 1) {
            printf("User %s has right to %s on %s", username, privilege, table);
	    AK_EPI;
            return EXIT_SUCCESS;
        }
        i = 0;
        while ((row = (struct list_node *) AK_get_row(i, "AK_user_group")) != NULL) {
	while ((row = (AK_list *) AK_get_row(i, "AK_user_group")) != NULL) {
            if (user_id == (int) * row->next->data) {
                groups[number_of_groups] = (int) * row->next->next->data;
                number_of_groups++;
            }
            i++;
            AK_free(row);
        }
        // set "flags" to 0
        checking_privileges[0] = 0;
        checking_privileges[1] = 0;
        checking_privileges[2] = 0;
        checking_privileges[3] = 0;
        for (i = 0; i < number_of_groups; i++) {
            int j = 0;
            while ((row = (AK_list *) AK_get_row(j, "AK_group_right")) != NULL) {
                if ((groups[i] == (int) * row->next->next->data) && (table_id == (int) * row->next->next->next->data)) {
                    strcpy(found_privilege, row->next->next->next->next->data);
                    if (strcmp(found_privilege, "UPDATE") == 0)
                        checking_privileges[0] = 1;
                    if (strcmp(found_privilege, "DELETE") == 0)
                        checking_privileges[1] = 1;
                    if (strcmp(found_privilege, "INSERT") == 0)
                        checking_privileges[2] = 1;
                    if (strcmp(found_privilege, "SELECT") == 0)
                        checking_privileges[3] = 1;
                }
                j++;
                AK_free(row);
            }
        }
        for (i = 0; i < 4; i++) {
            if (checking_privileges[i] == 1) {
                has_right = 1;
            } else {
                has_right = 0;
                break;
            }
        }
        if (has_right == 1) {
            printf("User %s has right to %s on %s", username, privilege, table);
	    AK_EPI;
            return EXIT_SUCCESS;
        }
    }}// if privilege is not ALL
    else {
        //while ((row = (AK_list *) AK_get_row(i, "AK_user_right")) != NULL) {
	while ((row = (struct list_node *) AK_get_row(i, "AK_user_right")) != NULL) {  
            if ((strcmp(row->next->next->data, username) == 0) && (table_id == (int) * row->next->next->next->data) && (strcmp(row->next->next->next->next->data, privilege) == 0)) {
                has_right = 1;
                printf("User %s has right to %s on %s", username, privilege, table);
                AK_EPI;
                return EXIT_SUCCESS;
            }
            i++;
            AK_free(row);
        }
        i = 0;
        while ((row = (struct list_node *) AK_get_row(i, "AK_user_group")) != NULL) {
	//while ((row = (AK_list *) AK_get_row(i, "AK_user_group")) != NULL) {
            if (user_id == (int) * row->next->data) {
                groups[number_of_groups] = (int) * row->next->next->data;
                number_of_groups++;
            }
            i++;
            AK_free(row);
        }
        for (i = 0; i < number_of_groups; i++) {
            int j = 0;
            //while ((row = (AK_list *) AK_get_row(j, "AK_group_right")) != NULL) {
	    while ((row = (struct list_node *) AK_get_row(j, "AK_group_right")) != NULL) {  
                if ((groups[i] == (int) * row->next->next->data) && (table_id == (int) * row->next->next->next->data) && (strcmp(row->next->next->next->next->data, privilege) == 0)) {
                    has_right = 1;
                    printf("User %s has right to %s on %s", username, privilege, table);
		    AK_EPI;
                    return EXIT_SUCCESS;
                }
                j++;
                AK_free(row);
            }
        }
    }

    printf("User %s has no right to %s on %s", username, privilege, table);
    AK_EPI;
    return EXIT_ERROR;
}

/**
 * @author Jurica Hlevnjak
 * @brief Function check if user have any privilege or belong to group. Used in drop user for restriction.
 * @param user name of user
 * @return EXIT_ERROR or EXIT_SUCCESS
 */
int AK_check_user_privilege(char *user) {
    AK_PRO;
    int user_id = AK_user_get_id(user);
    int i = 0;
    //AK_list *row;
    struct list_node *row;
    int privilege = 0;

    while ((row = (struct list_node *) AK_get_row(i, "AK_user_right")) != NULL) {
    //while ((row = (AK_list *) AK_get_row(i, "AK_user_right")) != NULL) {
        if ((int) *row->next->next->data == user_id) {
            privilege = 1;
	    AK_EPI;
            return EXIT_SUCCESS;
        }
        i++;
        AK_free(row);
    }
    int j = 0;
    while ((row = (struct list_node *) AK_get_row(j, "AK_user_group")) != NULL) {
    //while ((row = (AK_list *) AK_get_row(j, "AK_user_group")) != NULL) {
        if ((int) *row->next->data == user_id) {
            privilege = 1;
	    AK_EPI;
            return EXIT_SUCCESS;
        }
        j++;
        AK_free(row);
    }
    if (privilege == 0) {
  	AK_EPI;
        return EXIT_ERROR;
    }
    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Jurica Hlevnjak
 * @brief Function check if group have any privilege or user. Used in drop group for restriction.
 * @param group name of group
 * @return EXIT_ERROR or EXIT_SUCCESS
 */
int AK_check_group_privilege(char *group) {
    AK_PRO;
    int group_id = AK_group_get_id(group);
    int i = 0;
    //AK_list *row;
    struct list_node *row;
    int privilege = 0;

    while ((row = (struct list_node *) AK_get_row(i, "AK_group_right")) != NULL) {
    //while ((row = (AK_list *) AK_get_row(i, "AK_group_right")) != NULL) {
        if ((int) *row->next->next->data == group_id) {
            privilege = 1;
	    AK_EPI;
            return EXIT_SUCCESS;
        }
        i++;
        AK_free(row);
    }
    int j = 0;
    while ((row = (struct list_node *) AK_get_row(j, "AK_user_group")) != NULL) {
    //while ((row = (AK_list *) AK_get_row(j, "AK_user_group")) != NULL) {
        if ((int) *row->next->next->data == group_id) {
            privilege = 1;
	    AK_EPI;
            return EXIT_SUCCESS;
        }
        j++;
        AK_free(row);
    }
    if (privilege == 0) {
	AK_EPI;
        return EXIT_ERROR;
    }
    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Kristina Takač, updated by Tomislav Ilisevic
 * @brief Function that tests functions above for privileges
 * @return no return value                                                     
 */
void AK_privileges_test() {
    AK_PRO;
    printf("\nThis is PRIVILEGES test!\n\n");
    AK_user_add("proba", (int*) 123, NEW_ID);
    AK_user_add("kritakac", (int*) 321, NEW_ID);
    AK_user_add("mrvasedam", (int*) 569, NEW_ID);
    AK_print_table("AK_user");
    //AK_user_rename("proba", "test", (int*) 123);
    AK_print_table("AK_user");

    AK_group_add("group1", NEW_ID);
    AK_group_add("group2", NEW_ID);
    AK_group_add("group3", NEW_ID);
    AK_print_table("AK_group");
    //AK_group_rename("group1", "group9");
    //AK_group_rename("group2", "group4");
    AK_print_table("AK_group");

    AK_add_user_to_group("kritakac", "group1");
    AK_add_user_to_group("proba", "group1");
    AK_add_user_to_group("proba", "group2");
    AK_print_table("AK_user_group");

    AK_grant_privilege_group("group2", "professor", "DELETE");
    AK_grant_privilege_group("group1", "student", "ALL");
    AK_grant_privilege_group("group2", "student", "ALL");

    AK_print_table("AK_group_right");
    //AK_revoke_privilege_group("group2", "student", "ALL");
    AK_print_table("AK_group_right");

    AK_grant_privilege_user("kritakac", "student", "DELETE");
    AK_grant_privilege_user("proba", "student", "UPDATE");
    AK_grant_privilege_user("kritakac", "professor", "SELECT");
    AK_grant_privilege_user("kritakac", "employee", "ALL");

    AK_print_table("AK_user_right");
    //AK_revoke_privilege_user("proba", "student1", "UPDATE");
    AK_print_table("AK_user_right");

    /*
        printf("\nCheck if user kritakac has right ALL on table student!\n");
        AK_check_privilege("kritakac", "student", "ALL");

        printf("\nCheck if user proba has right UPDATE on table student!\n");
        AK_check_privilege("proba", "student", "UPDATE");
     */

	int uspjesno[15] = {0};

	printf("Test for function AK_check_group_privilege - check if exist the privilege or not!\n");
	printf("\n1. Test: group1 with added privileges..\nResult: ");
	if(AK_check_group_privilege("group1") == EXIT_SUCCESS) {
		printf("For group '%s' exist privilege!\n", "group1");
		uspjesno[0]=1;
	} else {
		printf("No privilege for group '%s'!\n", "group1");
	}
	printf("\n2. Test: group3 without privileges..\nResult: ");
	if(AK_check_group_privilege("group3") == EXIT_SUCCESS) {
		printf("For group '%s' exist privilege!\n", "group3");
		uspjesno[1]=1;
	} else {
		printf("No privilege for group '%s'!\n", "group3");
	}
	AK_print_table("AK_group_right");


	printf("Test for function AK_revoke_privilege_group - remove privileges for group from some table\n");
	printf("\n3. Test: real group with added privileges..group2..\nResult: ");
	if(AK_revoke_privilege_group("group2", "student", "ALL") == EXIT_SUCCESS) {
		printf("Group '%s' has no more privileges '%s' upon table '%s'!\n", "group2", "ALL", "student");
		uspjesno[2]=1;
	} else {
		printf("Error during removing privileges for group '%s'!\n", "group2");
	}
	printf("\n4. Test: real group without added privileges for table professor..group2..\nResult: ");
	if(AK_revoke_privilege_group("group2", "professor", "ALL") == EXIT_SUCCESS) {
		printf("Group '%s' has no more privileges '%s' upon table '%s'!\n", "group2", "ALL", "professor");
		uspjesno[3]=1;
	} else {
		printf("Error during removing privileges for group '%s'!\n", "group2");
	}
	printf("\n5. Test: real group without added any privileges..group3..\nResult: ");
	if(AK_revoke_privilege_group("group3", "student", "ALL") == EXIT_SUCCESS) {
		printf("Group '%s' has no more privileges '%s' upon table '%s'!\n", "group3", "ALL", "student");
		uspjesno[4]=1;
	} else {
		printf("Error during removing privileges for group '%s'!\n", "group3");
	}
	printf("\n6. Test: Group that not exist..group6..\nResult: ");
	if(AK_revoke_privilege_group("group6", "student", "ALL") == EXIT_SUCCESS) {
		printf("Group '%s' has no more privileges '%s' upon table '%s'!\n", "group6", "ALL", "student");
		uspjesno[5]=1;
	} else {
		printf("Error during removing privileges for group '%s'!\n", "group6");
	}
	AK_print_table("AK_group_right");


	printf("Test for function AK_remove_all_users_from_group - removes all users from some group\n");
	printf("\n7. Test: group1 with added users..\nResult: ");
	if(AK_remove_all_users_from_group("group1") == EXIT_SUCCESS) {
		printf("Users deleted from group '%s'!\n", "group1");
		uspjesno[6]=1;
	} else {
		printf("Error deleting user from group '%s'!\n", "group1");
	}
	printf("\n8. Test: group3 without users..\nResult: ");
	if(AK_remove_all_users_from_group("group3") == EXIT_SUCCESS) {
		printf("Users deleted from group '%s'!\n", "group3");
		uspjesno[7]=1;
	} else {
		printf("Error deleting user from group '%s'!\n", "group3");
	}
	AK_print_table("AK_user_group");

	printf("Test for function AK_remove_user_from_all_groups - removes all users from all groups\n");
	printf("\n9. Test: user in group: grupe1 - kritakac..\nResult: ");
	if(AK_remove_user_from_all_groups("kritakac") == EXIT_SUCCESS) {
		printf("User '%s' is removed from all groups!\n", "kritakac");
		uspjesno[8]=1;
	} else {
		printf("Error deleting user '%s' from groups!\n", "kritakac");
	}
	printf("\n10. Test: User in two groups - proba..\nResult: ");
	if(AK_remove_user_from_all_groups("proba") == EXIT_SUCCESS) {
		printf("User '%s' is removed from all groups!\n", "proba");
		uspjesno[9]=1;
	} else {
		printf("Error deleting user '%s' from groups!\n", "proba");
	}
	printf("\n11. Test: User not in any group - mrvasedam..\nResult: ");
	if(AK_remove_user_from_all_groups("mrvasedam") == EXIT_SUCCESS) {
		printf("User '%s' is removed from all groups!\n", "mrvasedam");
		uspjesno[10]=1;
	} else {
		printf("Error deleting user '%s' from groups!\n", "mrvasedam");
	}
	AK_print_table("AK_user_group");


	printf("Test for function AK_group_remove_by_name - removes group by name\n");
	printf("\n12. Test: Group that exists..group3..\nResult: ");
	if(AK_group_remove_by_name("group3") == EXIT_SUCCESS) {
		printf("Group '%s' is removed!\n", "group3");
		uspjesno[11]=1;
	} else {
		printf("Error deleting group '%s'!\n", "group3");
	}
	printf("\n13. Test: Group that not exists..group6..\nResult: ");
	if(AK_group_remove_by_name("group6") == EXIT_SUCCESS) {
		printf("Group '%s' is removed!\n", "group6");
		uspjesno[12]=1;
	} else {
		printf("Error deleting group '%s'!\n", "group6");
	}
	AK_print_table("AK_group");


	printf("\nSummary: \n");
	int br=0;
	int brFail=0;
	for(br=0;br<13;br++){
		printf("%i. Test: %s \n", (br+1),  (uspjesno[br]==1?"Pass":"Fail"));
		if(uspjesno[br]==0) brFail++;
	}

	if(brFail==0) printf("\nALL TESTS ARE PASSED! \n");
	else printf("\nThere are %i tests that failed.. \n", brFail);


    AK_EPI;
}