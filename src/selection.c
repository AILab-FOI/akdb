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
 * @brief  Value comparison according to data type
 * @author Dino Laktašić
 * @param AK_list_elem el - list element
 * @param const char *op - comparison operator
 * @param const void *a - left operand
 * @param const void *b - right operand
 * @return int - 0 if false, 1 if true
 */
int AK_selection_check_rs(AK_list_elem el, const char *op, const void *a, const void *b) {
    switch (el->type) {
        case TYPE_INT:
            //printf("a:%i , b:%i\n", *((int *)a), *((int *)b));
            if (*(int *) a < *(int *) b && strcmp(op, "<") == 0)
                return 1;
            else if (*((int *) a) > *((int *) b) && strcmp(op, ">") == 0)
                return 1;
            else if (*((int *) a) <= *((int *) b) && strcmp(op, "<=") == 0)
                return 1;
            else if (*((int *) a) >= *((int *) b) && strcmp(op, ">=") == 0)
                return 1;
            else
                return 0;
            break;

        case TYPE_FLOAT:
            //printf("a:%f , b:%f\n", *((float *)a), *((float *)b));
            if (*((float *) a) < *((float *) b) && strcmp(op, "<") == 0)
                return 1;
            else if (*((float *) a) > *((float *) b) && strcmp(op, ">") == 0)
                return 1;
            else if (*((float *) a) <= *((float *) b) && strcmp(op, "<=") == 0)
                return 1;
            else if (*((float *) a) >= *((float *) b) && strcmp(op, ">=") == 0)
                return 1;
            else
                return 0;
            break;

        case TYPE_VARCHAR:
            //printf("a:%s , b:%s\n", (const char *)a, (const char *)b);
            if (strcmp((const char *) a, (const char *) b) < 0 && strcmp(op, "<") == 0)
                return 1;
            if (strcmp((const char *) a, (const char *) b) > 0 && strcmp(op, ">") == 0)
                return 1;
            if (strcmp((const char *) a, (const char *) b) <= 0 && strcmp(op, "<=") == 0)
                return 1;
            if (strcmp((const char *) a, (const char *) b) >= 0 && strcmp(op, ">=") == 0)
                return 1;
            else
                return 0;
            break;
    }
}

/**
 * @brief  Evaluate logical expression
 * @author Matija Šestak, updated by Dino Laktašić
 * @param AK_mem_block* - memory block from cache
 * @param AK_header* - table header
 * @param int - number of the attributes
 * @param AK_list* - list with posfix notation of the logical expression
 * @param int - current tuple (first in row)
 * @result int - 0 if false, 1 if true
 */
