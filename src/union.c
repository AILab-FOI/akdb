/**
@file union.c Provides functions for relational union operation
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

#include "union.h"
 
/**
 * @brief  Function to make union of the two tables
 * Union is implemented for working with multiple sets of data, i.e. duplicate 
 * tuples can be written in same table (union)
 * @author Dino Laktašić
 * @param srcTable1 - name of the first table
 * @param srcTable2 - name of the second table
 * @param dstTable - name of the new table
 * @return if success returns EXIT_SUCCESS, else returns EXIT_ERROR
 */
int AK_union(char *srcTable1, char *srcTable2, char *dstTable) {
	table_addresses *src_addr1 = (table_addresses*) get_table_addresses(srcTable1);
    table_addresses *src_addr2 = (table_addresses*) get_table_addresses(srcTable2);

    int startAddress1 = src_addr1->address_from[0];
    int startAddress2 = src_addr2->address_from[0];
	
    if ((startAddress1 != 0) && (startAddress2 != 0)) {
        register int i, j, k;
        i = j = k = 0;

        AK_mem_block *tbl1_temp_block = (AK_mem_block *) AK_get_block(startAddress1);
        AK_mem_block *tbl2_temp_block = (AK_mem_block *) AK_get_block(startAddress2);
        
        int num_att = AK_check_tables_scheme(tbl1_temp_block, tbl2_temp_block, "Union");

		int address, type, size;
        char data[MAX_VARCHAR_LENGTH];

		//initialize new segment
        AK_header *header = (AK_header *) malloc(num_att * sizeof (AK_header));
        memcpy(header, tbl1_temp_block->block->header, num_att * sizeof (AK_header));
        AK_initialize_new_segment(dstTable, SEGMENT_TYPE_TABLE, header);
        free(header);

        list *row_root = (list *) malloc(sizeof (list));
		
		//writing first block or table to new segment
		for (i = 0; src_addr1->address_from[i] != 0; i++) {
            startAddress1 = src_addr1->address_from[i];

            if (startAddress1 != 0) {

                //BLOCK: for each block in table1 extent
                for (j = startAddress1; j < src_addr1->address_to[i]; j++) {
                    tbl1_temp_block = (AK_mem_block *) AK_get_block(j); //read block from first table

                    //if there is data in the block
                    if (tbl1_temp_block->block->free_space != 0) {

						for (k = 0; k < DATA_BLOCK_SIZE; k++) {
							if (tbl1_temp_block->block->tuple_dict[k].type == FREE_INT)
								break;
								
							address = tbl1_temp_block->block->tuple_dict[k].address;
							size = tbl1_temp_block->block->tuple_dict[k].size;
							type = tbl1_temp_block->block->tuple_dict[k].type;

							memset(data, '\0', MAX_VARCHAR_LENGTH);
							memcpy(data, tbl1_temp_block->block->data + address, size);
						
							InsertNewElementForUpdate(type, data, dstTable, tbl1_temp_block->block->header[k % num_att].att_name, row_root, 0);
							
							if ((k + 1) % num_att == 0 && k != 0) {
								insert_row(row_root);
								DeleteAllElements(row_root);
							}
						}
					}
				}
			} else break;
		}
		
		//writing first block or table to new segment
		for (i = 0; src_addr2->address_from[i] != 0; i++) {
            startAddress2 = src_addr2->address_from[i];

            if (startAddress2 != 0) {

                //BLOCK: for each block in table2 extent
                for (j = startAddress2; j < src_addr2->address_to[i]; j++) {
                    tbl2_temp_block = (AK_mem_block *) AK_get_block(j); //read block from second table

                    //if there is data in the block
                    if (tbl2_temp_block->block->free_space != 0) {
				
						for (k = 0; k < DATA_BLOCK_SIZE; k++) {
							if (tbl2_temp_block->block->tuple_dict[k].type == FREE_INT)
								break;
						
							address = tbl2_temp_block->block->tuple_dict[k].address;
							size = tbl2_temp_block->block->tuple_dict[k].size;
							type = tbl2_temp_block->block->tuple_dict[k].type;
							
							memset(data, '\0', MAX_VARCHAR_LENGTH);
							memcpy(data, tbl2_temp_block->block->data + address, size);

							InsertNewElementForUpdate(type, data, dstTable, tbl2_temp_block->block->header[k % num_att].att_name, row_root, 0);
							
							if ((k + 1) % num_att == 0) {
								insert_row(row_root);
								DeleteAllElements(row_root);
							}
						}
					}
				}
			} else break;
		}
		
		free(src_addr1);
        free(src_addr2);
		dbg_messg(LOW, REL_OP, "UNION_TEST_SUCCESS\n\n");
		return EXIT_SUCCESS;
	} else {
		dbg_messg(LOW, REL_OP, "\nAK_union: Table/s doesn't exist!");
        free(src_addr1);
        free(src_addr2);
		return EXIT_ERROR;
	}
}

/**
 * @brief  Function for union operator testing
 * @author Dino Laktašić
 */
void op_union_test() {
    printf("\n********** UNION TEST **********\n\n");

    AK_union("professor", "assistant", "union_test");
    AK_print_table("union_test");
}
