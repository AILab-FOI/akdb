/**
@file dbman.c Defines functions for the disk manager
 */
/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * dbman.c
 * Copyright (C) Markus Schatten 2009 <markus.schatten@foi.hr>
 *
 * main.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * main.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "dbman.h"

/**
 @author Markus Schatten

 Initializes a new database file named DB_FILE

 @param size size of new file in in blocks

 @return EXIT_SUCCESS if the file has been written to disk, EXIT_ERROR otherwise

 */
int AK_init_db_file(int size) {
    register int i = 0, j, k;

    AK_block * block = NULL;

    db_file_size = size;

    if ((db = fopen(DB_FILE, "wb")) == NULL) {
        printf("AK_init_db_file: ERROR. Cannot open db file %s.\n", DB_FILE);
        exit(EXIT_ERROR);
    }


    printf("AK_init_db_file: Initializing DB file..."
            "\nPlease be patient, this can take several minutes depending "
            "on disk performance.\n");

    if ((block = (AK_block *) malloc(sizeof ( AK_block))) == NULL) {
        printf("AK_init_db_file: ERROR. Cannot allocate block %d\n", i);
        exit(EXIT_ERROR);
    }


    for (i = 0; i < MAX_ATTRIBUTES; i++) {
        block->header[ i ].type = FREE_INT;
        for (j = 0; j < MAX_ATT_NAME; j++) {
            block->header[ i ].att_name[ j ] = FREE_CHAR;
        }
        for (j = 0; j < MAX_CONSTRAINTS; j++) {
            block->header[ i ].integrity[ j ] = FREE_INT;
            for (k = 0; k < MAX_CONSTR_NAME; k++) {
                block->header[ i ].constr_name[ j ][ k ] = FREE_CHAR;
            }
            for (k = 0; k < MAX_CONSTR_CODE; k++) {
                block->header[ i ].constr_code[ j ][ k ] = FREE_CHAR;
            }
        }
    }

    for (i = 0; i < DATA_BLOCK_SIZE; i++) {
        block->tuple_dict[ i ].type = FREE_INT;
        block->tuple_dict[ i ].address = FREE_INT;
        block->tuple_dict[ i ].size = FREE_INT;
    }
    for (i = 0; i < DATA_BLOCK_SIZE * DATA_ENTRY_SIZE; i++) {
        block->data[ i ] = FREE_CHAR;
    }

    block->type = BLOCK_TYPE_FREE;
    block->chained_with = NOT_CHAINED;
    block->free_space = DATA_BLOCK_SIZE * DATA_ENTRY_SIZE * sizeof ( int);
    block->last_tuple_dict_id = 0;

    for (i = 0; i < db_file_size; i++) {
        block->address = i;
        if (fwrite(block, sizeof ( *block), 1, db) != 1) {
            printf("AK_init_db_file: ERROR. Cannot write block %d\n", i);
            exit(EXIT_ERROR);
        }
    }
    free(block);

    fclose(db);

    printf("AK_init_db_file: Done!\n");

    return ( EXIT_SUCCESS);
}

/**
 @author Markus Schatten

 Reads a block at a given address (block number less than db_file_size)

 @param address block number (address)
 @return pointer to block allocated in memory
 */
AK_block * AK_read_block(int address) {
    AK_block * block = (AK_block *) malloc(sizeof ( AK_block));

    if ((db = fopen(DB_FILE, "r")) == NULL) {
        printf("AK_read_block: ERROR. Cannot open db file %s.\n", DB_FILE);
        exit(EXIT_ERROR);
    }

    if (fseek(db, address * sizeof ( AK_block), SEEK_SET) != 0) {
        printf("AK_read_block: ERROR. Cannot set position to provided address block %d.\n", address);
        exit(EXIT_ERROR);

    }
    if (fread(block, sizeof ( AK_block), 1, db) == 0) {
        printf("AK_read_block: ERROR. Cannot read block %d.\n", address);
        exit(EXIT_ERROR);
    }

    fclose(db);

    return ( block);
}

/**
 @author Markus Schatten

 Writes a block to DB file

 @param block poiner to block allocated in memory to write
 @return EXIT_SUCCESS if successful, EXIT_ERROR otherwise
 */
int AK_write_block(AK_block * block) {
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
    if (DEBUG)
        printf("AK_write_block: Written block at address %d\n", block->address * sizeof ( AK_block));
    return ( EXIT_SUCCESS);
}

