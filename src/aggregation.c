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

/**
Function aggregates a given table by given attributes
@param att_root - list of atributes by which we aggregate
@param new_table - table name for aggregated table
@return EXIT_SUCCESS if continues succesfuly, when not EXIT_ERROR
@author Dejan Frankovic
*/
int AK_aggregation (KK_header *att_root,int *att_tasks,char *source_table, char *new_table) {
    int i,j;
    int header_size = 5; // for development purposes only.. must change later on to actually represent the number of attributes passed on
    KK_header agg_head[MAX_ATTRIBUTES];
    int agg_group_number = 0;
    int inttemp;
    double doubletemp;

    char agg_h_name[MAX_ATT_NAME];
    int agg_h_type;
    char group_h_name[MAX_ATT_NAME];

    //AK_agg_value needed_values[header_size];
    AK_agg_value *needed_values = (AK_agg_value*)malloc(sizeof(AK_agg_value)*header_size);
    //AK_list *row_root =  (AK_list*) malloc( sizeof(AK_list) );

    for (i = 0;i < header_size;i++) {
        agg_h_type = att_root[i].type;
        switch (att_tasks[i]) {
            case AGG_TASK_GROUP:
                strcpy(agg_h_name,att_root[i].att_name);
                strcpy(group_h_name, att_root[i].att_name);
                agg_group_number ++;
                break;
            case AGG_TASK_COUNT:
                sprintf(agg_h_name,"Cnt(%s)",att_root[i].att_name);
                break;
            case AGG_TASK_SUM:
                sprintf(agg_h_name,"Sum(%s)",att_root[i].att_name);
                break;
            case AGG_TASK_MAX:
                sprintf(agg_h_name,"Max(%s)",att_root[i].att_name);
                break;
            case AGG_TASK_MIN:
                sprintf(agg_h_name,"Min(%s)",att_root[i].att_name);
                break;
            case AGG_TASK_AVG:
                sprintf(agg_h_name,"Avg(%s)",att_root[i].att_name);
                break;
        }
        needed_values[i].agg_task = att_tasks[i];
        strcpy(needed_values[i].att_name,att_root[i].att_name);
        agg_head[i] = *(KK_header*)KK_create_header(agg_h_name,agg_h_type,FREE_INT,FREE_CHAR,FREE_CHAR);
    }

    for(i = header_size; i < MAX_ATTRIBUTES; i++ )
    {
	memcpy( &agg_head[i], "\0", sizeof( "\0" ));
    }

    int startAddress = KK_initialize_new_segment( new_table, SEGMENT_TYPE_TABLE, agg_head);
    
    if( startAddress != EXIT_ERROR )
        printf( "\nTABLE %s CREATED!\n", new_table );

    //sort_segment(source_table,group_h_name);



    
    
    search_params search_parameters[agg_group_number];
    search_result sresult;
    j=0;


    table_addresses *addresses = (table_addresses* ) get_table_addresses( source_table );
    KK_header *t_header = AK_get_header( source_table );
    int num_attr = AK_num_attr( source_table );

    
    //InitL(row_root);


    int k, l,m,n, counter;
    char data[ MAX_VARCHAR_LENGHT ];

    element row_root =  (element) malloc( sizeof(list) );
    InitializeList(row_root);

    i = 0;
    counter = 0;
    while( addresses->address_from[ i ] != 0 ){
        for( j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++ ){
            KK_block *temp = (KK_block*) KK_read_block( j );
            for( k = 0; k < DATA_BLOCK_SIZE; k+=num_attr ){
                counter++;
                if( temp->tuple_dict[k].type == FREE_INT )
                  break;
                n=0;
                for( l = 0; l < num_attr; l++ ){
                    for (m=0;m<header_size;m++)
                    {
                        if (strcmp(needed_values[m].att_name, temp->header[m].att_name)==0) {
                            memcpy(needed_values[m].data, &(temp->data[temp->tuple_dict[k+l+1].address]), temp->tuple_dict[k+l+1].size);
                            needed_values[m].data[temp->tuple_dict[k+l+1].size] = '\0';
                            if (needed_values[m].agg_task == AGG_TASK_GROUP) {
                                search_parameters[n].iSearchType = SEARCH_PARTICULAR;
                                //memcpy(search_parameters[n].pData_lower,&(temp->data[temp->tuple_dict[k+l+1].address]), temp->tuple_dict[k+l+1].size);
                                search_parameters[n].pData_lower = &(temp->data[temp->tuple_dict[k+l+1].address]);
                                //strcpy(search_parameters[n].szAttribute,temp->header[m].att_name);
                                search_parameters[n].szAttribute = &(temp->header[m].att_name);
                                n++;
                            }
                        }
                    }
                }

                /*sresult = */KK_tunnel(&search_parameters);
                //sresult = KK_search_unsorted(new_table,&search_parameters,agg_group_number);
                if (sresult.iNum_tuple_addresses == 0) {
                    DeleteAllElements(row_root);
                    for (l=0;l<header_size;l++) {
                        InsertNewElement(agg_head[l].type, needed_values[l].data, new_table, agg_head[l].att_name, row_root);
                    }
                    insert_row( row_root );
                }
                else {
                    KK_mem_block *mem_block = KK_get_block(sresult.aiBlocks[0]);
                    for (l=0;l<num_attr;l++) {
                        for (m=0;m<header_size;m++) {
                            if (strcmp(needed_values[m].att_name, temp->header[m].att_name)==0) {
                                switch (needed_values[m].agg_task) {
                                    case AGG_TASK_COUNT:
                                        inttemp =  *((int *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + l].address)) + 1;
                                        memcpy(needed_values[m].data,&inttemp,sizeof(int));
                                        break;
                                    case AGG_TASK_SUM:
                                        switch (agg_head[m].type) {
                                            case TYPE_INT:
                                                inttemp = *((int*)needed_values[m].data) + *((int *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + l].address));
                                                memcpy(needed_values[m].data,&inttemp,sizeof(int));
                                                break;
                                            case TYPE_FLOAT:
                                            case TYPE_NUMBER:
                                                doubletemp = *((double*)needed_values[m].data) + *((double *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + l].address));
                                                memcpy(needed_values[m].data,&doubletemp,sizeof(double));
                                                break;
                                        }
                                        break;
                                    case AGG_TASK_MAX:
                                        switch (agg_head[m].type) {
                                            case TYPE_INT:
                                                inttemp = *((int *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + l].address));
                                                if (*((int*)needed_values[m].data) < inttemp)
                                                    memcpy(needed_values[m].data,inttemp, sizeof(int));
                                                break;
                                            case TYPE_FLOAT:
                                            case TYPE_NUMBER:
                                                doubletemp = *((double *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + l].address));
                                                if (*((double*)needed_values[m].data) < doubletemp)
                                                    memcpy(needed_values[m].data,&doubletemp,sizeof(double));
                                                break;
                                        }
                                        break;
                                    case AGG_TASK_MIN:
                                        switch (agg_head[m].type) {
                                            case TYPE_INT:
                                                inttemp = *((int *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + l].address));
                                                if (*((int*)needed_values[m].data) > inttemp)
                                                    memcpy(needed_values[m].data, &inttemp,sizeof(int));
                                                break;
                                            case TYPE_FLOAT:
                                            case TYPE_NUMBER:
                                                doubletemp = *((double *) (mem_block->block->data + mem_block->block->tuple_dict[sresult.aiTuple_addresses[i] + l].address));
                                                if (*((double*)needed_values[m].data) > doubletemp)
                                                    memcpy(needed_values[m].data, &doubletemp,sizeof(double));
                                                break;
                                        }
                                        break;
                                    case AGG_TASK_AVG:
                                        break;
                                    case AGG_TASK_GROUP:
                                        break;
                                }
                            }
                        }
                    }
                    KK_deallocate_search_result(sresult);

                    DeleteAllElements(row_root);
                    for (l = 0; l<header_size;l++) {
                        InsertNewElement(agg_head[l].type, needed_values[l].data, new_table, agg_head[l].att_name, row_root);
                    }

                    update_delete_row_from_block(mem_block->block, row_root, 0);
                    mem_block->timestamp_last_change = clock();
                    mem_block->dirty = BLOCK_DIRTY;
                }

            }
        }
      i++;
    }


    return EXIT_SUCCESS;
}

