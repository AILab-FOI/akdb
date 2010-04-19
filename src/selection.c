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

#define TYPE_OPERAND 10
#define TYPE_OPERATOR 11
//file is under construction but some functions however work ;-)
//you can: get number of attributes in table, table header, print table
//selection is still under construction



//returns number of attributes in table
int AK_num_attr( char * tblName ){
    int num_attr = 0;
    table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );
    if( addresses->address_from[0] == 0 )
        num_attr = -1;
    else{
        KK_block *temp = (KK_block *)KK_read_block( addresses->address_from[0] );
        int i = 0;
        while( strcmp( temp->header[i].att_name,"" ) != 0 ){
            num_attr++;
            i++;
        }
    }
    return num_attr;
}

//returns number of records
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
                KK_block *temp = (KK_block*) KK_read_block( j );
                for( k = 0; k < DATA_BLOCK_SIZE; k++ ){
                    if( temp->tuple_dict[k].type == FREE_INT )
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

KK_header* AK_get_header( char *tblName ){
    table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );
    if( addresses->address_from[0] == NULL )
        return 0;
    KK_block *temp = (KK_block*) KK_read_block( addresses->address_from[0] );

    int num_attr = AK_num_attr( tblName );
    KK_header *head = (KK_header*)malloc( num_attr * sizeof(KK_header));
    memcpy( head, temp->header, num_attr * sizeof(KK_header));
    return head;
}

//print table
void AK_print_table( char *tblName ){
        int num_attr, num_rec;
        int i, j, k;
        //start and end addresses of segment's
        table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );

        num_attr = AK_num_attr( tblName );
        num_rec = AK_get_num_records( tblName );

        KK_block *temp = (KK_block*)KK_read_block( addresses->address_from[0]);
        KK_header *temp_head = AK_get_header( tblName );
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
                KK_block *temp = (KK_block*) KK_read_block( j );
                for( k = 0; k < DATA_BLOCK_SIZE; k++ ){
                    if( temp->tuple_dict[ k ].size > 0 ){
                        switch( temp->tuple_dict[ k ].type ){
                            case TYPE_INT:
                                memcpy( &temp_int, &(temp->data[ temp->tuple_dict[ k ].address]),
                                        temp->tuple_dict[k].size);
                                printf( "%-10d", temp_int );
                                break;
                            case TYPE_FLOAT:
                                memcpy( &temp_float, &(temp->data[ temp->tuple_dict[ k ].address]),
                                        temp->tuple_dict[k].size);
                                printf( "%-10.2f", temp_float );
                                break;

                            case TYPE_VARCHAR:
                                memcpy( temp_char, &(temp->data[ temp->tuple_dict[ k ].address]),
                                        temp->tuple_dict[k].size);
                                temp_char[ temp->tuple_dict[k].size ] = '\0';
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

int AK_selection_check_expr( KK_block *block, KK_header *header, int num_attr, AK_list *expr, int current_tuple ){
    return 1;
}

int AK_selection( char *srcTable, char *dstTable, AK_list *expr ){
    KK_header *t_header = AK_get_header( srcTable );
    int num_attr = AK_num_attr( srcTable );

    int startAddress = KK_initialize_new_segment( dstTable, SEGMENT_TYPE_TABLE, t_header);

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
            KK_block *temp = (KK_block*) KK_read_block( j );
            for( k = 0; k < DATA_BLOCK_SIZE; k+=num_attr ){
                if( temp->tuple_dict[k].type == FREE_INT )
                    break;
                if( AK_selection_check_expr( temp, t_header, num_attr, expr, k ) ){
                    for( l = 0; l < num_attr; l++ ){
                        int type = temp->tuple_dict[ k+l ].type;
                        int size = temp->tuple_dict[ k+l ].size;
                        int address = temp->tuple_dict[ k + l ].address;
                        memcpy( data, &(temp->data[address]), size );
                        data[ size] = '\0';
                        InsertNewElement(type, data, dstTable, t_header[ l ].att_name, row_root );
                   }
                    insert_row( row_root );
                    DeleteAllElements(row_root);
                }
            }
        }
        i++;
    }
    AK_print_table( dstTable );
}

void op_selection_test(){
    printf( "\n********** SELECTION TEST by Matija Šestak **********\n");

    //create header
    KK_header t_header[ 5 ];
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

    //create table
    char *tblName = "student";
    int startAddress = KK_initialize_new_segment( tblName, SEGMENT_TYPE_TABLE, t_header);

    if( startAddress != EXIT_ERROR )
        printf( "\nTABLE %s CREATED!\n", tblName );

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
    
    //print table "student"
    AK_print_table( tblName );

    AK_list expr;
    InitL( &expr );
    InsertAtEndL( TYPE_OPERAND, "ime", 4, &expr );
    InsertAtEndL( TYPE_VARCHAR, "matija", 8, &expr );
    InsertAtEndL( TYPE_OPERATOR, "=", 1, &expr );
    
    AK_selection( tblName, "selection_test", &expr );
    DeleteAllL( &expr );
    //dealocate variables ;)
}
