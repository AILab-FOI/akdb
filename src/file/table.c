/**
@file table.c Provides functions for table abstraction
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

#include "../file/table.h"
#include <time.h>

AK_create_table_parameter* AK_create_create_table_parameter(int type, char* name) {
	AK_create_table_parameter* par = malloc(sizeof(AK_create_table_parameter));
	par->type = type;
	strcpy(par->name, name);
	return par;
}

void AK_create_table(char* tblName, AK_create_table_parameter* parameters, int attribute_count) {
	int i;
  AK_header t_header[ MAX_ATTRIBUTES ];
  AK_header* temp;
	for (i = 0; i < attribute_count; i++) {
		switch (parameters[i].type) {
			case TYPE_INT:
				temp = (AK_header*) AK_create_header(parameters[i].name, TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
				memcpy(t_header + i, temp, sizeof ( AK_header));
				break;
			case TYPE_VARCHAR:
				temp = (AK_header*) AK_create_header(parameters[i].name, TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
				memcpy(t_header + i, temp, sizeof ( AK_header));
				break;
			case TYPE_FLOAT:
				temp = (AK_header*) AK_create_header(parameters[i].name, TYPE_FLOAT, FREE_INT, FREE_CHAR, FREE_CHAR);
				memcpy(t_header + i, temp, sizeof ( AK_header));
				break;
		}
	}
	memset(t_header + attribute_count, 0, MAX_ATTRIBUTES - attribute_count);
}

/**
 * @author Matija Šestak.
 * @brief  Determine the number of attributes in the table
 * <ol>
 * <li>Read addresses of extents</li>
 * <li>If there is no extents in the table, return -1</li>
 * <li>else read the first block</li>
 * <li>while  header tuple exists in the block, increment num_attr</li>
 * </ol>
 * @param  * tblName table name
 * @return number of attributes in the table
 */
int AK_num_attr(char * tblName) {
    int num_attr = 0;

    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(tblName);
    if (addresses->address_from[0] == 0)
        num_attr = -1;
    else {
        AK_block *temp_block = (AK_block*) AK_read_block(addresses->address_from[0]);

        while (strcmp(temp_block->header[num_attr].att_name, "\0") != 0) {
            num_attr++;
        }
        free(temp_block);
    }
    free(addresses);

    return num_attr;
}

/**
 * @author Matija Šestak.
 * @brief  Determine number of rows in the table
 * <ol>
 * <li>Read addresses of extents</li>
 * <li>If there is no extents in the table, return -1</li>
 * <li>For each extent from table</li>
 * <li>For each block in the extent</li>
 * <li>Get a block</li>
 * <li>Exit if there is no records in block</li>
 * <li>Count tuples in block</li>
 * <li>Return the number of tuples divided by number of attributes</li>
 * </ol>
 * @param *tableName table name
 * @return number of rows in the table
 */
int AK_get_num_records(char *tblName) {
    int num_rec = 0;
    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(tblName);
    if (addresses->address_from[0] == 0)
        return -1;
    int i = 0, j, k;
    AK_mem_block *temp = (AK_mem_block*) AK_get_block(addresses->address_from[0]);
    while (addresses->address_from[ i ] != 0) {
        for (j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++) {
            temp = (AK_mem_block*) AK_get_block(j);
            if (temp->block->last_tuple_dict_id == 0)
                break;
            for (k = 0; k < DATA_BLOCK_SIZE; k++) {
                if (temp->block->tuple_dict[ k ].size > 0) {
                    num_rec++;
                }
            }
        }
        i++;
    }

    int num_head = AK_num_attr(tblName);
    return num_rec / num_head;
}

/**
 * @author Matija Šestak.
 * @brief  Function that getts table header
 * <ol>
 * <li>Read addresses of extents</li>
 * <li>If there is no extents in the table, return -1</li>
 * <li>else read the first block</li>
 * <li>allocate array</li>
 * <li>copy table header to the array</li>
 * </ol> 
 * @param  *tblName table name
 * @result array of table header
 */
