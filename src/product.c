/**
@file product.c Provides functions for relational product operation
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

#include "product.h"

/**
 * @brief  Function to make product of two tables
 * @author Dino Laktašić
 * @param srcTable1 - name of the first table
 * @param srcTable2 - name of the second table
 * @param dstTable - name of the product table
 * @return if success returns EXIT_SUCCESS, else returns EXIT_ERROR
 */
int AK_product(char *srcTable1, char * srcTable2, char * dstTable) {
    table_addresses *src_addr1 = (table_addresses *) get_table_addresses(srcTable1);
    table_addresses *src_addr2 = (table_addresses *) get_table_addresses(srcTable2);

    int startAddress1 = src_addr1->address_from[0];
    int startAddress2 = src_addr2->address_from[0];

    if ((startAddress1 != 0) && (startAddress2 != 0)) {
		//initialize new segment
		int num_att1 = AK_num_attr(srcTable1);
		int num_att2 = AK_num_attr(srcTable2);

        register int i, j, k, l, m, n, o;
        i = j = k = l = 0;

		int address, type, size;
		
        char data1[MAX_VARCHAR_LENGTH];
		char data2[MAX_VARCHAR_LENGTH];
	
		AK_mem_block *tbl1_temp_block = (AK_mem_block *) AK_get_block(startAddress1);
        AK_mem_block *tbl2_temp_block = (AK_mem_block *) AK_get_block(startAddress2);
		
		//Currently it works with headers no longer than MAX_ATTRIBUTES. The same header is written in all allocated table blocks.
		//This is wrong and need to be corrected.
		//If header doesn't fit in the first block than system must write the remain attributes from header to the new block.
		//Correction must be handled in all functions that write, read or count header attributes.
		int head = 0;
		AK_header header[MAX_ATTRIBUTES];
		
		while (strcmp(tbl1_temp_block->block->header[head].att_name, "") != 0) {
            memcpy(&header[head], &tbl1_temp_block->block->header[head], sizeof (tbl1_temp_block->block->header[head]));
            head++;
        }
		
		head = 0;
		while (strcmp(tbl2_temp_block->block->header[head].att_name, "") != 0) {
            memcpy(&header[head + num_att1], &tbl2_temp_block->block->header[head], sizeof (tbl2_temp_block->block->header[head]));
            head++;
        }
		
        AK_initialize_new_segment(dstTable, SEGMENT_TYPE_TABLE, header);

        dbg_messg(LOW, REL_OP, "\nTABLE %s CREATED from %s and %s\n", dstTable, srcTable1, srcTable2);
		dbg_messg(MIDDLE, REL_OP, "\nAK_product: start copying data\n");

        list *row_root = (list *) malloc(sizeof (list));

        //for each extent in table1 that contains blocks needed for join
        for (i = 0; i < src_addr1->address_from[i] != 0; i++) {
            startAddress1 = src_addr1->address_from[i];

            if (startAddress1 != 0) {
                dbg_messg(MIDDLE, REL_OP, "\nProduct: copy extent1: %d\n", i);

                //for each block in table1 extent
                for (j = startAddress1; j < src_addr1->address_to[i]; j++) {
                    dbg_messg(MIDDLE, REL_OP, "Product: copy block1: %d\n", j);

                    tbl1_temp_block = (AK_mem_block *) AK_get_block(j);

                    //if there is data in the block
                    if (tbl1_temp_block->block->free_space != 0) {
                        //for each extent in table2 that contains blocks needed for join
                        for (k = 0; k < src_addr2->address_from[k] != 0; k++) {
                            startAddress2 = src_addr2->address_from[k];

                            if (startAddress2 != 0) {
                                dbg_messg(MIDDLE, REL_OP, "Product: copy extent2: %d\n", k);

                                //for each block in table2 extent
                                for (l = startAddress2; l < src_addr2->address_to[k]; l++) {
                                    dbg_messg(MIDDLE, REL_OP, "Product: copy block2: %d\n", l);

                                    tbl2_temp_block = (AK_mem_block *) AK_get_block(l);

                                    //if there is data in the block
                                    if (tbl2_temp_block->block->free_space != 0) {
										
										//TUPLE_DICTS: for each tuple_dict in the block
                                        for (m = 0; m < DATA_BLOCK_SIZE; m += num_att1) {
                                            if (tbl1_temp_block->block->tuple_dict[m + 1].type == FREE_INT)
                                                break;

											//TUPLE_DICTS: for each tuple_dict in the block
                                            for (n = 0; n < DATA_BLOCK_SIZE; n += num_att2) {
                                                if (tbl2_temp_block->block->tuple_dict[n + 1].type == FREE_INT)
                                                    break;
													
												//for each element in row
												for (o = 0; o < num_att1; o++) {
													address = tbl1_temp_block->block->tuple_dict[m + o].address;
													size = tbl1_temp_block->block->tuple_dict[m + o].size;
													type = tbl1_temp_block->block->tuple_dict[m + o].type;
													
													memset(data1, '\0', MAX_VARCHAR_LENGTH);
													memcpy(data1, &(tbl1_temp_block->block->data[address]), size);
													
													InsertNewElementForUpdate(type, data1, dstTable, tbl1_temp_block->block->header[o].att_name, row_root, 0);
												}
												
												//for each element in row
												for (o = 0; o < num_att2; o++) {
													address = tbl2_temp_block->block->tuple_dict[n + o].address;
													size = tbl2_temp_block->block->tuple_dict[n + o].size;
													type = tbl2_temp_block->block->tuple_dict[n + o].type;
													
													memset(data2, '\0', MAX_VARCHAR_LENGTH);
													memcpy(data2, &(tbl2_temp_block->block->data[address]), size);

													InsertNewElementForUpdate(type, data2, dstTable, tbl2_temp_block->block->header[o].att_name, row_root, 0);
												}
												
												insert_row(row_root);
												DeleteAllElements(row_root);
											}
										}
                                    }
                                }
                            } else break;
                        }
                    }
                }
            } else break;
        }
		
        free(src_addr1);
        free(src_addr2);
		dbg_messg(LOW, REL_OP, "PRODUCT_TEST_SUCCESS\n\n");
        return EXIT_SUCCESS;
    } else {
        dbg_messg(LOW, REL_OP, "\n AK_product: Table/s doesn't exist!");
        free(src_addr1);
        free(src_addr2);
        return EXIT_ERROR;
    }
}

/**
 * @brief  Function for product operator testing
 * @author Dino Laktašić
 */
void op_product_test() {
    printf("\n********** PRODUCT TEST **********\n\n");

    AK_product("employee", "department", "product_test");
    AK_print_table("product_test");
}