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
 * @author Matija Šestak.
 * @brief  Alocate empty list
 * @param AK_list* - root of the list
 * @result void
 */
void InitL( AK_list *L ){
    L->next = NULL;
}

/**
 * @author Matija Šestak.
 * @brief  Get the first element of the list
 * @param AK_list* - root of the list
 * @result AK_list_elem - first element of the list
 */
AK_list_elem FirstL( AK_list *L ){
    return L->next;
}

/**
 * @author Matija Šestak.
 * @brief  Get the last element of the list
 * @param AK_list* - root of the list
 * @result AK_list_elem - last element of the list
 */
AK_list_elem EndL( AK_list *L ){
    AK_list_elem current = FirstL( L );
    if( current == NULL )
        return NULL;
    while( current->next != NULL )
        current = current->next;
    return current;
}

/**
 * @author Matija Šestak.
 * @brief  Get the next element of the list
 * @param AK_list_elem - current element in the list
 * @param AK_list* - root of the list
 * @result AK_list_elem - next element in the list
 */
AK_list_elem NextL( AK_list_elem current, AK_list *L ){
    if( current == NULL )
        return NULL;
    return current->next;
}

/**
 * @author Matija Šestak.
 * @brief  Get the previous element of the list
 * @param AK_list_elem - current element in the list
 * @param AK_list* - root of the list
 * @result AK_list_elem - previous element in the list
 */
AK_list_elem PreviousL( AK_list_elem current, AK_list *L ){
    if( current == NULL || current == L )
        return NULL;
    AK_list_elem previous = L;
    while( previous->next != current )
        previous = previous->next;
    return previous;
}

/**
 * @author Matija Šestak.
 * @brief  Test whether the list is empty
 * @param AK_list* - root of the list
 * @result int - returns 1 if the list is empty, otherwise returns 0
 */
int IsEmptyL( AK_list *L ){
    return FirstL( L ) == NULL ? 1 : 0;
}

/**
 * @author Matija Šestak.
 * @brief  Inserts new element before the current element in the list
 * @param int - data type of the new element
 * @param char* - new data
 * @param int - data size of the new element in the list
 * @param AK_list_elem - current element in the list
 * @param AK_list* - root of the list
 * @result void
 */
void InsertBeforeL( int type, char* data, int size, AK_list_elem current, AK_list *L ){
    AK_list_elem new_elem = (AK_list_elem)malloc( sizeof( struct list_elem ));
    new_elem->size = size;
    new_elem->type = type;
    memcpy( new_elem->data, data, new_elem->size );
    if( current == NULL ){
        L->next = new_elem;
        new_elem->next = NULL;
        return;
    }
    AK_list_elem previous = PreviousL( current, L );
    previous->next = new_elem;
    new_elem->next = current;
}

/**
 * @author Matija Šestak.
 * @brief  Inserts new element after the current element in the list
 * @param int - data type of the new element
 * @param char* - new data
 * @param int - data size of the new element in the list
 * @param AK_list_elem - current element in the list
 * @param AK_list* - root of the list
 * @result void
 */
void InsertAfterL( int type, char* data, int size, AK_list_elem current, AK_list *L ){
    AK_list_elem new_elem = (AK_list_elem)malloc( sizeof( struct list_elem ));
    new_elem->size = size;
    new_elem->type = type;
    memcpy( new_elem->data, data, new_elem->size );
    if( current == NULL ){
        L->next = new_elem;
        new_elem->next = NULL;
        return;
    }
    AK_list_elem next = NextL( current, L );
    new_elem->next = current->next;
    current->next = new_elem;
}

/**
 * @author Matija Šestak.
 * @brief  Inserts new element at the begin of the list
 * @param int - data type of the new element
 * @param char* - new data
 * @param int - data size of the new element in the list
 * @param AK_list* - root of the list
 * @result void
 */
void InsertAtBeginL( int type, char* data, int size, AK_list *L ){
    InsertBeforeL( type, data, size, FirstL( L ), L );
}

/**
 * @author Matija Šestak.
 * @brief  Inserts new element at the end of the list
 * @param int - data type of the new element
 * @param char* - new data
 * @param int - data size of the new element in the list
 * @param AK_list* - root of the list
 * @result void
 */
void InsertAtEndL( int type, char* data, int size, AK_list *L ){
    InsertAfterL( type, data, size, EndL( L ), L );
}

/**
 * @author Matija Šestak.
 * @brief  Deletes the current element in the list
 * @param AK_list_elem - current element in the list
 * @param AK_list* - root of the list
 * @result void
 */