AK_header *AK_get_header(char *tblName) {
    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(tblName);
    if (addresses->address_from[0] == 0)
        return 0;
    AK_mem_block *temp = (AK_mem_block*) AK_get_block(addresses->address_from[0]);

    int num_attr = AK_num_attr(tblName);
    AK_header *head = (AK_header*) calloc(num_attr, sizeof (AK_header));
    memcpy(head, temp->block->header, num_attr * sizeof (AK_header));

    return head;
}

/**
 * @author Matija Šestak.
 * @brief  Function that gets attribute name for some zero-based index
 * @param *tblName table name
 * @param index zero-based index
 * @return attribute name
 */
char *AK_get_attr_name(char *tblName, int index) {
    int num_attr = AK_num_attr(tblName);
    if (index >= num_attr)
        return NULL;
    else {
        AK_header *header = AK_get_header(tblName);
        return (header + index)->att_name;
    }
}

/**
 * @author Matija Šestak.
 * @brief  Function that gets zero-based index for atrribute
 * @param  *tblName table name
 * @param *attrName attribute name
 * @return zero-based index
 */
int AK_get_attr_index(char *tblName, char *attrName) {
    if (tblName == NULL || attrName == NULL)
        return -1;
    int num_attr = AK_num_attr(tblName);
    AK_header *header = AK_get_header(tblName);
    int index = 0;
    while (index < num_attr) {
        if (strcmp(attrName, (header + index)->att_name) == 0)
            return index;
        index++;
    }
    return -1;
}

/**
 * @author Matija Šestak.
 * @brief  Function that gets all values in some column and put on the list
 * @param num zero-based column index
 * @param  *tblName table name
 * @return column values list
 */
AK_list *AK_get_column(int num, char *tblName) {
    int num_attr = AK_num_attr(tblName);
    if (num >= num_attr || num < 0)
        return NULL;
    AK_list *row_root = (AK_list*) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);

    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(tblName);
    int i, j, k;
    char data[ MAX_VARCHAR_LENGTH ];

    i = 0;
    while (addresses->address_from[i] != 0) {
        for (j = addresses->address_from[i]; j < addresses->address_to[i]; j++) {
            AK_mem_block *temp = (AK_mem_block*) AK_get_block(j);
            if (temp->block->last_tuple_dict_id == 0) break;
            for (k = num; k < DATA_BLOCK_SIZE; k += num_attr) {
                if (temp->block->tuple_dict[k].type != FREE_INT) {
                    int type = temp->block->tuple_dict[k].type;
                    int size = temp->block->tuple_dict[k].size;
                    int address = temp->block->tuple_dict[k].address;
                    memcpy(data, &(temp->block->data[address]), size);
                    data[ size ] = '\0';
                    Ak_InsertAtEnd_L(type, data, size, row_root);
                }
            }
        }
        i++;
    }
    return row_root;
}

/**
 * @author Markus Schatten, Matija Šestak.
 * @brief  Function that gets all values in some row and put on the list
 * @param num zero-based row index
 * @param  * tblName table name
 * @return row values list
 */
AK_list * AK_get_row(int num, char * tblName) {
    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(tblName);
    AK_list *row_root = calloc(1, sizeof (AK_list));
    Ak_Init_L(row_root);

    int num_attr = AK_num_attr(tblName);
    int i, j, k, l, counter;
    i = 0;

    char data[MAX_VARCHAR_LENGTH];
    counter = -1;
    while (addresses->address_from[i] != 0) {
        for (j = addresses->address_from[i]; j < addresses->address_to[i]; j++) {
            AK_mem_block *temp = (AK_mem_block*) AK_get_block(j);
            if (temp->block->last_tuple_dict_id == 0)
                break;
            for (k = 0; k < DATA_BLOCK_SIZE; k += num_attr) {
                if (temp->block->tuple_dict[k].size > 0)
                    counter++;
                if (counter == num) {
                    for (l = 0; l < num_attr; l++) {
                        int type = temp->block->tuple_dict[k + l].type;
                        int size = temp->block->tuple_dict[k + l].size;
                        int address = temp->block->tuple_dict[k + l].address;
                        memcpy(data, &(temp->block->data[address]), size);
                        data[size] = '\0';
                        Ak_InsertAtEnd_L(type, data, size, row_root);
                    }
                    return row_root;
                }
            }
        }
        i++;
    }
    return NULL;
}

