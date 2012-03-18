/**
@file selection.c Provides functions for relational selection operation
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

#include "selection.h"



/**
 * @brief  Implementation of selection
 * @author Matija Šestak.
 * @param *srcTable - source table name
 * @param *dstTable - destination table name
 * @param *expr - list with posfix notation of the logical expression
 * @result int - return EXIT_SUCCESS
 */
int AK_selection(char *srcTable, char *dstTable, AK_list *expr) {
    AK_header *t_header = (AK_header *) AK_get_header(srcTable);
    int num_attr = AK_num_attr(srcTable);

    int startAddress = AK_initialize_new_segment(dstTable, SEGMENT_TYPE_TABLE, t_header);

    if (startAddress == EXIT_ERROR) {
        return EXIT_ERROR;
    }

    dbg_messg(LOW, REL_OP, "\nTABLE %s CREATED from %s!\n", dstTable, srcTable);

    table_addresses *src_addr = (table_addresses*) AK_get_table_addresses(srcTable);

    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    InitL(row_root);

    int i, j, k, l, type, size, address;
    char data[MAX_VARCHAR_LENGTH];

    for (i = 0; src_addr->address_from[i] != 0; i++) {

        for (j = src_addr->address_from[i]; j < src_addr->address_to[i]; j++) {

            AK_mem_block *temp = (AK_mem_block *) AK_get_block(j);

            if (temp->block->last_tuple_dict_id == 0)
                break;
            for (k = 0; k < DATA_BLOCK_SIZE; k += num_attr) {

                if (temp->block->tuple_dict[k].type == FREE_INT)
                    break;

				for (l = 0; l < num_attr; l++) {
					type = temp->block->tuple_dict[k + l].type;
					size = temp->block->tuple_dict[k + l].size;
					address = temp->block->tuple_dict[k + l].address;
					memcpy(data, &(temp->block->data[address]), size);
					data[size] = '\0';
					InsertNewElement(type, data, dstTable, t_header[l].att_name, row_root);
				}

				if (AK_check_if_row_satisfies_expression(row_root, expr))
                    insert_row(row_root);

				DeleteAllL(row_root);
            }
        }
    }

    free(src_addr);
    free(t_header);
    free(row_root);

	dbg_messg(LOW, REL_OP, "SELECTION_TEST_SUCCESS\n\n");
    return EXIT_SUCCESS;
}


/**
 * @brief  Function for selection operator testing
 * @author Matija Šestak, updated by Dino Laktašić
 */
void op_selection_test() {
    printf("\n********** SELECTION TEST **********\n");

    AK_list *expr = (AK_list *) malloc(sizeof (AK_list));
    InitL(expr);

    int num = 2010;
    //float weight = 83.750;
    //InsertAtEndL( TYPE_ATTRIBS, "weight", sizeof("weight"), expr );
    //InsertAtEndL( TYPE_FLOAT, &weight, sizeof(float), expr );
    //InsertAtEndL( TYPE_OPERATOR, "<", sizeof("<"), expr );
    InsertAtEndL(TYPE_ATTRIBS, "year", sizeof ("year"), expr);
    InsertAtEndL(TYPE_INT, &num, sizeof (int), expr);
    InsertAtEndL(TYPE_OPERATOR, "<", sizeof ("<"), expr);
    InsertAtEndL(TYPE_ATTRIBS, "firstname", sizeof ("firstname"), expr);
    InsertAtEndL(TYPE_VARCHAR, "Robert", sizeof ("Robert"), expr);
    InsertAtEndL(TYPE_OPERATOR, "=", sizeof ("="), expr);
    InsertAtEndL(TYPE_OPERATOR, "OR", sizeof ("OR"), expr);
    //InsertAtEndL( TYPE_OPERATOR, "AND", sizeof("AND"), expr );

    printf("\nQUERY: SELECT * FROM student WHERE year < 2010 OR firstname = 'Robert';\n\n");

    char *tblName = "student";

    AK_selection(tblName, "selection_test", expr);
    AK_print_table("selection_test");

    DeleteAllL(expr);
    free(expr);
}
