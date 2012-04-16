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
/**
  @brief Function examines whether list L contains operator ele
  @param L list of elements
  @param ele operator to be found in list
  @return 1 if operator ele is found in list, otherwise 0
*/
int Ak_If_ExistOp(AK_list *L, char *ele) {
    AK_list *Currentelement_op;
    Currentelement_op = L->next;
    while (Currentelement_op) {
        //printf("iz liste: %s, iz tablice: %s \n", Currentelement_op->attribute_name, ele);
        if (strcmp(Currentelement_op->attribute_name, ele) == 0) {
            return 1;
        }
        Currentelement_op = (AK_list_elem) Currentelement_op->next;
    }
    return 0;
}

/**
 * @author Saša Vukšić
 * @brief Function reads table on which we create index and call functions for creating index 
          (the function should be revised so it can initialize SEGMENT_TYPE_INDEX instead of SEGMENT_TYPE_TABLE :) ).
           Elements that will be in index are put in list indexLista and headerAttributes. According to those elements new indexes
	   are created.
	   
 * @param tblName name of table
 * @param attributes list of attributes on which we will create indexes
 * @return No return value
 * */
void AK_create_Index(char *tblName, AK_list *attributes) {
    int num_attr, num_rec;
    int i, j, k;
    //start and end addresses of segment's
    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(tblName);

    num_attr = AK_num_attr(tblName);
    num_rec = AK_get_num_records(tblName);

    AK_block *temp = (AK_block*) AK_read_block(addresses->address_from[0]);
    AK_header *temp_head = (AK_header *)AK_get_header(tblName);
    int temp_int;
    char temp_char[ MAX_VARCHAR_LENGTH ];
    float temp_float;

    AK_list_elem some_element;
    AK_list_elem e, ee;
    AK_list *headerAtributes = (AK_list *) malloc(sizeof (AK_list));

    int br;
    char * indexName;

    AK_header t_header[ MAX_ATTRIBUTES ];
    AK_header t_headerr[ MAX_ATTRIBUTES ];
    AK_header* tempHeader;

    for (i = 0; i < num_attr; i++) {
        some_element = Ak_First_L(attributes);
        while (some_element != 0) {
            //printf("atribut u listi: %s, atribut u zaglavlju: %s \n",some_element->attribute_name, (temp_head+i)->att_name);
            if (strcmp((temp_head + i)->att_name, some_element->attribute_name) == 0) {
                //printf("Atribut %s postoji u tablici, nalazi se na poziciji: %d\n",(temp_head+i)->att_name,i);
                Ak_Init_L(headerAtributes);
                br = 0;
                while (addresses->address_from[ br ] != 0) {
                    for (j = addresses->address_from[ br ]; j < addresses->address_to[ br ]; j++) {
                        AK_block *temp = (AK_block*) AK_read_block(j);
                        for (k = i; k < DATA_BLOCK_SIZE; k = k + num_attr) {
                            if (temp->tuple_dict[ k ].size > 0) {
                                switch (temp->tuple_dict[ k ].type) {
                                    case TYPE_INT:
                                        memcpy(&temp_int, &(temp->data[ temp->tuple_dict[ k ].address]),
                                                temp->tuple_dict[k].size);
                                        temp_int = sprintf(temp_char, "%d", temp_int);

                                        ee = (AK_list_elem) Ak_First_L(headerAtributes);
                                        if (ee == 0) {
                                            Ak_Insert_New_Element(TYPE_VARCHAR, temp_char, "indexLista", temp_char, headerAtributes);
                                        } else {
                                            if (Ak_If_ExistOp(headerAtributes, temp_char) == 0) {
                                                ee = (AK_list_elem) Ak_End_L(headerAtributes);
                                                Ak_Insert_New_Element(TYPE_VARCHAR, temp_char, "indexLista", temp_char, ee);
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

                                        ee = (AK_list_elem) Ak_First_L(headerAtributes);
                                        if (ee == 0) {
                                            Ak_Insert_New_Element(TYPE_VARCHAR, temp_char, "indexLista", temp_char, headerAtributes);
                                        } else {
                                            if (Ak_If_ExistOp(headerAtributes, temp_char) == 0) {
                                                ee = (AK_list_elem) Ak_End_L(headerAtributes);
                                                Ak_Insert_New_Element(TYPE_VARCHAR, temp_char, "indexLista", temp_char, ee);
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
                int z, br;
                char inde[50];
                int startAddress;
                switch ((temp_head + i)->type) {
                    case TYPE_VARCHAR:
                        brr = 2; //3 First two places are reserved for block and row pointers
                        e = (AK_list_elem) Ak_First_L(headerAtributes);
                        while (e != 0) {
                            //printf("%s : %s \n", (temp_head + i)->att_name, e->attribute_name);
                            tempHeader = (AK_header*) AK_create_header(e->attribute_name, TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
                            memcpy(t_header + brr, tempHeader, sizeof ( AK_header));
                            e = (AK_list_elem) Ak_Next_L(e);
                            brr++;
                        }
                        //dodaje se jos jedan atribut za pokazivac(adresu)
                        tempHeader = (AK_header*) AK_create_header("addBlock", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                        memcpy(t_header, tempHeader, sizeof ( AK_header));
                        tempHeader = (AK_header*) AK_create_header("indexTd", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                        memcpy(t_header + 1, tempHeader, sizeof ( AK_header));
                        //tempHeader = (AK_header*)AK_create_header( "sizeTd", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                        //memcpy( t_header + 2 , tempHeader, sizeof( AK_header ));

                        for (z = brr; z < MAX_ATTRIBUTES; z++) {
                            memcpy(t_header + z, "\0", sizeof ( "\0"));
                        }

                        //inicijalizira se fajl za index s novim headerom
                        strcpy(inde, tblName);
                        indexName = strcat(inde, (temp_head + i)->att_name);
                        indexName = strcat(indexName, "_bmapIndex");

                        startAddress = AK_initialize_new_segment(indexName, SEGMENT_TYPE_TABLE, t_header);
                        if (startAddress != EXIT_ERROR)
                            printf("\nINDEX %s CREATED!\n", indexName);

                        //tu bi trebalo pozvati funkciju koja puni index podacima određenog atributa
                        Ak_create_Index(tblName, indexName, (temp_head + i)->att_name, i, num_attr, t_header);
                        break;

                    case TYPE_INT:
                        brr = 2; //3 First two places are reserved for block and row pointers

                        tempHeader = (AK_header*) AK_create_header("addBlock", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                        memcpy(t_headerr, tempHeader, sizeof ( AK_header));
                        tempHeader = (AK_header*) AK_create_header("indexTd", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                        memcpy(t_headerr + 1, tempHeader, sizeof ( AK_header));
                        //tempHeader = (AK_header*)AK_create_header( "sizeTd", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                        //memcpy( t_headerr + 2 , tempHeader, sizeof( AK_header ));

                        e = (AK_list_elem) Ak_First_L(headerAtributes);
                        while (e != 0) {
                            //printf("%s : %s : %i \n", (temp_head + i)->att_name, e->attribute_name,brr);
                            tempHeader = (AK_header*) AK_create_header(e->attribute_name, TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                            memcpy(t_headerr + brr, tempHeader, sizeof ( AK_header));
                            e = (AK_list_elem) Ak_Next_L(e);
                            brr++;
                        }


                        for (z = brr; z < MAX_ATTRIBUTES; z++) {
                            memcpy(t_headerr + z, "\0", sizeof ( "\0"));
                        }

                        //inicijalizira se fajl za index s novim headerom
                        strcpy(inde, tblName);
                        indexName = strcat(inde, (temp_head + i)->att_name);
                        indexName = strcat(indexName, "_bmapIndex");

                        startAddress = AK_initialize_new_segment(indexName, SEGMENT_TYPE_TABLE, t_headerr);
                        if (startAddress != EXIT_ERROR)
                            printf("\nINDEX %s CREATED!\n", indexName);
                        Ak_create_Index(tblName, indexName, (temp_head + i)->att_name, i, num_attr, t_headerr);
                        break;
                }

            }
            some_element = (AK_list_elem) Ak_Next_L(some_element);
        }
    }

    AK_list_elem ele = Ak_First_L(headerAtributes);
    //ispis liste za header TEST
    /*
            while(ele != 0)
            {
                printf("Atribut : %s\n",ele->attribute_name);
                ele = GetNextelementOp(ele);
            }
     * */
    Ak_DeleteAll_L(headerAtributes);
    free(headerAtributes);
    free(tempHeader);
    free(temp_head);

}

/**
 * @author Saša Vukšić
 * @brief Function that tests printing header of table
 * @param tblName name of table who's header we are printing
   @return No return value
 **/
void Ak_print_Header_Test(char* tblName) {
    AK_header *temp_head = AK_get_header(tblName);
    int i;
    int num_attr;
    num_attr = AK_num_attr(tblName);
    printf("Broj atributa u zaglavlju: %d", num_attr);
    printf("\n");
    for (i = 0; i < num_attr; i++)
        printf("%-10s", (temp_head + i)->att_name);
    printf("\n----------------------------------------------\n");
    free(temp_head);
}

/*
/*
 * @author Saša Vukšić
 * @brief Function that loads index table with value of particulary atribute
 * @param tblName source table
 * @param tblNameIndex new name of index table
 * @param attributeName attribute on which we make index
 * @param positionTbl position of attribute in header of table
 * @param numAtributes number of attributes in table
 * @param headerIndex header of index table
 * @return No return value
 */
void Ak_create_Index(char *tblName, char *tblNameIndex, char *attributeName, int positionTbl, int numAtributes, AK_header *headerIndex) {
    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(tblName);
    AK_block *temp = (AK_block*) AK_read_block(addresses->address_from[0]);

    //AK_header *t_header = AK_get_header( tblNameIndex );
    int num_attrIndex = AK_num_attr(tblNameIndex);

    int i, j, k;
    int temp_int;
    int temp_indexTd;
    char temp_char[ MAX_VARCHAR_LENGTH ];
    float temp_float;
    int brojac, br;
    i = 0;
    AK_list_elem row_root;


    while (addresses->address_from[ i ] != 0) {
        for (j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++) {
            AK_block *temp = (AK_block*) AK_read_block(j);
            for (k = positionTbl; k < DATA_BLOCK_SIZE; k = k + numAtributes) {
                if (temp->tuple_dict[ k ].size > 0) {
                    switch (temp->tuple_dict[ k ].type) {
                        case TYPE_INT:
                            memcpy(&temp_int, &(temp->data[ temp->tuple_dict[ k ].address]),
                                    temp->tuple_dict[k].size);
                            temp_int = sprintf(temp_char, "%d", temp_int);
                            row_root = (AK_list_elem) malloc(sizeof (AK_list));
                            Ak_Init_L(row_root);
                            temp_indexTd = k - positionTbl;
                            Ak_Insert_New_Element(TYPE_INT, &(temp->address), tblNameIndex, "addBlock", row_root);
                            Ak_Insert_New_Element(TYPE_INT, &temp_indexTd, tblNameIndex, "indexTd", row_root);
                            //       for( br = 2; br < num_attrIndex; br++ )
                            //       {
                            //           if(strcmp((headerIndex + br)->att_name,temp_char) == 0)
                            //           {
                            Ak_Insert_New_Element(TYPE_VARCHAR, "1", tblNameIndex, temp_char, row_root);
                            //           }
                            //           else
                            //           {
                            //               InsertNewElement( TYPE_VARCHAR, "0", tblNameIndex, (headerIndex + br)->att_name, row_root );
                            //           }

                            //       }
                            Ak_insert_row(row_root);
                            Ak_DeleteAll_L(row_root);
                            free(row_root);
                            //printf( "%-10d", temp_int );
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
                            //printf( "%s: %i \n", temp_char,temp->tuple_dict[ k ].address );
                            //printf("Pozicija u headeru: %i \n",posOfAttr(tblNameIndex,temp_char));
                            //prolazi kroz header i dodaje nule na sve pozicije osim na onu na kojoj se nalazi unutar headera ovaj atribut
                            row_root = (AK_list_elem) malloc(sizeof (AK_list));
                            Ak_Init_L(row_root);
                            temp_indexTd = k - positionTbl;
                            Ak_Insert_New_Element(TYPE_INT, &(temp->address), tblNameIndex, "addBlock", row_root);
                            Ak_Insert_New_Element(TYPE_INT, &temp_indexTd, tblNameIndex, "indexTd", row_root);
                            //       for( br = 2; br < num_attrIndex; br++ )
                            //       {
                            //           if(strcmp((headerIndex + br)->att_name,temp_char) == 0)
                            //           {
                            Ak_Insert_New_Element(TYPE_VARCHAR, "1", tblNameIndex, temp_char, row_root);
                            //           }
                            //           else
                            //           {
                            //               InsertNewElement( TYPE_VARCHAR, "0", tblNameIndex, (headerIndex + br)->att_name, row_root );
                            //           }

                            //       }
                            Ak_insert_row(row_root);
                            Ak_DeleteAll_L(row_root);
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

/**
 * @author Saša Vukšić
 * @brief Function gets adresses of the particuliary attribute from bitmap index. It fetches addresses of indexes and header
	  of index table.  Using while loop it goes through index and gets necessary data. Those data are put in list called
	  add_root.
 * @param indexName name of index
 * @param attribute name of attribute
 * @return list of adresses
 **/
list_ad* Ak_get_Attribute(char *indexName, char *attribute) {
    int num_attr, num_rec;
    int i, j, k;
    //start and end addresses of segment's
    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(indexName);

    num_attr = AK_num_attr(indexName);
    num_rec = AK_get_num_records(indexName);

    AK_block *temp = (AK_block*) AK_read_block(addresses->address_from[0]);
    AK_header *temp_head = (AK_header *)AK_get_header(indexName);
    int temp_int;
    char temp_char[ MAX_VARCHAR_LENGTH ];
    float temp_float;
    int b, br = 0;
    int addBlock, indexTd;

    list_ad *add_root = (list_ad *) malloc(sizeof (list_ad));
    Ak_InitializelistAd(add_root);

    printf("\n");
    for (b = 0; b < num_attr; b++) {
        if (strcmp((temp_head + b)->att_name, attribute) == 0) {
            br = b;
            //printf("XXXXXXXXXXXXXXXXXXXXXXXXXX%i",br);
        }
    }
    free(temp_head);
    //printf("XXXXXXXXXXXXXXXXXXXXXXXXXX%i",br);
    i = 0;
    while (addresses->address_from[ i ] != 0) {
        for (j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++) {
            AK_block *temp = (AK_block*) AK_read_block(j);
            for (k = br; k < DATA_BLOCK_SIZE; k = k + num_attr) {
                if (temp->tuple_dict[ k ].size > 0) {
                    switch (temp->tuple_dict[ k ].type) {
                        case TYPE_INT:
                            memcpy(&temp_int, &(temp->data[ temp->tuple_dict[ k ].address]),
                                    temp->tuple_dict[k].size);
                            printf("%-10d", temp_int);
                            break;
                        case TYPE_FLOAT:
                            memcpy(&temp_float, &(temp->data[ temp->tuple_dict[ k ].address]),
                                    temp->tuple_dict[k].size);
                            printf("%-10.2f", temp_float);
                            break;

                        case TYPE_VARCHAR:
                            memcpy(temp_char, &(temp->data[ temp->tuple_dict[ k ].address]),
                                    temp->tuple_dict[k].size);
                            memcpy(&addBlock, &(temp->data[ temp->tuple_dict[ k - br ].address]),
                                    temp->tuple_dict[0].size);
                            memcpy(&indexTd, &(temp->data[ temp->tuple_dict[ k - (br - 1) ].address]),
                                    temp->tuple_dict[1].size);
                            temp_char[ temp->tuple_dict[k].size ] = '\0';
                            if (strcmp(temp_char, "1") == 0) {
                                //printf( "%-10s", temp_char );
                                //printf("adresa bloka: %i  veličina podatka: %i adresa td-a: %i\n",addBlock,addSize,addTd);
                                Ak_Insert_NewelementAd(addBlock, indexTd, attribute, add_root);
                            }
                            break;
                    }
                    if ((k + 1) % num_attr == 0)
                        printf("\n");
                }
            }
        }
        i++;
    }
    return add_root;
    free(add_root);
}

/**
 * @author Saša Vukšić
 * @brief Function for test list creation 
 * @return No return value
 **/
void Ak_create_List_Address_Test() {
    list_ad *add_root = (list_ad *) malloc(sizeof (list_ad));
    Ak_InitializelistAd(add_root);
    Ak_Insert_NewelementAd(1, 1, "prvi", add_root);
    element_ad some_element;
    some_element = Ak_Get_First_elementAd(add_root);
    Ak_Insert_NewelementAd(2, 2, "drugi", some_element);
    element_ad some_elementt = Ak_Get_Last_elementAd(add_root);
    Ak_Insert_NewelementAd(3, 3, "treci", some_elementt);

    element_ad ele = Ak_Get_First_elementAd(add_root);

    while (ele != 0) {
        printf("Adresa : %s\n", ele->attName);
        ele = Ak_Get_Next_elementAd(ele);
    }



}

/**
 * @author Saša Vukšić
 * @brief Function for printing list of adresses
 * @param list list of adresses
 * @return No return value
 **/
void Ak_print_Att_Test(list_ad *list) {

    //list_ad *list = getAttribute("studentfirstname","Matija");
    element_ad ele = Ak_Get_First_elementAd(list);
    while (ele != 0) {
        //printf("Atribut : %s Blok: %i Adresa td: %i Size: %i\n",ele->attName,ele->add.addBlock,ele->add.addTd,ele->add.sizeTd);
        ele = Ak_Get_Next_elementAd(ele);
    }
}

/**
 * @author Saša Vukšić
 * @brief Function for getting values from the bitmap index if there is one for given table. It should be started when we are 		  making selection on the table with bitmap index. 
 * @param tableName name of table
 * @param attributeValue value of attribute
 * @return list of adresses
 **/
list_ad* AK_get_Attribute(char *tableName, char *attributeName, char *attributeValue) {
    list_ad *list = (list_ad *) malloc(sizeof (list_ad));
    Ak_InitializelistAd(list);
    char inde[50];
    char *indexName;



    strcpy(inde, tableName);
    indexName = strcat(inde, attributeName);
    indexName = strcat(indexName, "_bmapIndex");

    // printf("Naziv indexa: %s",indexName);

    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(indexName);
    AK_block *temp = (AK_block*) AK_read_block(addresses->address_from[0]);
    if (addresses->address_from[ 0 ] == 0) {
        printf("Ne postoji index za tablicu: %s nad atributom: %s", tableName, attributeName);
    } else {
        list = Ak_get_Attribute(indexName, attributeValue);
    }

    return list;
}

/**
 * @author Saša Vukšić
 * @brief Function for updating the index, only on values that alredy exist. If there is no value in bitmap index or bitmap index 		  on this value, warning is showed to the user. Otherwise, bitmap index is updated with new attribute value.
 * @param addBlock adress of block
 * @param addTD adress of tuple dict
 * @param tableName name of table
 * @param attributeName name of attribute
 * @param attributeValue value of atribute
 * @param newAttributeValue new value of updated attribute
 * @return No return value
 **/
void AK_update(int addBlock, int addTd, char *tableName, char *attributeName, char *attributeValue, char *newAttributeValue) {
    char inde[50];
    char *indexName;
    int b, num_attr, k, j;
    int i = 0;
    int temp_adr_block;
    int temp_adr_Td;
    int pos;
    int posNew = -1;

    strcpy(inde, tableName);
    indexName = strcat(inde, attributeName);
    indexName = strcat(indexName, "_bmapIndex");

    //printf("Naziv indexa: %s\n",indexName);
    AK_header *temp_head = AK_get_header(indexName);
    num_attr = AK_num_attr(indexName);

    for (i = 0; i < num_attr; i++) {
        if (strcmp((temp_head + i)->att_name, attributeValue) == 0) {
            //printf("XXXXXXXXXXXXXXXXXXX%i\n",i);
            pos = i;
        }
    }
    i = 0;
    for (i = 0; i < num_attr; i++) {
        if (strcmp((temp_head + i)->att_name, newAttributeValue) == 0) {
            posNew = i;
        }
    }

    if (posNew == -1) {
        printf("Moguc update samo za postojece vrijednosti ! \n");
        exit(1);
    }

    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(indexName);
    AK_block *temp = (AK_block*) AK_read_block(addresses->address_from[0]);
    if (addresses->address_from[ 0 ] == 0) {
        printf("Ne postoji index za tablicu: %s nad atributom: %s", tableName, attributeName);
    } else {
        for (b = 0; b < num_attr; b++) {
            if (strcmp((temp_head + b)->att_name, attributeValue) == 0) {
                //printf("XXXXXXXXXXXXXXXXXXXXXXXXXX%i",b);
                i = 0;
                while (addresses->address_from[ i ] != 0) {
                    for (j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++) {
                        AK_block *temp = (AK_block*) AK_read_block(j);
                        for (k = 0; k < DATA_BLOCK_SIZE; k = k + num_attr) {
                            if (temp->tuple_dict[ k ].size > 0) {
                                memcpy(&temp_adr_block, &(temp->data[ temp->tuple_dict[ k ].address]), temp->tuple_dict[k].size);
                                memcpy(&temp_adr_Td, &(temp->data[ temp->tuple_dict[ k + 1 ].address]), temp->tuple_dict[k + 1].size);
                                //printf("xx:%i\n",temp_adr_block);
                                if ((temp_adr_block == addBlock) && (temp_adr_Td == addTd)) {
                                    // printf( "Adresa bloka je: %d, Adresat Td-a je: %d xxxx:%i ", temp_adr_block, temp_adr_Td, temp->address );
                                    // temp->data[ temp->tuple_dict[ k+pos ].address] = NULL;
                                    memcpy(&(temp->data[ temp->tuple_dict[ k + pos ].address]), "n", 1);
                                    //temp->data[ temp->tuple_dict[ k+posNew ].address] = NULL;
                                    memset(&(temp->data[ temp->tuple_dict[ k + posNew ].address]), 0, 4);
                                    memcpy(&(temp->data[ temp->tuple_dict[ k + posNew ].address]), "1", 1);
                                    Ak_write_block(temp);
                                }
                            }
                        }

                    }
                    i++;
                }
            }
        }
    }
}

/**
 * @author Saša Vukšić
 * @brief Function for writing new value in block when index is updated
 * @param block block to write on
 * @return EXIT_SUCESS when write operation is successful, otherwise EXIT_ERROR
 **/
int Ak_write_block(AK_block * block) {
    if ((db = fopen(DB_FILE, "r+")) == NULL) {
        printf("AK_write_block: ERROR. Cannot open db file %s.\n", DB_FILE);
        exit(EXIT_ERROR);
    }
    if (fseek(db, block->address * sizeof ( AK_block), SEEK_SET) != 0) {
        printf("AK_write_block: ERROR. Cannot set position to provided address block %d.\n", block->address);
        exit(EXIT_ERROR);
    }
    if (fwrite(block, sizeof ( *block), 1, db) != 1) {
        printf("AK_write_block: ERROR. Cannot write block at provided address %d.\n", block->address);
        exit(EXIT_ERROR);
    }
    fclose(db);
	Ak_dbg_messg(MIDDLE, INDICES, "AK_write_block: Written block at address %d\n", block->address * sizeof ( AK_block));
    return ( EXIT_SUCCESS);
}

/**
 * @author Saša Vukšić
 * @brief Function that creates test table and makes index on test table, print indexes
 * @return No return value
 * */
void Ak_bitmap_test() {
    printf("\n********** BITMAP INDEX TEST **********\n\n");

    char *tblName = "assistant";

    AK_list *att_root = (AK_list *) malloc(sizeof (AK_list));
    Ak_Init_L(att_root);

    Ak_Insert_New_Element(TYPE_VARCHAR, "firstname", tblName, "firstname", att_root);
    AK_list_elem some_element;
    some_element = (AK_list_elem) Ak_First_L(att_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "tel", tblName, "tel", some_element);

    AK_create_Index(tblName, att_root);

    AK_print_table(tblName);
    AK_print_table("assistantfirstname_bmapIndex");
    AK_print_table("assistanttel_bmapIndex");

    AK_update(321, 58, tblName, "firstname", "Alen", "Markus");
}