/**
 @author Nikola Bakoš

 Function to alocate new extent of blocks

 @param start_address address (block number) to start searching for sufficient space
 @param old_size size of previous extent in same segment (in blocks)
 @param extent_type type of extent (can be one of:
        SEGMENT_TYPE_SYSTEM_TABLE,
        SEGMENT_TYPE_TABLE,
        SEGMENT_TYPE_INDEX,
        SEGMENT_TYPE_TRANSACTION,
        SEGMENT_TYPE_TEMP
 @param header pointer to header that should be written to the new extent (all blocks)

 @return address (block number) of new extent if successful, EXIT_ERROR otherwise
 */
int AK_new_extent(int start_address, int old_size, int extent_type, AK_header *header) {
    int req_free_space; /// var - How much of space is required for extent
    int first_addr_of_extent = -1; /// var - Start address for extent
    int nAlocated_blocks = 0; /// var - Number of blocks to be alocated for extent
    register int i, j; /// vars for loop [for]
    AK_block *block;

    /// if the old_size is 0 then the size of new extent is INITIAL_EXTENT_SIZE
    if (old_size == 0) {
        req_free_space = INITIAL_EXTENT_SIZE;
    } else {
        float RESIZE_FACTOR = 0;

        switch (extent_type) {
            case SEGMENT_TYPE_TABLE:
                RESIZE_FACTOR = EXTENT_GROWTH_TABLE;
                break;
            case SEGMENT_TYPE_INDEX:
                RESIZE_FACTOR = EXTENT_GROWTH_INDEX;
                break;
            case SEGMENT_TYPE_TRANSACTION:
                RESIZE_FACTOR = EXTENT_GROWTH_TRANSACTION;
                break;
            case SEGMENT_TYPE_TEMP:
                RESIZE_FACTOR = EXTENT_GROWTH_TEMP;
                break;
        }

        req_free_space = old_size + old_size * RESIZE_FACTOR;
    }

    for (i = start_address; i < db_file_size; i++) {
        if (((int) (req_free_space - 1) > (db_file_size - i))) {
            printf("AK_new_extent: ERROR. Not enought space for new extent. Requested space was: %d\n", req_free_space);
            return (EXIT_ERROR);
        }

        /// check the block is free
        block = AK_read_block(i); /// read block

        if ((block->type) != BLOCK_TYPE_FREE) /// if the block is used
        {
            nAlocated_blocks = 0;
            first_addr_of_extent = -1;
            continue; /// goto next iteration of for loop
        } else {
            if (nAlocated_blocks == 0)
                first_addr_of_extent = i; /// if it is a first free block

            nAlocated_blocks++; /// increase the number of block by 1

            if (nAlocated_blocks != 0 && (req_free_space == nAlocated_blocks)) /// if requested space for extent is OK
            {
                break; /// exit loop
            }
        }
    }
    int success = 0; /// var to check the number of written blocks


    for (j = first_addr_of_extent; j < (first_addr_of_extent + nAlocated_blocks); j++) {
        block = AK_read_block(j); /// read block
        int x = 0;
        for (x = 0; x < MAX_ATTRIBUTES; x++) {
            if (header[ x ].type == 0) {
                break;
            } else {
                memcpy(& block->header[ x ], & header[ x ], sizeof ( *header)); /// copy header information
            }
        }
        block->type = BLOCK_TYPE_NORMAL; /// set the block type
        block->free_space = 0;
        block->last_tuple_dict_id = 0;
        if (AK_write_block(block) == EXIT_SUCCESS) /// if write of block succeded increase var success, else nothing
        {
            success++;
        }

        free(block); /// free the block
    }
    if (DEBUG)
        printf("AK_new_extent: first_addr_of_extent= %i , nAlocated_blocks= %i , zavrsna_adresa= %i, success= %i\n", first_addr_of_extent, nAlocated_blocks, j, success);

    if (success != nAlocated_blocks) /// if some blocks are not succesfully alocated, whitch means the extend alocation has FAILED
    {
        printf("AK_new_extent: ERROR. Cannot allocate extent %d\n", first_addr_of_extent);
        return ( EXIT_ERROR);
    }
    return ( first_addr_of_extent);

}

/**
 @author Tomislav Fotak

 Function to allocate new segment of extents. In this phase of implementation, only extents
 containing INITIAL_EXTENT_SIZE blocks can be allocated

 @param name (character pointer) name of segment
 @param type segment type (possible values:
        SEGMENT_TYPE_SYSTEM_TABLE,
        SEGMENT_TYPE_TABLE,
        SEGMENT_TYPE_INDEX,
        SEGMENT_TYPE_TRANSACTION,
        SEGMENT_TYPE_TEMP
                                                           )
 @param header (header pointer) pointer to header that should be written to the new extent (all blocks)
 @return EXIT_SUCCESS for success or EXIT_ERROR if some error occurs
 */
