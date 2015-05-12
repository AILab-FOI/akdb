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
#include "../../auxi/iniparser.h"

/**
  @author Saša Vukšić
  @brief Function examines whether list L contains operator ele
  @param L list of elements
  @param ele operator to be found in list
  @return 1 if operator ele is found in list, otherwise 0
 */
  int Ak_If_ExistOp(struct list_node *L, char *ele)
  {
    struct list_node *Currentelement_op;
    AK_PRO;
    Currentelement_op = L->next;
    while (Currentelement_op)
    {
        //printf("iz liste: %s, iz tablice: %s \n", Currentelement_op->attribute_name, ele);
        if (strcmp(Currentelement_op->attribute_name, ele) == 0)
        {
            AK_EPI;
            return 1;
        }
        Currentelement_op = (struct list_node *) Currentelement_op->next;
    }
    AK_EPI;
    return 0;
}

/**
 * @author Saša Vukšić, Lovro Predovan
 * @brief Function reads table on which we create index and call functions for creating index
           Elements that will be in index are put in list indexLista and headerAttributes. According to those elements new indexes
           are created.

 * @param tblName name of table
 * @param attributes list of attributes on which we will create indexes
 * @return No return value
 * */
 void AK_create_Index_Table(char *tblName, struct list_node *attributes)
 {
    int num_attr;
    int i, j, k;

    table_addresses *addresses;
    AK_header *temp_head;
    int temp_int;
    char temp_char[ MAX_VARCHAR_LENGTH ];
    float temp_float;
    struct list_node *some_element;
    struct list_node *e, *ee;
    struct list_node *headerAtributes;
    int br;
    char * indexName;
    int indexed_attr_position;


    AK_header t_header[ MAX_ATTRIBUTES ];
    AK_header t_headerr[ MAX_ATTRIBUTES ];
    AK_header* tempHeader;

    AK_PRO;

    addresses = (table_addresses*) AK_get_table_addresses(tblName);
    num_attr = AK_num_attr(tblName);
    temp_head = (AK_header *) AK_get_header(tblName);
    headerAtributes = (struct list_node *) AK_malloc(sizeof (struct list_node));


    for (i = 0; i < num_attr; i++)
    {
        some_element = Ak_First_L2(attributes);
        while (some_element != 0)
        {

            //printf("atribut u listi: %s, atribut u zaglavlju: %s \n",some_element->attribute_name, (temp_head+i)->att_name);
            if (strcmp((temp_head + i)->att_name, some_element->attribute_name) == 0)
            {
                //printf("Atribut %s postoji u tablici, nalazi se na poziciji: %d\n",(temp_head+i)->att_name,i);
                indexed_attr_position = i;
                Ak_Init_L3(&headerAtributes);
                br = 0;

                while (addresses->address_from[ br ] != 0)
                {
                    for (j = addresses->address_from[ br ]; j < addresses->address_to[ br ]; j++)
                    {
                        AK_block *temp = (AK_block*) AK_read_block(j);
                        for (k = i; k < DATA_BLOCK_SIZE; k = k + num_attr)
                        {
                            if (temp->tuple_dict[ k ].size > 0)
                            {
                                switch (temp->tuple_dict[ k ].type)
                                {


                                    case TYPE_INT:
                                    memcpy(&temp_int, &(temp->data[ temp->tuple_dict[ k ].address]),
                                     temp->tuple_dict[k].size);
                                    temp_int = sprintf(temp_char, "%d", temp_int);

                                    ee = (struct list_node *) Ak_First_L2(headerAtributes);
                                    if (ee == 0)
                                    {
                                        Ak_Insert_New_Element(TYPE_VARCHAR, temp_char, "indexLista", temp_char, headerAtributes);
                                    }
                                    else
                                    {
                                        if (Ak_If_ExistOp(headerAtributes, temp_char) == 0)
                                        {
                                            ee = (struct list_node *) Ak_End_L2(headerAtributes);
                                            Ak_Insert_New_Element(TYPE_VARCHAR, temp_char, "indexLista", temp_char, ee);
                                        }
                                    }
                                    //printf("TEMP %-10s \n", temp_char);
                                    break;

                                //FLOAT VALUES ARE USUALLY NOT BEING INDEXED
                                    case TYPE_FLOAT:
                                    break;


                                    case TYPE_VARCHAR:
                                    memcpy(temp_char, &(temp->data[ temp->tuple_dict[ k ].address]),
                                     temp->tuple_dict[k].size);
                                    temp_char[ temp->tuple_dict[k].size ] = '\0';

                                    ee = (struct list_node *) Ak_First_L2(headerAtributes);
                                    if (ee == 0)
                                    {
                                        Ak_Insert_New_Element(TYPE_VARCHAR, temp_char, "indexLista", temp_char, headerAtributes);
                                    }
                                    else
                                    {
                                        if (Ak_If_ExistOp(headerAtributes, temp_char) == 0)
                                        {
                                            ee = (struct list_node *) Ak_End_L2(headerAtributes);
                                            Ak_Insert_New_Element(TYPE_VARCHAR, temp_char, "indexLista", temp_char, ee);
                                        }
                                    }
                                    //printf("TEMP : %-10s \n", temp_char);
                                    break;
                                }

                            }
                        }
                    }
                    br++;
                }


                //PART WHERE WE CHECK EVERY ATTRIBUTE
                int brr;
                int z;
                char inde[50];
                int startAddress;
                switch ((temp_head + i)->type)
                {
                    case TYPE_VARCHAR:
                    brr = 2; //3 First two places are reserved for block and row pointers
                    e = (struct list_node *) Ak_First_L2(headerAtributes);
                    while (e != 0)
                    {
                        //printf("%s : %s \n", (temp_head + i)->att_name, e->attribute_name);
                        tempHeader = (AK_header*) AK_create_header(e->attribute_name, TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
                        memcpy(t_header + brr, tempHeader, sizeof ( AK_header));
                        e = (struct list_node *) Ak_Next_L2(e);
                        brr++;
                    }
                    //ADDITIONAL ATTRIBTE IS PRESENT FOR BLOCK ADDRESS
                    tempHeader = (AK_header*) AK_create_header("addBlock", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                    memcpy(t_header, tempHeader, sizeof ( AK_header));
                    tempHeader = (AK_header*) AK_create_header("indexTd", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                    memcpy(t_header + 1, tempHeader, sizeof ( AK_header));

                    for (z = brr; z < MAX_ATTRIBUTES; z++)
                    {
                        memcpy(t_header + z, "\0", sizeof ( "\0"));
                    }

                    //INITIALIZATION OF NEW HEADER FILE
                    strcpy(inde, tblName);
                    indexName = strcat(inde, (temp_head + i)->att_name);
                    indexName = strcat(indexName, "_bmapIndex");

                    startAddress = AK_initialize_new_index_segment(indexName,AK_get_table_id(tblName),indexed_attr_position, t_header);


                    if (startAddress != EXIT_ERROR)
                    {
                        printf("\nINDEX %s CREATED!\n", indexName);
                        //funtion that fills index with attribute values
                        Ak_create_Index(tblName, indexName, (temp_head + i)->att_name, i, num_attr, t_header);
                    }

                    break;

                    case TYPE_INT:
                    brr = 2; //3 First two places are reserved for block and row pointers

                    tempHeader = (AK_header*) AK_create_header("addBlock", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                    memcpy(t_headerr, tempHeader, sizeof ( AK_header));

                    tempHeader = (AK_header*) AK_create_header("indexTd", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                    memcpy(t_headerr + 1, tempHeader, sizeof ( AK_header));
                    //tempHeader = (AK_header*)AK_create_header( "sizeTd", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                    //memcpy( t_headerr + 2 , tempHeader, sizeof( AK_header ));

                    e = (struct list_node *) Ak_First_L2(headerAtributes);
                    while (e != 0)
                    {
                        //printf("%s : %s : %i \n", (temp_head + i)->att_name, e->attribute_name,brr);
                        tempHeader = (AK_header*) AK_create_header(e->attribute_name, TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
                        memcpy(t_headerr + brr, tempHeader, sizeof ( AK_header));
                        e = (struct list_node *) Ak_Next_L2(e);
                        brr++;
                    }


                    for (z = brr; z < MAX_ATTRIBUTES; z++)
                    {
                        memcpy(t_headerr + z, "\0", sizeof ( "\0"));
                    }

                    strcpy(inde, tblName);
                    indexName = strcat(inde, (temp_head + i)->att_name);
                    indexName = strcat(indexName, "_bmapIndex");

                    startAddress = AK_initialize_new_index_segment(indexName,AK_get_table_id(tblName),indexed_attr_position, t_headerr);
                    if (startAddress != EXIT_ERROR)
                    {
                        printf("\nINDEX %s CREATED!\n", indexName);
                        Ak_create_Index(tblName, indexName, (temp_head + i)->att_name, i, num_attr, t_header);
                    }
                    break;
                }

            }
            some_element = (struct list_node *) Ak_Next_L2(some_element);
        }
    }

    Ak_DeleteAll_L3(&headerAtributes);
    AK_free(headerAtributes);
    AK_free(tempHeader);
    AK_free(temp_head);
    AK_EPI;

}


/**
 * @author Saša Vukšić, Lovro Predovan
 * @brief Function that loads index table with value of particulary atribute
 * @param tblName source table
 * @param tblNameIndex new name of index table
 * @param attributeName attribute on which we make index
 * @param positionTbl position of attribute in header of table
 * @param numAtributes number of attributes in table
 * @param headerIndex header of index table
 * @return No return value
 */
 void Ak_create_Index(char *tblName, char *tblNameIndex, char *attributeName, int positionTbl, int numAtributes, AK_header *headerIndex)
 {

    table_addresses *addresses;
    AK_block *temp;

    AK_header *t_header = AK_get_header( tblNameIndex );

    int i, j, k;
    int temp_int;
    int temp_indexTd;
    char temp_char[ MAX_VARCHAR_LENGTH ];
    float temp_float;
    i = 0;
    struct list_node *row_root;

    AK_PRO;

    addresses = (table_addresses*) AK_get_table_addresses(tblName);
    temp = (AK_block*) AK_read_block(addresses->address_from[0]);

    while (addresses->address_from[ i ] != 0)
    {
        for (j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++)
        {
            AK_block *temp = (AK_block*) AK_read_block(j);
            for (k = positionTbl; k < DATA_BLOCK_SIZE; k = k + numAtributes)
            {

                if (temp->tuple_dict[ k ].size > 0)
                {
                    switch (temp->tuple_dict[ k ].type)
                    {

                        case TYPE_INT:
                            memcpy(&temp_int, &(temp->data[ temp->tuple_dict[ k ].address]),
                             temp->tuple_dict[k].size);
                            temp_int = sprintf(temp_char, "%d", temp_int);
                            row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
                            Ak_Init_L3(&row_root);
                            temp_indexTd = k - positionTbl;

                            Ak_Insert_New_Element(TYPE_INT, &(temp->address), tblNameIndex, "addBlock", row_root);
                            Ak_Insert_New_Element(TYPE_INT, &temp_indexTd, tblNameIndex, "indexTd", row_root);
                            Ak_Insert_New_Element(TYPE_VARCHAR, "1", tblNameIndex, temp_char, row_root);

                            Ak_insert_row(row_root);
                            Ak_DeleteAll_L3(&row_root);
                            AK_free(row_root);
                                //printf( "%-10d", temp_int );
                        break;


                        case TYPE_FLOAT:
                        break;

                        case TYPE_VARCHAR:
                            memcpy(temp_char, &(temp->data[ temp->tuple_dict[ k ].address]),
                             temp->tuple_dict[k].size);
                            temp_char[ temp->tuple_dict[k].size ] = '\0';
                            row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
                            Ak_Init_L3(&row_root);
                            temp_indexTd = k - positionTbl;

                            Ak_Insert_New_Element(TYPE_INT, &(temp->address), tblNameIndex, "addBlock", row_root);
                            Ak_Insert_New_Element(TYPE_INT, &temp_indexTd, tblNameIndex, "indexTd", row_root);
                            Ak_Insert_New_Element(TYPE_VARCHAR, "1", tblNameIndex, temp_char, row_root);

                            Ak_insert_row(row_root);


                            Ak_DeleteAll_L3(&row_root);
                            AK_free(row_root);
                        break;
                    }
                }
            }
        }
        i++;
    }

    AK_free(addresses);
    AK_free(temp);
    // AK_free(t_header);
    AK_EPI;
}

/**
 * @author Saša Vukšić, Lovro Predovan
 * @brief Function gets adresses of the particuliary attribute from bitmap index. It fetches addresses of indexes and header
          of index table.  Using while loop it goes through index and gets necessary data. Those data are put in list called
          add_root.
 * @param indexName name of index
 * @param attribute name of attribute
 * @return list of adresses
 **/
 list_ad* Ak_get_Attribute(char *indexName, char *attribute)
 {
    int num_attr;
    int i, j, k;

    //start and end addresses of segment's
    table_addresses *addresses;
    AK_header *temp_head;
    int temp_int;
    char temp_char[ MAX_VARCHAR_LENGTH ];
    float temp_float;
    int b, br = 0;
    int addBlock, indexTd;
    list_ad *add_root;
    AK_PRO;

    addresses = (table_addresses*) AK_get_index_addresses(indexName);
    num_attr = AK_num_index_attr(indexName);
    temp_head = (AK_header *) AK_get_index_header(indexName);

    add_root = (list_ad *) AK_malloc(sizeof (list_ad));
    Ak_InitializelistAd(add_root);

    for (b = 0; b < num_attr; b++)
    {
        if (strcmp((temp_head + b)->att_name, attribute) == 0)
        {
            br = b;
        }
    }
    AK_free(temp_head);
    i = 0;

    while (addresses->address_from[ i ] != 0)
    {

        for (j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++)
        {

            AK_block *temp = (AK_block*) AK_read_block(j);
            for (k = br; k < DATA_BLOCK_SIZE; k = k + num_attr)
            {
                if (temp->tuple_dict[ k ].size > 0)
                {

                    switch (temp->tuple_dict[ k ].type)
                    {
                        case TYPE_INT:
                        memcpy(&temp_int, &(temp->data[ temp->tuple_dict[ k ].address]),
                         temp->tuple_dict[k].size);
                        break;
                        case TYPE_FLOAT:
                        memcpy(&temp_float, &(temp->data[ temp->tuple_dict[ k ].address]),
                         temp->tuple_dict[k].size);
                        printf("float: %-10.2f", temp_float);
                        break;

                        case TYPE_VARCHAR:
                        memcpy(temp_char, &(temp->data[ temp->tuple_dict[ k ].address]),
                         temp->tuple_dict[k].size);

                        memcpy(&addBlock, &(temp->data[ temp->tuple_dict[ k - br ].address]),
                         temp->tuple_dict[0].size);

                        memcpy(&indexTd, &(temp->data[ temp->tuple_dict[ k - (br - 1) ].address]),
                         temp->tuple_dict[1].size);

                        temp_char[ temp->tuple_dict[k].size ] = '\0';

                        if (strcmp(temp_char, "1") == 0)
                        {             
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
    AK_EPI;
    return add_root;
    AK_free(add_root);
}


/**
 * @author Saša Vukšić, Lovro Predovan
 * @brief Function for printing list of adresses
 * @param list list of adresses
 * @return No return value
 **/
 void Ak_print_Att_Test(list_ad *list)
 {
    element_ad ele;
    AK_PRO;
    ele = Ak_Get_First_elementAd(list);

    while (ele != 0)
    {
        printf("Attribute : %s Block address: %i Index position: %i\n",ele->attName,ele->add.addBlock,ele->add.indexTd);
        ele = Ak_Get_Next_elementAd(ele);
    }
    AK_EPI;
}

/**
 * @author Saša Vukšić
 * @brief Function for getting values from the bitmap index if there is one for given table. It should be started when we are 		  making selection on the table with bitmap index.
 * @param tableName name of table
 * @param attributeValue value of attribute
 * @return list of adresses
 **/
 list_ad* AK_get_Attribute(char *tableName, char *attributeName, char *attributeValue)
 {
    list_ad *list;
    char inde[50];
    char *indexName;
    AK_PRO;

    list = (list_ad *) AK_malloc(sizeof (list_ad));
    Ak_InitializelistAd(list);
    strcpy(inde, tableName);
    indexName = strcat(inde, attributeName);
    indexName = strcat(indexName, "_bmapIndex");

    table_addresses *addresses = (table_addresses*) AK_get_index_addresses(indexName);
    if (addresses->address_from[ 0 ] == 0)
    {
        printf("There is no index for table: %s on attribute: %s", tableName, attributeName);
    }
    else
    {
        list = Ak_get_Attribute(indexName, attributeValue);
    }
    AK_EPI;
    return list;
}

/**
 * @author Saša Vukšić
 * @brief Function for updating the index, only on values that alredy exist. If there is no value in bitmap index or bitmap index on this value, warning is showed to the user. Otherwise, bitmap index is updated with new attribute value.
 * @param addBlock adress of block
 * @param addTD adress of tuple dict
 * @param tableName name of table
 * @param attributeName name of attribute
 * @param attributeValue value of atribute
 * @param newAttributeValue new value of updated attribute
 * @return No return value
 **/
 void AK_update(int addBlock, int addTd, char *tableName, char *attributeName, char *attributeValue, char *newAttributeValue)
 {
    char inde[50];
    char *indexName;
    int b, num_attr, k, j;
    int i = 0;
    int temp_adr_block;
    int temp_adr_Td;
    int pos;
    int posNew = -1;
    AK_PRO;
    strcpy(inde, tableName);
    indexName = strcat(inde, attributeName);
    indexName = strcat(indexName, "_bmapIndex");

    AK_header *temp_head = AK_get_index_header(indexName);
    num_attr = AK_num_index_attr(indexName);

    for (i = 0; i < num_attr; i++)
    {
        if (strcmp((temp_head + i)->att_name, attributeValue) == 0)
        {
            pos = i;
        }
    }

    i = 0;
    for (i = 0; i < num_attr; i++)
    {
            if (strcmp((temp_head + i)->att_name, attributeValue) == 0)
            {
                posNew = i;
            }
        }

        if (posNew == -1)
        {
            printf("Update is posible ONLY for existing values! \n");
            AK_EPI;
            exit(1);
        }

        table_addresses *addresses = (table_addresses*) AK_get_index_addresses(indexName);
        if (addresses->address_from[ 0 ] == 0)
        {
            printf("There is no index for table : %s on attribute: %s", tableName, attributeName);
        }
        else
        {
            for (b = 0; b < num_attr; b++)
            {
                if (strcmp((temp_head + b)->att_name, attributeValue) == 0)
                {
                    i = 0;
                    while (addresses->address_from[ i ] != 0)
                    {
                        for (j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++)
                        {
                            AK_block *temp = (AK_block*) AK_read_block(j);

                            for (k = 0; k < DATA_BLOCK_SIZE; k = k + num_attr)
                            {
                                if (temp->tuple_dict[ k ].size > 0)
                                {
                                    memcpy(&temp_adr_block, &(temp->data[ temp->tuple_dict[ k ].address]), temp->tuple_dict[k].size);
                                    memcpy(&temp_adr_Td, &(temp->data[ temp->tuple_dict[ k + 1 ].address]), temp->tuple_dict[k + 1].size);
                                    printf("%i vs.%i, %i vs %i\n",temp_adr_block,addBlock,temp_adr_Td,addTd);

                                    if ((temp_adr_block == addBlock) && (temp_adr_Td == addTd))
                                    {
                                        printf( "Adresa bloka je: %d, Adresat Td-a je: %d segment:%i \n", temp_adr_block, temp_adr_Td, temp->address );
                                    //temp->data[ temp->tuple_dict[ k+pos ].address] = NULL;
                                        memcpy(&(temp->data[ temp->tuple_dict[ k + pos ].address]), "n", 1);
                                    //temp->data[ temp->tuple_dict[ k+posNew ].address] = NULL;
                                        memset(&(temp->data[ temp->tuple_dict[ k + posNew ].address]), 0, 4);
                                        memcpy(&(temp->data[ temp->tuple_dict[ k + posNew ].address]), "1", 1);                                
                                        AK_PRO;

                                        Ak_write_block(temp);
                                    //Ak_update_row(temp);
                                    }
                                }
                            }

                        }
                        i++;
                    }
                }
            }
        }
        printf("Index %s updated for %s table\n",indexName,tableName);
        AK_EPI;
    }

/**
 * @author Saša Vukšić
 * @brief Function for writing new value in block when index is updated
 * @param block block to write on
 * @return EXIT_SUCESS when write operation is successful, otherwise EXIT_ERROR
 **/
 int Ak_write_block(AK_block * block)
 {
    /*Legacy method -> not really working
    AK_PRO;
    //Method for updating should be imlemented or reused some existing one...wainting for feedback
    if ((db = fopen(DB_FILE, "r+")) == NULL)
    {
        printf("AK_write_block: ERROR. Cannot open db file %s.\n", DB_FILE);
        AK_EPI;
        exit(EXIT_ERROR);
    }
    if (fseek(db, block->address * sizeof ( AK_block), SEEK_SET) != 0)
    {
        printf("AK_write_block: ERROR. Cannot set position to provided address block %d.\n", block->address);
        AK_EPI;
        exit(EXIT_ERROR);
    }
    if (AK_fwrite(block, sizeof ( *block), 1, db) == 1)
    {
        fclose(db);
        Ak_dbg_messg(MIDDLE, INDICES, "AK_write_block: Written block at address %d\n", block->address * sizeof ( AK_block));
        printf("AK_write_block: Written block at address %i items written: %i\n", block->address * sizeof ( AK_block),AK_fwrite(block, sizeof ( *block), 1, db));
        AK_EPI;
        return ( EXIT_SUCCESS);

    }
    else
    {
        printf("AK_write_block: ERROR. Cannot write block at provided address %d.\n", block->address);
        AK_EPI;
        exit(EXIT_ERROR);
    }*/
    }


/**
 * @author Lovro Predovan
 * @brief Function for updating the index,function deletes and recrates index values again if different number of params is detected 
 * @param tableName name of table
 * @param attributeName name of attribute
 * @param newAttributeValue new value of updated attribute
 * @return No return value
 **/
 void AK_add_to_bitmap_index(char *tableName, char *attributeName)
 {
    char inde[50];
    char *tblNameIndex;
    int tbl_num_rows, idx_num_rows;

    AK_PRO;
    strcpy(inde, tableName);
    tblNameIndex = strcat(inde, attributeName);
    tblNameIndex = strcat(tblNameIndex, "_bmapIndex");

    idx_num_rows = AK_get_index_num_records(tblNameIndex);
    tbl_num_rows = AK_get_num_records(tableName);

    //This may not be the best method but it's damn effective one
    if(tbl_num_rows > idx_num_rows){
        AK_delete_bitmap_index(tblNameIndex);
        struct list_node *att_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
        Ak_Init_L3(&att_root);
        Ak_Insert_New_Element(TYPE_VARCHAR, attributeName, tableName, attributeName, att_root);
        AK_create_Index_Table(tableName, att_root);
    }
    printf("Index %s updated for %s table\n",tblNameIndex,tableName);
    AK_EPI;
}


/**
 * @author Saša Vukšić
 * @brief Function that tests printing header of table
 * @param tblName name of table who's header we are printing
   @return No return value
 **/
   void Ak_print_Header_Test(char* tblName)
   {
    AK_header *temp_head;
    int i;
    int num_attr;
    AK_PRO;
    temp_head = AK_get_header(tblName);
    num_attr = AK_num_attr(tblName);
    printf("Number of attributes in header: %d", num_attr);
    printf("\n");
    for (i = 0; i < num_attr; i++)
        printf("%-10s", (temp_head + i)->att_name);
    printf("\n----------------------------------------------\n");
    AK_free(temp_head);
    AK_EPI;
}

/**
 * @author Lovro Predovan
 * @brief Function that deletes bitmap index based on name of index
 * @param Bitmap index table name
   @return No return value
 **/
   void AK_delete_bitmap_index(char *indexName) {
    AK_PRO;
    AK_delete_segment(indexName, SEGMENT_TYPE_INDEX);
    printf("INDEX %s DELETED!\n", indexName);
    AK_EPI;
}



/**
 * @author Saša Vukšić updated by Lovro Predovan
 * @brief Function that creates test table and makes index on test table,
            also prints original tables indexes tables and indexes,tests updating into tables
 * @return No return value
 * */
 void Ak_bitmap_test()
 {
    list_ad *list;
    int id_prof;
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));


    AK_PRO;
    printf("\n********** BITMAP INDEX TEST **********\n\n");

    //INDEXED TABLE
    char *tblName = "assistant";

    //CREATING BITMAP INDEXES ON ATTRIBUTES FIRSTNAME AND TEL
    struct list_node *att_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&att_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "firstname", tblName, "firstname", att_root);
    

    struct list_node *some_element;
    some_element = (struct list_node *) Ak_First_L2(att_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "tel", tblName, "tel", some_element);
    AK_create_Index_Table(tblName, att_root);


    AK_print_table(tblName);
    AK_print_table("AK_index");
    //AK_print_table("AK_relation");
    //AK_print_table("AK_sequence");


    //Ak_print_Header_Test("AK_index");
    printf("\n************Index tables******************\n\n");
    AK_print_index_table("assistantfirstname_bmapIndex");
    AK_print_index_table("assistanttel_bmapIndex");

    printf("Bitmap index tables should be now visible\n");


    //Test value that doesn't passes
    printf("\n\n************Printing non existing index table******************\n\n");
    AK_print_index_table("assistanrandom_bmapIndex");
    printf("\n\n***************************************\n\n");


    printf("\n\n************Updating table indexes******************\n\n");
    Ak_Init_L3(&row_root);
    id_prof = 35890;
    Ak_DeleteAll_L3(&row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Miran", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Zlatović", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, "042390858", tblName, "tel", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "miran.zlatovic@foi.hr", tblName, "email", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "www.foi.hr/nastavnici/zlatovic.miran/index.html", tblName, "web_page", row_root);
    Ak_insert_row(row_root);
    Ak_DeleteAll_L3(&row_root);
    AK_free(row_root);

    //some old legacy code for updating, shoud be revised
    //AK_update(346, 6, tblName, "firstname", "Markus", "Alen");
    //

    AK_add_to_bitmap_index(tblName, "firstname");

    AK_print_table(tblName);
    AK_print_index_table("assistantfirstname_bmapIndex");
    
    printf("\n\n************Searching index locations******************\n\n");
    Ak_print_Att_Test(AK_get_Attribute(tblName,"firstname","Markus"));
    Ak_print_Att_Test(AK_get_Attribute(tblName,"tel","858928176"));
    
     printf("\n\n************Deleting bitmap index******************\n\n");
    AK_delete_bitmap_index("assistantfirstname_bmapIndex");
    //AK_print_index_table("assistantfirstname_bmapIndex");
    AK_print_table("AK_index");


    printf("\n\n********** BITMAP INDEX TEST END**********\n\n");
    printf("\n\n BITMAP INDEX TEST PASSED\n\n");
    AK_EPI;
}
