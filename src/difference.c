/**
@file difference.c Provides functions for relational difference operation
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
#include "difference.h"

/**
 * @brief  Function to make difference of the two tables
 * @author Dino Laktašić
 * @param srcTable1 - name of the first table
 * @param srcTable2 - name of the second table
 * @param dstTable - name of the new table
 * @return if success returns EXIT_SUCCESS, else returns EXIT_ERROR
 */
int AK_difference(char *srcTable1, char *srcTable2, char *dstTable) {
	table_addresses *src_addr1 = (table_addresses*) AK_get_table_addresses(srcTable1);
    table_addresses *src_addr2 = (table_addresses*) AK_get_table_addresses(srcTable2);

    int startAddress1 = src_addr1->address_from[0];
    int startAddress2 = src_addr2->address_from[0];
	
    if ((startAddress1 != 0) && (startAddress2 != 0)) {
        register int i, j, k, l, m, n, o;
        i = j = k = l = 0;

        AK_mem_block *tbl1_temp_block = (AK_mem_block *) AK_get_block(startAddress1);
        AK_mem_block *tbl2_temp_block = (AK_mem_block *) AK_get_block(startAddress2);
		
		int num_att = AK_check_tables_scheme(tbl1_temp_block, tbl2_temp_block, "Difference");
		 
		if (num_att == EXIT_ERROR) {
			return EXIT_ERROR;
		}

		int address, type, size;
		int different, num_rows, temp_int;
		different = num_rows = temp_int = 0;
		
		float temp_float = 0;
		
        char data1[MAX_VARCHAR_LENGTH];
		char data2[MAX_VARCHAR_LENGTH];
		
		//initialize new segment
        AK_header *header = (AK_header *) malloc(num_att * sizeof (AK_header));
        memcpy(header, tbl1_temp_block->block->header, num_att * sizeof (AK_header));
        AK_initialize_new_segment(dstTable, SEGMENT_TYPE_TABLE, header);
        free(header);

        AK_list *row_root = (AK_list *) malloc(sizeof (AK_list));
		
		for (i = 0; src_addr1->address_from[i] != 0; i++) {
            startAddress1 = src_addr1->address_from[i];

            if (startAddress1 != 0) {

                //BLOCK: for each block in table1 extent
                for (j = startAddress1; j < src_addr1->address_to[i]; j++) {
                    tbl1_temp_block = (AK_mem_block *) AK_get_block(j); //read block from first table

                    //if there is data in the block
                    if (tbl1_temp_block->block->free_space != 0) {
						
						//TABLE2: for each extent in table2
                        for (k = 0; k < src_addr2->address_from[k] != 0; k++) {
                            startAddress2 = src_addr2->address_from[k];

                            if (startAddress2 != 0) {

                                //BLOCK: for each block in table2 extent
                                for (l = startAddress2; l < src_addr2->address_to[k]; l++) {
                                    tbl2_temp_block = (AK_mem_block *) AK_get_block(l);

                                    //if there is data in the block
                                    if (tbl2_temp_block->block->free_space != 0) {
										
										//TUPLE_DICTS: for each tuple_dict in the block
                                        for (m = 0; m < DATA_BLOCK_SIZE; m += num_att) {
                                            if (tbl1_temp_block->block->tuple_dict[m + 1].type == FREE_INT)
                                                break;

											//TUPLE_DICTS: for each tuple_dict in the block
                                            for (n = 0; n < DATA_BLOCK_SIZE; n += num_att) {
                                                if (tbl2_temp_block->block->tuple_dict[n + 1].type == FREE_INT)
                                                    break;
												
												//for each element in row
												for (o = 0; o < num_att; o++) {
													address = tbl1_temp_block->block->tuple_dict[m + o].address;
													size = tbl1_temp_block->block->tuple_dict[m + o].size;
													type = tbl1_temp_block->block->tuple_dict[m + o].type;
													
													switch (type) {
														case TYPE_INT: 
															memcpy(&temp_int, &(tbl1_temp_block->block->data[address]), size);
															sprintf(data1, "%d", temp_int);
															break;
														case TYPE_FLOAT:
															memcpy(&temp_float, &(tbl1_temp_block->block->data[address]), size);
															sprintf(data1, "%f", temp_float);
															break;
														case TYPE_VARCHAR:
														default:
															memset(data1, '\0', MAX_VARCHAR_LENGTH);
															memcpy(data1, &(tbl1_temp_block->block->data[address]), size);
													}
													
													address = tbl2_temp_block->block->tuple_dict[n + o].address;
													size = tbl2_temp_block->block->tuple_dict[n + o].size;
													type = tbl2_temp_block->block->tuple_dict[n + o].type;
													
													switch (type) {
														case TYPE_INT: 
															memcpy(&temp_int, &(tbl2_temp_block->block->data[address]), size);
															sprintf(data2, "%d", temp_int);
															break;
														case TYPE_FLOAT:
															memcpy(&temp_float, &(tbl2_temp_block->block->data[address]), size);
															sprintf(data2, "%f", temp_float);
															break;
														case TYPE_VARCHAR:
														default:
															memset(data2, '\0', MAX_VARCHAR_LENGTH);
															memcpy(data2, &(tbl2_temp_block->block->data[address]), size);
													}
													
													//if they are different
													if (strcmp(data1, data2) != 0) { 
														different++;
														//printf("%s != %s\n", data1, data2);
														break;
													}
												}
												num_rows++;
											}
											//if there is a difference between tuple_dicts
											if (num_rows - different == 0) {
												//printf("Num_rows: %i, Different_rows: %i\n", num_rows, different);
												Ak_DeleteAllL(row_root);

												for (o = 0; o < num_att; o++) {
													address = tbl1_temp_block->block->tuple_dict[m + o].address;
													size = tbl1_temp_block->block->tuple_dict[m + o].size;
													type = tbl1_temp_block->block->tuple_dict[m + o].type;
													
													memset(data1, '\0', MAX_VARCHAR_LENGTH);
													memcpy(data1, tbl1_temp_block->block->data + address, size);

													Ak_Insert_New_Element_For_Update(type, data1, dstTable, tbl1_temp_block->block->header[o].att_name, row_root, 0);
												}

												Ak_insert_row(row_root);
											}
											num_rows = different = 0;
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
		Ak_dbg_messg(LOW, REL_OP, "DIFFERENCE_TEST_SUCCESS\n\n");
		return EXIT_SUCCESS;
	} else {
		Ak_dbg_messg(LOW, REL_OP, "\nAK_difference: Table/s doesn't exist!");
        free(src_addr1);
        free(src_addr2);
		return EXIT_ERROR;
	}
}

/**
 * @brief  Function for difference operator testing
 * @author Dino Laktašić
 */
void Ak_op_difference_test() {
    printf("\n********** DIFFERENCE TEST **********\n\n");

    AK_difference("professor", "assistant", "difference_test");
    AK_print_table("difference_test");
}
