/**
@file privileges.h Header file that provides functions and defines for privileges.c
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
#ifndef PRIVILEGES
#define PRIVILEGES

#include "../auxi/test.h"
#include "../file/table.h"
#include "../file/fileio.h"
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
TestResult AK_privileges_test();

#endif