int AK_new_segment(char * name, int type, AK_header *header) {
    int segment_start_addr = 1; /// start adress for segment because we can not allocate segment in block 0
    int i; /// counter
    AK_block *block;
    int current_extent_start_addr;
    int first_allocated_block = -1;

    for (i = segment_start_addr; i <= db_file_size; i++) {
        if (DEBUG)
            printf("AK_new_segment: Reading block %d, %s.\n", i, name);
        /// check if the block is free
        block = AK_read_block(i);

        if (block->type == BLOCK_TYPE_FREE) {
            current_extent_start_addr = AK_new_extent(i, 0, type, header); /// allocate new extent
            /// if extent is successfully allocated, increment number of allocated extents and move to
            /// next block after allocated extent, else move for INITIAL_EXTENT_SIZE blocks, so in that way get
            /// either first block of new extent or some block in this extent which will not be free
            if (current_extent_start_addr != EXIT_ERROR) {
                if (first_allocated_block == -1)
                    first_allocated_block = current_extent_start_addr;

                break;
            } else {
                i += INITIAL_EXTENT_SIZE - 1;
            }
        }
    }
    if (first_allocated_block == -1) {
        return ( EXIT_ERROR);
    }
    return first_allocated_block;
}

/**
 @author Matija Novak

 Function for creating header and initalize with default values

 @param name (char) - name of the atribute
 @param type (int) - type of the atribute
 @param integrity (int) - standard integrity costraint
 @param constr_name (char) - extra integrity constraint name
 @param contr_code - extra integrity costraint code
 @return AK_header
 */
AK_header * AK_create_header(char * name, int type, int integrity, char * constr_name, char * contr_code) {
    AK_header * catalog_header = (AK_header *) malloc(sizeof ( AK_header));
    if (DEBUG)
        printf("AK_create_header: Header: %s, %d\n", name, strlen(name));
    catalog_header->type = type;
    memcpy(catalog_header->att_name, name, strlen(name));
    int i = 0;
    int j = 0;
    int k = 0;
    /// initialize catalog_header->integrity and catalog_header->constr_name[][] and catalog_header->constr_code[][] with data given as functions parameters
    for (i = 0; i < MAX_CONSTRAINTS; i++) {
        catalog_header->integrity[ i ] = integrity;
        for (j = 0; j < MAX_CONSTR_NAME; j++) {
            catalog_header->constr_name[ i ][ j ] = constr_name;
        }
        for (k = 0; k < MAX_CONSTR_CODE; k++) {
            catalog_header->constr_code[ i ][ k ] = contr_code;
        }
    }

    return catalog_header;
}

/**
 @author Matija Novak

 Function for inserting entry in tuple_dict and data of a block

 @param block_adress (AK_block) - adress of a block in which we want insert data
 @param type (int) - type of entry_data
 @param entry_data (char) - data which is inserted, can be int but must first be converted to char
 @param i (int) -adress in tuple_dict array (example block_address->tuple_dict[i])

 @return nothing because it gets the address of an block like a function parameter
 and works directly with the orginal block
 */

void AK_insert_entry(AK_block * block_address, int type, void * entry_data, int i) {
    AK_tuple_dict * catalog_tuple_dict = (AK_tuple_dict *) malloc(sizeof ( AK_tuple_dict));

    if (DEBUG)
        printf("AK_insert_entry: Insert data: %d  Size of data:\n", *((int *) entry_data));

    /// using strlen becuse sizeof(entry_data) is always 4
    /// copy data into bloc->data on start position bloc->free_space
    if (type == TYPE_INT) {
        memcpy(block_address->data + block_address->free_space, entry_data, AK_type_size(type, entry_data));
        if (DEBUG) {
            printf("AK_insert_entry: Insert data: %d  Size of data:\n", (int) entry_data);
        }
    } else {
        memcpy(block_address->data + block_address->free_space, entry_data, AK_type_size(type, entry_data));
        if (DEBUG) {
            printf("AK_insert_entry: Insert data: %s  Size of data:\n", (char *) entry_data);
        }
    }
    /// address of entry data in block->data
    catalog_tuple_dict->address = block_address->free_space;

    /// calculate next free space for the next entry data
    block_address->free_space += AK_type_size(type, entry_data); ///sizeof(entry_data)+1);///(sizeof(int));
    /// no need for "+strlen(entry_data)" while "+1" is like "new line"

    /// type of entry data
    catalog_tuple_dict->type = type;
    /// size of entry data
    catalog_tuple_dict->size = AK_type_size(type, entry_data);

    /// copy tuple_dict to block->tuple_dict[i]
    /// must use & becouse tuple_dict[i] is value and catalog_tuple_dict adress
    memcpy(& block_address->tuple_dict[i], catalog_tuple_dict, sizeof (* catalog_tuple_dict));
}

