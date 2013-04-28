/**
@file fileio.c Provides functions for file input/output
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
#include "fileio.h"

//START SPECIAL FUNCTIONS FOR WORK WITH row_element_structure

/** 
   * @author Matija Novak
   * @brief Function inserts new element after some element, to insert on first place give list as before element. New element
            is allocated. Type, data, attribute name and constraint of new elemets are set according to function arguments. Pointers
            are changed so that before element points to new element.
   * @param newtype type of the data
   * @param data the data
   * @param table table name
   * @param attribute_name attribute name
   * @param element element after we which insert the new element
   * @param constraint NEW_VALUE if data is new value, SEARCH_CONSTRAINT if data is constraint to search for
   * @return No return value
 */
void Ak_Insert_New_Element_For_Update(int newtype, void * data, char * table, char * attribute_name, AK_list_elem ElementBefore, int newconstraint) {
    AK_list *newElement = (AK_list *) malloc(sizeof (AK_list));
    newElement->type = newtype;

    memcpy(newElement->data, data, AK_type_size(newtype, data));
    if (newtype == TYPE_VARCHAR) {
        newElement->data[AK_type_size(newtype, data)] = '\0';
    }

    memcpy(newElement->table, table, strlen(table));
    newElement->table[strlen(table)] = '\0';

    memcpy(newElement->attribute_name, attribute_name, strlen(attribute_name));
    newElement->attribute_name[strlen(attribute_name)] = '\0';

    newElement->constraint = newconstraint;

    newElement->next = ElementBefore->next;
    ElementBefore->next = newElement;
}

/** 
   * @author Matija Novak, changed by Dino Laktašić
   * @brief Function inserts new element after some element, to insert on first place give list as before element. It calls function
            Ak_Insert_New_Element_For_Update.
   * @param newtype type of the data
   * @param data the data
   * @param table table name
   * @param attribute_name attribute name
   * @param element element after we which insert the new element
   * @param constraint is NEW_VALUE
   * @return No return value
 */
void Ak_Insert_New_Element(int newtype, void * data, char * table, char * attribute_name, AK_list_elem ElementBefore) {
  AK_archive_log("Ak_Insert_New_Element", newtype, data, table, attribute_name); //ARCHIVE_LOG  
  Ak_Insert_New_Element_For_Update(newtype, data, table, attribute_name, ElementBefore, NEW_VALUE);
}

//END SPECIAL FUNCTIONS row_element_structure

/** @author Matija Novak, updated by Dino Laktašić
        @brief Function inserts one row into some block.  Firstly it checks wether block contain attributes from the list. Then
               data, type, size and last_tuple_id are put in temp_block.
        @param row_root list of elements to insert
        @param temp_block block in which we insert data
        @return EXIT SUCCES if success
 */