/**
 * @author Matija Šestak.
 * @brief Function that gets value in some row and column
 * @param row zero-based row index
 * @param column zero-based column index
 * @param *tblName table name
 * @return value in the list
 */
AK_list_elem AK_get_tuple(int row, int column, char *tblName) {
    int num_rows = AK_get_num_records(tblName);
    int num_attr = AK_num_attr(tblName);

    if (row >= num_rows || column >= num_attr)
        return NULL;

    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(tblName);

    AK_list *row_root = (AK_list*) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);

    int i, j, k, counter;
    char data[ MAX_VARCHAR_LENGTH ];

    i = 0;
    counter = -1;
    while (addresses->address_from[ i ] != 0) {
        for (j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++) {
            AK_mem_block *temp = (AK_mem_block*) AK_get_block(j);
            if (temp->block->last_tuple_dict_id == 0) break;
            for (k = 0; k < DATA_BLOCK_SIZE; k += num_attr) {
                if (temp->block->tuple_dict[k].size > 0)
                    counter++;
                if (counter == row) {
                    int type = temp->block->tuple_dict[ k + column ].type;
                    int size = temp->block->tuple_dict[ k + column ].size;
                    int address = temp->block->tuple_dict[ k + column ].address;
                    memcpy(data, &(temp->block->data[address]), size);
                    data[ size ] = '\0';
                    Ak_InsertAtEnd_L(type, data, size, row_root);
                    return (AK_list_elem) Ak_First_L(row_root);
                }
            }
        }
        i++;
    }
    return NULL;
}

/**
 * @author Matija Šestak.
 * @brief  Function that converts tuple value to string
 * @param *tuple tuple in the list
 * @return tuple value as a string
 */
char * AK_tuple_to_string(AK_list *tuple) {
    int temp_int;
    float temp_float;
    char temp_char[ MAX_VARCHAR_LENGTH ];

    char *buff = (char*) malloc(MAX_VARCHAR_LENGTH);

	//assert(tuple->type);
	
    switch (tuple->type) {
        case TYPE_INT:
            memcpy(&temp_int, tuple->data, tuple->size);
            sprintf(buff, "%d", temp_int);
            return buff;
            break;
        case TYPE_FLOAT:
            memcpy(&temp_float, tuple->data, tuple->size);
            sprintf(buff, "%f", temp_float);
            return buff;
            break;
        case TYPE_VARCHAR:
            memcpy(temp_char, tuple->data, tuple->size);
            temp_char[ tuple->size ] = '\0';
            sprintf(buff, "%s", temp_char);
            return buff;
            break;
    }
    return NULL;
}

/**
 * @author Dino Laktašić. 
 * @brief Function that prints row spacer 
 * @param col_len[] max lengths for each attribute cell 
 * @param length total table width 
 * @return printed row spacer
 */
void AK_print_row_spacer(int col_len[], int length) {
    int i, j, col, temp;

    j = col = temp = 0;

    for (i = 0; i < length; i++) {
        if (!i || i == temp + j) {
            j += TBL_BOX_OFFSET + 1;
            temp += col_len[col++] + 1;
            printf("+");
        } else {
            printf("-");
        }
    }
}

/**
 * @author Dino Laktašić
 * @brief  Function that prints table row 
 * @param col_len[] array of max lengths for each attribute
 * @param *row  list with row elements
 * @return No return value
 */
