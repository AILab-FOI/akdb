/**
@file drop.c Provides DROP functions
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

// DROP table

#define DROP_TABLE 0

void AK_drop(int type, AK_drop_arguments *drop_arguments) {

    char *tblName;

    switch (type) {
        case DROP_TABLE:

            tblName = (char*) drop_arguments->value;

            if (!AK_table_exist(tblName) == 0) {

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
                char *sys_table;
                sys_table = "AK_relation";

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

                Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, tblName, "AK_relation", "name", row_root, 1);

                Ak_delete_row(row_root);

                
                printf("TABLE %s DROPPED!\n", tblName);

            }

            break;


        default:
            break;
    }
}

void AK_drop_test() {
    printf("=============================================\n");
    printf("==================DROP_TEST==================\n");
    AK_print_table("AK_relation");
    AK_drop_arguments *drop_arguments = malloc(sizeof (AK_drop_arguments));
    drop_arguments->value = "department";
    AK_drop(DROP_TABLE, drop_arguments);
    AK_print_table("department");
    AK_print_table("AK_relation");
    drop_arguments->value = "professor2";
    AK_drop(DROP_TABLE, drop_arguments);
    AK_print_table("professor2");
    AK_print_table("AK_relation");
    printf("================END_DROP_TEST================\n");
}