int AK_selection_check_expr(AK_mem_block *mem_block, AK_header *header, int num_attr, AK_list *expr, int current_tuple) {
    int true = 1, false = 0;
    int i, found, result;

    AK_list *temp = (AK_list *) malloc(sizeof (AK_list));
    InitL(temp);

    AK_list_elem el = (AK_list_elem) FirstL(expr);
    AK_list_elem a, b, c;

    char data[MAX_VARCHAR_LENGTH];

    while (el) {
        if (el->type == TYPE_ATTRIBS) {
            found = 0;
            for (i = 0; i < num_attr; i++) {
                if (strcmp(el->data, header[i].att_name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                return 0;
            } else {
                int address = mem_block->block->tuple_dict[current_tuple + i].address;
                int size = mem_block->block->tuple_dict[current_tuple + i].size;
                int type = mem_block->block->tuple_dict[current_tuple + i].type;
                memset(data, 0, MAX_VARCHAR_LENGTH);
                memcpy(data, &(mem_block->block->data[address]), size);
                InsertAtEndL(type, data, size, temp);
            }
        } else if (el->type == TYPE_OPERATOR) {
            //operators implementation
            a = (AK_list_elem) EndL(temp);
            b = (AK_list_elem) PreviousL(a, temp);

            if (strcmp(el->data, "=") == 0) {
                if (memcmp(a->data, b->data, a->size) == 0)
                    InsertAtEndL(TYPE_INT, &true, sizeof (int), temp);
                else
                    InsertAtEndL(TYPE_INT, &false, sizeof (int), temp);

            } else if (strcmp(el->data, "<>") == 0) {
                if (memcmp(a->data, b->data, a->size) != 0)
                    InsertAtEndL(TYPE_INT, &true, sizeof (int), temp);
                else
                    InsertAtEndL(TYPE_INT, &false, sizeof (int), temp);

            } else if (strcmp(el->data, "OR") == 0) {
                int val_a, val_b;
                memcpy(&val_a, a->data, sizeof (int));
                memcpy(&val_b, b->data, sizeof (int));

                if (val_a || val_b)
                    InsertAtEndL(TYPE_INT, &true, sizeof (int), temp);
                else
                    InsertAtEndL(TYPE_INT, &false, sizeof (int), temp);

            } else if (strcmp(el->data, "AND") == 0) {
                int val_a, val_b;
                memcpy(&val_a, a->data, sizeof (int));
                memcpy(&val_b, b->data, sizeof (int));

                if (val_a && val_b)
                    InsertAtEndL(TYPE_INT, &true, sizeof (int), temp);
                else
                    InsertAtEndL(TYPE_INT, &false, sizeof (int), temp);

            } else {
                int rs;

                void *va = (void *) malloc(b->size);
                void *vb = (void *) malloc(a->size);
                memcpy(va, b->data, b->size);
                memcpy(vb, a->data, a->size);
                memcpy(va + b->size, "\0", 1);
                memcpy(vb + a->size, "\0", 1);

                if (strcmp(el->data, "<") == 0) {
                    rs = AK_selection_check_rs(b, "<", va, vb);
                } else if (strcmp(el->data, ">") == 0) {
                    rs = AK_selection_check_rs(b, ">", va, vb);
                } else if (strcmp(el->data, "<=") == 0) {
                    rs = AK_selection_check_rs(b, "<=", va, vb);
                } else if (strcmp(el->data, ">=") == 0) {
                    rs = AK_selection_check_rs(b, ">=", va, vb);
                }

                free(va);
                free(vb);

                InsertAtEndL(TYPE_INT, &rs, sizeof (int), temp);
            }

            DeleteL(a, temp);
            DeleteL(b, temp);

        } else {
            InsertAtEndL(el->type, el->data, el->size, temp);
        }
        el = el->next;
    }

    memcpy(&result, ((AK_list_elem) FirstL(temp))->data, sizeof (int));
    DeleteAllL(temp);

    return result;
}

/**
 * @brief  Implementation of selection
 * @author Matija Šestak.
 * @param char* srcTable - source table name
 * @param char* dstTable - destination table name
 * @param AK_list* - list with posfix notation of the logical expression
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

    table_addresses *src_addr = (table_addresses*) get_table_addresses(srcTable);

    element row_root = (element) malloc(sizeof (list));
    InitializeList(row_root);

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
                if (AK_selection_check_expr(temp, t_header, num_attr, expr, k)) {
                    for (l = 0; l < num_attr; l++) {
						type = temp->block->tuple_dict[k + l].type;
                        size = temp->block->tuple_dict[k + l].size;
                        address = temp->block->tuple_dict[k + l].address;
                        memcpy(data, &(temp->block->data[address]), size);
                        data[size] = '\0';
                        InsertNewElement(type, data, dstTable, t_header[l].att_name, row_root);
                    }
                    insert_row(row_root);
                    DeleteAllElements(row_root);
                }
            }
        }
    }

    free(src_addr);
    free(t_header);
	dbg_messg(LOW, REL_OP, "SELECTION_TEST_SUCCESS\n");
    return EXIT_SUCCESS;
}

///Function for selection testing

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
    InsertAtEndL(TYPE_VARCHAR, "Dino", sizeof ("Dino"), expr);
    InsertAtEndL(TYPE_OPERATOR, "=", sizeof ("="), expr);
    InsertAtEndL(TYPE_OPERATOR, "OR", sizeof ("OR"), expr);
    //InsertAtEndL( TYPE_OPERATOR, "AND", sizeof("AND"), expr );

    printf("\nQUERY: SELECT * FROM student WHERE year < 2010 OR firstname = 'Dino';\n\n");

    char *tblName = "student";

    AK_selection(tblName, "selection_test", expr);
    AK_print_table("selection_test");

    DeleteAllL(expr);
}