void DeleteL( AK_list_elem current, AK_list *L){
    if( current == NULL )
        return;
    AK_list_elem previous = PreviousL( current, L );
    AK_list_elem next = NextL( current, L );

    previous->next = next;
    free( current );
}

/**
 * @author Matija Šestak.
 * @brief  Makes list empty.
 * @param AK_list* - root of the list
 * @result void
 */
void DeleteAllL( AK_list *L ){
    AK_list_elem current;
    while( current = FirstL(L)){
        L->next = current->next;
        free( current );
    }
}

/**
 * @author Matija Šestak.
 * @brief  Get a number of the elements in the list
 * @param AK_list* - root of the list
 * @result int - size of the list
 */
int SizeL( AK_list *L ){
    int size = 0;
    AK_list_elem current = FirstL( L );
    while( current ){
        size++;
        current = NextL( current, L );
    }
    return size;
}

/**
 * @author Matija Šestak.
 * @brief  Retrieves data from the current element in the list
 * @param AK_list_elem - current element in the list
 * @param AK_list* - root of the list
 * @result char* - data from the list element
 */
char* RetrieveL( AK_list_elem current, AK_list *L ){
    if( current == NULL )
        return NULL;
    char *data = (char*)malloc(current->size);
    memcpy( data, current->data, current->size );
    return data;
}

/**
 * @author Matija Šestak.
 * @brief  Get a type of the current list element
 * @param AK_list_elem - current list element
 * @param AK_list* - root of the list
 * @result int - data type  of the current list element
 */
int GetTypeL( AK_list_elem current, AK_list *L ){
    return (current == NULL ) ? 0:current->type;
}

/**
 * @author Matija Šestak.
 * @brief  Get a data size of the element
 * @param AK_list_elem - current list element
 * @param AK_list* - root of the list
 * @result int - data size of the current list element
 */
int GetSizeL( AK_list_elem current, AK_list *L ){
    return (current == NULL ) ? 0:current->size;
}


/**
 * @brief  Determine the number of attributes in the table
 * <ol>
 * <li>Read addresses of extents</li>
 * <li>If there is no extents in the table, return -1</li>
 * <li>else read the first block</li>
 * <li>while  header tuple exists in the block, increment num_attr</li>
 * </ol>
 * @author Matija Šestak.
 * @param char* - table name
 * @result int - number of attributes in the table
 */
int AK_num_attr( char * tblName ){
    int num_attr = 0;
    table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );
    if( addresses->address_from[0] == 0 )
        num_attr = -1;
    else{
        AK_block *temp = (AK_block *)AK_read_block( addresses->address_from[0] );
        int i = 0;
        while( strcmp( temp->header[i].att_name,"\0" ) != 0 ){
            num_attr++;
            i++;
        }
    }
    return num_attr;
}

/**
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
 * @author Matija Šestak.
 * @param char* - table name
 * @result int - number of rows in the table
 */
int AK_get_num_records( char *tblName ){
    int num_rec = 0;
    table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );
    if( addresses->address_from[0] == NULL )
        num_rec = -1;
    else{
        int i, j, k;
        i = 0;
        while( addresses->address_from[ i ] != 0 ){
            for( j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++ ){
                AK_mem_block *temp = (AK_mem_block*) AK_get_block( j );
                if( temp->block->last_tuple_dict_id == 0)
                    break;
                for( k = 0; k < DATA_BLOCK_SIZE; k++ ){
                    if( temp->block->tuple_dict[k].type == FREE_INT )
                        break;
                    num_rec++;
                }
            }
            i++;
        }
    }
    int num_head = AK_num_attr( tblName );
    return num_rec / num_head;
}

/**
 * @brief  Get table header
 * <ol>
 * <li>Read addresses of extents</li>
 * <li>If there is no extents in the table, return -1</li>
 * <li>else read the first block</li>
 * <li>allocate array</li>
 * <li>copy table header to the array</li>
 * </ol>
 * @author Matija Šestak.
 * @param char* - table name
 * @result AK_header* - array of table header
 */
AK_header* AK_get_header( char *tblName ){
    table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );
    if( addresses->address_from[0] == NULL )
        return 0;
    AK_mem_block *temp = (AK_mem_block*) AK_get_block( addresses->address_from[0] );

    int num_attr = AK_num_attr( tblName );
    AK_header *head = (AK_header*)malloc( num_attr * sizeof(AK_header));
    memcpy( head, temp->block->header, num_attr * sizeof(AK_header));
    return head;
}