/**
 @author Matija Novak

 Function initialises the sytem table catalog and writes the result in first (0) block in db_file

 @param adresses of system table blocks in db_file
 @return nothing EXIT_SUCCESS if initialization was succesful if not returns EXIT_ERROR
 */
int AK_init_system_tables_catalog(int relation, int attribute, int index, int view, int sequence, int function, int function_arguments,
        int trigger, int trigger_conditions, int db, int db_obj, int user, int group, int right, int constraint, int constraintNull, int reference) {
    if (DEBUG)
        printf("AK_init_system_tables_catalog: Initializing system tables catalog\n");
    AK_block * catalog_block = (AK_block *) malloc(sizeof ( AK_block));
    /// first header attribute of catalog_block
    AK_header * catalog_header_name = (AK_header *) malloc(sizeof ( AK_header));
    catalog_header_name = AK_create_header("Name", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    /// second attribute of catalog_block
    AK_header * catalog_header_address = (AK_header *) malloc(sizeof ( AK_header));
    catalog_header_address = AK_create_header("Address", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);

    /// initialize other elements of block (adress, type, chained_with, free_space)
    catalog_block->address = 0;
    catalog_block->type = BLOCK_TYPE_NORMAL;
    catalog_block->chained_with = NOT_CHAINED;
    catalog_block->free_space = 0; ///using as an address for the first free space in block->data


    /// merge catalog_heder with heders created before
    memcpy(& catalog_block->header[0], catalog_header_name, sizeof ( * catalog_header_name));
    memcpy(& catalog_block->header[1], catalog_header_address, sizeof ( * catalog_header_address));

    AK_tuple_dict tuple_dict[25];
    memcpy(catalog_block->tuple_dict, tuple_dict, sizeof ( *tuple_dict));

    int i = 0;
    /// insert data and tuple_dict in block
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_relation", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &relation, i);
    i++;
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_attribute", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &attribute, i);
    i++;
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_index", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &index, i);
    i++;
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_view", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &view, i);
    i++;
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_sequence", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &sequence, i);
    i++;
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_function", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &function, i);
    i++;
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_function_arguments", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &function_arguments, i);
    i++;
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_trigger", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &trigger, i);
    i++;
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_trigger_conditions", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &trigger_conditions, i);
    i++;
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_db", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &db, i);
    i++;
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_db_obj", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &db_obj, i);
    i++;
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_user", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &user, i);
    i++;
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_group", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &group, i);
    i++;
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_right", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &right, i);
    i++;
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_constraints_between", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &constraint, i);
    i++;
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_constraints_not_null", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &constraintNull, i);
    i++;
    AK_insert_entry(catalog_block, TYPE_VARCHAR, "AK_reference", i);
    i++;
    AK_insert_entry(catalog_block, TYPE_INT, &reference, i);
    i++;

    /// call function for writing the block on the first place in the file (ie. first block is on position zero)
    if (AK_write_block(catalog_block) == EXIT_SUCCESS) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_ERROR;
    }
}

/**
 @author Miroslav Policki

 Sets the first num ints of a block of memory to the specified value

 @param block pointer to the block of memory to fill
 @param value int value to be set
 @param num number of ints in the block of memory to be set
 */
void AK_memset_int(void *block, int value, size_t num) {
    size_t i;

    for (i = 0; i < num; i++)
        *((int *) block + i) = value;
}