int Ak_insert_row_to_block(AK_list *row_root, AK_block *temp_block) {
    AK_list_elem some_element;
    int type; //type od entry data
    int id = 0; //id tuple dict in which is inserted next data
    int head = 0; //index of header which is curently inserted
    int search_elem; //serch for tuple dict id and searc for data in list
    char entry_data[MAX_VARCHAR_LENGTH];
    
    while (strcmp(temp_block->header[head].att_name, "\0") != 0) {//inserting values of the list one by one
        while (temp_block->tuple_dict[id].size != FREE_INT) {//searches for free tuple dict, maybe it can be last_tuple_dict_id
            id++;
        }
        Ak_dbg_messg(HIGH, FILE_MAN, "insert_row_to_block: Position to write (tuple_dict_index) %d, header_att_name %s\n", id, temp_block->header[head].att_name);
        
        search_elem = 1;
        some_element = (AK_list_elem) Ak_First_L(row_root);
        while (search_elem) {
            if ((strcmp(some_element->attribute_name, temp_block->header[head].att_name) == 0)
                    && (some_element->constraint == 0)) {//found correct element

                type = some_element->type;
                
                memset(entry_data, '\0', MAX_VARCHAR_LENGTH);
                memcpy(entry_data, some_element->data, AK_type_size(type, some_element->data));

                search_elem = 0;
            } else {
                some_element = (AK_list_elem) Ak_Next_L(some_element);
                if (some_element == 0) { //no data exist for this header write null
                    memcpy(entry_data, "null", strlen("null"));
                    type = TYPE_VARCHAR;
                    search_elem = 0;
                }
            }
        }

        memcpy(temp_block->data + temp_block->free_space, entry_data, (int)AK_type_size(type, entry_data));
        temp_block->tuple_dict[id].address = temp_block->free_space;
        temp_block->free_space += AK_type_size(type, entry_data);
        temp_block->tuple_dict[id].type = type;
        temp_block->tuple_dict[id].size = AK_type_size(type, entry_data);

        memcpy(entry_data, temp_block->data + temp_block->tuple_dict[id].address, temp_block->tuple_dict[id].size);
            
        Ak_dbg_messg(HIGH, FILE_MAN, "insert_row_to_block: Insert: data: %s, size: %d\n", entry_data, AK_type_size(type, entry_data));
        head++; //go to next header
    }
    //writes the last used tuple dict id

    temp_block->last_tuple_dict_id = id;

    return EXIT_SUCCESS;
}

/** @author Matija Novak, updated by Matija Šestak (function now uses caching), updated by Dejan Frankovic (added reference check), updated by Dino         Laktašić (removed variable free, variable table initialized using memset)
        @brief Function inserts a one row into table. Firstly it is checked whether inserted row would violite reference integrity. 
        Then it is checked in which table should row be inserted. If there is no free space for new table, new extent is allocated. New block is            allocated on given address. Row is inserted in this block and dirty flag is set to BLOCK_DIRTY. 
        @param row_root list of elements which contain data of one row
        @return EXIT_SUCCESS if success else EXIT_ERROR

 */
int Ak_insert_row(AK_list *row_root) {
    Ak_dbg_messg(HIGH, FILE_MAN, "insert_row: Start testing reference integrity.\n");

    if (AK_reference_check_entry(row_root) == EXIT_ERROR) {
        printf("Could not insert row. Reference integrity violation.\n");
        return EXIT_ERROR;
    }
    
    Ak_dbg_messg(HIGH, FILE_MAN, "insert_row: Start inserting data\n");
    AK_list_elem some_element = (AK_list_elem) Ak_First_L(row_root);

    char table[MAX_ATT_NAME];   
    
    memset(table, '\0', MAX_ATT_NAME);
    memcpy(&table, some_element->table, strlen(some_element->table));
    
    Ak_dbg_messg(HIGH, FILE_MAN, "insert_row: Insert into table: %s\n", table);
    int adr_to_write;

    adr_to_write = (int) AK_find_free_space(AK_get_table_addresses(table));
    if (adr_to_write == -1)
        adr_to_write = (int) AK_init_new_extent(table, SEGMENT_TYPE_TABLE);

    if (adr_to_write == 0) {
        return EXIT_ERROR;
    }
    Ak_dbg_messg(HIGH, FILE_MAN, "insert_row: Insert into block on adress: %d\n", adr_to_write);

    AK_mem_block *mem_block = (AK_mem_block *) AK_get_block(adr_to_write);

    int end = (int) Ak_insert_row_to_block(row_root, mem_block->block);

    //AK_write_block(mem_block->block);
    mem_block->dirty = BLOCK_DIRTY;

    //timestamp
    unsigned long timestamp;
    timestamp = clock(); /// get the timestamp
    mem_block->timestamp_last_change = timestamp; /// set timestamp_last_change
    return end;
}

/** 
   * @author Matija Novak, updated by Dino Laktašić
   * @brief Function updates or deletes row from table in given block. Given list of elements is firstly back-upped. According to given argument in function, delete or update is peformed.
   * @param temp_block block to work with
   * @param row_list list of elements which contain data for delete or update
   * @param operation UPDATE or DELETE
   * @return No return value
 */
