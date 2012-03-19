/**
@file aggregation.c Provides functions for aggregation and grouping
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

#include "aggregation.h"

extern search_result AK_search_unsorted(char *szRelation, search_params *aspParams, int iNum_search_params);

/**
 Function calculates how meany attributes there are in a header.
 @param header A header array
 @return Number of attributes defined in header array
 @author Dejan Frankovic
 */
int AK_header_size(AK_header *header) {
    int counter = 0;
    while (*((char*) & header[counter]) != '\0') {
        counter++;
    }
    return counter;
}

/**
 Initialize the input object for aggregation
 @param input - the input object
  @author Dejan Frankovic
 */
void AK_agg_input_init(AK_agg_input *input) {
    int i;
    for (i = 0; i < MAX_ATTRIBUTES; i++) {
        memcpy(&(*input).attributes[i], "\0", sizeof ("\0"));
        (*input).tasks[i] = -1;
        (*input).counter = 0;
    }
}

/**
 Adds a header with a task in input object for aggregation
@param header - a header that is being aggregated
@param agg_task - the task which is to be done on the header
@param input - the input object
@return On success, returns EXIT_SUCCESS, otherwise EXIT_FAILURE
 @author Dejan Frankovic
 */
int AK_agg_input_add(AK_header header, int agg_task, AK_agg_input *input) {
    if ((char*) & header == '\0' || agg_task < 0 || (*input).counter == MAX_ATTRIBUTES)
        return EXIT_FAILURE;
    (*input).attributes[(*input).counter] = header;
    (*input).tasks[(*input).counter] = agg_task;
    (*input).counter++;
    return EXIT_SUCCESS;
}

/**
 Adds a header with a task on the beginning of the input object for aggregation
@param header - a header that is being aggregated
@param agg_task - the task which is to be done on the header
@param input - the input object
@return On success, returns EXIT_SUCCESS, otherwise EXIT_FAILURE
 @author Dejan Frankovic
 */
int AK_agg_input_add_to_beginning(AK_header header, int agg_task, AK_agg_input *input) {
    if ((char*) & header == '\0' || agg_task < 0 || (*input).counter == MAX_ATTRIBUTES)
        return EXIT_FAILURE;

    int i;
    for (i = (*input).counter; i > 0; i--) {
        (*input).attributes[i] = (*input).attributes[i - 1];
        (*input).tasks[i] = (*input).tasks[i - 1];
    }

    (*input).attributes[0] = header;
    (*input).tasks[0] = agg_task;
    (*input).counter++;

    return EXIT_SUCCESS;
}

/**
 Used to handle AVG (average) aggregation
@param input - the input object
 @author Dejan Frankovic
 */
void AK_agg_input_fix(AK_agg_input *input) {
    int i = 0;
    while ((*input).tasks[i] != -1) {
        if ((*input).tasks[i] == AGG_TASK_AVG) {
            AK_agg_input_add_to_beginning((*input).attributes[i], AGG_TASK_AVG_COUNT, input); //bitno je da se AVG_COUNT i AVG_SUM dodaju na pocetak tak da se uvijek racunaju prije samog AVGa
            i++;
            AK_agg_input_add_to_beginning((*input).attributes[i], AGG_TASK_AVG_SUM, input);
            i++;
        }
        i++;
    }
}

/**
Function aggregates a given table by given attributes
@param input - input object with list of atributes by which we aggregate and types of aggregations
@param source_table - table name for the source table
@param agg_table - table name for aggregated table
@return EXIT_SUCCESS if continues succesfuly, when not EXIT_ERROR
@author Dejan Frankovic
 */