int AK_register_system_tables(int relation, int attribute, int index, int view, int sequence, int function, int function_arguments,
        int trigger, int trigger_conditions, int db, int db_obj, int user, int group, int right, int constraint, int constraintNull, int reference) {
    AK_block *relationTable = AK_read_block(relation);
    int i = 1, j = 0;
    int end;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_relation", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &relation, j);
    j++;
    end = relation + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_attribute", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &attribute, j);
    j++;
    end = attribute + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_index", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &index, j);
    j++;
    end = index + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_view", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &view, j);
    j++;
    end = view + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_sequence", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &sequence, j);
    j++;
    end = sequence + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_function", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &function, j);
    j++;
    end = function + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_function_arguments", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &function_arguments, j);
    j++;
    end = function_arguments + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_trigger", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &trigger, j);
    j++;
    end = trigger + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_trigger_conditions", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &trigger_conditions, j);
    j++;
    end = trigger_conditions + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_db", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &db, j);
    j++;
    end = db + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_db_obj", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &db_obj, j);
    j++;
    end = db_obj + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_user", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &user, j);
    j++;
    end = user + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_group", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &group, j);
    j++;
    end = group + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_right", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &right, j);
    j++;
    end = right + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_constraints_between", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &constraint, j);
    j++;
    end = constraint + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_constraints_not_null", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &constraintNull, j);
    j++;
    end = constraintNull + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    AK_insert_entry(relationTable, TYPE_INT, &i, j);
    j++;
    AK_insert_entry(relationTable, TYPE_VARCHAR, "AK_reference", j);
    j++;
    AK_insert_entry(relationTable, TYPE_INT, &reference, j);
    j++;
    end = reference + INITIAL_EXTENT_SIZE;
    AK_insert_entry(relationTable, TYPE_INT, &end, j);
    j++;
    i++;

    relationTable->last_tuple_dict_id = j;

    AK_write_block(relationTable);
    return EXIT_SUCCESS;
}

/**
 @author Miroslav Policki

 Initializes the system catalog

 @return EXIT_SUCCESS if the system catalog has been initialized, EXIT_ERROR otherwise

 */
