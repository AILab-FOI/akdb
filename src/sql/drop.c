/**
 *@author 
 *@file drop.c Provides DROP functions
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
 17 */

#include "drop.h"
#include "view.h"

// DROP table

#define DROP_TABLE 0
#define DROP_INDEX 1
#define DROP_VIEW 2

char *system_catalog[] = {
    "AK_relation",
    "AK_attribute",
    "AK_index",
    "AK_view",
    "AK_sequence",
    "AK_function",
    "AK_function_arguments",
    "AK_trigger",
    "AK_trigger_conditions",
    "AK_db",
    "AK_db_obj",
    "AK_user",
    "AK_group",
    "AK_user_group",
    "AK_user_right",
    "AK_group_right",
    "AK_constraints_between",
    "AK_constraints_not_null",
    "AK_constraints_unique",
    "AK_reference"
};

void AK_drop(int type, AK_drop_arguments *drop_arguments) {

    char *tblName;
    char *sys_table;

    switch (type) {
        case DROP_TABLE:

            sys_table = "AK_relation";
            tblName = (char*) drop_arguments->value;

            int status = 0;
            int x = 0;
            for (x = 0; x < 20; x++) {
                if (strcmp(tblName, system_catalog[x]) == 0) {
                    status = 1;
                    printf("Table %s is a System catalog table! DROP can not be preformed!\n", tblName);
                    break;
                }
            }

            if ((!AK_if_exist(tblName, sys_table) == 0) && (status != 1)) {

                AK_drop_helper_function(tblName, sys_table);

                printf("TABLE %s DROPPED!\n", tblName);

            }

            break;

        case DROP_INDEX:

            tblName = (char*) drop_arguments->value;
            sys_table = "AK_index";

            if (!AK_if_exist(tblName, sys_table) == 0) {
                AK_drop_helper_function(tblName, sys_table);

                printf("INDEX %s DROPPED!\n", tblName);
            }

            break;

        case DROP_VIEW:

            tblName = (char*) drop_arguments->value;
            sys_table = "AK_view";

            if ((AK_table_empty(sys_table)) || (AK_if_exist(tblName, sys_table) == 0)) {
                printf("View %s does not exist!\n", tblName);
            } else {
                //AK_drop_helper_function(tblName, sys_table);
                AK_view_remove_by_name(tblName);
                printf("VIEW %s DROPPED!\n", tblName);
            }

            break;

        default:
            break;
    }
}