void Ak_update_delete_row_from_block(AK_block *temp_block, AK_list *row_root, int operation) {
    int head = 0; //counting headers
    int del = 1; //if can delete gorup of tuple dicts which are in the same row of table
    int exists_equal_attrib = 0; //if we found at least one header in the list
    int diff_varchar_exist = 0; //to now on update if exist varchar that is not the same so that i must delete/insert the entry
    char entry_data[MAX_VARCHAR_LENGTH]; //entry data when haeader is found in list which is copied to compare with data in block
    
    AK_list * row_root_backup = (AK_list *) malloc(sizeof (AK_list));
    Ak_Init_L(row_root_backup);

    AK_list_elem some_element = (AK_list_elem) Ak_First_L(row_root);

    while (some_element) {//make a copy of list
        Ak_Insert_New_Element_For_Update(some_element->type, some_element->data, some_element->table, some_element->attribute_name, row_root_backup, some_element->constraint);
        some_element = (AK_list_elem) Ak_Next_L(some_element);
    }

    int i, overflow, address, type, size;
    
    for (i = 0; i < DATA_BLOCK_SIZE; i++) { //freeze point, if there is no i++
        head = 0;

        address = temp_block->tuple_dict[i].address;
        type = temp_block->tuple_dict[i].type;
        size = temp_block->tuple_dict[i].size;
        overflow = address + size;

        while (strcmp(temp_block->header[head].att_name, "\0") != 0) { //going through headers
                some_element = (AK_list_elem) Ak_First_L(row_root);

                while (some_element) {
                    //if we found header that is constraint in list
                    if ((strcmp(some_element->attribute_name, temp_block->header[head].att_name) == 0) && (some_element->constraint == SEARCH_CONSTRAINT)) {
                        
                        exists_equal_attrib = 1;
                        
                        if ((overflow < (temp_block->free_space + 1)) && (overflow > -1)) {
                            //before there was for loop to clear (check if memset works correct)
                            memset(entry_data, '\0', MAX_VARCHAR_LENGTH);
                            memcpy(entry_data, temp_block->data + address, size);
                            
                            if (strcmp(entry_data, some_element->data) != 0) { //is the data equal on which we delete
                                del = 0; //if one constraint doesn't metch we dont delete or update
                            }
                        } else {
                            del = 0;
                        }
                    }

                    //update if there is varchar which we must change (when yes we delete old data and insert new one else only update data)
                    if ((strcmp(some_element->attribute_name, temp_block->header[head].att_name) == 0) && (some_element->constraint == NEW_VALUE) && 
                        (operation == UPDATE) && (diff_varchar_exist == 0) && (type == TYPE_VARCHAR)) {

                        memset(entry_data, '\0', MAX_VARCHAR_LENGTH);
                        memcpy(entry_data, temp_block->data + address, size);
                        
                        //if data is not the same must make new insert and old delete
                        if (strlen(entry_data) != strlen(some_element->data)) {
                            diff_varchar_exist = 1;
                        }
                    }
                    some_element = (AK_list_elem) Ak_Next_L(some_element);
                }
                head++; //next header
                //i++; //next tuple dict
       
        }

        //i+=2;
        //address = temp_block->tuple_dict[i].address;
        //printf("ADR %d \n:", temp_block->tuple_dict[i++].address);
        //printf("III %d\n", i);
        //type = temp_block->tuple_dict[i++].type;
        //size = temp_block->tuple_dict[i++].size;

        if (operation == DELETE) {//delete
            if ((exists_equal_attrib == 1) && (del == 1)) {
                int j, sd;
                sd = head;
                
                for (j = i - head; j < i; j++) {//delete one row

                    int k = temp_block->tuple_dict[j].address;
                    int l = temp_block->tuple_dict[j].size;
                    //printf("ADR %d \n:", temp_block->tuple_dict[j+sd].address);
                    //printf(" SZ %d \n:", temp_block->tuple_dict[j+sd].size);
                    memset(temp_block->data + k, '\0', l);
                    Ak_dbg_messg(HIGH, FILE_MAN, "update_delete_row_from_block: from: %d, to: %d\n", k, l + k);
                    
                    //clean tuple dict
                    temp_block->tuple_dict[j].size = 0;
                    temp_block->tuple_dict[j].type = 0;
                    temp_block->tuple_dict[j].address = 0;
                }
            }
        } 
        else {//update
            //is there an varchar to which has changed the size
            //when yes delete all insert new, else update data
            if ((exists_equal_attrib == 1) && (del == 1)) {
                int j,ss;
                char up_entry[MAX_VARCHAR_LENGTH];
                
                for (j = i - head; j < i; j++) {//go through row
                    ss = head;
                    int k = temp_block->tuple_dict[j+ss].address;
                    int l = temp_block->tuple_dict[j+ss].size;
                    
                    if (diff_varchar_exist == 1) {//delete and insert row
                        int exist_new_data = 0;
                        some_element = (AK_list_elem) Ak_First_L(row_root);

                        while (some_element) {//going through list elements
                            if ((strcmp(some_element->attribute_name, temp_block->header[(j+ss) % head].att_name) == 0)
                                && (some_element->constraint == NEW_VALUE)) {//if exist new data then we must not copy old one
                                exist_new_data = 1;
                            }
                            some_element = (AK_list_elem) Ak_Next_L(some_element);
                        }

                        if (exist_new_data == 0) {//exist data which we must copy while we dont have the new one
                            memset(up_entry, '\0', MAX_VARCHAR_LENGTH);
                            memcpy(up_entry, temp_block->data + k, l);

                            some_element = (AK_list_elem) Ak_First_L(row_root);

                            Ak_Insert_New_Element_For_Update(temp_block->tuple_dict[j+ss].type, up_entry,
                                some_element->table, temp_block->header[j % head].att_name, some_element, NEW_VALUE);
                        }
                        
                        memset(temp_block->data + k, '\0', l);
                        temp_block->tuple_dict[j+ss].size = 0;
                        temp_block->tuple_dict[j+ss].type = 0;
                        temp_block->tuple_dict[j+ss].address = 0;
                    } 
                    else {//update row
                        memset(up_entry, '\0', MAX_VARCHAR_LENGTH);
                        memcpy(up_entry, temp_block->data + k, l);

                        some_element = (AK_list_elem) Ak_First_L(row_root);

                        while (some_element) {//going throught elements
                            if ((strcmp(some_element->attribute_name, temp_block->header[j % head].att_name) == 0)
                                && (some_element->constraint == NEW_VALUE)) {//update element
                                if (strcmp(up_entry, some_element->data) != 0) {
                                    memcpy(temp_block->data + k, some_element->data, l);
                                }
                            }
                            some_element = (AK_list_elem) Ak_Next_L(some_element);
                        }
                    }
                }

                if (diff_varchar_exist == 1) {//must insert new data becouse we deleted the old one? BUBA
                    Ak_insert_row(row_root);
                    Ak_DeleteAll_L(row_root);

                    some_element = (AK_list_elem) Ak_First_L(row_root_backup);
                    
                    while (some_element) {//make a copy of list
                        Ak_Insert_New_Element_For_Update(some_element->type, some_element->data, some_element->table, 
                        some_element->attribute_name, row_root, some_element->constraint);
                        some_element = (AK_list_elem) Ak_Next_L(some_element);
                    }
                }
            }
        }        
        del = 1;
        diff_varchar_exist = 0;
        exists_equal_attrib = 0;
    }
    free(row_root_backup);
}