/**
 * @brief  Print table
 * @author Matija Šestak.
 * @param char* - table name
 * @result void
 */
void AK_print_table( char *tblName ){
        int num_attr, num_rec;
        int i, j, k;
        //start and end addresses of segment's
        table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );

        num_attr = AK_num_attr( tblName );
        num_rec = AK_get_num_records( tblName );

        AK_mem_block *temp = (AK_mem_block*)AK_get_block( addresses->address_from[0]);
        AK_header *temp_head = AK_get_header( tblName );
        int temp_int;
        char temp_char[ MAX_VARCHAR_LENGHT ];
        float temp_float;

        printf( "\n");
        for( i = 0; i < num_attr; i++ )
            printf( "%-10s", (temp_head+i)->att_name );
        printf( "\n----------------------------------------------\n");
        free( temp_head );
        
        i = 0;
        while( addresses->address_from[ i ] != 0 ){
            for( j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++ ){
                temp = (AK_mem_block*) AK_get_block( j );
                if( temp->block->last_tuple_dict_id == 0)
                    break;
                for( k = 0; k < DATA_BLOCK_SIZE; k++ ){
                    if( temp->block->tuple_dict[ k ].size > 0 ){
                        switch( temp->block->tuple_dict[ k ].type ){
                            case TYPE_INT:
                                memcpy( &temp_int, &(temp->block->data[ temp->block->tuple_dict[ k ].address]),
                                        temp->block->tuple_dict[k].size);
                                printf( "%-10d", temp_int );
                                break;
                            case TYPE_FLOAT:
                                memcpy( &temp_float, &(temp->block->data[ temp->block->tuple_dict[ k ].address]),
                                        temp->block->tuple_dict[k].size);
                                printf( "%-10.2f", temp_float );
                                break;

                            case TYPE_VARCHAR:
                                memcpy( temp_char, &(temp->block->data[ temp->block->tuple_dict[ k ].address]),
                                        temp->block->tuple_dict[k].size);
                                temp_char[ temp->block->tuple_dict[k].size ] = '\0';
                                printf( "%-10s", temp_char );
                                break;
                        }
                        if( (k+1) % num_attr == 0 )
                            printf( "\n");
                    }
                }
            }
            i++;
        }
        printf( "\n");
        printf( "(%d RECORDS FOUND)\n\n", num_rec );
}

/**
 * @brief  Evaluate logical expression
 * @author Matija Šestak.
 * @param AK_mem_block* - memory block from cache
 * @param AK_header* - table header
 * @param int - number of the attributes
 * @param AK_list* - list with posfix notation of the logical expression
 * @param int - current tuple (first in row)
 * @result int - 0 if false, 1 if true
 */
