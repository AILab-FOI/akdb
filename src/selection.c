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

void InitL( AK_list *L ){
    L->next = NULL;
}

AK_list_elem FirstL( AK_list *L ){
    return L->next;
}

AK_list_elem EndL( AK_list *L ){
    AK_list_elem current = FirstL( L );
    if( current == NULL )
        return NULL;
    while( current->next != NULL )
        current = current->next;
    return current;
}

AK_list_elem NextL( AK_list_elem current, AK_list *L ){
    if( current == NULL )
        return NULL;
    return current->next;
}

AK_list_elem PreviousL( AK_list_elem current, AK_list *L ){
    if( current == NULL || current == L )
        return NULL;
    AK_list_elem previous = L;
    while( previous->next != current )
        previous = previous->next;
    return previous;
}

int IsEmptyL( AK_list *L ){
    return FirstL( L ) == NULL ? 1 : 0;
}

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

void InsertAtBeginL( int type, char* data, int size, AK_list *L ){
    InsertBeforeL( type, data, size, FirstL( L ), L );
}

void InsertAtEndL( int type, char* data, int size, AK_list *L ){
    InsertAfterL( type, data, size, EndL( L ), L );
}

void DeleteL( AK_list_elem current, AK_list *L){
    if( current == NULL )
        return;
    AK_list_elem previous = PreviousL( current, L );
    AK_list_elem next = NextL( current, L );

    previous->next = next;
    free( current );
}

void DeleteAllL( AK_list *L ){
    AK_list_elem current;
    while( current = FirstL(L)){
        L->next = current->next;
        free( current );
    }
}

int SizeL( AK_list *L ){
    int size = 0;
    AK_list_elem current = FirstL( L );
    while( current ){
        size++;
        current = NextL( current, L );
    }
    return size;
}

char* RetrieveL( AK_list_elem current, AK_list *L ){
    if( current == NULL )
        return NULL;
    char *data = (char*)malloc(current->size);
    memcpy( data, current->data, current->size );
    return data;
}

int GetTypeL( AK_list_elem current, AK_list *L ){
    return (current == NULL ) ? 0:current->type;
}

int GetSizeL( AK_list_elem current, AK_list *L ){
    return (current == NULL ) ? 0:current->size;
}


//returns number of attributes in table
int AK_num_attr( char * tblName ){
    int num_attr = 0;
    table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );
    if( addresses->address_from[0] == 0 )
        num_attr = -1;
    else{
        KK_block *temp = (KK_block *)KK_read_block( addresses->address_from[0] );
        int i = 0;
        while( strcmp( temp->header[i].att_name,"\0" ) != 0 ){
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
                int type = block->tuple_dict[ current_tuple + i ].type;
                int size = block->tuple_dict[ current_tuple + i ].size;
                memcpy( data, &(block->data[block->tuple_dict[current_tuple + i].address]), size );
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
    AK_print_table( dstTable );
}

void op_selection_test(){
    printf( "\n********** SELECTION TEST by Matija Šestak **********\n");

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
    
    AK_selection( tblName, "selection_test", &expr );
    DeleteAllL( &expr );
    
    
    printf("op_selection_test: After selection: %d\n", AK_num_attr( tblName ) );
    
    //dealocate variables ;)
}