/** 
      * @author Matija Novak, updated by Matija Šestak (function now uses caching)
      * @brief Function updates or deletes the whole segment of an table. Addresses for given table atr fetched. For each block
        in extent row is updated or deleted according to operator del.
      * @param row_root elements of one row
      * @param del - DELETE or UPDATE
      * @return EXIT_SUCCESS if success
 */
int Ak_delete_update_segment(AK_list *row_root, int del) {
    char table[MAX_ATT_NAME];
    AK_list_elem some_element = (AK_list_elem) Ak_First_L(row_root);

    //memset(table, '\0', MAX_ATT_NAME);
    strcpy(table, some_element->table);
    table[strlen(some_element->table)] = '\0';
    Ak_dbg_messg(HIGH, FILE_MAN, "delete_update_segment: table to delete_update from: %s, source %s\n", table, some_element->table);

    table_addresses * addresses = (table_addresses *) AK_get_table_addresses(table);

    AK_mem_block *mem_block;
    int startAddress, j, i;
    
    for (j = 0; j < MAX_EXTENTS_IN_SEGMENT; j++) { //going through extent
        startAddress = addresses->address_from[j];
        if (startAddress != 0) {
            Ak_dbg_messg(HIGH, FILE_MAN, "delete_update_segment: delete_update extent: %d\n", j);

            for (i = startAddress; i <= addresses->address_to[j]; i++) { //going through blocks
                Ak_dbg_messg(HIGH, FILE_MAN, "delete_update_segment: delete_update block: %d\n", i);

                mem_block = (AK_mem_block *) AK_get_block(i);
                Ak_update_delete_row_from_block(mem_block->block, row_root, del);
                mem_block->dirty = BLOCK_DIRTY;
            }
        } else 
            break;
    }
    return EXIT_SUCCESS;
}