int AK_selection_check_expr( AK_mem_block *mem_block, AK_header *header, int num_attr, AK_list *expr, int current_tuple ){
    AK_list temp;
    InitL( &temp );
    int true = 1, false = 0;
    
    AK_list_elem el = FirstL( expr );
    AK_list_elem a,b,c;
    
    int i;
    int found;

    char data[ MAX_VARCHAR_LENGHT ];
    while( el ){
        if( el->type == TYPE_OPERAND ){
            found = 0;
            for( i = 0; i < num_attr; i++ ){
                if( strcmp( el->data, header[i].att_name) == 0 ){
                    found = 1;
                    break;
                }
            }
            if( !found )
                return 0;
            else{
                int type = mem_block->block->tuple_dict[ current_tuple + i ].type;
                int size = mem_block->block->tuple_dict[ current_tuple + i ].size;
                memcpy( data, &(mem_block->block->data[mem_block->block->tuple_dict[current_tuple + i].address]), size );
                InsertAtEndL( type, data, size, &temp );
            }
        }
        else
            if( el->type == TYPE_OPERATOR ){
                //operators implementation                    
                a = EndL( &temp );
                b = PreviousL( a, &temp );

                if( strcmp( el->data, "=") == 0 ){
                    if( memcmp( a->data, b->data, a->size ) == 0 )
                        InsertAtEndL( TYPE_INT, &true, sizeof( int ), &temp );
                    else
                        InsertAtEndL( TYPE_INT, &false, sizeof( int ), &temp );
                }
                else
                    if( strcmp( el->data, "<>") == 0 ){
                        if( memcmp( a->data, b->data, a->size ) != 0 )
                            InsertAtEndL( TYPE_INT, &true, sizeof( int ), &temp );
                        else
                            InsertAtEndL( TYPE_INT, &false, sizeof( int ), &temp );
                    }
                else
                    if( strcmp( el->data, "OR") == 0 ){
                        int val_a, val_b;
                        memcpy( &val_a, a->data, sizeof(int));
                        memcpy( &val_b, b->data, sizeof( int));
                        if( val_a || val_b )
                            InsertAtEndL( TYPE_INT, &true, sizeof( int ), &temp );
                        else
                            InsertAtEndL( TYPE_INT, &false, sizeof( int ), &temp );
                    }
                else
                    if( strcmp( el->data, "AND") == 0 ){
                        int val_a, val_b;
                        memcpy( &val_a, a->data, sizeof(int));
                        memcpy( &val_b, b->data, sizeof( int));
                        if( val_a && val_b )
                            InsertAtEndL( TYPE_INT, &true, sizeof( int ), &temp );
                        else
                            InsertAtEndL( TYPE_INT, &false, sizeof( int ), &temp );
                    }
                else
                    {
                        int v1_int, v2_int;
                        float v1_float, v2_float;
                        char v1_varchar[ MAX_VARCHAR_LENGHT], v2_varchar[ MAX_VARCHAR_LENGHT];

                        switch( b->type ){
                            case TYPE_INT:
                                memcpy( &v1_int, b->data, b->size);
                                memcpy( &v2_int, a->data, a->size);
                                break;
                            case TYPE_FLOAT:
                                memcpy( &v1_float, b->data, b->size);
                                memcpy( &v2_float, a->data, a->size);
                                break;
                            case TYPE_VARCHAR:
                                memcpy( v1_varchar, b->data, b->size);
                                memcpy( v2_varchar, a->data, a->size);
                                break;
                        }

                    int rs;
                    if( strcmp( el->data, "<") == 0 ){
                        switch( b->type ){
                            case TYPE_INT:
                                if( v1_int < v2_int )
                                    rs = 1;
                                else
                                    rs = 0;
                                break;
                            case TYPE_FLOAT:
                                if( v1_float < v2_float )
                                    rs = 1;
                                else
                                    rs = 0;
                                break;
                            case TYPE_VARCHAR:
                                if( strcmp( v1_varchar, v2_varchar) < 0 )
                                    rs = 1;
                                else
                                    rs = 0;
                                break;
                        }
                    }
                    else
                        if( strcmp( el->data, ">") == 0 ){
                        switch( b->type ){
                            case TYPE_INT:
                                if( v1_int > v2_int )
                                    rs = 1;
                                else
                                    rs = 0;
                                break;
                            case TYPE_FLOAT:
                                if( v1_float > v2_float )
                                    rs = 1;
                                else
                                    rs = 0;
                                break;
                            case TYPE_VARCHAR:
                                if( strcmp( v1_varchar, v2_varchar) > 0 )
                                    rs = 1;
                                else
                                    rs = 0;
                                break;
                        }
                        }
                    else
                        if( strcmp( el->data, "<=") == 0 ){
                        switch( b->type ){
                            case TYPE_INT:
                                if( v1_int <= v2_int )
                                    rs = 1;
                                else
                                    rs = 0;
                                break;
                            case TYPE_FLOAT:
                                if( v1_float <= v2_float )
                                    rs = 1;
                                else
                                    rs = 0;
                                break;
                            case TYPE_VARCHAR:
                                if( strcmp( v1_varchar, v2_varchar) <= 0 )
                                    rs = 1;
                                else
                                    rs = 0;
                                break;
                        }
                        }
                    else
                        if( strcmp( el->data, ">=") == 0 ){
                        switch( b->type ){
                            case TYPE_INT:
                                if( v1_int >= v2_int )
                                    rs = 1;
                                else
                                    rs = 0;
                                break;
                            case TYPE_FLOAT:
                                if( v1_float >= v2_float )
                                    rs = 1;
                                else
                                    rs = 0;
                                break;
                            case TYPE_VARCHAR:
                                if( strcmp( v1_varchar, v2_varchar) >= 0 )
                                    rs = 1;
                                else
                                    rs = 0;
                                break;
                        }
                      }
                      if( rs )
                           InsertAtEndL( TYPE_INT, &true, sizeof( int ), &temp );
                      else
                           InsertAtEndL( TYPE_INT, &false, sizeof( int ), &temp );
                    }
                    DeleteL( a, &temp );
                    DeleteL( b, &temp );
                }
            else{
                InsertAtEndL( el->type, el->data, el->size, &temp );
            }
        el = NextL( el, expr );
    }
    int result;
    memcpy( &result, FirstL( &temp )->data, sizeof(int));
    DeleteAllL( &temp );

    return result;
}