void AK_drop_helper_function(char *tblName, char *sys_table) {

    table_addresses *addresses;

    //addresses = AK_get_segment_addresses(tblName, SEGMENT_TYPE_TABLE);
    addresses = (table_addresses*) AK_get_table_addresses(tblName);

    AK_mem_block *mem_block;

    int from = 0, to = 0, i = 0, j = 0, c;

    for (j = 0; j < MAX_EXTENTS_IN_SEGMENT; j++) {
        if (addresses->address_from != 0) {
            from = addresses->address_from[j];
            to = addresses->address_to[j];

            if (from == 0 || to == 0) break;
            for (i = from; i <= to; i++) {
                mem_block = (AK_mem_block *) AK_get_block(i);
                mem_block->block->type = BLOCK_TYPE_FREE;

                for (c = 0; c < DATA_BLOCK_SIZE; c++) {
                    mem_block->block->tuple_dict[c].type = FREE_INT;
                    mem_block->block->data[c] = FREE_CHAR;
                }
            }
        } else break;
    }


    //int segmentType = SEGMENT_TYPE_TABLE;
    int data_adr = 0;
    int data_size = 0;
    int data_type = 0;
    int address_sys;
    char name_sys[MAX_ATT_NAME];

    /*
    switch (segmentType) {
    case SEGMENT_TYPE_TABLE:
    sys_table = "AK_relation";
    break;
    case SEGMENT_TYPE_INDEX:
    sys_table = "AK_index";
    break;
    }
     */

    AK_mem_block *mem_block2 = (AK_mem_block *) AK_get_block(0);

    for (i = 0; i < DATA_BLOCK_SIZE; i++) {
        memset(name_sys, 0, MAX_ATT_NAME);

        if (mem_block2->block->tuple_dict[i].address == FREE_INT) {
            break;
        }

        data_adr = mem_block2->block->tuple_dict[i].address;
        data_size = mem_block2->block->tuple_dict[i].size;
        data_type = mem_block2->block->tuple_dict[i].type;
        memcpy(name_sys, mem_block2->block->data + data_adr, data_size);

        i++;
        data_adr = mem_block2->block->tuple_dict[i].address;
        data_size = mem_block2->block->tuple_dict[i].size;
        data_type = mem_block2->block->tuple_dict[i].type;
        memcpy(&address_sys, mem_block2->block->data + data_adr, data_size);

        if (strcmp(name_sys, sys_table) == 0) {
            break;
        }
    }

    mem_block2 = (AK_mem_block *) AK_get_block(address_sys);
    table_addresses *addresses2;

    //addresses2 = (table_addresses *) malloc(sizeof (table_addresses));
    addresses2 = (table_addresses*) AK_get_table_addresses(tblName);

    //memset(addresses2->address_from, 0, MAX_EXTENTS_IN_SEGMENT);
    //memset(addresses2->address_to, 0, MAX_EXTENTS_IN_SEGMENT);

    for (i = 0; i < MAX_EXTENTS_IN_SEGMENT; i++) {
        addresses2->address_from[i] = 0;
        addresses2->address_to[i] = 0;
    }


    char name[MAX_VARCHAR_LENGTH];
    //int address_from;
    //int address_to;

    for (i = 0; i < DATA_BLOCK_SIZE; i++) {
        if (mem_block2->block->tuple_dict[i].type == FREE_INT)
            break;
        i++;
        memcpy(name, &(mem_block2->block->data[mem_block2->block->tuple_dict[i].address]), mem_block2->block->tuple_dict[i].size);
        name[ mem_block2->block->tuple_dict[i].size] = '\0';
        if (strcmp(name, tblName) == 0) {
            i++;
            mem_block2->block->data[mem_block2->block->tuple_dict[i].address] = 0;
            i++;
            mem_block2->block->data[mem_block2->block->tuple_dict[i].address] = 0;
        }

        /*   i++;
             memcpy(&address_from, &(mem_block2->block->data[mem_block2->block->tuple_dict[i].address]), mem_block2->block->tuple_dict[i].size);
             i++;
             memcpy(&address_to, &(mem_block2->block->data[mem_block2->block->tuple_dict[i].address]), mem_block2->block->tuple_dict[i].size);

             //if found the table that addresses we need
             if (strcmp(name, tblName) == 0) {
                 addresses2->address_from[j] = address_from;
                 addresses2->address_to[j] = address_to;
                 j++;
             }
             if (segmentType == SEGMENT_TYPE_INDEX) {
                 i += 2;
             }
         */

    }

    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list_elem));
    Ak_Init_L(row_root);
    Ak_DeleteAll_L(row_root);

    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, tblName, sys_table, "name", row_root, 1);

    Ak_delete_row(row_root);
}

int AK_if_exist(char *tblName, char *sys_table) {
    int num_rows = AK_get_num_records(sys_table);
    int a;
    int exist = 0;

    for (a = 0; a < num_rows; a++) {
        AK_list_elem el;
        el = AK_get_tuple(a, 1, sys_table);
        // printf("  Element %s !", el->data);
        if (!strcmp(tblName, el->data)) {
            exist = 1;
        }
    }
    return exist;
}

void AK_drop_test() {
    printf("=========================================================\n");
    printf("========================DROP_TEST========================\n");
    AK_print_table("AK_relation");
    AK_drop_arguments *drop_arguments = malloc(sizeof (AK_drop_arguments));
    // drop table
    drop_arguments->value = "department";
    AK_drop(DROP_TABLE, drop_arguments);
    AK_print_table("department");
    AK_print_table("AK_relation");
    // drop system catalog table
    drop_arguments->value = "AK_attribute";
    AK_drop(DROP_TABLE, drop_arguments);
    AK_print_table("AK_attribute");
    AK_print_table("AK_relation");
    // drop view
    AK_print_table("AK_view");
    drop_arguments->value = "view300";
    AK_drop(DROP_VIEW, drop_arguments);
    AK_print_table("AK_view");
    // drop hash index
    drop_arguments->value = "student_hash_index";
    AK_print_table("AK_index");
    // AK_print_table("student_hash_index");
    AK_drop(DROP_INDEX, drop_arguments);
    AK_print_table("student_hash_index");
    // drop hash index
    drop_arguments->value = "assistantfirstname_bmapIndex";
    AK_print_table("AK_index");
    // AK_print_table("assistantfirstname_bmapIndex");
    AK_drop(DROP_INDEX, drop_arguments);
    AK_print_table("assistantfirstname_bmapIndex");
    AK_print_table("AK_index");
    printf("======================END_DROP_TEST======================\n");
}
