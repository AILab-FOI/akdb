#ifndef PRIVILEGES
#define PRIVILEGES

#include "../file/table.h"
#include "../file/id.h"
#include "../rec/archive_log.h"
#include "../auxi/mempro.h"

int AK_user_add(char *username, int *password, int set_id);
int AK_user_get_id(char *username);
int AK_group_add(char *name, int set_id);
int AK_group_get_id(char *name);
int AK_grant_privilege_user(char *username, char *table, char *right);
int AK_revoke_privilege_user(char *username, char *table, char *right);
int AK_revoke_all_privileges_user(char *username);
int AK_grant_privilege_group(char *groupname, char *table, char *right);
int AK_revoke_privilege_group(char *groupname, char *table, char *right);
int AK_revoke_all_privileges_group(char *groupname);
int AK_add_user_to_group(char *user, char *group);
int AK_remove_user_from_all_groups(char *user);
int AK_remove_all_users_from_group(char *group);
int AK_check_privilege(char *username, char *table, char *privilege);
int AK_check_user_privilege(char *user);
int AK_check_group_privilege(char *group);
int AK_group_remove_by_name(char *name);
int AK_user_rename(char *old_name, char *new_name, int *password);
int AK_group_rename(char *old_name, char *new_name);
void AK_privileges_test();

#endif