/** @author Matija Novak, Dejan Frankovic (added referential integrity)
        @brief Function deletes rows
        @param row_root elements of one row
        @returs EXIT_SUCCESS if success
 */
int Ak_delete_row(AK_list *row_root) {
    if (AK_reference_check_restricion(row_root, DELETE) == EXIT_ERROR) {
        Ak_dbg_messg(HIGH, FILE_MAN, "Could not delete row. Reference integrity violation (restricted).\n");
        return EXIT_ERROR;
    }
    if (AK_reference_check_if_update_needed(row_root, DELETE) == EXIT_SUCCESS) {
        AK_reference_update(row_root, DELETE);
    }

    Ak_delete_update_segment(row_root, DELETE);
    return EXIT_SUCCESS;
}

/** @author Matija Novak, Dejan Frankovic (added referential integrity)
        @brief Function updates rows of some table
        @param row_root elements of one row
        @return EXIT_SUCCESS if success
 */
int Ak_update_row(AK_list *row_root) {
    if (AK_reference_check_restricion(row_root, UPDATE) == EXIT_ERROR) {
        Ak_dbg_messg(HIGH, FILE_MAN, "Could not update row. Reference integrity violation (restricted).\n");
        return EXIT_ERROR;
    }
    if (AK_reference_check_if_update_needed(row_root, UPDATE) == EXIT_SUCCESS) {
        AK_reference_update(row_root, UPDATE);
    }
    Ak_delete_update_segment(row_root, UPDATE);
    return EXIT_SUCCESS;
}