void AK_print_row(int col_len[], AK_list *row) {
    AK_list_elem el = (AK_list_elem) Ak_First_L(row);

    int i = 0;
    void *data = (void *) calloc(MAX_VARCHAR_LENGTH, sizeof (void));

    printf("\n|");
    while (el != NULL) {
        memset(data, 0, MAX_VARCHAR_LENGTH);
        switch (el->type) {
            case FREE_CHAR:
                //case FREE_INT:
                printf(" %-*s|", col_len[i] + TBL_BOX_OFFSET, "null");
                break;
            case TYPE_INT:
                memcpy(data, el->data, sizeof (int));
                printf("%*i |", col_len[i] + TBL_BOX_OFFSET, *((int *) data));
                break;
            case TYPE_FLOAT:
                memcpy(data, el->data, sizeof (float));
                printf("%*.3f |", col_len[i] + TBL_BOX_OFFSET, *((float *) data));
                break;
                //Currently this header types are handled like TYPE_VARCHAR (print as string left aligned)
                //case TYPE_INTERNAL:
                //case TYPE_NUMBER:
                //case TYPE_DATE:
                //case TYPE_DATETIME:
                //case TYPE_TIME:
                //case TYPE_BLOB:
                //case TYPE_BOOL:
            case TYPE_VARCHAR:
            default:
                memcpy(data, el->data, el->size);
                printf(" %-*s|", col_len[i] + TBL_BOX_OFFSET, (const char *) data);
                break;
        }
        el = el->next;
        i++;
    }
    printf("\n");
    free(data);
}

/**
 * @author Dino Laktašić and Mislav Čakarić (replaced old print table function by new one)
 * @brief  Function for printing table
 * @param *tblName table name
 * @return No return value
 */
void AK_print_table(char *tblName) {
    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(tblName);
    if (addresses->address_from[0] == 0) {
        printf("Table %s does not exist!\n", tblName);
    } else {
        AK_header *head = AK_get_header(tblName);

        int i, j, k, l;
        int num_attr = AK_num_attr(tblName);
        int num_rows = AK_get_num_records(tblName);
        int len[num_attr]; //max length for each attribute in row
        int length = 0; //length of spacer
        struct timeval  end_time, start_time;

        //store lengths of header attributes
        for (i = 0; i < num_attr; i++) {
            len[i] = strlen((head + i)->att_name);
        }

        //for each header attribute iterate through all table rows and check if
        //there is longer element than previously longest and store it in array
        for (i = 0; i < num_attr; i++) {
            for (j = 0; j < num_rows; j++) {
                AK_list_elem el = AK_get_tuple(j, i, tblName);
                switch (el->type) {
                    case TYPE_INT:
                        length = AK_chars_num_from_number(*((int *) (el)->data), 10);
                        if (len[i] < length) {
                            len[i] = length;
                        }
                        break;
                    case TYPE_FLOAT:
                        length = AK_chars_num_from_number(*((float *) (el)->data), 10);
                        if (len[i] < length) {
                            len[i] = length;
                        }
                        break;
                    case TYPE_VARCHAR:
                    default:
                        if (len[i] < el->size) {
                            len[i] = el->size;
                        }
                        break;
                }
            }
        }
        //num_attr is number of char | + space in printf
        //set offset to change the box size
        length = 0;
        for (i = 0; i < num_attr; length += len[i++]);
        length += num_attr * TBL_BOX_OFFSET + 2 * num_attr + 1;

        //start measuring time
        gettimeofday(&start_time, NULL);

        printf("Table: %s\n", tblName);

        if (num_attr <= 0 || num_rows <= 0) {
            printf("Table is empty.\n");
        } else {
            //print table header
            AK_print_row_spacer(len, length);
            printf("\n|");

            for (i = 0; i < num_attr; i++) {
                //print attributes center aligned inside box
                k = (len[i] - (int)strlen((head + i)->att_name) + TBL_BOX_OFFSET + 1);
                if (k % 2 == 0) {
                    k /= 2;
                    printf("%-*s%-*s|", k, " ", k + (int)strlen((head + i)->att_name), (head + i)->att_name);
                } else {
                    k /= 2;
                    printf("%-*s%-*s|", k, " ", k + (int)strlen((head + i)->att_name) + 1, (head + i)->att_name);
                }

                //print attributes left aligned inside box
                //printf(" %-*s|", len[i] + MAX_TABLE_BOX_OFFSET, (head + i)->att_name);
            }
            printf("\n");
            AK_print_row_spacer(len, length);

            AK_list *row_root = (AK_list*) malloc(sizeof (AK_list));
            Ak_Init_L(row_root);

            i = 0;
			int type, size, address;
			
            while (addresses->address_from[i] != 0) {
                for (j = addresses->address_from[i]; j < addresses->address_to[i]; j++) {
                    AK_mem_block *temp = (AK_mem_block*) AK_get_block(j);
                    if (temp->block->last_tuple_dict_id == 0)
                        break;
                    for (k = 0; k < DATA_BLOCK_SIZE; k += num_attr) {
                        if (temp->block->tuple_dict[k].size > 0) {
                            for (l = 0; l < num_attr; l++) {
								type = temp->block->tuple_dict[k + l].type;
                                size = temp->block->tuple_dict[k + l].size;
                                address = temp->block->tuple_dict[k + l].address;
                                Ak_InsertAtEnd_L(type, &(temp->block->data[address]), size, row_root);
                            }
                            AK_print_row(len, row_root);
                            AK_print_row_spacer(len, length);
                            Ak_DeleteAll_L(row_root);
                        }
                    }
                }
                i++;
            }
            printf("\n");

            /*
            //print table rows
            for (i = 0; i < num_rows; i++) {
                    AK_list *row = AK_get_row(i, tblName);
                    AK_print_row(len, row);
                    AK_print_row_spacer(len, length);
                    DeleteAllL(row);
            }
            printf("\n");*/
            //print table rows number and time spent to generate table
            gettimeofday(&end_time, NULL);

            if ((double) (end_time.tv_sec - start_time.tv_sec) == 0){
            	printf("%i rows found, duration: %f μs\n",num_rows, (double) (end_time.tv_usec - start_time.tv_usec));
            } else {
            	printf("%i rows found, duration: %f s\n",num_rows, (double) (end_time.tv_usec - start_time.tv_usec)/1000000 + (double) (end_time.tv_sec - start_time.tv_sec));
            }
        }
    }
}

