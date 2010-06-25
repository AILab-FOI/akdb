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


//START GLOBAL FUNCTIONS TO WORK WITH LIST

/**	@author Matija Novak
        alocate empty list
        @param L - root of the list
        @result void
 */
void InitializeList(list *L) {
    L->next = 0;
}

/** 	@author Matija Novak
        Get the frst list element
        @param L - root of the list
        @result element_op first element of the list
 */
element GetFirstElement(list *L) {
    return (element) L->next;
}

/** 	@author Matija Novak
        Get the last list element
        @param L - root of the list
        @result element_op last element of the list
 */
element GetLastElement(list *L) {
    list *CurrentElement;
    CurrentElement = L;
    while (CurrentElement->next)
        CurrentElement = (element) CurrentElement->next;
    if (CurrentElement != L)
        return (element) CurrentElement;
    else
        return 0;
}

/** 	@author Matija Novak
        Gets the next list element of an given element
        @param CurrenetElelemnt - some element of the list form which we want the next element
        @result element - next element of given element_op
 */
element GetNextElement(element CurrentElement) {
    if (CurrentElement->next == 0) {
        return 0;
    } else {
        list *NextElement;
        NextElement = (element) CurrentElement->next;
        return (element) NextElement;
    }
}

/** 	@author Matija Novak
        Get the previous element of some element in the list
        @param CurrentElelemnt - element of which we want the previous element
        @param L - root of the list
        @result element_op - previous element of the element that we give as first parameter
 */
element GetPreviousElement(element CurrentElement, list *L) {
    list *PreviousElement;
    PreviousElement = L;
    while ((PreviousElement->next != 0) && ((element) PreviousElement->next != CurrentElement))
        PreviousElement = (element) PreviousElement->next;
    if (PreviousElement->next != 0 && PreviousElement != L) {
        return (element) PreviousElement;
    } else {
        return 0;
    }
}

/** 	@author Matija Novak
        Get the posititn of given elelment
        @param SearchElement - element which posititon we search for
        @param L - root of the list
        @result returns the posititon number of some elelemnt
 */
int GetPositionOfElement(element SearchedElement, list *L) {
    list *CurrentElement;
    int i = 0;
    CurrentElement = L;
    while (CurrentElement->next != 0 && CurrentElement != SearchedElement) {
        CurrentElement = (list *) CurrentElement->next;
        i++;
    }
    return i;
}

/** 	@author Matija Novak
        Delete given elelment from the list
        @param DeletedElement - element which we delete
        @param L - root of the list
        @result void
 */
void DeleteElement(element DeletedElement, list *L) {
    element PreviousElement = (element) GetPreviousElement(DeletedElement, L);
    if (PreviousElement != 0) {
        PreviousElement->next = DeletedElement->next;
    } else {
        L->next = DeletedElement->next;
    }
    free(DeletedElement);
}

/** 	@author Matija Novak
        Delete all elelments from the list
        @param L - root of the list
        @result void
 */
void DeleteAllElements(list *L) {
    list *CurrentElement = L;
    list *DeletedElement = (list *) L->next;
    while (CurrentElement->next != 0) {
        CurrentElement->next = DeletedElement->next;
        ;
        free(DeletedElement);
        DeletedElement = (list *) CurrentElement->next;
    }
}

//END GLOBAL FUNCTIONS

//START SPECIAL FUNCTIONS FOR WORK WITH row_element_structure

/** 	@author Matija Novak
        Inserts new element_op after some element, to insert on first place give list as before element
        @param newtype - type of the data
        @param data - the data
        @param table - table name
        @param attribute_name - attribute name
        @param element_op - element after we which insert the new element
        @param constraint - is 0
        @result void
 */
void InsertNewElement(int newtype, void * data, char * table, char * attribute_name, element ElementBefore) {
    list *newElement = (list *) malloc(sizeof (list));

    newElement->type = newtype;

    memcpy(newElement->data, data, AK_type_size(newtype, data));
    if (newtype == TYPE_VARCHAR) {
        newElement->data[AK_type_size(newtype, data)] = '\0';
    }

    memcpy(newElement->table, table, strlen(table));
    newElement->table[strlen(table)] = '\0';

    memcpy(newElement->attribute_name, attribute_name, strlen(attribute_name));
    newElement->attribute_name[strlen(attribute_name)] = '\0';

    newElement->constraint = 0;

    newElement->next = ElementBefore->next;
    ElementBefore->next = newElement;
}

