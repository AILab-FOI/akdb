#ifndef PRIVILEGES
#define PRIVILEGES

#include "table.h"

#endif

int AK_user_add(char *username, int password);
int AK_user_get_id(char *username);
int AK_group_add(char *name);
int AK_group_get_id(char *name);
int AK_grant_privilege_user(char *username, char *table, char *right);
void AK_revoke_privilege_user(char *username, char *table, char *right);
int AK_grant_privilege_group(char *groupname, char *table, char *right);
void AK_revoke_privilege_group(char *groupname, char *table, char *right);
int AK_add_user_to_group(char *user, char *group);
int AK_check_privilege(char *username, char *table, char *privilege);
void AK_privileges_test();