char* FILEPATH = "table_test.txt";
FILE *fp;


/**
 * @author Dino Laktašić. 
 * @brief Function that prints row spacer 
 * update by Luka Rajcevic
 * @param col_len[] max lengths for each attribute cell 
 * @param length total table width 
 * @return printed row spacer
 */
void AK_print_row_spacer_to_file(int col_len[], int length) {
    fp = fopen(FILEPATH,"a");
    int i, j, col, temp;

    j = col = temp = 0;

    for (i = 0; i < length; i++) {
        if (!i || i == temp + j) {
            j += TBL_BOX_OFFSET + 1;
            temp += col_len[col++] + 1;
            fprintf(fp, "+");
        } else {
            fprintf(fp, "-");
        }
    }

    fclose(fp);
}

/**
 * @author Dino Laktašić
 * @brief  Function that prints table row 
 * update by Luka Rajcevic
 * @param col_len[] array of max lengths for each attribute
 * @param *row  list with row elements
 * @return No return value
 */
void AK_print_row_to_file(int col_len[], AK_list *row) {
    fp = fopen(FILEPATH,"a");
    AK_list_elem el = (AK_list_elem) Ak_First_L(row);

    int i = 0;
    void *data = (void *) calloc(MAX_VARCHAR_LENGTH, sizeof (void));

    fprintf(fp, "\n|");
    while (el != NULL) {
        memset(data, 0, MAX_VARCHAR_LENGTH);
        switch (el->type) {
            case FREE_CHAR:
                //case FREE_INT:
                fprintf(fp, " %-*s|", col_len[i] + TBL_BOX_OFFSET, "null");
                break;
            case TYPE_INT:
                memcpy(data, el->data, sizeof (int));
                fprintf(fp, "%*i |", col_len[i] + TBL_BOX_OFFSET, *((int *) data));
                break;
            case TYPE_FLOAT:
                memcpy(data, el->data, sizeof (float));
                fprintf(fp, "%*.3f |", col_len[i] + TBL_BOX_OFFSET, *((float *) data));
                break;
            case TYPE_VARCHAR:
            default:
                memcpy(data, el->data, el->size);
                fprintf(fp, " %-*s|", col_len[i] + TBL_BOX_OFFSET, (const char *) data);
                break;
        }
        el = el->next;
        i++;
    }
    fprintf(fp, "\n");
    fclose(fp);
    free(data);
}