/**
 * @brief  Implementation of selection
 * @author Matija Šestak.
 * @param char* - source table name
 * @param char* - destination table name
 * @param AK_list* - list with posfix notation of the logical expression
 * @result int - return EXIT_SUCCESS
 */
int AK_selection( char *srcTable, char *dstTable, AK_list *expr ){
    AK_header *t_header = AK_get_header( srcTable );
    int num_attr = AK_num_attr( srcTable );

    int startAddress = AK_initialize_new_segment( dstTable, SEGMENT_TYPE_TABLE, t_header);

    if( startAddress != EXIT_ERROR )
        printf( "\nTABLE %s CREATED!\n", dstTable );

    table_addresses *src_addr = (table_addresses*) get_table_addresses( srcTable );

    element row_root =  (element) malloc( sizeof(list) );
    InitializeList(row_root);

    int i, j, k, l;
    char data[ MAX_VARCHAR_LENGHT ];
    
    i = 0;
    while( src_addr->address_from[ i ] != 0 ){
        for( j = src_addr->address_from[ i ]; j < src_addr->address_to[ i ]; j++ ){
            AK_mem_block *temp = (AK_mem_block*) AK_get_block( j );
            if( temp->block->last_tuple_dict_id == 0)
               break;
            for( k = 0; k < DATA_BLOCK_SIZE; k+=num_attr ){
                if( temp->block->tuple_dict[k].type == FREE_INT )
                    break;
                if( AK_selection_check_expr( temp, t_header, num_attr, expr, k ) ){
                    for( l = 0; l < num_attr; l++ ){
                        int type = temp->block->tuple_dict[ k+l ].type;
                        int size = temp->block->tuple_dict[ k+l ].size;
                        int address = temp->block->tuple_dict[ k + l ].address;
                        memcpy( data, &(temp->block->data[address]), size );
                        data[ size ] = '\0';
                        InsertNewElement(type, data, dstTable, t_header[ l ].att_name, row_root );
                   }
                    insert_row( row_root );
                    DeleteAllElements(row_root);
                }
            }
        }
        i++;
    }
    return (EXIT_SUCCESS);
}

///Function for selection testing
void op_selection_test(){
    printf( "\n********** SELECTION TEST by Matija Šestak **********\n");

    int i;
    //create header
    AK_header t_header[ MAX_ATTRIBUTES ];
    AK_header* temp;

    temp = (AK_header*)AK_create_header( "mbr", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header, temp, sizeof( AK_header ));

    temp = (AK_header*)AK_create_header( "firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header + 1, temp, sizeof( AK_header ));

    temp = (AK_header*)AK_create_header( "lastname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header + 2, temp, sizeof( AK_header ));

    temp = (AK_header*)AK_create_header( "year", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header + 3, temp, sizeof( AK_header ));

    temp = (AK_header*)AK_create_header( "tezina", TYPE_FLOAT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header + 4, temp, sizeof( AK_header ));

    for( i = 5; i < MAX_ATTRIBUTES; i++ )
    {
	memcpy( t_header + i, "\0", sizeof( "\0" ));
    }
    
    //create table
    char *tblName = "student";
    
    printf("op_selection_test: Before segment initialization: %d\n", AK_num_attr( tblName ) );
    
    int startAddress = AK_initialize_new_segment( tblName, SEGMENT_TYPE_TABLE, t_header);
    
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
    
    
    printf("op_selection_test: After print: %d\n", AK_num_attr( tblName ) );

    AK_list expr;
    InitL( &expr );
    int num = 2002;
    
    InsertAtEndL( TYPE_OPERAND, "year", 4, &expr );
    InsertAtEndL( TYPE_INT, &num, sizeof(int), &expr );
    InsertAtEndL( TYPE_OPERATOR, ">", 1, &expr );
    InsertAtEndL( TYPE_OPERAND, "firstname", 9, &expr );
    InsertAtEndL( TYPE_VARCHAR, "Matija", 6, &expr );
    InsertAtEndL( TYPE_OPERATOR, "=", 1, &expr );
    InsertAtEndL( TYPE_OPERATOR, "OR", 2, &expr );

    printf( "QUERY:\nSELECT * FROM student WHERE year > 2002 OR firstname = 'Matija';\n");
    AK_selection( tblName, "selection_test", &expr );
    AK_print_table( "selection_test" );

    DeleteAllL( &expr );
    
    
    printf("op_selection_test: After selection: %d\n", AK_num_attr( tblName ) );
    
    //dealocate variables ;)
}