/** 	@author Matija Novak
        Inserts new element_op after some element, to insert on first place give list as before element
        @param newtype - type of the data
        @param data - the data
        @param table - table name
        @param attribute_name - attribute name
        @param element_op - element after we which insert the new element
        @param constraint - 0 if data is new value, 1 if data is constraint to search for
        @result void
 */
void InsertNewElementForUpdate(int newtype, void * data, char * table, char * attribute_name, element ElementBefore, int newconstraint) {
    list *newElement = (list *) malloc(sizeof (list));
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

//END SPECIAL FUNCTIONS row_element_structure

/**	@author Matija Novak
        insets one row into some block
        @param row_root - list of elements to insert
        @param temp_block -block in which we insert data
        @result int - EXIT SUCCES if success
 */
int insert_row_to_block(list *row_root, AK_block *temp_block) {
    element some_element;
    int unosi = 1; //used to run until all headers are inserted
    int type; //type od entry data
    unsigned char entry_data[MAX_VARCHAR_LENGHT];
    int size; //size of entry data
    int id = -1; //id tuple dict in which is inserted next data
    int head = 0; //index of header which is curently inserted
    int search_tuple_id, search_elem; //serch for tuple dict id and searc for data in list

    while (unosi) {//inserting values of the list one by one
        search_tuple_id = 1;
        while (search_tuple_id) {//searches for free tuple dict
            id++;
            if (temp_block->tuple_dict[id].size == FREE_INT) {//found free tuple_dict
                search_tuple_id = 0;
            }
        }
        if (DEBUG)
            printf("insert_row_to_block: Position to write (tuple_dict_index) %d, header_att_name %s\n", id, temp_block->header[head].att_name);

        if (strcmp(temp_block->header[head].att_name, "") != 0) { //if exist more headers

            search_elem = 1;
            some_element = (element) GetFirstElement(row_root);
            while (search_elem) {//going throught list of elements
                if ((strcmp(some_element->attribute_name, temp_block->header[head].att_name) == 0)
                        && (some_element->constraint == 0)) {//found correct element

                    int free2 = 0; //free varchar varijable
                    for (free2; free2 < MAX_VARCHAR_LENGHT; free2++)
                        entry_data[free2] = '\0';

                    type = some_element->type;

                    memcpy(entry_data, some_element->data, AK_type_size(type, some_element->data));

                    search_elem = 0;
                } else {
                    some_element = (element) GetNextElement(some_element);
                    if (some_element == 0) {//no data exist for this header write null
                        memcpy(entry_data, "null", strlen("null"));
                        type = TYPE_VARCHAR;
                        search_elem = 0;
                    }

                }
            }

            memcpy(temp_block->data + temp_block->free_space, entry_data, AK_type_size(type, entry_data));
            temp_block->tuple_dict[id].address = temp_block->free_space;
            temp_block->free_space += AK_type_size(type, entry_data);
            temp_block->tuple_dict[id].type = type;
            temp_block->tuple_dict[id].size = AK_type_size(type, entry_data);

            memcpy(entry_data, temp_block->data + temp_block->tuple_dict[id].address, temp_block->tuple_dict[id].size);

            if (DEBUG)
                printf("insert_row_to_block: Insert: Insert data: %s Size of data: %d\n", entry_data, AK_type_size(type, entry_data));
            head++; //go to next header
        } else {//no more headers

            unosi = 0;
        }
    }
    //writes the last used tuple dict id

    temp_block->last_tuple_dict_id = id;
    ;
    return EXIT_SUCCESS;
}

/**	@author Matija Novak, updated by Matija Šestak (function now uses caching), updated by Dejan Frankovic (added reference check)
        inserts a one row into table
        @param list of elements which contain data of one row
        @result EXIT_SUCCESS if success elese EXIT_ERROR

 */
int insert_row(list *row_root) {
    if (DEBUG) {
        printf("insert_row: Start testing reference integrity.\n");
    }

    if (AK_reference_check_entry(row_root) == EXIT_ERROR) {
        printf("Could not insert row. Reference integrity violation.\n");
        return EXIT_ERROR;
    }
    if (DEBUG) {
        printf("insert_row: Start inserting data\n");
    }
    element some_element;
    some_element = (element) GetFirstElement(row_root);

    char table[MAX_ATT_NAME];
    int free2 = 0;
    for (free2; free2 < MAX_ATT_NAME; free2++)
        table[free2] = '\0';

    memcpy(&table, some_element->table, strlen(some_element->table));
    if (DEBUG)
        printf("insert_row: Insert into table: %s\n", table);
    int adr_to_write;

    adr_to_write = (int) find_free_space(get_table_addresses(&table));
    if (adr_to_write == -1)
        adr_to_write = (int) AK_init_new_extent(table, SEGMENT_TYPE_TABLE);

    if (adr_to_write == 0) {
        return EXIT_ERROR;
    }
    if (DEBUG)
        printf("insert_row: Insert into block on adress: %d\n", adr_to_write);

    AK_mem_block *mem_block;

    mem_block = (AK_mem_block *) AK_get_block(adr_to_write);

    int end = (int) insert_row_to_block(row_root, mem_block->block);

    //AK_write_block(mem_block->block);
    mem_block->dirty = BLOCK_DIRTY;
    return end;
}

/**	@author Matija Novak
        update or delete rows table block
        @param temp_block - block to work with
        @param row_list - list of elements which contain data for delete or update
        @param what -if 0 then update, if 1 then delete
        @result void
 */
void update_delete_row_from_block(AK_block *temp_block, list *row_root, int what) {
    int next = 1; //moving through headers
    int head = 0; //counting headers
    int i; //go through all tuple_dicts
    int delete = 1; //if can delete gorup of tuple dict which are in same row of table
    int search_elem = 1; //search throug list elemnts if contains some header
    unsigned char entry_data[MAX_VARCHAR_LENGHT]; //entry data when haeader is found in list which is copied to compare with data in block
    int exits_equal_attrib = 0; //if we fund at least one header in list
    element some_element; //element of list
    int difrent_varchar_exist = 0; //to now on update if extist varchar that is not the same so that i must delete/insert the entry
    int free2 = 0;
    list * row_root_backup = (list *) malloc(sizeof (list));
    InitializeList(row_root_backup);

    search_elem = 1;

    some_element = (element) GetFirstElement(row_root);

    while (search_elem) {//make a copy of list
        InsertNewElementForUpdate(some_element->type, some_element->data,
                some_element->table, some_element->attribute_name, row_root_backup, some_element->constraint);

        some_element = (element) GetNextElement(some_element);
        if (some_element == 0) {//if there is more elements in list
            search_elem = 0;
        }
    }

    for (i = 0; i < DATA_BLOCK_SIZE;) {//going through tuple_dicts
        next = 1;
        head = 0;

        // if end of block has been reached
        //          if (temp_block->tuple_dict[i].address<0)
        //            break;

        while (next) {//going throught headers
            if (strcmp(temp_block->header[head].att_name, "") != 0) {//if there are more headers
                search_elem = 1;
                some_element = (element) GetFirstElement(row_root);

                while (search_elem) {//going throught list elements

                    if ((strcmp(some_element->attribute_name, temp_block->header[head].att_name) == 0)
                            && (some_element->constraint == 1)) {//if we found header that is constraint in list
                        free2 = 0;
                        for (free2; free2 < MAX_VARCHAR_LENGHT; free2++)
                            entry_data[free2] = '\0';

                        exits_equal_attrib = 1;


                        int overflov = temp_block->tuple_dict[i].size + temp_block->tuple_dict[i].address;
                        if ((overflov < (temp_block->free_space + 1)) && (overflov>-1)) {
                            memcpy(entry_data,
                                    temp_block->data + temp_block->tuple_dict[i].address,
                                    temp_block->tuple_dict[i].size);


                            if (strcmp(entry_data, some_element->data) != 0) {//is the data equal on which we delete
                                delete = 0; //if one constraint doesn't metch we dont delete or update
                            }
                        } else {
                            delete = 0;
                        }
                    }

                    int type = temp_block->tuple_dict[i].type;

                    if ((strcmp(some_element->attribute_name, temp_block->header[head].att_name) == 0) && (some_element->constraint == 0) && (what == 0) && (difrent_varchar_exist == 0) && (type == TYPE_VARCHAR)) {//update if there is varchar which we must change (when yes we delete old data and insert new one else only update data)
                        free2 = 0;
                        for (free2; free2 < MAX_VARCHAR_LENGHT; free2++)
                            entry_data[free2] = '\0';

                        memcpy(entry_data,
                                temp_block->data + temp_block->tuple_dict[i].address,
                                temp_block->tuple_dict[i].size);

                        if (strlen(entry_data) != strlen(some_element->data)) {//if data is not the same must make new insert and old delete
                            difrent_varchar_exist = 1;
                        }

                    }

                    some_element = (element) GetNextElement(some_element);
                    if (some_element == 0) {//is there more elements in list
                        search_elem = 0;
                    }
                }
                head++; //next header
                i++; //next tuple dict
            } else {//no more headers
                next = 0;
            }
        }
        if (what == 1) {//delete
            if ((exits_equal_attrib == 1) && (delete == 1)) {
                int j = 0;
                for (j = i - head; j < i; j++) {//delete one row
                    int k = temp_block->tuple_dict[j].address;
                    int l = k + temp_block->tuple_dict[j].size;
                    if (DEBUG)
                        printf("update_delete_row_from_block: from: %d, to: %d \n", k, l);
                    for (k; k < l; k++) {//empty data
                        char prazan = '\0';
                        memcpy(temp_block->data + k, &prazan, 1);
                        if (DEBUG)
                            printf("update_delete_row_from_block: counter = %d", k);
                    }
                    //clean tuple dict
                    temp_block->tuple_dict[j].size = 0;
                    temp_block->tuple_dict[j].type = 0;
                    temp_block->tuple_dict[j].address = 0;
                }
            }
        } else {//update

            //is there an varchar to which has changed the size
            //when yes delete all insert new, else update data
            if ((exits_equal_attrib == 1) && (delete == 1)) {
                int j = 0;
                int up_type;
                char up_entry[MAX_VARCHAR_LENGHT];
                for (j = i - head; j < i; j++) {//go through row
                    if (difrent_varchar_exist == 1) {//delete and insert row
                        search_elem = 1;
                        some_element = (element) GetFirstElement(row_root);
                        int exist_new_data = 0;

                        while (search_elem) {//going through list elements
                            if ((strcmp(some_element->attribute_name, temp_block->header[j % head].att_name) == 0)
                                    && (some_element->constraint == 0)) {//if exist new data then we must not copy old one
                                exist_new_data = 1;
                            }
                            some_element = (element) GetNextElement(some_element);
                            if (some_element == 0) {//is there more elements in list
                                search_elem = 0;
                            }
                        }

                        if (exist_new_data == 0) {//exist data which we must copy while we dont have the new one
                            free2 = 0;
                            for (free2; free2 < MAX_VARCHAR_LENGHT; free2++)
                                up_entry[free2] = '\0';

                            memcpy(up_entry, temp_block->data + temp_block->tuple_dict[j].address,
                                    temp_block->tuple_dict[j].size);

                            some_element = (element) GetFirstElement(row_root);

                            InsertNewElementForUpdate(temp_block->tuple_dict[j].type, up_entry,
                                    some_element->table, temp_block->header[j % head].att_name,
                                    some_element, 0);
                        }

                        //delete old data
                        int k = temp_block->tuple_dict[j].address;
                        int l = k + temp_block->tuple_dict[j].size;

                        for (k; k < l; k++) {
                            char prazan = '\0';
                            memcpy(temp_block->data + k, &prazan, 1);
                        }

                        temp_block->tuple_dict[j].size = 0;
                        temp_block->tuple_dict[j].type = 0;
                        temp_block->tuple_dict[j].address = 0;



                    } else {//update row
                        free2 = 0;
                        for (free2; free2 < MAX_VARCHAR_LENGHT; free2++)
                            up_entry[free2] = '\0';

                        memcpy(up_entry,
                                temp_block->data + temp_block->tuple_dict[j].address,
                                temp_block->tuple_dict[j].size);

                        search_elem = 1;
                        some_element = (element) GetFirstElement(row_root);

                        while (search_elem) {//going throught elements
                            if ((strcmp(some_element->attribute_name, temp_block->header[j % head].att_name) == 0)
                                    && (some_element->constraint == 0)) {//update element
                                if (strcmp(up_entry, some_element->data) != 0) {
                                    memcpy(temp_block->data + temp_block->tuple_dict[j].address,
                                            some_element->data, temp_block->tuple_dict[j].size);
                                }
                            }

                            some_element = (element) GetNextElement(some_element);
                            if (some_element == 0) {//is there more elements in list
                                search_elem = 0;
                            }
                        }

                    }
                }

                if (difrent_varchar_exist == 1) {//must insert new data becouse we deleted the old one? BUBA

                    insert_row(row_root);

                    DeleteAllElements(row_root);

                    search_elem = 1;
                    some_element = (element) GetFirstElement(row_root_backup);
                    while (search_elem) {//make a copy of list

                        InsertNewElementForUpdate(some_element->type,
                                some_element->data,
                                some_element->table,
                                some_element->attribute_name, row_root,
                                some_element->constraint);

                        some_element = (element) GetNextElement(some_element);
                        if (some_element == 0) {//if there is more elements in list
                            search_elem = 0;
                        }
                    }
                }
            }
        }
        difrent_varchar_exist = 0;
        delete = 1;
        exits_equal_attrib = 0;
    }
    free(row_root_backup);
}

/**	@author Matija Novak, updated by Matija Šestak (function now uses caching)
        function update or delete the whole segmet of an table
        @param row_root -  elements of one row
        @param delete - 1 we make delete, 0 we make update
        @returs EXIT_SUCCESS if success
 */
int delete_update_segment(list *row_root, int delete) {
    element some_element;
    some_element = (element) GetFirstElement(row_root);
    char table[MAX_ATT_NAME];

    strcpy(table, some_element->table);
    table[strlen(some_element->table)] = '\0';
    if (DEBUG)
        printf("delete_update_segment: table to delete_update from: %s, sorce %s", table, some_element->table);

    table_addresses * addresses;
    addresses = (table_addresses *) get_table_addresses(&table);

    AK_mem_block *mem_block;

    int from = 0, to = 0, j = 0, i = 0;
    for (j = 0; j < MAX_EXTENTS_IN_SEGMENT; j++) {//going through extent
        from = addresses->address_from[j];
        if (from != 0) {
            if (DEBUG)
                printf("delete_update_segment: delete_update extent: %d", j);

            to = addresses->address_to[j];
            for (i = from; i <= to; i++) {//going throut blocks
                if (DEBUG)
                    printf("delete_update_segment: delete_update block: %d", i);

                mem_block = (AK_mem_block *) AK_get_block(i);

                update_delete_row_from_block(mem_block->block, row_root, delete);
                mem_block->dirty = BLOCK_DIRTY;
            }
        } else break;
    }
    return EXIT_SUCCESS;
}

/**	@author Matija Novak, Dejan Frankovic (added referential integrity)
        function delete rows
        @param row_root -  elements of one row
        @returs EXIT_SUCCESS if success
 */
int delete_row(list *row_root) {
    if (AK_reference_check_restricion(row_root, 1) == EXIT_ERROR) {
        printf("Could not delete row. Reference integrity violation (restricted).\n");
        return EXIT_ERROR;
    }
    if (AK_reference_check_if_update_needed(row_root, 1) == EXIT_SUCCESS) {
        AK_reference_update(row_root, 1);
    }

    delete_update_segment(row_root, 1);
    return EXIT_SUCCESS;
}

/**	@author Matija Novak, Dejan Frankovic (added referential integrity)
        function update rows of some table
        @param row_root -  elements of one row
        @returs EXIT_SUCCESS if success
 */
int update_row(list *row_root) {
    if (AK_reference_check_restricion(row_root, 0) == EXIT_ERROR) {
        printf("Could not update row. Reference integrity violation (restricted).\n");
        return EXIT_ERROR;
    }
    if (AK_reference_check_if_update_needed(row_root, 0) == EXIT_SUCCESS) {
        AK_reference_update(row_root, 0);
    }

    delete_update_segment(row_root, 0);
    return EXIT_SUCCESS;
}

void fileio_test() {
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
        {TYPE_INT, "Redni_broj", 0, '\0', '\0',},
        {TYPE_VARCHAR, "Ime", 0, '\0', '\0',},
        {TYPE_VARCHAR, "Prezime", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };

    int startAddress = AK_initialize_new_segment("testna", SEGMENT_TYPE_TABLE, t_header);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", "testna");

    //END CREATING TABLE TESTA
    //CREATE TABLE TESTNA_druga
    /*header_red_br = (AK_header *) AK_create_header( "Adresa", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
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

    free(block);*/

    AK_header t_header2[4] = {
        {TYPE_VARCHAR, "Adresa", 0, '\0', '\0',},
        {TYPE_VARCHAR, "Ime", 0, '\0', '\0',},
        {TYPE_VARCHAR, "Prezime", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };

    int startAddress1 = AK_initialize_new_segment("testna_druga", SEGMENT_TYPE_TABLE, t_header2);

    if (startAddress1 != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", "testna_druga");
    //END CREATING TABLE TESTNA_DRUGA

    //INSERT ENTRY TO SYSTEM_RELATION CATALOG
    //	AK_block * block2;
    /*	header_red_br = (AK_header *) AK_create_header( "obj_id", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
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
            memcpy( & block2->header[3], header_surname2, sizeof( * header_surname ) ); */

    /*block2 = (AK_block *) AK_read_block(1);
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

    /*OVO JE UBITI SVE VEĆ SPREMNO*/
    //prepraing data and inserting data to list
    list *row_root = (list *) malloc(sizeof (list));
    InitializeList(row_root);
    element some_element;
    int broj;

    broj = 1;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &broj, "testna", "Redni_broj", row_root);
    ;
    InsertNewElementForUpdate(TYPE_VARCHAR, "Matija", "testna", "Ime", row_root, 0);
    InsertNewElement(TYPE_VARCHAR, "Novak", "testna", "Prezime", row_root);
    insert_row(row_root);


    DeleteAllElements(row_root);
    broj = 2;
    InsertNewElement(TYPE_INT, &broj, "testna", "Redni_broj", row_root);
    InsertNewElementForUpdate(TYPE_VARCHAR, "Nikola", "testna", "Ime", row_root, 0);
    InsertNewElement(TYPE_VARCHAR, "Bakoš", "testna", "Prezime", row_root);
    some_element = GetFirstElement(row_root);
    insert_row(row_root);

    DeleteAllElements(row_root);
    broj = 3;
    InsertNewElement(TYPE_INT, &broj, "testna", "Redni_broj", row_root);
    InsertNewElementForUpdate(TYPE_VARCHAR, "Matija", "testna", "Ime", row_root, 0);
    InsertNewElement(TYPE_VARCHAR, "Bakoš", "testna", "Prezime", row_root);
    insert_row(row_root);

    int i;
    //for (i=5;i<50;i++)
    for (i = 5; i < 10; i++) {
        DeleteAllElements(row_root);
        broj = i;
        InsertNewElement(TYPE_INT, &broj, "testna", "Redni_broj", row_root);
        InsertNewElementForUpdate(TYPE_VARCHAR, "Maja", "testna", "Ime", row_root, 0);
        InsertNewElement(TYPE_VARCHAR, "Vacenovski", "testna", "Prezime", row_root);
        some_element = GetFirstElement(row_root);
        insert_row(row_root);
    }

    //AK_print_table("testna");

    DeleteAllElements(row_root);
    broj = 1;
    InsertNewElementForUpdate(TYPE_INT, &broj, "testna", "Redni_broj", row_root, 1);
    delete_row(row_root);

    // AK_print_table("testna");

    DeleteAllElements(row_root);
    broj = 1;
    InsertNewElementForUpdate(TYPE_INT, &broj, "testna", "Redni_broj", row_root, 0);
    InsertNewElementForUpdate(TYPE_VARCHAR, "Nikola", "testna", "Ime", row_root, 1);
    InsertNewElementForUpdate(TYPE_VARCHAR, "Bakoš", "testna", "Prezime", row_root, 1);
    update_row(row_root);

    AK_print_table("testna");

    DeleteAllElements(row_root);
    InsertNewElementForUpdate(TYPE_VARCHAR, "Matija", "testna", "Ime", row_root, 1);
    InsertNewElementForUpdate(TYPE_VARCHAR, "", "testna", "Prezime", row_root, 0);
    update_row(row_root);

    AK_print_table("testna");

    DeleteAllElements(row_root);
    broj = 3;
    InsertNewElementForUpdate(TYPE_INT, &broj, "testna", "Redni_broj", row_root, 1);
    InsertNewElementForUpdate(TYPE_VARCHAR, "Slonic", "testna", "Ime", row_root, 0);
    InsertNewElementForUpdate(TYPE_VARCHAR, "Marko", "testna", "Prezime", row_root, 0);
    update_row(row_root);

    //AK_print_table("testna");

    DeleteAllElements(row_root);
    InsertNewElementForUpdate(TYPE_VARCHAR, "Slonic", "testna", "Ime", row_root, 1);
    delete_row(row_root);

    //AK_print_table("testna");

    DeleteAllElements(row_root);
    InsertNewElementForUpdate(TYPE_VARCHAR, "Maja", "testna", "Ime", row_root, 1);
    InsertNewElementForUpdate(TYPE_VARCHAR, "Mihi", "testna", "Ime", row_root, 0);
    update_row(row_root);

    //AK_print_table("testna");

    /*DeleteAllElements(row_root);
    InsertNewElement(TYPE_VARCHAR,"Brace Radica 13","testna_druga","Adresa",row_root);
    InsertNewElementForUpdate(TYPE_VARCHAR,"Nikola","testna_durga","Ime",row_root,0);
    InsertNewElement(TYPE_VARCHAR,"Bakoš","testna_druga","Prezime",row_root);
    some_element=GetFirstElement(row_root);
    insert_row(row_root);

    DeleteAllElements(row_root);
    InsertNewElement(TYPE_VARCHAR,"Kalnička 54","testna_druga","Adresa",row_root);
    InsertNewElementForUpdate(TYPE_VARCHAR,"Mihi","testna_durga","Ime",row_root,0);
    InsertNewElement(TYPE_VARCHAR,"Vacenovski","testna_druga","Prezime",row_root);
    some_element=GetFirstElement(row_root);
    insert_row(row_root);

    DeleteAllElements(row_root);
    InsertNewElement(TYPE_VARCHAR,"Neka","testna_druga","Adresa",row_root);
    InsertNewElementForUpdate(TYPE_VARCHAR,"Maja","testna_durga","Ime",row_root,0);
    InsertNewElement(TYPE_VARCHAR,"Vacenovski","testna_druga","Prezime",row_root);
    some_element=GetFirstElement(row_root);
    insert_row(row_root);

    DeleteAllElements(row_root);
    broj=133;
    InsertNewElement(TYPE_INT,&broj,"testna","Redni_broj",row_root);
    InsertNewElementForUpdate(TYPE_VARCHAR,"Maja","testna","Ime",row_root,0);
    InsertNewElement(TYPE_VARCHAR,"Vacenovski","testna","Prezime",row_root);
    some_element=GetFirstElement(row_root);
    insert_row(row_root);

    DeleteAllElements(row_root);
    broj=1334;
    InsertNewElement(TYPE_INT,&broj,"testna","Redni_broj",row_root);
    InsertNewElementForUpdate(TYPE_VARCHAR,"Matija","testna","Ime",row_root,0);
    InsertNewElement(TYPE_VARCHAR,"Novak","testna","Prezime",row_root);
    some_element=GetFirstElement(row_root);
    insert_row(row_root);*/

    DeleteAllElements(row_root);
    free(row_root);
}