/**
 * @author Dino Laktašić and Mislav Čakarić (replaced old print table function by new one)
 * update by Luka Rajcevic
 * @brief  Function for printing table
 * @param *tblName table name
 * @return No return value
 */
void AK_print_table_to_file(char *tblName) {
    fp = fopen(FILEPATH,"a");
    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(tblName);
    if (addresses->address_from[0] == 0) {
        fprintf(fp, "Table %s does not exist!\n", tblName);
        
    } else {
        AK_header *head = AK_get_header(tblName);

        int i, j, k, l;
        int num_attr = AK_num_attr(tblName);
        int num_rows = AK_get_num_records(tblName);
        int len[num_attr]; //max length for each attribute in row
        int length = 0; //length of spacer

        //store lengths of header attributes
        for (i = 0; i < num_attr; i++) {
            len[i] = strlen((head + i)->att_name);
        }

        //for each header attribute iterate through all table rows and check if
        //there is longer element than previously longest and store it in array
        for (i = 0; i < num_attr; i++) {
            for (j = 0; j < num_rows; j++) {
                AK_list_elem el = AK_get_tuple(j, i, tblName);
                switch (el->type) {
                    case TYPE_INT:
                        length = AK_chars_num_from_number(*((int *) (el)->data), 10);
                        if (len[i] < length) {
                            len[i] = length;
                        }
                        break;
                    case TYPE_FLOAT:
                        length = AK_chars_num_from_number(*((float *) (el)->data), 10);
                        if (len[i] < length) {
                            len[i] = length;
                        }
                        break;
                    case TYPE_VARCHAR:
                    default:
                        if (len[i] < el->size) {
                            len[i] = el->size;
                        }
                        break;
                }
            }
        }
        //num_attr is number of char | + space in printf
        //set offset to change the box size
        length = 0;
        for (i = 0; i < num_attr; length += len[i++]);
        length += num_attr * TBL_BOX_OFFSET + 2 * num_attr + 1;
    
        fprintf(fp, "Table: %s\n", tblName);

        if (num_attr <= 0 || num_rows <= 0) {
            fprintf(fp, "Table is empty.\n");
        } else {
            //print table header
            fclose(fp);
            AK_print_row_spacer_to_file(len, length);
            fp = fopen(FILEPATH,"a");
            fprintf(fp, "\n|");

            for (i = 0; i < num_attr; i++) {
                //print attributes center aligned inside box
                k = (len[i] - (int)strlen((head + i)->att_name) + TBL_BOX_OFFSET + 1);
                if (k % 2 == 0) {
                    k /= 2;
                    fprintf(fp, "%-*s%-*s|", k, " ", k + (int)strlen((head + i)->att_name), (head + i)->att_name);
                } else {
                    k /= 2;
                    fprintf(fp, "%-*s%-*s|", k, " ", k + (int)strlen((head + i)->att_name) + 1, (head + i)->att_name);
                }

                //print attributes left aligned inside box
                //printf(" %-*s|", len[i] + MAX_TABLE_BOX_OFFSET, (head + i)->att_name);
            }
            fprintf(fp, "\n");
            fclose(fp);
            AK_print_row_spacer_to_file(len, length);
            AK_list *row_root = (AK_list*) malloc(sizeof (AK_list));
            Ak_Init_L(row_root);

            i = 0;
            int type, size, address;
            
            while (addresses->address_from[i] != 0) {
                for (j = addresses->address_from[i]; j < addresses->address_to[i]; j++) {
                    AK_mem_block *temp = (AK_mem_block*) AK_get_block(j);
                    if (temp->block->last_tuple_dict_id == 0)
                        break;
                    for (k = 0; k < DATA_BLOCK_SIZE; k += num_attr) {
                        if (temp->block->tuple_dict[k].size > 0) {
                            for (l = 0; l < num_attr; l++) {
                                type = temp->block->tuple_dict[k + l].type;
                                size = temp->block->tuple_dict[k + l].size;
                                address = temp->block->tuple_dict[k + l].address;
                                Ak_InsertAtEnd_L(type, &(temp->block->data[address]), size, row_root);
                            }
                            AK_print_row_to_file(len, row_root);
                            AK_print_row_spacer_to_file(len, length);
                            Ak_DeleteAll_L(row_root);
                        }
                    }
                }
                i++;
            }
            fp = fopen(FILEPATH,"a");
            fprintf(fp, "\n");
        }
    }
    fclose(fp);  
}

