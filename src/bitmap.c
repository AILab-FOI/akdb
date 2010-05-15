/**
@file bitmap.c Provides functions for bitmap indexes
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

#include "bitmap.h"


int IfExistOp(list_op *L, char *ele)
{
        list_op *Currentelement_op;
        Currentelement_op = L->next;
        while (Currentelement_op)
        {
            //printf("iz liste: %s, iz tablice: %s \n", Currentelement_op->attribute_name, ele);
            if(strcmp(Currentelement_op->attribute_name,ele) == 0)
            {
                return 1;
            }
                Currentelement_op = (element_op) Currentelement_op->next;
        }
        return 0;
}
//potrebno pokrenuti ovu metodu jer kreira testnu tablicu (podatke)
void bitmap_test()
{
    printf( "\n********** Function for creating table **********\n");

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
    InsertNewElement( TYPE_VARCHAR, "Oleg", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Vincek", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

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

    mbr++; year++; weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Matija", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Sestak", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Dino", tblName, "firstname", row_root);
    InsertNewElement( TYPE_VARCHAR, "Alagic", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Miroslav", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Zver", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Josip", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Vincek", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; weight += 0.75;year++;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Josip", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Vincek", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; weight += 0.75;year++;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Josip", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Vincek", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; weight += 0.75;year++;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Josip", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Vincek", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    year++;mbr++; weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Josip", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Vincek", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    year++;mbr++; weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Josip", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Vincek", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );


    printf("op_selection_test: After data insertion: %d\n", AK_num_attr( tblName ) );

    list_op *att_root =  (list_op *) malloc( sizeof(list_op) );
	InitializelistOp(att_root);
	InsertNewelementOp("student","firstname",att_root);
	element_op some_element;
	some_element = GetFirstelementOp(att_root);
	InsertNewelementOp("student","year",some_element);

    AKcreateIndex( tblName, att_root );

}



void AKcreateIndex(char *tblName, list_op *attributes)
{
    int num_attr, num_rec;
        int i, j, k;
        //start and end addresses of segment's
        table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );

        num_attr = AK_num_attr( tblName );
        num_rec = AK_get_num_records( tblName );

        AK_block *temp = (AK_block*)AK_read_block( addresses->address_from[0]);
        AK_header *temp_head = AK_get_header( tblName );
        int temp_int;
        char temp_char[ MAX_VARCHAR_LENGHT ];
        float temp_float;

        element_op some_element;
        element_op e,ee;
        list_op *headerAtributes = (list_op *) malloc( sizeof(list_op) );

        int br;
        char * indexName;

        AK_header t_header[ MAX_ATTRIBUTES ];
        AK_header t_headerr[ MAX_ATTRIBUTES ];
        AK_header* tempHeader;

        for( i = 0; i < num_attr; i++ )
        {
           some_element=GetFirstelementOp(attributes);
           while (some_element != 0)
           {
            //printf("atribut u listi: %s, atribut u zaglavlju: %s \n",some_element->attribute_name, (temp_head+i)->att_name);
            if(strcmp((temp_head+i)->att_name,some_element->attribute_name)==0)
            {
                printf("Atribut %s postoji u tablici, nalazi se na poziciji: %d\n",(temp_head+i)->att_name,i);
                InitializelistOp(headerAtributes);
                 br = 0;
                 while( addresses->address_from[ br ] != 0 ){
                    for( j = addresses->address_from[ br ]; j < addresses->address_to[ br ]; j++ ){
                        AK_block *temp = (AK_block*) AK_read_block( j );
                        for (k = i; k < DATA_BLOCK_SIZE; k=k+num_attr) {
                                if (temp->tuple_dict[ k ].size > 0) {
                                    switch (temp->tuple_dict[ k ].type) {
                                        case TYPE_INT:
                                            memcpy(&temp_int, &(temp->data[ temp->tuple_dict[ k ].address]),
                                                    temp->tuple_dict[k].size);
                                            temp_int = sprintf(temp_char,"%d",temp_int);

                                            ee = (element_op) GetFirstelementOp(headerAtributes);
                                            if(ee == 0)
                                            {
                                                InsertNewelementOp("indexLista",temp_char,headerAtributes);
                                            }
                                            else
                                            {
                                            if(IfExistOp(headerAtributes,temp_char) == 0)
                                             {
                                                ee = (element_op) GetLastelementOp(headerAtributes);
                                                InsertNewelementOp("indexLista",temp_char,ee);
                                             }
                                            }
                                           // printf("%-10s", temp_char);
                                            break;
                                        case TYPE_FLOAT:
                                            memcpy(&temp_float, &(temp->data[ temp->tuple_dict[ k ].address]),
                                                    temp->tuple_dict[k].size);
                                            printf("%-10.2f", temp_float);
                                            break;

                                        case TYPE_VARCHAR:
                                            memcpy(temp_char, &(temp->data[ temp->tuple_dict[ k ].address]),
                                                    temp->tuple_dict[k].size);
                                            temp_char[ temp->tuple_dict[k].size ] = '\0';

                                            ee = (element_op) GetFirstelementOp(headerAtributes);
                                            if(ee == 0)
                                            {
                                                InsertNewelementOp("indexLista",temp_char,headerAtributes);
                                            }
                                            else
                                            {
                                            if(IfExistOp(headerAtributes,temp_char) == 0)
                                             {
                                                ee = (element_op) GetLastelementOp(headerAtributes);
                                                InsertNewelementOp("indexLista",temp_char,ee);
                                             }
                                            }
                                           // printf("%-10s", temp_char);
                                            break;
                                    }
                                       // printf("\n");
                                }
                        }
                    }
                    br++;
                 }
                 //dio gdje se obradjuje svaki od atributa
                     int brr;
                     int z,br;
                     char inde[50];
                     int startAddress;
                 switch((temp_head + i)->type)
                 {
                     case TYPE_VARCHAR:
                         brr = 3;
                         e = (element_op) GetFirstelementOp(headerAtributes);
                        while (e != 0) {
                            //printf("%s : %s \n", (temp_head + i)->att_name, e->attribute_name);
                            tempHeader = (AK_header*)AK_create_header( e->attribute_name, TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
                            memcpy( t_header + brr, tempHeader, sizeof( AK_header ));
                            e = (element_op) GetNextelementOp(e);
                            brr++;
                        }
                        //dodaje se jos jedan atribut za pokazivac(adresu)
                        tempHeader = (AK_header*)AK_create_header( "addBlock", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                        memcpy( t_header , tempHeader, sizeof( AK_header ));
                        tempHeader = (AK_header*)AK_create_header( "addTd", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                        memcpy( t_header + 1 , tempHeader, sizeof( AK_header ));
                        tempHeader = (AK_header*)AK_create_header( "sizeTd", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                        memcpy( t_header + 2 , tempHeader, sizeof( AK_header ));

                        for(z = brr; z < MAX_ATTRIBUTES; z++ )
                        {
                            memcpy( t_header + z, "\0", sizeof( "\0" ));
                        }

                        //inicijalizira se fajl za index s novim headerom
                        strcpy(inde,tblName);
                        indexName = strcat(inde,(temp_head+i)->att_name);
                        indexName = strcat(indexName,"_bmapIndex");

                        startAddress = AK_initialize_new_segment( indexName, SEGMENT_TYPE_TABLE, t_header);
                           if( startAddress != EXIT_ERROR )
                             printf( "\nINDEX %s CREATED!\n", indexName );

                        //tu bi trebalo pozvati funkciju koja puni index podacima određenog atributa
                        createIndex(tblName,indexName,(temp_head+i)->att_name,i,num_attr,t_header);
                        break;

                     case TYPE_INT:
                            brr = 3;

                            tempHeader = (AK_header*)AK_create_header( "addBlock", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                            memcpy( t_headerr , tempHeader, sizeof( AK_header ));
                            tempHeader = (AK_header*)AK_create_header( "addTd", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                            memcpy( t_headerr + 1 , tempHeader, sizeof( AK_header ));
                            tempHeader = (AK_header*)AK_create_header( "sizeTd", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                            memcpy( t_headerr + 2 , tempHeader, sizeof( AK_header ));

                            e = (element_op) GetFirstelementOp(headerAtributes);
                            while (e != 0) {
                            //printf("%s : %s : %i \n", (temp_head + i)->att_name, e->attribute_name,brr);
                            tempHeader = (AK_header*)AK_create_header( e->attribute_name, TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                            memcpy( t_headerr + brr, tempHeader, sizeof( AK_header ));
                            e = (element_op) GetNextelementOp(e);
                            brr++;
                            }


                        for(z = brr; z < MAX_ATTRIBUTES; z++ )
                        {
                            memcpy( t_headerr + z, "\0", sizeof( "\0" ));
                        }

                        //inicijalizira se fajl za index s novim headerom
                        strcpy(inde,tblName);
                        indexName = strcat(inde,(temp_head+i)->att_name);
                        indexName = strcat(indexName,"_bmapIndex");

                        startAddress = AK_initialize_new_segment( indexName, SEGMENT_TYPE_TABLE, t_headerr);
                           if( startAddress != EXIT_ERROR )
                             printf( "\nINDEX %s CREATED!\n", indexName );
                        createIndex(tblName,indexName,(temp_head+i)->att_name,i,num_attr,t_headerr);
                         break;
                 }

            }
            some_element = (element_op) GetNextelementOp(some_element);
           }
        }

       element_op ele = GetFirstelementOp(headerAtributes);
       //ispis liste za header TEST
/*
        while(ele != 0)
        {
            printf("Atribut : %s\n",ele->attribute_name);
            ele = GetNextelementOp(ele);
        }
 * */
        DeleteAllelementsOp(headerAtributes);
        free( headerAtributes );
        free(tempHeader);
        free( temp_head );

}

