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
 * @brief  Inserts a new user in the AK_user table 
 * @param *username username of user to be added
 * @param *password password of user to be added
 * @param set_id obj_id of the new user
 * @return user_id
 */
int AK_user_add(char *username, int *password, int set_id) {

    char *tblName = "AK_user";
    AK_PRO;

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
 * @brief  Returns an ID of the given user
 * @param *username username of user whose id we are looking for
 * @return user_id, otherwise EXIT_ERROR
 */
int AK_user_get_id(char *username) {
    int i = 0;
    struct list_node *row;
    AK_PRO;

    while ((row = (struct list_node *) AK_get_row(i, "AK_user")) != NULL) {
        struct list_node *elem_in_strcmp = Ak_GetNth_L2(2, row);
        if (strcmp(elem_in_strcmp->data, username) == 0) {
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
 * @brief Removes the given user
 * @param name Name of the user to be removed
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_user_remove_by_name(char *name) {
    AK_PRO;

    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, name, "AK_user", "username", row_root, 1);
    int result = Ak_delete_row(row_root);

    AK_EPI;
    return result;
}

/**
 * @author Ljubo Barać, update by Lidija Lastavec, update by Marko Flajšek
 * @brief Renames the given user
 * @param old_name Name of the user to be renamed
 * @param new_name New name of the user
 * @param password Password of the user to be renamed (should be provided)
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_user_rename(char *old_name, char *new_name, int *password) {
    AK_PRO;
    int result = 0;
    int *user_id = AK_user_get_id(old_name);

    result = AK_user_remove_by_name(old_name);
    result = AK_user_add(new_name, password, user_id);

    AK_EPI;
    return result;
}

/**
 * @author Kristina Takač, edited by Ljubo Barać
 * @brief Adds a new group
 * @param *name name of group to be added
 * @param set_id non default id to be passed
 * @return id of group                                                      
 */
int AK_group_add(char *name, int set_id) {
    char *tblName = "AK_group";
    AK_PRO;

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
 * @brief Returns an ID of from the given group name
 * @param *name name of group whose id we are looking for
 * @return id of group, otherwise EXIT_ERROR                                                      
 */
int AK_group_get_id(char *name) {
    int i = 0;
    struct list_node *row;
    AK_PRO;

    while ((row = (struct list_node *) AK_get_row(i, "AK_group")) != NULL) {
        struct list_node *elem_in_strcmp = Ak_GetNth_L2(2, row);
        if (strcmp(elem_in_strcmp->data, name) == 0) {
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
 * @brief Removes the given group
 * @param name Name of the group to be removed
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_group_remove_by_name(char *name) {
    AK_PRO;
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));

    Ak_Init_L3(&row_root);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, name, "AK_group", "name", row_root, 1);
    int result = Ak_delete_row(row_root);

    AK_EPI;
    return result;
}

/**
 * @author Ljubo Barać, update by Lidija Lastavec
 * @brief Renames the given group
 * @param old_name Name of the group to be renamed
 * @param new_name New name of the group
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_group_rename(char *old_name, char *new_name) {
    AK_PRO;
    int result = 0;
    int *group_id = AK_group_get_id(old_name);

    result = AK_group_remove_by_name(old_name);
    result = AK_group_add(new_name, group_id);

    AK_EPI;
    return result;
}

/**
 * @author Kristina Takač, updated by Mario Peroković, inserting user id instead of username in AK_user_right, updated by Marko Flajšek
 * @brief Grants specific privilege to the desired user on a given table
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
            struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
            Ak_Init_L3(&row_root);
            privilege_id = AK_get_id();

            Ak_Insert_New_Element(TYPE_INT, &privilege_id, "AK_user_right", "obj_id", row_root);
            Ak_Insert_New_Element(TYPE_INT, &user_id, "AK_user_right", "user_id", row_root);
            Ak_Insert_New_Element(TYPE_INT, &table_id, "AK_user_right", "table_id", row_root);
            Ak_Insert_New_Element(TYPE_VARCHAR, &rights[i * 10], "AK_user_right", "right_type", row_root);
            Ak_insert_row(row_root);

            AK_free(row_root);
        }
    } else {
        struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
        Ak_Init_L3(&row_root);

        privilege_id = AK_get_id();
        Ak_Insert_New_Element(TYPE_INT, &privilege_id, "AK_user_right", "obj_id", row_root);
        Ak_Insert_New_Element(TYPE_INT, &user_id, "AK_user_right", "user_id", row_root);
        Ak_Insert_New_Element(TYPE_INT, &table_id, "AK_user_right", "table_id", row_root);
        Ak_Insert_New_Element(TYPE_VARCHAR, right, "AK_user_right", "right_type", row_root);
        Ak_insert_row(row_root);
        AK_free(row_root);
    }

    AK_EPI;
    return privilege_id;
}

/**
 * @author Kristina Takač, updated by Mario Peroković - added comparing by table id, and use of user_id in AK_user_right
 * @brief Revokes user's privilege on the given table
 * @param *username username of user to whom we want to grant privilege
 * @param *table name of table on which privilege will be revoked from user
 * @param *right type of privilege which will be revoked from user on given table
 * @return EXIT_SUCCESS if privilege is revoked, EXIT_ERROR if it isn't
 */
int AK_revoke_privilege_user(char *username, char *table, char *right) {
    AK_PRO;
    printf("Revoke: %s %s %s", username, table, right);
    int table_id = AK_get_table_obj_id(table);
    int user_id = AK_user_get_id(username);
    int result;

    if (strcmp(right, "ALL") == 0) {
        struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
        Ak_Init_L3(&row_root);
        int i = 0;

        struct list_node *row;

        while ((row = (struct list_node *) AK_get_row(i, "AK_user_right")) != NULL) { //prolaz kroz redove
            struct list_node *user = Ak_GetNth_L2(2, row);
            struct list_node *table = Ak_GetNth_L2(3, row);
            if (((int) *user->data == user_id) && (table_id != -1) && ((int) *table->data == table_id)) { //provjera valjanosti parametara
                Ak_Insert_New_Element_For_Update(TYPE_INT, &user_id, "AK_user_right", "user_id", row_root, 1);
                result = Ak_delete_row(row_root);
            }
            i++;

            Ak_DeleteAll_L3(&row_root);
            AK_free(row);
        }
    } else {
        struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
        Ak_Init_L3(&row_root);
        int i = 0;

        struct list_node *row;

        while ((row = (struct list_node *) AK_get_row(i, "AK_user_right")) != NULL) {
            struct list_node *user = Ak_GetNth_L2(2, row);
            struct list_node *table = Ak_GetNth_L2(3, row);
            struct list_node *right = Ak_GetNth_L2(4, row);
            if (((int) *user->data == user_id) && (table_id == (int) * table->data) && (table_id != -1) && (strcmp(right->data, right) == 0)) {
                int id = (int) * row->next->data;
                // strange, but it works! :)
                id = id + 1;
                Ak_Insert_New_Element_For_Update(TYPE_INT, &id, "AK_user_right", "obj_id", row_root, 1);
                result = Ak_delete_row(row_root);
            }//if
            i++;

            Ak_DeleteAll_L3(&row_root);
            AK_free(row);
        }
    }

    if (result == EXIT_ERROR) {
        AK_EPI;
        return EXIT_ERROR;
    }

    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Jurica Hlevnjak
 * @brief Revokes ALL user's privileges on ALL tables (for DROP user)
 * @param username name of user from whom we want to revoke all privileges
 * @return EXIT_SUCCESS if privilege is revoked, EXIT_ERROR if it isn't
 */
int AK_revoke_all_privileges_user(char *username) {
    AK_PRO;
    int user_id = AK_user_get_id(username);
    int result;

    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);
    int i = 0;

    struct list_node *row;

    while ((row = (struct list_node *) AK_get_row(i, "AK_user_right")) != NULL) {
        struct list_node *user = Ak_GetNth_L2(2, row);
        if ((int) *user->data == user_id) {
            Ak_Insert_New_Element_For_Update(TYPE_INT, &user_id, "AK_user_right", "user_id", row_root, 1);
            result = Ak_delete_row(row_root);
        }
        i++;

        Ak_DeleteAll_L3(&row_root);
        AK_free(row);
    }

    if (result == EXIT_ERROR) {
        AK_EPI;
        return EXIT_ERROR;
    }

    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Kristina Takač.
 * @brief Grants privilege to given group on given table
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
            struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
            Ak_Init_L3(&row_root);
            privilege_id = AK_get_id();

            Ak_Insert_New_Element(TYPE_INT, &privilege_id, "AK_group_right", "obj_id", row_root);
            Ak_Insert_New_Element(TYPE_INT, &group_id, "AK_group_right", "group_id", row_root);
            Ak_Insert_New_Element(TYPE_INT, &table_id, "AK_group_right", "table_id", row_root);
            Ak_Insert_New_Element(TYPE_VARCHAR, &rights[i * 10], "AK_group_right", "right_type", row_root);
            Ak_insert_row(row_root);

            AK_free(row_root);
        }
    } else {
        struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
        Ak_Init_L3(&row_root);

        privilege_id = AK_get_id();
        Ak_Insert_New_Element(TYPE_INT, &privilege_id, "AK_group_right", "obj_id", row_root);
        Ak_Insert_New_Element(TYPE_INT, &group_id, "AK_group_right", "group_id", row_root);
        Ak_Insert_New_Element(TYPE_INT, &table_id, "AK_group_right", "table_id", row_root);
        Ak_Insert_New_Element(TYPE_VARCHAR, right, "AK_group_right", "right_type", row_root);
        Ak_insert_row(row_root);
        AK_free(row_root);
    }

    AK_EPI;
    return privilege_id;
}

/**
 * @author Kristina Takač, updated by Mario Peroković - added comparing by table id
 * @brief Revokes group's privilege on the given table 
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
        printf("Revoke  %s %s %s", groupname, table, right);

        struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
        Ak_Init_L3(&row_root);
        int i = 0;

        struct list_node *row;

        while ((row = (struct list_node *) AK_get_row(i, "AK_group_right")) != NULL) {
            struct list_node *group = Ak_GetNth_L2(2, row);
            struct list_node *table = Ak_GetNth_L2(3, row);
            if ((int) * group->data == group_id && (int) * table->data == table_id) {
                Ak_Insert_New_Element_For_Update(TYPE_INT, &group_id, "AK_group_right", "group_id", row_root, 1);
                result = Ak_delete_row(row_root);
            }
            i++;

            Ak_DeleteAll_L3(&row_root);
            AK_free(row);
        }
    } else {
        struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
        Ak_Init_L3(&row_root);
        int i = 0;

        struct list_node *row;

        while ((row = (struct list_node *) AK_get_row(i, "AK_group_right")) != NULL) {
            struct list_node *group = Ak_GetNth_L2(2, row);
            struct list_node *table = Ak_GetNth_L2(3, row);
            struct list_node *right = Ak_GetNth_L2(4, row);
            if (((int) * group->data == group_id) && (table_id == (int) * table->data) && (strcmp(right->data, right))) {
                int id = (int) * row->next->data;
                Ak_Insert_New_Element_For_Update(TYPE_INT, &id, "AK_group_right", "obj_id", row_root, 1);
                result = Ak_delete_row(row_root);
            }
            i++;

            Ak_DeleteAll_L3(&row_root);
            AK_free(row);
        }
    }

    if (result == EXIT_ERROR) {
        AK_EPI;
        return EXIT_ERROR;
    }
    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Jurica Hlevnjak
 * @brief Revokes ALL privileges from the desired group on ALL tables (needed for DROP group)
 * @param groupname name of group from which we want to revoke all privileges
 * @return EXIT_SUCCESS if privilege is revoked, EXIT_ERROR if it isn't
 */
int AK_revoke_all_privileges_group(char *groupname) {
    AK_PRO;
    int group_id = AK_group_get_id(groupname);
    int result;

    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);
    int i = 0;

    struct list_node *row;

    while ((row = (struct list_node *) AK_get_row(i, "AK_group_right")) != NULL) {
        struct list_node *group = Ak_GetNth_L2(2, row);
        if ((int) *group->data == group_id) {
            Ak_Insert_New_Element_For_Update(TYPE_INT, &group_id, "AK_group_right", "group_id", row_root, 1);
            result = Ak_delete_row(row_root);
        }
        i++;

        Ak_DeleteAll_L3(&row_root);
        AK_free(row);
    }

    if (result == EXIT_ERROR) {
        AK_EPI;
        return EXIT_ERROR;
    }

    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Kristina Takač, updated by Mario Peroković, added verifying the existence of user in the group, updated by Maja Vračan 
 * @brief Puts the desired user in the given group
 * @param *user username of user which will be put in group
 * @param *group name of group in which user will be put
 * @return EXIT_SUCCESS or EXIT_ERROR if the user is already in the group
 */
int AK_add_user_to_group(char *user, char *group) {
    AK_PRO;
    int user_id = AK_user_get_id(user);
    int group_id = AK_group_get_id(group);

    //if group doesn't exist
    if (group_id == -1) {
        AK_EPI;
        return EXIT_ERROR;
    }

    int i = 0;

    struct list_node *row;

    while ((row = (struct list_node *) AK_get_row(i, "AK_user_group")) != NULL) {

        // if user is already in group, return error
        if (user_id == (int) *row->next->data) {
            AK_EPI;
            return EXIT_ERROR;
        }
        i++;
    }

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
 * @author Jurica Hlevnjak, update by Lidija Lastavec
 * @brief Removes user from all groups. Used for DROP user.
 * @param user name of user
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_remove_user_from_all_groups(char *user) {
    AK_PRO;

    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);

    int user_id = AK_user_get_id(user);
    int i = 0;
    int result;

    struct list_node *row;

    while ((row = (struct list_node *) AK_get_row(i, "AK_user_group")) != NULL) {
        struct list_node *user = Ak_GetNth_L2(1, row);
        if (user_id == (int) *user->data) {
            Ak_Insert_New_Element_For_Update(TYPE_INT, &user_id, "AK_user_group", "user_id", row_root, 1);
            result = Ak_delete_row(row_root);
            if (result == EXIT_ERROR) {
                printf("User '%s' is NOT in any group!\n", user);
                AK_EPI;
                return EXIT_ERROR;
            }
        }
        i++;

        Ak_DeleteAll_L3(&row_root);
        AK_free(row);
    }

    //if (result == EXIT_ERROR) return EXIT_ERROR;
    printf("User '%s' is removed from all groups!\n", user);
    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Jurica Hlevnjak, update by Lidija Lastavec
 * @brief Function removes all users from group. Used for DROP group.
 * @param group name of group
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_remove_all_users_from_group(char *group) {
    AK_PRO;

    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);

    int group_id = AK_group_get_id(group);
    int i = 0;
    int result;

    struct list_node *row;

    while ((row = (struct list_node *) AK_get_row(i, "AK_user_group")) != NULL) {
        struct list_node *group = Ak_GetNth_L2(2, row);
        if (group_id == (int) *group->data) {
            Ak_Insert_New_Element_For_Update(TYPE_INT, &group_id, "AK_user_group", "group_id", row_root, 1);
            result = Ak_delete_row(row_root);

        }
        i++;
        if (result == EXIT_ERROR) {
            printf("Group '%s' has NOT any user!\n", group);
            AK_EPI;
            return EXIT_ERROR;
        }

        Ak_DeleteAll_L3(&row_root);
        AK_free(row);
    }

    // if (result == EXIT_ERROR) return EXIT_ERROR;
    printf("Users deleted from group '%s'!\n", group);
    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Kristina Takač, updated by Marko Flajšek
 * @brief Checks whether the given user has a right for the given operation on the given table
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

    struct list_node *row;

    if (strcmp(privilege, "ALL") == 0) {

        int checking_privileges[4] = {0, 0, 0, 0};
        char found_privilege[10];
        while ((row = (struct list_node *) AK_get_row(i, "AK_user_right")) != NULL) {

            struct list_node *username_elem = Ak_GetNth_L2(2, row);
            struct list_node *table_elem = Ak_GetNth_L2(3, row);
            if (((int) * username_elem->data == user_id) && (table_id == (int) * table_elem->data)) {

                struct list_node *privilege_elem = Ak_GetNth_L2(4, row);
                strcpy(found_privilege, privilege_elem->data);
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
            printf("User %s has all privileges in the %s table\n", username, table);
            AK_EPI;
            return EXIT_SUCCESS;
        }

        i = 0;
        while ((row = (struct list_node *) AK_get_row(i, "AK_user_group")) != NULL) {
            while ((row = (AK_list *) AK_get_row(i, "AK_user_group")) != NULL) {
                struct list_node *user = Ak_GetNth_L2(1, row);
                if (user_id == (int) * user->data) {
                    struct list_node *group = Ak_GetNth_L2(2, row);
                    groups[number_of_groups] = (int) * group->data;
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
                    struct list_node *group_elem = Ak_GetNth_L2(2, row);
                    struct list_node *table_elem = Ak_GetNth_L2(3, row);
                    struct list_node *privilege_elem = Ak_GetNth_L2(4, row);

                    if ((groups[i] == (int) * group_elem->data) && (table_id == (int) * table_elem->data)) {
                        strcpy(found_privilege, privilege_elem->data);
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
                printf("User %s has all privileges in the %s table\n", username, table);
                AK_EPI;
                return EXIT_SUCCESS;
            }
        }
    }// if privilege is not ALL
    else {

        while ((row = (struct list_node *) AK_get_row(i, "AK_user_right")) != NULL) {
            struct list_node *username_elem = Ak_GetNth_L2(2, row);
            struct list_node *table_elem = Ak_GetNth_L2(3, row);
            struct list_node *privilege_elem = Ak_GetNth_L2(4, row);

            if (((int) * username_elem->data == user_id) && (table_id == (int) * table_elem->data) && (strcmp(privilege_elem->data, privilege) == 0)) {

                has_right = 1;
                printf("User %s has the right to %s data in the %s table", username, privilege, table);
                AK_EPI;
                return EXIT_SUCCESS;
            }
            i++;
            AK_free(row);
        }
        i = 0;
        while ((row = (struct list_node *) AK_get_row(i, "AK_user_group")) != NULL) {
            struct list_node *user = Ak_GetNth_L2(1, row);

            if (user_id == (int) * user->data) {
                struct list_node *group = Ak_GetNth_L2(2, row);
                groups[number_of_groups] = (int) * group->data;
                number_of_groups++;
            }
            i++;
            AK_free(row);
        }
        for (i = 0; i < number_of_groups; i++) {
            int j = 0;

            while ((row = (struct list_node *) AK_get_row(j, "AK_group_right")) != NULL) {
                struct list_node *groups_elem = Ak_GetNth_L2(2, row);
                struct list_node *table_elem = Ak_GetNth_L2(3, row);
                struct list_node *privilege_elem = Ak_GetNth_L2(4, row);
                if ((groups[i] == (int) * groups_elem->data) && (table_id == (int) * table_elem->data) && (strcmp(privilege_elem->data, privilege) == 0)) {
                    has_right = 1;
                    printf("User %s has the right to %s data in the %s table\n", username, privilege, table);
                    AK_EPI;
                    return EXIT_SUCCESS;
                }
                j++;
                AK_free(row);
            }
        }
    }

    printf("User %s has no right to %s data in the %s table\n", username, privilege, table);
    AK_EPI;
    return EXIT_ERROR;
}

/**
 * @author Jurica Hlevnjak, updated by Lidija Lastavec
 * @brief Checks if the user has any privileges or belongs to any group. Used in drop user for restriction.
 * @param user name of user
 * @return EXIT_ERROR or EXIT_SUCCESS
 */
int AK_check_user_privilege(char *user) {
    AK_PRO;
    int user_id = AK_user_get_id(user);
    int i = 0;

    struct list_node *row;
    int privilege = 0;

    while ((row = (struct list_node *) AK_get_row(i, "AK_user_right")) != NULL) {
        struct list_node *user_elem = Ak_GetNth_L2(2, row);
        if ((int) *user_elem->data == user_id) {
            privilege = 1;
            printf("User %s has some privileges!", user);
            AK_EPI;
            return EXIT_SUCCESS;
        }
        i++;
        AK_free(row);
    }

    int j = 0;
    while ((row = (struct list_node *) AK_get_row(j, "AK_user_group")) != NULL) {
        struct list_node *user_elem = Ak_GetNth_L2(1, row);
        if ((int) *user_elem->data == user_id) {
            privilege = 1;
            printf("User %s belongs to some group", user);
            AK_EPI;
            return EXIT_SUCCESS;
        }
        j++;
        AK_free(row);
    }

    if (privilege == 0) {
        printf("User %s hasn't got any privileges!", user);
        AK_EPI;
        return EXIT_ERROR;
    }

    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Jurica Hlevnjak, updated by Lidija Lastavec, updated by Marko Flajšek
 * @brief Checks if the group has any privileges. Used in drop group for restriction.
 * @param group name of group
 * @return EXIT_ERROR or EXIT_SUCCESS
 */
int AK_check_group_privilege(char *group) {
    AK_PRO;
    int group_id = AK_group_get_id(group);
    int i = 0;

    struct list_node *row;
    int privilege = 0;

    while ((row = (struct list_node *) AK_get_row(i, "AK_group_right")) != NULL) {
        struct list_node *group_elem = Ak_GetNth_L2(2, row);
        if ((int) *group_elem->data == group_id) {
            privilege = 1;
            printf("Group %s has some privileges!\n", group);
            AK_EPI;
            return EXIT_SUCCESS;
        }
        i++;
        AK_free(row);
    }

    /*
    int j = 0;
    while ((row = (struct list_node *) AK_get_row(j, "AK_user_group")) != NULL) {
        struct list_node *group = Ak_GetNth_L2(2, row);
        if ((int) *group->data == group_id) {
            privilege = 1;
            AK_EPI;
            return EXIT_SUCCESS;
        }
        j++;
        AK_free(row);
    }
     */

    if (privilege == 0) {
        printf("Group %s hasn't got any privileges!\n", group);
        AK_EPI;
        return EXIT_ERROR;
    }

    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Kristina Takač, updated by Tomislav Ilisevic, updated by Lidija Lastavec, updated by Marko Flajšek
 * @brief Function that tests all the previous functions
 * @return no return value                                                     
 */
void AK_privileges_test() {
    AK_PRO;

    int successful[18] = {0};

    printf("\n\n");
    printf("||====================================================================|| \n");
    printf("||////////////////////////////////////////////////////////////////////|| \n");
    printf("||========================| privileges.c Tests |======================|| \n");
    printf("||////////////////////////////////////////////////////////////////////|| \n");
    printf("||====================================================================|| \n");


    /******************/
    /* 1. AK_user_add */
    /******************/

    printf("\n1. Test - AK_user_add function - Inserts a new user in the AK_user table\n");
    printf("\nTest data: user1 1111; user2 2222; user3 3333; user4 4444;\n\n");
    printf("Result:\n\n");

    if (AK_user_add("user1", 1111, NEW_ID) == EXIT_ERROR) {
        printf("\nTest 1. - Fail!\n\n");
    } else {
        printf("\nTest 1. - Pass!\n\n");
        successful[0] = 1;
    }

    //adding 3 more users for future tests
    AK_user_add("user2", 2222, NEW_ID);
    AK_user_add("user3", 3333, NEW_ID);
    AK_user_add("user4", 4444, NEW_ID);

    printf("\n");
    AK_print_table("AK_user");

    printf("\n\n||====================================================================|| \n");


    /*********************/
    /* 2. AK_user_rename */
    /*********************/

    printf("\n\n2. Test - AK_user_rename function - Renames a user in the AK_user table\n");
    printf("\nTest data: rename user4 to user5\n\n");
    printf("Result:\n\n");

    if (AK_user_rename("user4", "user5", 4444) == EXIT_ERROR) {
        printf("\nTest 2. - Fail!\n\n");
    } else {
        printf("\nTest 2. - Pass!\n\n");
        successful[1] = 1;
    }

    printf("\n");
    AK_print_table("AK_user");

    printf("\n\n||====================================================================|| \n");


    /*****************************/
    /* 3. AK_user_remove_by_name */
    /*****************************/

    printf("\n\n3. Test - AK_user_remove_by_name function - Removes a user by name from the AK_user table\n");
    printf("\nTest data: remove user5\n\n");

    if (AK_user_remove_by_name("user5") == EXIT_ERROR) {
        printf("\nTest 3. - Fail!\n");
    } else {
        printf("\nTest 3. - Pass!\n");
        successful[2] = 1;
    }

    AK_print_table("AK_user");

    printf("\n\n||====================================================================|| \n");


    /*******************/
    /* 4. AK_group_add */
    /*******************/

    printf("\n\n4. Test - AK_group_add function - Adds a new group in the AK_group table\n");
    printf("\nTest data: group1, group2, group3, group4\n\n");
    printf("Result:\n\n");

    if (AK_group_add("group1", NEW_ID) == EXIT_ERROR) {
        printf("\nTest 4. - Fail!\n\n");
    } else {
        printf("\nTest 4. - Pass!\n\n");
        successful[3] = 1;
    }

    //adding 3 more groups for future tests
    AK_group_add("group2", NEW_ID);
    AK_group_add("group3", NEW_ID);
    AK_group_add("group4", NEW_ID);

    printf("\n");
    AK_print_table("AK_group");

    printf("\n\n||====================================================================|| \n");


    /**********************/
    /* 5. AK_group_rename */
    /**********************/

    printf("\n\n5. Test - AK_group_rename function - Renames a group\n");
    printf("\nTest data: rename group4 to group5\n\n");
    printf("Result:\n\n");

    if (AK_group_rename("group4", "group5") == EXIT_ERROR) {
        printf("\nTest 5. - Fail!\n\n");
    } else {
        printf("\nTest 5. - Pass!\n\n");
        successful[4] = 1;
    }

    AK_print_table("AK_group");

    printf("\n\n||====================================================================|| \n");
    

    /******************************/
    /* 6. AK_group_remove_by_name */
    /******************************/

    printf("\n\n6. Test - AK_group_remove_by_name function - Remove a group by name from the AK_group table\n");
    printf("\nTest data: group5\n");

    if (AK_group_remove_by_name("group5") == EXIT_ERROR) {
        printf("\nTest 6. - Fail!\n\n");
    } else {
        printf("\nTest 6. - Pass!\n\n");
        successful[5] = 1;
    }

    AK_print_table("AK_group");

    printf("\n\n||====================================================================|| \n");


    /***************************/
    /* 7. AK_add_user_to_group */
    /***************************/

    printf("\n\n7. Test - AK_add_user_to_group function - Puts a user into the given group\n");
    printf("\nTest data: add user1 to group1; user2 to group1; user3 to group2, user4 to group3\n\n");
    printf("Result:\n\n");

    if (AK_add_user_to_group("user1", "group1") == EXIT_ERROR) {
        printf("\nTest 7. - Fail!\n\n");
    } else {
        printf("\nTest 7. - Pass!\n\n");
        successful[6] = 1;
    }

    //adding 3 more users to groups for future tests
    AK_add_user_to_group("user2", "group1");
    AK_add_user_to_group("user3", "group2");
    AK_add_user_to_group("user3", "group2");

    printf("\n");
    AK_print_table("AK_user_group");

    printf("\n\n||====================================================================|| \n");


    /****************************************/
    /* 8. AK_grant_privilege_group function */
    /****************************************/

    printf("\n\n8. Test - AK_grant_privilege_group function - Grants privilege to a given group on a given table\n");
    printf("\nTest data: privileges for all groups\n\n");
    printf("Result:\n\n");

    if (AK_grant_privilege_group("group1", "professor", "DELETE") == EXIT_ERROR) {
        printf("\nTest 8. - Fail!\n");
    } else {
        printf("\nTest 8. - Pass!\n");
        successful[7] = 1;
    }

    //adding 4 more privileges for future tests
    AK_grant_privilege_group("group1", "student", "ALL");
    AK_grant_privilege_group("group2", "student", "ALL");
    AK_grant_privilege_group("group2", "professor", "UPDATE");
    AK_grant_privilege_group("group2", "professor2", "DELETE");

    printf("\n");
    AK_print_table("AK_group_right");

    printf("\n\n||====================================================================|| \n");


    /********************************/
    /* 9. AK_revoke_privilege_group */
    /********************************/

    /*
    printf("\n\n9. Test - AK_revoke_privilege_group function - Revokes privilege for a group on a given table\n");
    printf("\n Test data: revoke privilege for group3 on table student\n   Result:\n\n");

    if (AK_revoke_privilege_group("group3", "student", "DELETE") == EXIT_ERROR) {
        printf("\n   Test 9. - Fail!\n");
    } else {
        printf("\n   Test 9. - Pass!\n");
        successful[8] = 1;
    }

    AK_print_table("AK_group_right");
    // if tested data were for group4,student,all - test pass but revoke ALL privileges for group4 on ALL tables
    // tested by Lidija Lastavec - BUG in function AK_revoke_privilege_group 

    printf(" ||====================================================================|| \n");


    /**************************************/
    /* 10. AK_revoke_all_privileges_group */
    /**************************************/

    /*
    printf("\n\n 10. Test - AK_revoke_all_privileges_group function - Revokes ALL privileges for a group on ALL tables\n");
    printf("\n Test data: revoke all privilege for group4 on all tables\n   Result:\n\n");

    if (AK_revoke_all_privileges_group("group4") == EXIT_ERROR) {
        printf("Test 10. - Fail!\n");
    } else {
        printf("Test 10. - Pass!\n");
        successful[9] = 1;
    }

    AK_print_table("AK_group_right");

    printf(" ||====================================================================|| \n");


    /*******************************/
    /* 11. AK_grant_privilege_user */
    /*******************************/

    printf("\n\n11. Test - AK_grant_privilege_user function - Grants privilege to a given user on a given table\n");
    printf("\nTest data: grant privileges for users user1, user2 and user3 on tables student and professor\n\n");
    printf("Result:\n\n");

    if (AK_grant_privilege_user("user1", "student", "UPDATE") == EXIT_ERROR) {
        printf("\nTest 11. - Fail!\n\n");
    } else {
        printf("\nTest 11. - Pass!\n\n");
        successful[10] = 1;
    }

    //adding 5 more privileges for future tests    
    AK_grant_privilege_user("user1", "student", "DELETE");
    AK_grant_privilege_user("user1", "student", "SELECT");
    AK_grant_privilege_user("user1", "professor", "ALL");
    AK_grant_privilege_user("user2", "student", "UPDATE");
    AK_grant_privilege_user("user2", "professor", "SELECT");

    printf("\n");
    AK_print_table("AK_user_right");

    printf("\n\n||====================================================================|| \n");


    /********************************/
    /* 12. AK_revoke_privilege_user */
    /********************************/

    /*
    printf("\n\n 12. Test - AK_revoke_privilege_user function - Revokes user's privilege on a given table\n");
    printf("\n Test data: revoke privileges for user test on table student\n   Result:\n\n");

    if (AK_revoke_privilege_user("test", "student", "UPDATE") == EXIT_ERROR) {
        printf("\n   Test 12. - Fail!\n");
    } else {
        printf("\n   Test 12. - Pass!\n");
        successful[11] = 1;
    }

    // test pass but didnt revoke privilages for user test - Lidija Lastavec
    AK_print_table("AK_user_right");

    printf(" ||====================================================================|| \n");


    /*************************************/
    /* 13. AK_revoke_all_privileges_user */
    /*************************************/

    /*
    printf("\n\n 13. Test - AK_revoke_all_privileges_user function - Revokes ALL user's privileges on ALL tables\n");
    printf("\n Test data: revoke privileges for user kritakac on all tables\n   Result:\n\n");
    if (AK_revoke_all_privileges_user("kritakac") == EXIT_ERROR) {
        printf("\n   Test 13. - Fail!\n");
    } else {
        printf("\n   Test 13. - Pass!\n");
        successful[12] = 1;
    }

    AK_print_table("AK_user_right");
    // test pass but didnt revoke ALL privilages for user kritakac - Lidija Lastavec

    printf(" ||====================================================================|| \n");


    /**************************/
    /* 14. AK_check_privilege */
    /**************************/

    printf("\n\n14. Test - AK_check_privilege function - Checks whether the given user has the right for the given operation on a desired table\n");
    printf("\nTest data: check if user user1 has the right to UPDATE data into the STUDENT table\n\n");
    printf("Result:\n\n");

    if (AK_check_privilege("user1", "student", "UPDATE") == EXIT_ERROR) {
        printf("\nTest 14. - Fail!\n\n");
    } else {
        printf("\nTest 14. - Pass!\n\n");
        successful[13] = 1;
    }

    //test for user without a right
    printf("\nTest data: check if user user1 has right to INSERT data into the PROFESSOR2 table\n\n");
    printf("Result:\n\n");

    if (AK_check_privilege("user1", "professor2", "INSERT") == EXIT_ERROR) {
        printf("\nTest 14. - Pass!\n");
    } else {
        printf("\nTest 14. - Fail!\n");
    }

    printf("\n");
    AK_print_table("AK_user_right");

    printf("\n\n||====================================================================|| \n");

    /********************************/
    /* 15. AK_check_group_privilege */
    /********************************/

    printf("\n\n15. Test - AK_check_group_privilege function - Checks whether the given group has any privileges\n");
    printf("\nTest data: check if group 1 has privileges\n\n");
    printf("Result:\n\n");

    if (AK_check_group_privilege("group1") == EXIT_SUCCESS) {
        printf("\nTest 15. - Pass!\n\n");
        successful[14] = 1;
    } else {
        printf("\nTest 15. - Fail!\n\n");
    }

    // test for NO answer - test PASS!
    printf("\nTest data: group4 - without privileges\n\n");
    printf("Result:\n\n");

    if (AK_check_group_privilege("group4") == EXIT_ERROR) {
        printf("\nTest 15. - Pass!\n");
    } else {
        printf("\nTest 15. - Fail!\n");
    }

    printf("\n");
    AK_print_table("AK_group_right");

    printf("\n\n||====================================================================|| \n");


    /*******************************/
    /* 16. AK_check_user_privilege */
    /*******************************/

    printf("\n\n16. Test - AK_check_user_privilege function - Checks if the given user has any privileges or belongs to any group\n");
    printf("\nTest data: check if user user1 has ANY privileges\n\n");
    printf("Result:\n\n");

    if (AK_check_user_privilege("user1") == EXIT_ERROR) {
        printf("\n\nTest 16. - Fail!\n\n");
    } else {
        printf("\n\nTest 16. - Pass!\n\n");
        successful[15] = 1;
    }

    //test for user without any privileges
    printf("\nTest data: check for user without privileges (user4)\n\n");
    printf("Result:\n\n");
    
    if (AK_check_user_privilege("user4") == EXIT_ERROR) {
        printf("\n\nTest 16. - Pass!\n");
    } else {
        printf("\n\nTest 16. - Fail!\n");
    }

    printf("\n");
    AK_print_table("AK_group_right");

    printf("\n\n||====================================================================|| \n");


    /**************************************/
    /* 17. AK_remove_all_users_from_group */
    /**************************************/

    printf("\n\n17. Test - AK_remove_all_users_from_group function - Removes all users from the given group\n");
    printf("\nTest data: remove users from group2 (with added users)\n\n");
    
    AK_print_table("AK_user_group");
    
    printf("\nResult:\n\n");

    if (AK_remove_all_users_from_group("group2") == EXIT_SUCCESS) {
        printf("\nTest 17. - Pass!\n");
        successful[16] = 1;
    } else {
        printf("\nTest 17. - Fail!\n");
    }

    printf("\n");
    AK_print_table("AK_user_group");

    printf("\n\n||====================================================================|| \n");


    /**************************************/
    /* 18. AK_remove_user_from_all_groups */
    /**************************************/

    printf("\n\n18. Test - AK_remove_user_from_all_groups function - Removes the given user from all groups\n");
    printf("\nTest data: remove user1 (member of group1)\n\n");
    
    AK_print_table("AK_user_group");
    
    printf("\nResult:\n\n");

    if (AK_remove_user_from_all_groups("user1") == EXIT_SUCCESS) {
        printf("\nTest 18. - Pass!\n");
        successful[17] = 1;
    } else {
        printf("\nTest 18. - Fail!\n");
    }

    printf("\n");
    AK_print_table("AK_user_group");

    printf("\n\n||====================================================================|| \n");

    /* END SUMMARY*/

    printf("\nSummary: \n");
    int num = 0;
    int numFail = 0;
    for (num = 0; num < 18; num++) {
        printf("%i. Test: %s \n", (num + 1), (successful[num] == 1 ? "Pass" : "Fail"));
        if (successful[num] == 0) numFail++;
    }

    if (numFail == 0) printf("\nALL TESTS ARE PASSED! \n");
    else printf("\nThere are %i tests that failed.. \n", numFail);

    AK_EPI;
}