/**
 * @author Matija Šestak.
 * @brief  Function that check whether table is empty
 * @param *tblName table name
 * @return true/false
 */
int AK_table_empty(char *tblName) {
    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(tblName);
    AK_mem_block *temp = (AK_mem_block*) AK_get_block(addresses->address_from[0]);
    return (temp->block->last_tuple_dict_id == 0) ? 1 : 0;
}

/**
 * @author Dejan Frankovic
 * @brief  Function that gets obj_id of named table from AK_relation system table
 * @param *table table name
 * @return obj_id of the table or EXIT_ERROR if there is no table with that name
 */
int AK_get_table_obj_id(char *table) {
    int i = 0, table_id = -1;
    AK_list *row;

    i = 0;
    while ((row = AK_get_row(i, "AK_relation")) != NULL) {
        if (strcmp(row->next->next->data, table) == 0) {
            memcpy(&table_id, row->next->data, sizeof (int));
            break;
        }
        i++;
    }
    if (table_id == -1)
        return EXIT_ERROR;
    return table_id;
}

/**
 * @author Dino Laktašić, abstracted from difference.c for use in difference.c, intersect.c and union.c by Tomislav Mikulček
 * @brief  Function to check if tables have the same relation schema
 * @param tbl1_temp_block first cache block of the first table 
 * @param tbl2_temp_block first cache block of the second table
 * @param operator_name the name of operator, used for displaying error message
 * @return if success returns num of attributes in schema, else returns EXIT_ERROR
 */
int AK_check_tables_scheme(AK_mem_block *tbl1_temp_block, AK_mem_block *tbl2_temp_block, char *operator_name) {
	int i;
	int num_att1 = 0;
	int num_att2 = 0;
	
	for (i = 0; i < MAX_ATTRIBUTES; i++) {
            if (strcmp(tbl1_temp_block->block->header[i].att_name, "\0") != 0) {
                num_att1++;
            } else {
                break;
            }

            if (strcmp(tbl2_temp_block->block->header[i].att_name, "\0") != 0) {
                num_att2++;
            } else {
                break;
            }

            if (strcmp(tbl1_temp_block->block->header[i].att_name, tbl2_temp_block->block->header[i].att_name) != 0) {
                printf("%s ERROR: Relation shemas are not the same! \n", operator_name);
                return EXIT_ERROR;
            }

            if (tbl1_temp_block->block->header[i].type != tbl2_temp_block->block->header[i].type) {
                printf("%s ERROR: Attributes are not of the same type!", operator_name);
                return EXIT_ERROR;
            }
        }

        if (num_att1 != num_att2) {
            printf("%s ERROR: Not same number of the attributes! \n", operator_name);
			return EXIT_ERROR;
        }
		
		return num_att1;
}

/**
 * @author Mislav Čakarić edited by Ljubo Barać
 * @brief Function for renaming table and/or attribute in table (moved from rename.c)
 * @param old_table_name old name of the table
 * @param new_table_name new name of the table
 * @param old_attr name of the attribute to rename
 * @param new_attr new name for the attribute to rename
 * @return EXIT_ERROR or EXIT_SUCCESS
 */