void printHeaderTest(char* tblName)
{
    AK_header *temp_head = AK_get_header( tblName );
    int i;
    int num_attr;
    num_attr = AK_num_attr( tblName );
    printf("Broj atributa u zaglavlju: %d", num_attr);
    printf( "\n");
        for( i = 0; i < num_attr; i++ )
            printf( "%-10s", (temp_head+i)->att_name );
        printf( "\n----------------------------------------------\n");
        free( temp_head );
}

/*
/*
 * Kreira index
 * @tblName - tablica iz koje vadimo podatke
 * @tblNameIndex - tablica koja je inicijalizirana za index
 * @attributeName - atribut nad kojim radimo index
 * @positionTbl - pozicija atributa u zaglavlju tablice
 * @numAtributes - broj atributa u tablici
 * @headerIndex - header inicijalizirane inde tablice
*/
void createIndex(char *tblName, char *tblNameIndex, char *attributeName, int positionTbl, int numAtributes, AK_header *headerIndex)
{
    table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );
    AK_block *temp = (AK_block*)AK_read_block( addresses->address_from[0]);

    //AK_header *t_header = AK_get_header( tblNameIndex );
    int num_attrIndex = AK_num_attr( tblNameIndex );

    int i,j,k;
    int temp_int;
    char temp_char[ MAX_VARCHAR_LENGHT ];
    float temp_float;
    int brojac,br;
    i = 0;
    element row_root;


    while( addresses->address_from[ i ] != 0 ){
        for( j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++ ){
            AK_block *temp = (AK_block*) AK_read_block( j );
            for( k = positionTbl; k < DATA_BLOCK_SIZE; k=k+numAtributes ){
                if( temp->tuple_dict[ k ].size > 0 ){
                    switch( temp->tuple_dict[ k ].type ){
                        case TYPE_INT:
                            memcpy( &temp_int, &(temp->data[ temp->tuple_dict[ k ].address]),
                                    temp->tuple_dict[k].size);
                            temp_int = sprintf(temp_char,"%d",temp_int);
                            row_root =  (element) malloc( sizeof(list) );
                            InitializeList(row_root);
                            InsertNewElement( TYPE_INT, &(temp->tuple_dict[ k ].address), tblNameIndex, "addTd", row_root );
                            InsertNewElement( TYPE_INT, &(temp->address), tblNameIndex, "addBlock", row_root );
                            InsertNewElement( TYPE_INT, &(temp->tuple_dict[ k ].size), tblNameIndex, "sizeTd", row_root );
                     //       for( br = 3; br < num_attrIndex; br++ )
                     //       {
                     //           if(strcmp((headerIndex + br)->att_name,temp_char) == 0)
                     //           {
                                    InsertNewElement( TYPE_VARCHAR, "1", tblNameIndex, temp_char, row_root );
                     //           }
                     //           else
                     //           {
                     //               InsertNewElement( TYPE_VARCHAR, "0", tblNameIndex, (headerIndex + br)->att_name, row_root );
                     //           }

                     //       }
                            insert_row( row_root );
                            DeleteAllElements(row_root);
                            free(row_root);
                            //printf( "%-10d", temp_int );
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
                            printf( "%s: %i \n", temp_char,temp->tuple_dict[ k ].address );
                            //printf("Pozicija u headeru: %i \n",posOfAttr(tblNameIndex,temp_char));
                            //prolazi kroz header i dodaje nule na sve pozicije osim na onu na kojoj se nalazi unutar headera ovaj atribut
                            row_root =  (element) malloc( sizeof(list) );
                            InitializeList(row_root);
                            InsertNewElement( TYPE_INT, &(temp->tuple_dict[ k ].address), tblNameIndex, "addTd", row_root );
                            InsertNewElement( TYPE_INT, &(temp->address), tblNameIndex, "addBlock", row_root );
                            InsertNewElement( TYPE_INT, &(temp->tuple_dict[ k ].size), tblNameIndex, "sizeTd", row_root );
                     //       for( br = 3; br < num_attrIndex; br++ )
                     //       {
                     //           if(strcmp((headerIndex + br)->att_name,temp_char) == 0)
                     //           {
                                    InsertNewElement( TYPE_VARCHAR, "1", tblNameIndex, temp_char, row_root );
                     //           }
                     //           else
                     //           {
                     //               InsertNewElement( TYPE_VARCHAR, "0", tblNameIndex, (headerIndex + br)->att_name, row_root );
                     //           }

                     //       }
                            insert_row( row_root );
                            DeleteAllElements(row_root);
                            free(row_root);
                            break;
                    }
                }
            }
        }
        i++;
    }

    free(addresses);
    free(temp);
   // free(t_header);
}