int AK_init_system_catalog() {
    int relation, attribute, index, view, sequence, function, function_arguments, trigger, trigger_conditions, db, db_obj, user, group, right, constraint, constraintNull, reference;
    int i;

    if (DEBUG)
        printf("AK_init_system_catalog: System catalog initialization started...\n");

    AK_header hConstraintNotNull[5] = {
        {TYPE_INT, "obj_id", 0, '\0', '\0'},
        {TYPE_VARCHAR, "tableName", 0, '\0', '\0'},
        {TYPE_VARCHAR, "constraintName", 0, '\0', '\0'},
        {TYPE_VARCHAR, "attributeName", 0, '\0', '\0'},
        {0, '\0', 0, '\0', '\0'}
    };


    AK_header hConstraintBetween[7] = {
        {TYPE_INT, "obj_id", 0, '\0', '\0'},
        {TYPE_VARCHAR, "tableName", 0, '\0', '\0'},
        {TYPE_VARCHAR, "constraintName", 0, '\0', '\0'},
        {TYPE_VARCHAR, "attributeName", 0, '\0', '\0'},
        {TYPE_VARCHAR, "start_value", 0, '\0', '\0'},
        {TYPE_VARCHAR, "end_value", 0, '\0', '\0'},
        {0, '\0', 0, '\0', '\0'}
    };


    AK_header hRelation[5] = {
        {TYPE_INT, "obj_id", 0, '\0', '\0'},
        {TYPE_VARCHAR, "name", 0, '\0', '\0'},
        {TYPE_INT, "start_address", 0, '\0', '\0'},
        {TYPE_INT, "end_address", 0, '\0', '\0'},
        {0, '\0', 0, '\0', '\0'}
    };

    AK_header hAttribute[5] = {
        {TYPE_INT, "obj_id", 0, '\0', '\0',},
        {TYPE_VARCHAR, "name", 0, '\0', '\0',},
        {TYPE_INT, "type", 0, '\0', '\0',},
        {TYPE_INT, "table_id", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };

    AK_header hIndex[7] = {
        {TYPE_INT, "obj_id", 0, '\0', '\0',},
        {TYPE_VARCHAR, "name", 0, '\0', '\0',},
        {TYPE_INT, "start_address", 0, '\0', '\0',},
        {TYPE_INT, "end_address", 0, '\0', '\0'},
        {TYPE_INT, "table_id", 0, '\0', '\0',},
        {TYPE_INT, "attribute_id", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };

    AK_header hView[4] = {
        {TYPE_INT, "obj_id", 0, '\0', '\0',},
        {TYPE_VARCHAR, "name", 0, '\0', '\0',},
        {TYPE_VARCHAR, "query", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };

    AK_header hSequence[8] = {
        {TYPE_INT, "obj_id", 0, '\0', '\0',},
        {TYPE_VARCHAR, "name", 0, '\0', '\0',},
        {TYPE_INT, "current_value", 0, '\0', '\0',},
        {TYPE_INT, "increment", 0, '\0', '\0',},
        {TYPE_INT, "max", 0, '\0', '\0',},
        {TYPE_INT, "min", 0, '\0', '\0',},
        {TYPE_BOOL, "cycle", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };

    AK_header hFunction[5] = {
        {TYPE_INT, "obj_id", 0, '\0', '\0',},
        {TYPE_VARCHAR, "name", 0, '\0', '\0',},
        {TYPE_INT, "arg_num", 0, '\0', '\0',},
        {TYPE_INT, "return_type", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };

    AK_header hFunction_arguments[4] = {
        {TYPE_INT, "func_id", 0, '\0', '\0',},
        {TYPE_INT, "att_num", 0, '\0', '\0',},
        {TYPE_INT, "att_type", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };

    AK_header hTrigger[7] = {
        {TYPE_INT, "obj_id", 0, '\0', '\0',},
        {TYPE_VARCHAR, "name", 0, '\0', '\0',},
        {TYPE_VARCHAR, "event", 0, '\0', '\0',},
        {TYPE_VARCHAR, "condition", 0, '\0', '\0',},
        {TYPE_INT, "action", 0, '\0', '\0',},
        {TYPE_INT, "on", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };

    AK_header hTrigger_conditions[5] = {
        {TYPE_INT, "trigger", 0, '\0', '\0',}, //pk
        {TYPE_INT, "id", 0, '\0', '\0',}, //pk
        {TYPE_VARCHAR, "data", 0, '\0', '\0',},
        {TYPE_INT, "type", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };


    AK_header hDb[3] = {
        {TYPE_INT, "obj_id", 0, '\0', '\0',},
        {TYPE_VARCHAR, "name", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };

    AK_header hDb_obj[3] = {
        {TYPE_INT, "db_id", 0, '\0', '\0',},
        {TYPE_INT, "artifact_id", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };


    AK_header hUser[4] = {
        {TYPE_INT, "obj_id", 0, '\0', '\0',},
        {TYPE_VARCHAR, "username", 0, '\0', '\0',},
        {TYPE_INT, "password", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };

    AK_header hGroup[3] = {
        {TYPE_INT, "obj_id", 0, '\0', '\0',},
        {TYPE_VARCHAR, "name", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };

    AK_header hRight[6] = {
        {TYPE_INT, "obj_id", 0, '\0', '\0',},
        {TYPE_VARCHAR, "name", 0, '\0', '\0',},
        {TYPE_INT, "group_id", 0, '\0', '\0',},
        {TYPE_INT, "artifact_id", 0, '\0', '\0',},
        {TYPE_INT, "right_type", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };

    AK_header hReference[7] = {
        {TYPE_VARCHAR, "table", 0, '\0', '\0',},
        {TYPE_VARCHAR, "constraint", 0, '\0', '\0',},
        {TYPE_VARCHAR, "attribute", 0, '\0', '\0',},
        {TYPE_VARCHAR, "parent", 0, '\0', '\0',},
        {TYPE_VARCHAR, "parent_attribute", 0, '\0', '\0',},
        {TYPE_INT, "type", 0, '\0', '\0',},
        {0, '\0', 0, '\0', '\0'}
    };

    for (i = 0; i < 4; i++) {
        AK_memset_int(hConstraintNotNull[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hConstraintNotNull[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hConstraintNotNull[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }

    for (i = 0; i < 6; i++) {
        AK_memset_int(hConstraintBetween[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hConstraintBetween[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hConstraintBetween[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }

    for (i = 0; i < 4; i++) {
        AK_memset_int(hRelation[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hRelation[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hRelation[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }

    for (i = 0; i < 4; i++) {
        AK_memset_int(hAttribute[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hAttribute[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hAttribute[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }

    for (i = 0; i < 6; i++) {
        AK_memset_int(hIndex[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hIndex[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hIndex[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }

    for (i = 0; i < 3; i++) {
        AK_memset_int(hView[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hView[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hView[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }

    for (i = 0; i < 7; i++) {
        AK_memset_int(hSequence[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hSequence[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hSequence[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }

    for (i = 0; i < 4; i++) {
        AK_memset_int(hFunction[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hFunction[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hFunction[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }

    for (i = 0; i < 3; i++) {
        AK_memset_int(hFunction_arguments[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hFunction_arguments[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hFunction_arguments[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }

    for (i = 0; i < 6; i++) {
        AK_memset_int(hTrigger[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hTrigger[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hTrigger[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }

    for (i = 0; i < 4; i++) {
        AK_memset_int(hTrigger_conditions[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hTrigger_conditions[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hTrigger_conditions[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }

    for (i = 0; i < 2; i++) {
        AK_memset_int(hDb[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hDb[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hDb[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }

    for (i = 0; i < 2; i++) {
        AK_memset_int(hDb_obj[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hDb_obj[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hDb_obj[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }

    for (i = 0; i < 3; i++) {
        AK_memset_int(hUser[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hUser[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hUser[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }

    for (i = 0; i < 2; i++) {
        AK_memset_int(hGroup[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hGroup[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hGroup[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }

    for (i = 0; i < 5; i++) {
        AK_memset_int(hRight[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hRight[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hRight[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }

    for (i = 0; i < 6; i++) {
        AK_memset_int(hReference[i].integrity, FREE_INT, MAX_CONSTRAINTS);
        memset(hReference[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
        memset(hReference[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
    }
    if (DEBUG)
        printf("AK_init_system_catalog: Creating new segments...\n");

    relation = AK_new_segment("AK_relation", SEGMENT_TYPE_SYSTEM_TABLE, hRelation);
    attribute = AK_new_segment("AK_attribute", SEGMENT_TYPE_SYSTEM_TABLE, hAttribute);
    index = AK_new_segment("AK_index", SEGMENT_TYPE_SYSTEM_TABLE, hIndex);
    view = AK_new_segment("AK_view", SEGMENT_TYPE_SYSTEM_TABLE, hView);
    sequence = AK_new_segment("AK_sequence", SEGMENT_TYPE_SYSTEM_TABLE, hSequence);
    function = AK_new_segment("AK_function", SEGMENT_TYPE_SYSTEM_TABLE, hFunction);
    function_arguments = AK_new_segment("AK_function_arguments", SEGMENT_TYPE_SYSTEM_TABLE, hFunction_arguments);
    trigger = AK_new_segment("AK_trigger", SEGMENT_TYPE_SYSTEM_TABLE, hTrigger);
    trigger_conditions = AK_new_segment("AK_trigger_conditions", SEGMENT_TYPE_SYSTEM_TABLE, hTrigger_conditions);
    db = AK_new_segment("AK_db", SEGMENT_TYPE_SYSTEM_TABLE, hDb);
    db_obj = AK_new_segment("AK_db_obj", SEGMENT_TYPE_SYSTEM_TABLE, hDb_obj);
    user = AK_new_segment("AK_user", SEGMENT_TYPE_SYSTEM_TABLE, hUser);
    group = AK_new_segment("AK_group", SEGMENT_TYPE_SYSTEM_TABLE, hGroup);
    right = AK_new_segment("AK_right", SEGMENT_TYPE_SYSTEM_TABLE, hRight);
    constraint = AK_new_segment("AK_constraints_between", SEGMENT_TYPE_SYSTEM_TABLE, hConstraintBetween);
    constraintNull = AK_new_segment("AK_constraints_not_null", SEGMENT_TYPE_SYSTEM_TABLE, hConstraintNotNull);
    reference = AK_new_segment("AK_reference", SEGMENT_TYPE_SYSTEM_TABLE, hReference);

    if (DEBUG)
        printf("AK_init_system_catalog: Segments created!\n");

    if (EXIT_SUCCESS == AK_init_system_tables_catalog(relation, attribute, index, view, sequence, function, function_arguments, trigger, trigger_conditions, db, db_obj, user, group, right, constraint, constraintNull, reference)) {
        AK_register_system_tables(relation, attribute, index, view, sequence, function, function_arguments, trigger, trigger_conditions, db, db_obj, user, group, right, constraint, constraintNull, reference);
        printf("AK_init_system_catalog: System catalog initialized!\n");
        return EXIT_SUCCESS;
    } else {
        printf("AK_init_system_catalog: ERROR. AK_init_system_tables_catalog(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d) failed.\n", relation, attribute, index, view, sequence, function, function_arguments, trigger, trigger_conditions, db, db_obj, user, group, right);
        return EXIT_ERROR;
    }
}

/**
 @author Markus Schatten

 Deletes a block by a given block address (resets the header and data)

 @param address - address of the block to be deleted

 @return returns EXIT_SUCCESS if deletion successful, else EXIT_ERROR
 */
int AK_delete_block(int address) {
    register int i, j, k;

    AK_header head[ MAX_ATTRIBUTES ];
    AK_tuple_dict tuple_dict[ DATA_BLOCK_SIZE ];
    unsigned char data[ DATA_BLOCK_SIZE * DATA_ENTRY_SIZE ];

    for (i = 0; i < MAX_ATTRIBUTES; i++) {
        head[ i ].type = FREE_INT;
        for (j = 0; j < MAX_ATT_NAME; j++) {
            head[ i ].att_name[ j ] = FREE_CHAR;
        }
        for (j = 0; j < MAX_CONSTRAINTS; j++) {
            head[ i ].integrity[ j ] = FREE_INT;
            for (k = 0; k < MAX_CONSTR_NAME; k++) {
                head[ i ].constr_name[ j ][ k ] = FREE_CHAR;
            }
            for (k = 0; k < MAX_CONSTR_CODE; k++) {
                head[ i ].constr_code[ j ][ k ] = FREE_CHAR;
            }
        }
    }

    for (i = 0; i < DATA_BLOCK_SIZE; i++) {
        tuple_dict[ i ].type = FREE_INT;
        tuple_dict[ i ].address = FREE_INT;
        tuple_dict[ i ].size = FREE_INT;
    }
    for (i = 0; i < DATA_BLOCK_SIZE * DATA_ENTRY_SIZE; i++) {
        data[ i ] = FREE_CHAR;
    }

    AK_block * block = AK_read_block(address);
    block->type = BLOCK_TYPE_FREE;
    block->chained_with = NOT_CHAINED;
    block->free_space = DATA_BLOCK_SIZE * DATA_ENTRY_SIZE * sizeof ( int);
    memcpy(block->header, head, sizeof ( *head));
    memcpy(block->tuple_dict, tuple_dict, sizeof ( *tuple_dict));
    memcpy(block->data, data, sizeof ( *data));

    if (AK_write_block(block) == EXIT_SUCCESS) {
        return EXIT_SUCCESS;
    }
    return EXIT_ERROR;
}

/**
 @author Dejan Sambolić

 Deletes an extent between begin and end blocks

 @param begin - address of extent's first block
 @param end - adress of extent's last block

 @return returns EXIT_SUCCESS if extent has been successfully deleted, EXIT_ERROR otherwise
 */
int AK_delete_extent(int begin, int end) {
    int address;
    for (address = begin; address < end + 1; address++) {
        if (AK_delete_block(address) == EXIT_ERROR) {
            return EXIT_ERROR;
        }
    }
    return ( EXIT_SUCCESS);
}

/**
 @author Mislav Čakarić
 @param name name of the segment
 @param type type of the segment
 @return success or error
 */
int AK_delete_segment(char * name, int type) {
    int i = 0;
    table_addresses *addresses = (table_addresses*) get_segment_addresses(name, type);
    while (addresses->address_from[ i ] != 0) {
        if (AK_delete_extent(addresses->address_from[i], addresses->address_to[i]) == EXIT_ERROR)
            return EXIT_ERROR;
        i++;
    }
    AK_list *row_root = (AK_list *) malloc(sizeof (AK_list));
    InitializeList(row_root);
    char *system_table;
    switch (type) {
        case SEGMENT_TYPE_TABLE:
            system_table = "AK_relation";
            break;
        case SEGMENT_TYPE_INDEX:
            system_table = "AK_index";
            break;
        case SEGMENT_TYPE_TRANSACTION:
            //TO-DO
            printf("Not implemented yet!\n");
        case SEGMENT_TYPE_TEMP:
            //TO-DO
            printf("Not implemented yet!\n");
        default:
            return EXIT_ERROR;
    }
    DeleteAllElements(row_root);
    InsertNewElementForUpdate(TYPE_VARCHAR, name, system_table, "name", row_root, 1);
    delete_row(row_root);
    free(row_root);
    return EXIT_SUCCESS;
}

/**
 @author Markus Schatten

 */
int AK_init_disk_manager() {
    int size_in_mb = DB_FILE_SIZE;
    float size = 1024 * 1024 * size_in_mb / sizeof ( AK_block);

    printf("AK_init_disk_manager: Initializing disk manager...\n\n");
    if (DEBUG) {
        printf("AK_init_disk_manager: Block size is: %d\n", sizeof ( AK_block));
        printf("AK_init_disk_manager: We need %d blocks for %d MiB\n", (int) size, size_in_mb);
    }

    if (AK_init_db_file((int) size) == EXIT_SUCCESS) {
        if (AK_init_system_catalog() == EXIT_SUCCESS) {
            printf("AK_init_disk_manager: Disk manager initialized!\n\n");
            return EXIT_SUCCESS;
        }
        printf("AK_init_disk_manager: ERROR. System catalog initialization failed!\n");
        return ( EXIT_ERROR);
    }
    printf("AK_init_disk_manager: ERROR. DB file initialization failed!");
    return ( EXIT_ERROR);
}