void Ak_fileio_test() {
    printf("\n\nThis is fileio test!\n");
    /*OVO JE UBITI SVE VEĆ SPREMNO*/
    //CREATE table testna
    /*AK_block * block = ( AK_block * ) malloc ( sizeof( AK_block ) );
    AK_header * header_red_br;
    header_red_br = (AK_header *) AK_create_header( "Redni_broj", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
    AK_header * header_name;
    header_name = (AK_header *) AK_create_header( "Ime", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
    AK_header * header_surname;
    header_surname = (AK_header *) AK_create_header( "Prezime", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;

    block->address = 10;
    block->type = BLOCK_TYPE_NORMAL;
    block->chained_with = NOT_CHAINED;
    block->free_space = 0;
    block->last_tuple_dict_id=0; ///using as an address for the first free space in block->dat

    memcpy( & block->header[0], header_red_br, sizeof( * header_red_br ) );
    memcpy( & block->header[1], header_name, sizeof( * header_name ) );
    memcpy( & block->header[2], header_surname, sizeof( * header_surname ) );
    
    AK_write_block( block );
    block->address = 11;
    AK_write_block( block );
    block->address = 12;
    AK_write_block( block );
    block->address = 13;
    AK_write_block( block );
     */
    AK_header t_header[4] = {
        {TYPE_INT, "Redni_broj", 0, '\0', '\0'},
        {TYPE_VARCHAR, "Ime", 0, '\0', '\0'},
        {TYPE_VARCHAR, "Prezime", 0, '\0', '\0'},
        {0, '\0', 0, '\0', '\0'}
    };

    int startAddress = AK_initialize_new_segment("testna", SEGMENT_TYPE_TABLE, t_header);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", "testna");

    //END CREATING TABLE TESTA
    //CREATE TABLE TESTNA_druga
    /*
    header_red_br = (AK_header *) AK_create_header( "Adresa", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
    header_name = (AK_header *) AK_create_header( "Ime", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
    header_surname = (AK_header *) AK_create_header( "Prezime", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;

    block->address = 20;
    block->type = BLOCK_TYPE_NORMAL;
    block->chained_with = NOT_CHAINED;
    block->free_space = 0;
    block->last_tuple_dict_id=0; ///using as an address for the first free space in block->dat

    memcpy( & block->header[0], header_red_br, sizeof( * header_red_br ) );
    memcpy( & block->header[1], header_name, sizeof( * header_name ) );
    memcpy( & block->header[2], header_surname, sizeof( * header_surname ) );

    AK_write_block( block );
    block->address = 21;
    AK_write_block( block );
    block->address = 22;
    AK_write_block( block );
    block->address = 23;
    AK_write_block( block );

    free(block);
    */
/*    AK_header t_header2[4] = {
        {TYPE_VARCHAR, "Adresa", 0, '\0', '\0',},
        {TYPE_VARCHAR, "Ime", 0, '\0', '\0',},
        {TYPE_VARCHAR, "Prezime", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };

    int startAddress1 = AK_initialize_new_segment("testna_druga", SEGMENT_TYPE_TABLE, t_header2);

    if (startAddress1 != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", "testna_druga");*/
    //END CREATING TABLE TESTNA_DRUGA

    //INSERT ENTRY TO SYSTEM_RELATION CATALOG
    //  AK_block * block2;
    /*
        header_red_br = (AK_header *) AK_create_header( "obj_id", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
            header_name = (AK_header *) AK_create_header( "Name", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
            header_surname = (AK_header *) AK_create_header( "start_addres", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
            AK_header * header_surname2;
            header_surname2 = (AK_header *) AK_create_header( "end_addres", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;

            block2->address = 1;
            block2->type = BLOCK_TYPE_NORMAL;
            block2->chained_with = NOT_CHAINED;
            block2->free_space = 0; ///using as an address for the first free space in block->dat

            memcpy( & block2->header[0], header_red_br, sizeof( * header_red_br ) );
            memcpy( & block2->header[1], header_name, sizeof( * header_name ) );
            memcpy( & block2->header[2], header_surname, sizeof( * header_surname ) );
            memcpy( & block2->header[3], header_surname2, sizeof( * header_surname ) ); 

    block2 = (AK_block *) AK_read_block(1);
    int broj;
    broj=1;
    AK_insert_entry(block2, TYPE_INT, &broj, 0 );
    AK_insert_entry(block2, TYPE_VARCHAR, "testna_druga", 1 );
    broj=20;
    AK_insert_entry(block2, TYPE_INT, &broj, 2 );
    broj=22;
    AK_insert_entry(block2, TYPE_INT, &broj, 3 );

    broj=2;
    AK_insert_entry(block2, TYPE_INT, &broj, 4 );
    AK_insert_entry(block2, TYPE_VARCHAR, "testna", 5 );
    broj=10;
    AK_insert_entry(block2, TYPE_INT, &broj, 6 );
    broj=13;
    AK_insert_entry(block2, TYPE_INT, &broj, 7 );

    int cisti;
    AK_write_block( block2 );

    free(header_red_br);
    free(header_name);
    free(header_surname);
    free(block2);*/
    //OVO JE UBITI SVE VEĆ SPREMNO
    //prepraing data and inserting data to list

    AK_list *row_root = (AK_list *) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);
    AK_list_elem some_element;
    int broj;

    broj = 1;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &broj, "testna", "Redni_broj", row_root);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "Matija", "testna", "Ime", row_root, 0);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Novak", "testna", "Prezime", row_root);
    Ak_insert_row(row_root);


    Ak_DeleteAll_L(row_root);
    broj = 2;
    Ak_Insert_New_Element(TYPE_INT, &broj, "testna", "Redni_broj", row_root);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "Nikola", "testna", "Ime", row_root, 0);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Bakoš", "testna", "Prezime", row_root);
    some_element = Ak_First_L(row_root);
    Ak_insert_row(row_root);

    Ak_DeleteAll_L(row_root);
    broj = 3;
    Ak_Insert_New_Element(TYPE_INT, &broj, "testna", "Redni_broj", row_root);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "Matija", "testna", "Ime", row_root, 0);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Bakoš", "testna", "Prezime", row_root);
    Ak_insert_row(row_root);

    int i;
    //for (i=5;i<50;i++)
    for (i = 5; i < 10; i++) {
        Ak_DeleteAll_L(row_root);
        broj = i;
        Ak_Insert_New_Element(TYPE_INT, &broj, "testna", "Redni_broj", row_root);
        Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "Maja", "testna", "Ime", row_root, 0);
        Ak_Insert_New_Element(TYPE_VARCHAR, "Vacenovski", "testna", "Prezime", row_root);
        some_element = Ak_First_L(row_root);
        Ak_insert_row(row_root);
    }

    AK_print_table("testna");