//sluzi za dohvacanje adresa odredjenog atributa u indexu
list_ad* getAttribute(char *indexName, char *attribute)
{
    int num_attr, num_rec;
        int i, j, k;
        //start and end addresses of segment's
        table_addresses *addresses = (table_addresses* ) get_table_addresses( indexName );

        num_attr = AK_num_attr( indexName );
        num_rec = AK_get_num_records( indexName );

        AK_block *temp = (AK_block*)AK_read_block( addresses->address_from[0]);
        AK_header *temp_head = AK_get_header( indexName );
        int temp_int;
        char temp_char[ MAX_VARCHAR_LENGHT ];
        float temp_float;
        int b,br = 0;
        int addBlock,addTd,addSize;

        list_ad *add_root =  (list_ad *) malloc( sizeof(list_ad) );
        InitializelistAd(add_root);

        printf( "\n");
        for( b = 0; b < num_attr; b++ )
        {
            if(strcmp((temp_head+b)->att_name,attribute) == 0)
            {
                br = b;
                //printf("XXXXXXXXXXXXXXXXXXXXXXXXXX%i",br);
            }
        }
        free( temp_head );
        //printf("XXXXXXXXXXXXXXXXXXXXXXXXXX%i",br);
        i = 0;
        while( addresses->address_from[ i ] != 0 ){
            for( j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++ ){
                AK_block *temp = (AK_block*) AK_read_block( j );
                for( k = br; k < DATA_BLOCK_SIZE; k=k+num_attr ){
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
                                memcpy( &addBlock, &(temp->data[ temp->tuple_dict[ k - br ].address]),
                                        temp->tuple_dict[0].size);
                                memcpy( &addTd, &(temp->data[ temp->tuple_dict[ k - (br - 1) ].address]),
                                        temp->tuple_dict[1].size);
                                memcpy( &addSize, &(temp->data[ temp->tuple_dict[ k - (br - 2) ].address]),
                                        temp->tuple_dict[2].size);
                                temp_char[ temp->tuple_dict[k].size ] = '\0';
                                if(strcmp(temp_char,"1") == 0 )
                                {
                                //printf( "%-10s", temp_char );
                               // printf("adresa bloka: %i  veličina podatka: %i adresa td-a: %i\n",addBlock,addSize,addTd);
                                InsertNewelementAd(addTd,addBlock,addSize,attribute,add_root);
                                }
                                break;
                        }
                        if( (k+1) % num_attr == 0 )
                            printf( "\n");
                    }
                }
            }
            i++;
        }
       return add_root;
        free(add_root);
}