int AK_aggregation(AK_agg_input *input, char *source_table, char *agg_table) {
    //int AK_aggregation (AK_header *att_root,int *att_tasks,char *source_table, char *new_table) {
    int i, j;
    AK_agg_input_fix(input);
    AK_header *att_root = (*input).attributes;
    int *att_tasks = (*input).tasks;
    int header_size = (*input).counter;
    // int header_size = AK_header_size(att_root);
    AK_header agg_head[MAX_ATTRIBUTES];
    int agg_group_number = 0;
    int inttemp = 0;
    double doubletemp = 0;
    float floattemp = 0;

    char agg_h_name[MAX_ATT_NAME];
    int agg_h_type;
    char group_h_name[MAX_ATT_NAME];

    AK_agg_value *needed_values = malloc(sizeof (AK_agg_value) * header_size);

    char new_table[MAX_ATT_NAME];
    sprintf(new_table, "_%s", agg_table);

    for (i = 0; i < header_size; i++) {
        agg_h_type = att_root[i].type;
        switch (att_tasks[i]) {
            case AGG_TASK_GROUP:
                strcpy(agg_h_name, att_root[i].att_name);
                strcpy(group_h_name, att_root[i].att_name);
                agg_group_number++;
                break;
            case AGG_TASK_COUNT:
                sprintf(agg_h_name, "Cnt(%s)", att_root[i].att_name);
                agg_h_type = TYPE_INT;
                break;
            case AGG_TASK_SUM:
                sprintf(agg_h_name, "Sum(%s)", att_root[i].att_name);
                if (agg_h_type != TYPE_INT && agg_h_type != TYPE_FLOAT && agg_h_type != TYPE_NUMBER)
                    agg_h_type = TYPE_INT;
                break;
            case AGG_TASK_MAX:
                sprintf(agg_h_name, "Max(%s)", att_root[i].att_name);
                if (agg_h_type != TYPE_INT && agg_h_type != TYPE_FLOAT && agg_h_type != TYPE_NUMBER)
                    agg_h_type = TYPE_INT;
                break;
            case AGG_TASK_MIN:
                sprintf(agg_h_name, "Min(%s)", att_root[i].att_name);
                if (agg_h_type != TYPE_INT && agg_h_type != TYPE_FLOAT && agg_h_type != TYPE_NUMBER)
                    agg_h_type = TYPE_INT;
                break;
            case AGG_TASK_AVG:
                sprintf(agg_h_name, "Avg(%s)", att_root[i].att_name);
                agg_h_type = TYPE_FLOAT;
                break;
            case AGG_TASK_AVG_COUNT:
                sprintf(agg_h_name, "_cAvg(%s)", att_root[i].att_name);
                agg_h_type = TYPE_INT;
                break;
            case AGG_TASK_AVG_SUM:
                sprintf(agg_h_name, "_sAvg(%s)", att_root[i].att_name);
                agg_h_type = TYPE_FLOAT;
                break;
        }
        needed_values[i].agg_task = att_tasks[i];
        strcpy(needed_values[i].att_name, att_root[i].att_name);
        agg_head[i] = *(AK_header*) AK_create_header(agg_h_name, agg_h_type, FREE_INT, FREE_CHAR, FREE_CHAR);
    }

    for (i = header_size; i < MAX_ATTRIBUTES; i++) {
        memcpy(&agg_head[i], "\0", sizeof ( "\0"));
    }

    int startAddress = AK_initialize_new_segment(new_table, SEGMENT_TYPE_TABLE, agg_head);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", new_table);

    // ovo bi bila neka optimizacija, da sort radi kak spada..
    //sort_segment(source_table,group_h_name);


    search_params search_parameters[agg_group_number];
    search_result sresult;
    j = 0;


    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(source_table);
    int num_attr = AK_num_attr(source_table);

    int k, l, m, n, o, counter;
    char data[ MAX_VARCHAR_LENGTH ];

    AK_block *temp;
    AK_mem_block *mem_block;

    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);

    i = 0;
    counter = 0;

    int tmpadr = 0, tmpadr1;

    while (addresses->address_from[ i ] != 0) {
        for (j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++) {
            temp = (AK_block*) AK_read_block(j);
            for (k = 0; k < DATA_BLOCK_SIZE; k += num_attr) {
                counter++;

                if (temp->tuple_dict[k].type == FREE_INT)
					break;

                n = 0;

                //aggregation when no grouping is done will be executed separately to skip the unsorted search
                if(agg_group_number == 0){
                	for (l = 0; l < num_attr; l++) {
						for (m = 0; m < header_size; m++) {
							if (strcmp(needed_values[m].att_name, temp->header[l].att_name) == 0) {
								switch (needed_values[m].agg_task) {
									case AGG_TASK_COUNT:
										//no break is intentional
									case AGG_TASK_AVG_COUNT:
										memcpy(needed_values[m].data, &counter, sizeof(int));
										needed_values[m].data[sizeof(int)] = '\0';
										break;

									case AGG_TASK_MAX:
										switch (agg_head[m].type) {
											case TYPE_INT:
												memcpy(&inttemp, &(temp->data[temp->tuple_dict[k + l].address]), temp->tuple_dict[k + l].size);
												if (*((int*)needed_values[m].data) < inttemp || counter == 1)
													memcpy(needed_values[m].data, &inttemp, sizeof (int));
												break;

											case TYPE_FLOAT:
												memcpy(&floattemp, &(temp->data[temp->tuple_dict[k + l].address]), temp->tuple_dict[k + l].size);
												if (*((float *)needed_values[m].data) < floattemp || counter == 1)
													memcpy(needed_values[m].data, &floattemp, sizeof(float));
												break;

											case TYPE_NUMBER:
												memcpy(&doubletemp, &(temp->data[temp->tuple_dict[k + l].address]), temp->tuple_dict[k + l].size);
												if (*((double *)needed_values[m].data) < doubletemp || counter == 1)
													memcpy(needed_values[m].data, &doubletemp, sizeof(double));
												break;
										}
										needed_values[m].data[temp->tuple_dict[k + l].size] = '\0';
										break;

									case AGG_TASK_MIN:
										switch (agg_head[m].type) {
											case TYPE_INT:
												memcpy(&inttemp, &(temp->data[temp->tuple_dict[k + l].address]), temp->tuple_dict[k + l].size);
												if (*((int*)needed_values[m].data) > inttemp || counter == 1)
													memcpy(needed_values[m].data, &inttemp, sizeof (int));
												break;

											case TYPE_FLOAT:
												memcpy(&floattemp, &(temp->data[temp->tuple_dict[k + l].address]), temp->tuple_dict[k + l].size);
												if (*((float *)needed_values[m].data) > floattemp || counter == 1)
													memcpy(needed_values[m].data, &floattemp, sizeof(float));
												break;

											case TYPE_NUMBER:
												memcpy(&doubletemp, &(temp->data[temp->tuple_dict[k + l].address]), temp->tuple_dict[k + l].size);
												if (*((double *)needed_values[m].data) > doubletemp || counter == 1)
													memcpy(needed_values[m].data, &doubletemp, sizeof(double));
												break;
										}
										needed_values[m].data[temp->tuple_dict[k + l].size] = '\0';
										break;

									case AGG_TASK_SUM:
										//no break is intentional
									case AGG_TASK_AVG_SUM:
										switch (agg_head[m].type) {
											case TYPE_INT:
												memcpy(&inttemp, &(temp->data[temp->tuple_dict[k + l].address]), temp->tuple_dict[k + l].size);
												inttemp += *((int*) needed_values[m].data);
												memcpy(needed_values[m].data, &inttemp, sizeof (int));
												break;

											case TYPE_FLOAT:
												memcpy(&floattemp, &(temp->data[temp->tuple_dict[k + l].address]), temp->tuple_dict[k + l].size);
												floattemp += *((float *)needed_values[m].data);
												memcpy(needed_values[m].data, &floattemp, sizeof (float));
												break;

											case TYPE_NUMBER:
												memcpy(&doubletemp, &(temp->data[temp->tuple_dict[k + l].address]), temp->tuple_dict[k + l].size);
												doubletemp += *((double*) needed_values[m].data);
												memcpy(needed_values[m].data, &doubletemp, sizeof (double));
												break;
										}
										needed_values[m].data[temp->tuple_dict[k + l].size] = '\0';
								}
							}
						}
					}
				}
				else{

					for (l = 0; l < num_attr; l++) {
						for (m = 0; m < header_size; m++) {
							if (strcmp(needed_values[m].att_name, temp->header[l].att_name) == 0) {

								memcpy(needed_values[m].data, &(temp->data[temp->tuple_dict[k + l].address]), temp->tuple_dict[k + l].size);
								needed_values[m].data[temp->tuple_dict[k + l].size] = '\0';

								if (needed_values[m].agg_task == AGG_TASK_GROUP) {
									search_parameters[n].iSearchType = SEARCH_PARTICULAR;
									search_parameters[n].pData_lower = malloc(temp->tuple_dict[k + l].size);
									memcpy(search_parameters[n].pData_lower, &(temp->data[temp->tuple_dict[k + l].address]), temp->tuple_dict[k + l].size);
									search_parameters[n].szAttribute = (temp->header[l].att_name);
									n++;
								}
							}
						}
					}

					sresult = AK_search_unsorted(new_table, search_parameters, agg_group_number);

					if (sresult.iNum_tuple_addresses == 0) {
						Ak_DeleteAll_L(row_root);

						for (l = 0; l < header_size; l++) {
							switch (needed_values[l].agg_task) {
								case AGG_TASK_COUNT:
									//no break is intentional
								case AGG_TASK_AVG_COUNT:
									inttemp = 1;
									memcpy(needed_values[l].data, &inttemp, sizeof (int));
									needed_values[l].data[sizeof (int) ] = '\0';
									Ak_Insert_New_Element(agg_head[l].type, needed_values[l].data, new_table, agg_head[l].att_name, row_root);
									break;

								case AGG_TASK_AVG:
									//no break is intentional
								case AGG_TASK_MAX:
									//no break is intentional
								case AGG_TASK_MIN:
									//no break is intentional
								case AGG_TASK_SUM:
									//no break is intentional
								case AGG_TASK_GROUP:
									//no break is intentional
								case AGG_TASK_AVG_SUM:
									//no break is intentional
								default:
									Ak_Insert_New_Element(agg_head[l].type, needed_values[l].data, new_table, agg_head[l].att_name, row_root);
							}

						}
						Ak_insert_row(row_root);
					} else {
						mem_block = AK_get_block(sresult.aiBlocks[0]);

						for (l = 0; l < num_attr; l++) {
							for (m = 0; m < header_size; m++) {
								if (strcmp(needed_values[m].att_name, temp->header[l].att_name) == 0) {
									switch (needed_values[m].agg_task) {
										case AGG_TASK_COUNT:
											//no break is intentional
										case AGG_TASK_AVG_COUNT:
											inttemp = *((int *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address)) + 1;
											memcpy(&mem_block->block->data[mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address], &inttemp, sizeof (int));
											break;

										case AGG_TASK_SUM:
											//no break is intentional
										case AGG_TASK_AVG_SUM:
											switch (agg_head[m].type) {
												case TYPE_INT:
													inttemp = *((int*) needed_values[m].data) + *((int *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address));
													memcpy(mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address, &inttemp, sizeof (int));
													break;

												case TYPE_FLOAT:
													floattemp = *((float*) needed_values[m].data) + *((float *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address));
													memcpy(mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address, &floattemp, sizeof (float));
													break;

												case TYPE_NUMBER:
													doubletemp = *((double*) needed_values[m].data) + *((double *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address));
													memcpy(mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address, &doubletemp, sizeof (double));
													break;
											}
											break;

										case AGG_TASK_MAX:
											switch (agg_head[m].type) {
												case TYPE_INT:
													inttemp = *((int *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address));
													if (*((int*) needed_values[m].data) > inttemp) {
														inttemp = *((int*) needed_values[m].data);
														memcpy(mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address, &inttemp, sizeof (int));
													}
													break;

												case TYPE_FLOAT:
													floattemp = *((float *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address));
													if (*((float*) needed_values[m].data) > floattemp) {
														floattemp = *((float*) needed_values[m].data);
														memcpy(mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address, &floattemp, sizeof (float));
													}
													break;

												case TYPE_NUMBER:
													doubletemp = *((double *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address));
													if (*((double*) needed_values[m].data) > doubletemp) {
														doubletemp = *((double*) needed_values[m].data);
														memcpy(mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address, &doubletemp, sizeof (double));
													}
													break;
											}
											break;

										case AGG_TASK_MIN:
											switch (agg_head[m].type) {
												case TYPE_INT:
													inttemp = *((int *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address));
													if (*((int*) needed_values[m].data) < inttemp) {
														inttemp = *((int*) needed_values[m].data);
														memcpy(mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address, &inttemp, sizeof (int));
													}
													break;

												case TYPE_FLOAT:
													floattemp = *((float *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address));
													if (*((float*) needed_values[m].data) < floattemp) {
														floattemp = *((float*) needed_values[m].data);
														memcpy(mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address, &floattemp, sizeof (float));
													}
													break;

												case TYPE_NUMBER:
													doubletemp = *((double *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address));
													if (*((double*) needed_values[m].data) < doubletemp) {
														doubletemp = *((double*) needed_values[m].data);
														memcpy(mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address, &doubletemp, sizeof (double));
													}
													break;
											}
											break;

										case AGG_TASK_AVG:
											inttemp = floattemp = -1;
											for (o = 0; o < header_size; o++) {
												sprintf(agg_h_name, "_cAvg(%s)", needed_values[m].att_name);

												if (strcmp(agg_h_name, mem_block->block->header[o].att_name) == 0) {
													inttemp = *((int*) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + o].address));
												}
												sprintf(agg_h_name, "_sAvg(%s)", needed_values[m].att_name);

												if (strcmp(agg_h_name, mem_block->block->header[o].att_name) == 0) {
													floattemp = *((float*) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + o].address));
												}

												if (inttemp != -1 && doubletemp != -1)
													break;
											}
											floattemp = floattemp / (float) inttemp;
											memcpy(mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + m].address, &floattemp, sizeof (float));
											break;

										case AGG_TASK_GROUP:
											break;
									}
								}
							}
						}
						AK_deallocate_search_result(sresult);
						Ak_DeleteAll_L(row_root);

						for (l = 0; l<header_size;l++) {
							if (needed_values[l].agg_task == AGG_TASK_GROUP)
							   inttemp = 1;
							else
								inttemp = 0;
							Ak_Insert_New_Element_For_Update(agg_head[l].type, needed_values[l].data, new_table, agg_head[l].att_name, row_root, inttemp);
						}

						Ak_update_delete_row_from_block(mem_block->block, row_root, 0);
						mem_block->timestamp_last_change = clock();
						mem_block->dirty = BLOCK_DIRTY;
					}
				}
            }
        }
        i++;
    }

    //if there is no grouping, we only need to insert one row into the table and we can insert it straight into the destination table
    if(agg_group_number == 0){

    	AK_header agg_head_final[MAX_ATTRIBUTES];
    	j = 0;

    	for (i = 0; i < header_size; i++) {
			if (agg_head[i].att_name[0] != '_') {
				agg_head_final[j] = agg_head[i];
				j++;
			}
		}

    	startAddress = AK_initialize_new_segment(agg_table, SEGMENT_TYPE_TABLE, agg_head_final);

		if (startAddress != EXIT_ERROR)
			printf("\nTABLE %s CREATED!\n", agg_table);

    	Ak_DeleteAll_L(row_root);

		for (l = 0; l < header_size; l++) {

			switch (needed_values[l].agg_task) {

				case AGG_TASK_AVG_SUM:
					floattemp = *((float *)needed_values[l].data);
					break;

				case AGG_TASK_AVG_COUNT:
					inttemp = *((int *)needed_values[l].data);
					break;

				case AGG_TASK_AVG:
					floattemp = floattemp/inttemp;
					memcpy(needed_values[l].data, &floattemp, sizeof(float));
					needed_values[l].data[sizeof(float)] = '\0';
					//no break is intentional
				case AGG_TASK_COUNT:
					//no break is intentional
				case AGG_TASK_MAX:
					//no break is intentional
				case AGG_TASK_MIN:
					//no break is intentional
				case AGG_TASK_SUM:
					//no break is intentional
				default:
					Ak_Insert_New_Element(agg_head[l].type, needed_values[l].data, agg_table, agg_head[l].att_name, row_root);
					break;
			}

		}
		Ak_insert_row(row_root);
    }
    else{

    	AK_list *projection_att = (AK_list *) malloc(sizeof (AK_list));
		Ak_Init_L(projection_att);

		for (i = 0; i < header_size; i++) {
			if (agg_head[i].att_name[0] != '_') {
				Ak_InsertAtEnd_L(TYPE_ATTRIBS, agg_head[i].att_name, strlen(agg_head[i].att_name), projection_att);
			}
		}

		AK_projection(new_table, agg_table, projection_att);

		Ak_DeleteAll_L(projection_att);
		free(projection_att);
    }

    //@TODO zamijeniti ovaj segment sa AK_drop_table() jednom kad ga netko napravi
	addresses = (table_addresses*) AK_get_table_addresses(new_table);
	i = 0;
	while (addresses->address_from[i] != 0) {
		AK_delete_extent(addresses->address_from[i], addresses->address_to[i]);
		i++;
	}

    free(needed_values);
    free(row_root);
    free(temp);

    return EXIT_SUCCESS;
}

void Ak_aggregation_test() {
    printf("aggregation.c: Present!\n");

    char *tblName = "student";
    AK_header *t_header = (AK_header *) AK_get_header(tblName);

    AK_agg_input aggregation;
    AK_agg_input_init(&aggregation);
//    AK_agg_input_add(t_header[1], AGG_TASK_GROUP, &aggregation);
    AK_agg_input_add(t_header[4], AGG_TASK_AVG, &aggregation);
    AK_agg_input_add(t_header[2], AGG_TASK_COUNT, &aggregation);
    AK_agg_input_add(t_header[4], AGG_TASK_SUM, &aggregation);
    AK_agg_input_add(t_header[4], AGG_TASK_MAX, &aggregation);
    AK_agg_input_add(t_header[4], AGG_TASK_MIN, &aggregation);
    free(t_header);

    AK_aggregation(&aggregation, tblName, "agg");
    AK_print_table("agg");
}