/*
    Ak_DeleteAll_L(row_root);
    broj = 1;
    Ak_Insert_New_Element_For_Update(TYPE_INT, &broj, "testna", "Redni_broj", row_root, 1);
    Ak_delete_row(row_root);

    AK_print_table("testna");
*/
    Ak_DeleteAll_L(row_root);
    broj = 3;
    Ak_Insert_New_Element_For_Update(TYPE_INT, &broj, "testna", "Redni_broj", row_root, SEARCH_CONSTRAINT);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "Francina", "testna", "Ime", row_root, NEW_VALUE);
    //Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "Bakoš", "testna", "Prezime", row_root, 1);
    Ak_update_row(row_root);
    AK_print_table("testna");


    Ak_DeleteAll_L(row_root);
    broj = 7;
    // za broj = 1 ne radi!
    //broj = 1;
    Ak_Insert_New_Element_For_Update(TYPE_INT, &broj, "testna", "Redni_broj", row_root, SEARCH_CONSTRAINT);
    //Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "Matija", "testna", "Ime", row_root, 1);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "M", "testna", "Prezime", row_root, NEW_VALUE);
    Ak_update_row(row_root);

    AK_print_table("testna");

/*
    Ak_DeleteAll_L(row_root);
    broj = 3;
    Ak_Insert_New_Element_For_Update(TYPE_INT, &broj, "testna", "Redni_broj", row_root, 1);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "Slonic", "testna", "Ime", row_root, 0);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "Marko", "testna", "Prezime", row_root, 0);
    Ak_update_row(row_root);

    //AK_print_table("testna");

    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "Slonic", "testna", "Ime", row_root, 1);
    Ak_delete_row(row_root);

    //AK_print_table("testna");

    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "Maja", "testna", "Ime", row_root, 1);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "Mihi", "testna", "Ime", row_root, 0);
    Ak_update_row(row_root);

    //AK_print_table("testna");
*/
    Ak_DeleteAll_L(row_root);
    free(row_root);
}

                        /*for (k; k < l; k++) {
                            char prazan = '\0';
                            memcpy(temp_block->data + k, &prazan, 1);
                        }*/