int AK_rename(char *old_table_name, char *old_attr, char *new_table_name, char *new_attr) {
    table_addresses *adresses = (table_addresses *)AK_get_table_addresses(old_table_name);
    int tab_addresses[MAX_NUM_OF_BLOCKS];
    int num_extents = 0, num_blocks = 0;
    register int i, j;
    AK_mem_block *mem_block;
    AK_archive_log("AK_rename", old_table_name, old_attr, new_table_name, new_attr); //ARCHIVE_LOG
	if(strcmp(old_attr, new_attr) != 0){
		//SEARCH FOR ALL BLOCKS IN SEGMENT
		i=0;
		while(adresses->address_from[i]){
			for (j = adresses->address_from[i]; j <= adresses->address_to[i]; j++) {
				tab_addresses[num_blocks] = j;
				num_blocks++;
			}
			num_extents++;
			i++;
		}

		AK_header newHeader[MAX_ATTRIBUTES];
		mem_block = (AK_mem_block *)AK_get_block(tab_addresses[0]);
		memcpy(&newHeader, mem_block->block->header, sizeof (mem_block->block->header));

		for (i = 0; i < MAX_ATTRIBUTES; i++) {
			if (strcmp(newHeader[i].att_name, old_attr)==0) {
				Ak_dbg_messg(HIGH, REL_OP, "AK_rename: the attribute names are the same at position %d!\n", i);
				memset(&newHeader[i].att_name, 0, MAX_ATT_NAME);
				memcpy(&newHeader[i].att_name, new_attr, strlen(new_attr));
				break;
			} else if (strcmp(newHeader[i].att_name, "\0")==0) { //if there is no more attributes
				Ak_dbg_messg(MIDDLE, REL_OP, "AK_rename: ERROR: atribute: %s does not exist in this table\n", old_attr);
				return (EXIT_ERROR);
			}
		}

		//replacing old headers with new ones
		for (i = 0; i < num_blocks; i++) {
			mem_block = (AK_mem_block *)AK_get_block(tab_addresses[i]);
			memcpy(&mem_block->block->header, newHeader, sizeof (AK_header) * MAX_ATTRIBUTES);
			mem_block->dirty=BLOCK_DIRTY;
		}
	}

    if(strcmp(old_table_name, new_table_name) != 0){//new name is different than old, and old needs to be replaced
        AK_list *expr;
	expr = 0;
	AK_selection(old_table_name, new_table_name, expr);
    	AK_delete_segment(old_table_name, SEGMENT_TYPE_TABLE);
    }

    return EXIT_SUCCESS;
}
/**
 * @author Unknown
 * @brief Function for testing table abstraction
 * @return No return value
 */
void AK_table_test() {
    printf("table.c: Present!\n");

    printf("\n********** TABLE ABSTRACTION TEST by Matija Šestak **********\n\n");

    printf("Table \"student\":AK_print_table\n");
    AK_print_table("student");
    printf("\n");

    printf("Table \"student\": AK_table_empty: ");
    if (AK_table_empty("student"))
        printf("true\n");
    else
        printf("false\n");
    printf("\n");

    printf("Table \"student\": AK_num_attr: ");
    printf("%d\n", AK_num_attr("student"));
    printf("\n");

    printf("Table \"student\": AK_get_num_records: ");
    printf("%d\n", AK_get_num_records("student"));
    printf("\n");

    printf("Table \"student\": AK_get_row: First row: \n");
    printf("\n");

    printf("Table \"student\": AK_get_attr_name for index 3: ");
    printf("%s\n", AK_get_attr_name("student", 3));
    printf("\n");

    printf("Table \"student\": AK_get_attr_index of \"year\": ");
    printf("%d\n", AK_get_attr_index("student", "year"));
    printf("\n");

    printf("Table \"student\": AK_get_tuple for row=0, column=1:");
    printf("%s\n", AK_tuple_to_string(AK_get_tuple(0, 1, "student")));
}


/**
  * @author Mislav Čakarić, edited by Ljubo Barać
  * @brief Function for rename operator testing (moved from rename.c)
  * @return No return value
 */
void AK_op_rename_test() {
    //printf( "rename_test: Present!\n" );
    printf("\n********** RENAME TEST **********\n\n");

	AK_print_table("AK_relation");
    AK_rename("student", "weight", "student2", "weight");
    AK_print_table("student2");
    //AK_print_table("student2");

    AK_print_table("AK_relation");
}