void createListAddressTest()
{
      list_ad *add_root =  (list_ad *) malloc( sizeof(list_ad) );
	InitializelistAd(add_root);
	InsertNewelementAd(1,1,1,"prvi",add_root);
	element_ad some_element;
	some_element = GetFirstelementAd(add_root);
	InsertNewelementAd(2,2,2,"drugi",some_element);
        element_ad some_elementt = GetLastelementAd(add_root);
        InsertNewelementAd(3,3,3,"treci",some_elementt);
     


        element_ad ele = GetFirstelementAd(add_root);

        while(ele != 0)
        {
            printf("Adresa : %s\n",ele->attName);
            ele = GetNextelementAd(ele);
        }



}

//sluzi za testiranje dohvacanja adresa atributa pomocu indexa
void printAttTest(list_ad *list)
{

    //list_ad *list = getAttribute("studentfirstname","Matija");
       element_ad ele = GetFirstelementAd(list);
        while(ele != 0)
        {
            printf("Atribut : %s Blok: %i Adresa td: %i Size: %i\n",ele->attName,ele->add.addBlock,ele->add.addTd,ele->add.sizeTd);
            ele = GetNextelementAd(ele);
        }
}

//ovo je glavna funkcija za dohvacanje, nju treba pokrenuti pri selekciji
list_ad* AKgetAttribute(char *tableName, char *attributeName, char *attributeValue)
{
    list_ad *list =  (list_ad *) malloc( sizeof(list_ad) );
    InitializelistAd(list);
    char inde[50];
    char *indexName;



    strcpy(inde,tableName);
    indexName = strcat(inde,attributeName);
    indexName = strcat(indexName,"_bmapIndex");

   // printf("Naziv indexa: %s",indexName);

    table_addresses *addresses = (table_addresses* ) get_table_addresses( indexName );
    AK_block *temp = (AK_block*)AK_read_block( addresses->address_from[0]);
    if(addresses->address_from[ 0 ] == 0)
    {
        printf("Ne postoji index za tablicu: %s nad atributom: %s", tableName, attributeName);
    }
    else
    {
       list = getAttribute(indexName, attributeValue);
    }

    return list;
}

/*
void AKupdate(int addBlock, int addTd, char *att)
{

}
 * */
/*
za testiranje potrebno u main.c :
 * createTableTest();
 * printAttTest(AKgetAttribute("student","firstname","Oleg"));
 *
 * moguce koristiti i neke od slijedecih funkcija:
 *
 * printHeaderTest("studentfirstname_bmapIndex");
 *      printHeaderTest("studentyear_bmapIndex");
 *      AK_print_table( "student" );
 *      AK_print_table( "studentfirstname_bmapIndex" );
 *      AK_print_table( "studentyear_bmapIndex" );
 *      createListAddressTest();
 *
 * */