void aggregation_test() 
{
  printf("aggregation.c: Present!\n");

  int i;
    //create header
    KK_header t_header[ MAX_ATTRIBUTES ];
    KK_header* temp;

    temp = (KK_header*)KK_create_header( "mbr", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header, temp, sizeof( KK_header ));

    temp = (KK_header*)KK_create_header( "firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header + 1, temp, sizeof( KK_header ));

    temp = (KK_header*)KK_create_header( "lastname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header + 2, temp, sizeof( KK_header ));

    temp = (KK_header*)KK_create_header( "year", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header + 3, temp, sizeof( KK_header ));

    temp = (KK_header*)KK_create_header( "tezina", TYPE_FLOAT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header + 4, temp, sizeof( KK_header ));

    for( i = 5; i < MAX_ATTRIBUTES; i++ )
    {
	memcpy( t_header + i, "\0", sizeof( "\0" ));
    }

    //create table
    char *tblName = "student";

    printf("op_selection_test: Before segment initialization: %d\n", AK_num_attr( tblName ) );

    int startAddress = KK_initialize_new_segment( tblName, SEGMENT_TYPE_TABLE, t_header);

    if( startAddress != EXIT_ERROR )
        printf( "\nTABLE %s CREATED!\n", tblName );


    printf("op_selection_test: After segment initialization: %d\n", AK_num_attr( tblName ) );

    element row_root =  (element) malloc( sizeof(list) );
    InitializeList(row_root);

    int mbr = 35890, year = 1999;
    float weight = 80.00;

    //insert rows in table student
    mbr++; year++; weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Matija", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Sestak", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Igor", tblName, "firstname", row_root);
    InsertNewElement( TYPE_VARCHAR, "Mesaric", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Dalibor", tblName, "firstname", row_root);
    InsertNewElement( TYPE_VARCHAR, "Slunjski", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Dino", tblName, "firstname", row_root);
    InsertNewElement( TYPE_VARCHAR, "Alagic", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Miroslav", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Zver", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Josip", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Vincek", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Netko", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Netkic", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    printf("op_selection_test: After data insertion: %d\n", AK_num_attr( tblName ) );

    //print table "student"
    AK_print_table( tblName );

    int tasks[5];
    tasks[0] = AGG_TASK_MAX;
    tasks[1] = AGG_TASK_GROUP;
    tasks[2] = AGG_TASK_COUNT;
    tasks[3] = AGG_TASK_MIN;
    tasks[4] = AGG_TASK_SUM;

   // AK_aggregation(t_header, tasks, tblName, "student_agg");
    AK_aggregation(t_header,tasks,tblName, "agg");

    AK_print_table("agg");
}