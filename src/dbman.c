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


#include "configuration.h"
#include "dbman.h"
#include "auxiliary.h"

/**
 @author Markus Schatten
 
 Initializes a new database file named DB_FILE

 @param size size of new file in in blocks
 
 @return EXIT_SUCCESS if the file has been written to disk, EXIT_ERROR otherwise

*/
int KK_init_db_file( int size )
{
	register int i, j, k;
	
	KK_header head[ MAX_ATTRIBUTES ];
	KK_tuple_dict tuple_dict[ DATA_BLOCK_SIZE ];
	unsigned char data[ DATA_BLOCK_SIZE * DATA_ENTRY_SIZE ];
	KK_block * block = ( KK_block * ) malloc ( sizeof( KK_block ) );
	
	db_file_size = size;
		
	if( ( db = fopen( DB_FILE, "wb" ) ) == NULL ) {
		printf( "KK_init_db_file: ERROR. Cannot open db file %s.\n", DB_FILE );
		exit( EXIT_ERROR );
	}
	

	printf( "KK_init_db_file: Initializing DB file..."
		"\nPlease be patient, this can take several minutes depending "
		"on disk performance.\n" );
	

	for( i = 0; i < MAX_ATTRIBUTES; i++ )
	{
		head[ i ].type = FREE_INT;
		for( j = 0; j < MAX_ATT_NAME; j++ )
		{
			head[ i ].att_name[ j ] = FREE_CHAR;
		}
		for( j = 0; j < MAX_CONSTRAINTS; j++ )
		{
			head[ i ].integrity[ j ] = FREE_INT;
			for( k = 0; k < MAX_CONSTR_NAME; k++ )
			{
				head[ i ].constr_name[ j ][ k ] = FREE_CHAR;
			}
			for( k = 0; k < MAX_CONSTR_CODE; k++ )
			{
				head[ i ].constr_code[ j ][ k ] = FREE_CHAR;
			}
		}
	}

	for( i = 0; i < DATA_BLOCK_SIZE; i++ )
	{
		tuple_dict[ i ].type = FREE_INT;
		tuple_dict[ i ].address = FREE_INT;
		tuple_dict[ i ].size = FREE_INT;		
	}
	for( i = 0; i < DATA_BLOCK_SIZE * DATA_ENTRY_SIZE; i++ )
	{
		data[ i ] = FREE_CHAR;
	}

	for( i = 0; i < db_file_size; i++ )
	{
		if( ( block = ( KK_block * ) malloc ( sizeof( KK_block ) ) ) == NULL )
		{
			printf( "KK_init_db_file: ERROR. Cannot allocate block %d\n", i );
			exit( EXIT_ERROR );
		}
		
		block->address = i;
		block->type = BLOCK_TYPE_FREE;
		block->chained_with = NOT_CHAINED;
		block->free_space = DATA_BLOCK_SIZE * DATA_ENTRY_SIZE * sizeof( int );
		memcpy( block->header, head, sizeof( *head ) );
		memcpy( block->tuple_dict, tuple_dict, sizeof( *tuple_dict ) );
		memcpy( block->data, data, sizeof( *data ) );		
		
		if( fwrite( block, sizeof( *block ), 1, db ) != 1 )
		{
			printf( "KK_init_db_file: ERROR. Cannot write block %d\n", i );
			exit( EXIT_ERROR );
		}
	}
	free( block );

	fclose( db );

	printf( "Done!\n" );

	return ( EXIT_SUCCESS );
}


/**
 @author Markus Schatten
 
 Reads a block at a given address (block number less than db_file_size)

 @param address block number (address)
 @return pointer to block allocated in memory
*/
KK_block * KK_read_block( int address )
{
	KK_block * block = ( KK_block * ) malloc ( sizeof( KK_block ) );

	if( ( db = fopen( DB_FILE, "r" ) ) == NULL ) 
	{
		printf( "KK_read_block: ERROR. Cannot open db file %s.\n", DB_FILE );
		exit( EXIT_ERROR );
	}	
	
	if( fseek( db, address * sizeof( KK_block ), SEEK_SET ) != 0 )
	{
		printf( "KK_read_block: ERROR. Cannot set position to provided address block %d.\n", address );
		exit( EXIT_ERROR );

	}
	if( fread( block, sizeof( KK_block ), 1, db ) == 0 )
	{
		printf( "KK_read_block: ERROR. Cannot read block %d.\n", address );
		exit( EXIT_ERROR );
	}

	fclose( db );

	return ( block );
}

/**
 @author Markus Schatten
 
 Writes a block to DB file

 @param block poiner to block allocated in memory to write
 @return EXIT_SUCCESS if successful, EXIT_ERROR otherwise
*/
int KK_write_block( KK_block * block )
{
	if( ( db = fopen( DB_FILE, "r+" ) ) == NULL ) {
		printf( "KK_write_block: ERROR. Cannot open db file %s.\n", DB_FILE );
		exit( EXIT_ERROR );
	}
	if( fseek( db, block->address * sizeof( KK_block ), SEEK_SET ) != 0 )
	{
		printf( "KK_write_block: ERROR. Cannot set position to provided address block %d.\n", block->address );
		exit( EXIT_ERROR );
	}
	if( fwrite( block, sizeof( *block ), 1, db ) != 1 )
	{
		printf( "KK_write_block: ERROR. Cannot write block at provided address %d.\n", block->address );
		exit( EXIT_ERROR );
	}
	return ( EXIT_SUCCESS );
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
int KK_new_extent( int start_address, int old_size, int extent_type, KK_header *header )
{
	int req_free_space; 				/// var - How much of space is required for extent
	int first_addr_of_extent = -1;			/// var - Start address for extent
	int nAlocated_blocks = 0; 			/// var - Number of blocks to be alocated for extent
	register int i, j; 				/// vars for loop [for]
	KK_block *block;

	/// if the old_size is 0 then the size of new extent is INITIAL_EXTENT_SIZE
	if( old_size == 0 )
	{
		req_free_space = INITIAL_EXTENT_SIZE;
	}
	else
	{
		float RESIZE_FACTOR = 0;

		switch(extent_type)
		{
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
	
	for( i = start_address; i < db_file_size; i++ )			
	{
		if( ( ( int ) ( req_free_space - 1 ) > ( db_file_size - i ) ) )
		{
			printf( "KK_new_extent: ERROR. Not enought space for new extent. Requested space was: %d\n", req_free_space );
			return(EXIT_ERROR);
		}
		
		/// check the block is free
		block = KK_read_block( i );   /// read block

		if( ( block->type ) != BLOCK_TYPE_FREE )  /// if the block is used
        {
			nAlocated_blocks = 0;
			first_addr_of_extent = -1;
			continue; 	/// goto next iteration of for loop
		}
		else
		{
			if( nAlocated_blocks == 0 )
				first_addr_of_extent = i;	/// if is a first free block
			
			nAlocated_blocks++;		/// increase number of block for 1
			
			if( nAlocated_blocks != 0 && ( req_free_space == nAlocated_blocks ) ) /// if requested space for extent is OK
			{
				break;	/// exit loop
			}
		}
	}
	int success = 0;    /// var for chack of number of blocks written

	
	for( j = first_addr_of_extent; j < ( first_addr_of_extent + nAlocated_blocks ); j++)
	{
		block = KK_read_block( j );   	/// read block
		int x=0;
		for(x=0;x<MAX_ATTRIBUTES;x++)
		{
			if(header[x].type==0)
			{break;}
			memcpy( & block->header[x], & header[x], sizeof( *header ) ); /// copy header information	
		}
		block->type = BLOCK_TYPE_NORMAL; /// set the block type
		block->free_space=0;
		if( KK_write_block( block ) == EXIT_SUCCESS ) /// if write of block succeded increase var success, else nothing
		{
			success++;
		}
		
		free(block);  /// free the block
	}
		if(DEBUG)
			printf( "KK_new_extent: first_addr_of_extent= %i , nAlocated_blocks= %i , zavrsna_adresa= %i, success= %i\n",first_addr_of_extent, nAlocated_blocks, j, success);
	
	if( success != nAlocated_blocks ) /// if some blocks are not succesfully alocated, whitch means the extend alocation has FAILED
	{
		printf( "KK_new_extent: ERROR. Cannot allocate extent %d\n", first_addr_of_extent );
		return( EXIT_ERROR );
	}
	//printf("vracam: %i\n",first_addr_of_extent);
	return( first_addr_of_extent );

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
int KK_new_segment(char* name, int type, KK_header *header)
{
	int segment_start_addr = 1; /// start adress for segment because we can not allocate segment in block 0
	int i; /// counter
	KK_block *block;
	int current_extent_start_addr;
	int first_allocated_block = -1;

	for ( i = segment_start_addr; i <= db_file_size; i++ )
	{
		/*if( DEBUG )
			printf( "KK_new_segment: Reading block %d.\n", i );*/
		/// check if the block is free
		block = KK_read_block(i);
		
		if ( block->type == BLOCK_TYPE_FREE )
		{			
			current_extent_start_addr = KK_new_extent( i, 0, type, header ); /// allocate new extent
			
			/// if extent is successfully allocatt_nameated, increment number of allocated extents and move to
			/// next block after allocated extent, else move for INITIAL_EXTENT_SIZE blocks, so in that way get
			/// either first block of new extent or some block in this extent which will not be free
			if ( current_extent_start_addr != EXIT_ERROR )
			{
				if (first_allocated_block == -1)
					first_allocated_block = current_extent_start_addr;
				
				break;
			}
			else
			{
				i += INITIAL_EXTENT_SIZE - 1;
			}
		}
	}
	if( first_allocated_block == -1 )
	{
		return( EXIT_ERROR );
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
 @return KK_header
 */
KK_header * KK_create_header(char * name, int type, int integrity, char * constr_name, char * contr_code ) 
{
	KK_header * catalog_header = ( KK_header * ) malloc ( sizeof( KK_header ) );
	if( DEBUG )
	 printf("KK_create_header: Header: %s, %d\n",name, strlen(name));
	catalog_header->type = type;
	memcpy(catalog_header->att_name,name, strlen(name));
	int i=0;
	int j=0;
	int k=0;
	/// initialize catalog_heder->integrity and catalog_header->constr_name[][] and catalog_header->constr_code[][] with data given as functions parameters
	for (i=0;i<MAX_CONSTRAINTS;i++)
	{
		catalog_header->integrity[ i ] = integrity;
		for (j=0;j<MAX_CONSTR_NAME;j++)
		{			
			catalog_header->constr_name[ i ][ j ] = constr_name;
		}
		for (k=0;k<MAX_CONSTR_CODE;k++)
		{
			catalog_header->constr_code[ i ][ k ] = contr_code;
		}
	}
	
	return catalog_header;
}
/**
 @author Matija Novak
 
 Function for inserting entry in tuple_dict and data of a block

 @param block_adress (KK_block) - adress of a block in which we want insert data
 @param type (int) - type of entry_data
 @param entry_data (char) - data which is inserted, can be int but must first be converted to char
 @param i (int) -adress in tuple_dict array (example block_address->tuple_dict[i])

 @return nothing because it gets the address of an block like a function parameter 
 and works directly with the orginal block
 */

void KK_insert_entry(KK_block * block_address, int type, void * entry_data, int i )
{
	KK_tuple_dict * catalog_tuple_dict = (KK_tuple_dict *) malloc (sizeof( KK_tuple_dict ));
	
	if( DEBUG )
		//printf("KK_insert_entry: Insert data: %d  Size of data:\n",(int)*entry_data);
	
	/// using strlen becuse sizeof(entry_data) is always 4
	/// copy data into bloc->data on start position bloc->free_space
	if (type==TYPE_INT)
	{
		int mm = entry_data;		
		printf("KK_insert_entry: prije Insert data: %d  Size of data:\n", mm);
		
		//memcpy(block_address->data+block_address->free_space, 4, 4);
		//block_address->data+block_address->free_space=entry_data;
		memcpy(block_address->data+block_address->free_space,entry_data, KK_type_size(type, entry_data));
		printf("KK_insert_entry: poslije Insert data: %d  Size of data:\n", entry_data);
	}
	else
	{
		memcpy(block_address->data+block_address->free_space,entry_data, KK_type_size(type, entry_data));
		printf("KK_insert_entry: Insert data: %s  Size of data:\n",entry_data);
	}	
	/// address of entry data in block->data
	catalog_tuple_dict->address=block_address->free_space;
	
	/// calculate next free space for the next entry data
	/// printf("\nxxxxxxxxxxxxxxx: %d,%d,%d,%d",strlen(int),sizeof(entry_data),strlen(entry_data),block_address->free_space);
	block_address->free_space+=KK_type_size(type, entry_data);///sizeof(entry_data)+1);///(sizeof(int));
	/// printf("\nxxxxxxxx:%d",block_address);
	/// no need for "+strlen(entry_data)" while "+1" is like "new line" 
	
	/// type of entry data 
	catalog_tuple_dict->type=type;
	/// size of entry data
	catalog_tuple_dict->size = KK_type_size(type, entry_data);
	
	/// copy tuple_dict to block->tuple_dict[i]
	/// must use & becouse tuple_dict[i] is value and catalog_tuple_dict adress
	memcpy(& block_address->tuple_dict[i],catalog_tuple_dict, sizeof(* catalog_tuple_dict));
}
/**
 @author Matija Novak
 
 Function initialises the sytem table catalog and writes the result in first (0) block in db_file

 @param adresses of system table blocks in db_file
 @return nothing EXIT_SUCCESS if initialization was succesful if not returns EXIT_ERROR
*/
int KK_init_system_tables_catalog( int relation, int attribute, int index, int view, int sequence, int function, int function_arguments, 
								  int trigger, int db, int db_obj, int user, int group, int right)
{	
	if( DEBUG )
	 printf("Start to initialize system tables catalog\n");
	KK_block * catalog_block = ( KK_block * ) malloc ( sizeof( KK_block ) );
	/// first header attribute of catalog_block
	KK_header * catalog_header_name  =  ( KK_header * ) malloc ( sizeof( KK_header ) );
	catalog_header_name = KK_create_header( "Name", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	/// second attribute of catalog_block
	KK_header * catalog_header_address = ( KK_header * ) malloc ( sizeof( KK_header ) );
	catalog_header_address = KK_create_header( "Address", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	
	/// initialize other elements of block (adress, type, chained_with, free_space)
	catalog_block->address = 0;
	catalog_block->type = BLOCK_TYPE_NORMAL;
	catalog_block->chained_with = NOT_CHAINED;
	catalog_block->free_space = 0; ///using as an address for the first free space in block->data

	
	/// merge catalog_heder with heders created before
	memcpy( & catalog_block->header[0], catalog_header_name, sizeof( * catalog_header_name ) );
	memcpy( & catalog_block->header[1], catalog_header_address, sizeof( * catalog_header_address ) );
	
	KK_tuple_dict tuple_dict[25];
	memcpy(catalog_block->tuple_dict, tuple_dict, sizeof( *tuple_dict ) );
	
	/// buf used for converting int to char
	char buf[5];
	/// insert data and tuple_dict in block
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_relation", 0 );
	/// convert int to char
	//sprintf(buf,"%d",relation);
	KK_insert_entry(catalog_block, TYPE_INT, &relation, 1 );
	
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_attribute", 2 );
	//sprintf(buf,"%d",attribute);
	KK_insert_entry(catalog_block, TYPE_INT, &attribute, 3 );
	
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_index", 4 );
	//sprintf(buf,"%d",index);
	KK_insert_entry(catalog_block, TYPE_INT, &index, 5 );
	
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_view", 6 );
	//sprintf(buf,"%d",view);
	KK_insert_entry(catalog_block, TYPE_INT, &view, 7 );
	
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_sequence", 8 );
	//sprintf(buf,"%d",sequence);
	KK_insert_entry(catalog_block, TYPE_INT, &sequence, 9 );
	
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_function",10 );
	//sprintf(buf,"%d",function);
	KK_insert_entry(catalog_block, TYPE_INT, &function, 11 );
	
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_function_arguments", 12 );
	//sprintf(buf,"%d",function_arguments);
	KK_insert_entry(catalog_block, TYPE_INT, &function_arguments, 13 );
	
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_trigger", 14 );
	//sprintf(buf,"%d",trigger);
	KK_insert_entry(catalog_block, TYPE_INT, &trigger, 15 );
	
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_db", 16 );
	//sprintf(buf,"%d",db);
	KK_insert_entry(catalog_block, TYPE_INT, &db, 17 );
	
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_db_obj", 18 );
	//sprintf(buf,"%d",db_obj);
	KK_insert_entry(catalog_block, TYPE_INT, &db_obj, 19 );
	
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_user", 20 );
	//sprintf(buf,"%d",user);
	KK_insert_entry(catalog_block, TYPE_INT, &user, 21 );
	
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_group", 22 );
	//sprintf(buf,"%d",group);
	KK_insert_entry(catalog_block, TYPE_INT, &group, 23 );
	
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_right", 24 );
	//sprintf(buf,"%d",right);
	KK_insert_entry(catalog_block, TYPE_INT, &right, 25 );
	
	/// call function for writing the block on the first place in the file (ie. first block is on position zero)
	if ( KK_write_block(catalog_block) == EXIT_SUCCESS )
	{
		return EXIT_SUCCESS;
	}
	else
	{
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
void KK_memset_int(void *block, int value, size_t num)
{
	size_t i;

	for(i = 0; i < num; i++)
		*((int *)block + i) = value;
}


/**
 @author Miroslav Policki
 
 Initializes the system catalog

 @return EXIT_SUCCESS if the system catalog has been initialized, EXIT_ERROR otherwise

*/
int KK_init_system_catalog() {
	int relation, attribute, index, view, sequence, function, function_arguments, trigger, db, db_obj, user, group, right;
	int i;
	
	if( DEBUG )
		printf( "KK_init_system_catalog: System catalog initialization started...\n" );

	KK_header hRelation[5] = {
		{TYPE_INT, "obj_id", 0, '\0', '\0'},
		{TYPE_VARCHAR, "name", 0, '\0', '\0'},
		{TYPE_INT, "start_address", 0, '\0', '\0'},
		{TYPE_INT, "end_address", 0, '\0', '\0'},
		{0, '\0', 0, '\0', '\0'}
	};
	
	KK_header hAttribute[5] = {
		{TYPE_INT, "obj_id", 0, '\0', '\0',},
		{TYPE_VARCHAR, "name", 0, '\0', '\0',},
		{TYPE_INT, "type", 0, '\0', '\0',},
		{TYPE_INT, "table_id", 0, '\0', '\0',},
		{0, '\0', 0, '\0', '\0'}
	};

	KK_header hIndex[7] = {
		{TYPE_INT, "obj_id", 0, '\0', '\0',},
		{TYPE_VARCHAR, "name", 0, '\0', '\0',},
		{TYPE_INT, "start_address", 0, '\0', '\0',},
		{TYPE_INT, "end_address", 0, '\0', '\0'},
		{TYPE_INT, "table_id", 0, '\0', '\0',},
		{TYPE_INT, "attribute_id", 0, '\0', '\0',},
		{0, '\0', 0, '\0', '\0'}
	};

	KK_header hView[4] = {
		{TYPE_INT, "obj_id", 0, '\0', '\0',},
		{TYPE_VARCHAR, "name", 0, '\0', '\0',},
		{TYPE_VARCHAR, "query", 0, '\0', '\0',},
		{0, '\0', 0, '\0', '\0'}
	};

	KK_header hSequence[8] = {
		{TYPE_INT, "obj_id", 0, '\0', '\0',},
		{TYPE_VARCHAR, "name", 0, '\0', '\0',},
		{TYPE_INT, "current_value", 0, '\0', '\0',},
		{TYPE_INT, "increment", 0, '\0', '\0',},
		{TYPE_INT, "max", 0, '\0', '\0',},
		{TYPE_INT, "min", 0, '\0', '\0',},
		{TYPE_BOOL, "cycle", 0, '\0', '\0',},
		{0, '\0', 0, '\0', '\0'}
	};

	KK_header hFunction[5] = {
		{TYPE_INT, "obj_id", 0, '\0', '\0',},
		{TYPE_VARCHAR, "name", 0, '\0', '\0',},
		{TYPE_INT, "arg_num", 0, '\0', '\0',},
		{TYPE_INT, "return_type", 0, '\0', '\0',},
		{0, '\0', 0, '\0', '\0'}
	};

	KK_header hFunction_arguments[4] = {
		{TYPE_INT, "func_id", 0, '\0', '\0',},
		{TYPE_INT, "att_num", 0, '\0', '\0',},
		{TYPE_INT, "att_type", 0, '\0', '\0',},
		{0, '\0', 0, '\0', '\0'}
	};

	KK_header hTrigger[6] = {
		{TYPE_INT, "obj_id", 0, '\0', '\0',},
		{TYPE_VARCHAR, "name", 0, '\0', '\0',},
		{TYPE_VARCHAR, "event", 0, '\0', '\0',},
		{TYPE_VARCHAR, "condition", 0, '\0', '\0',},
		{TYPE_INT, "action", 0, '\0', '\0',},
		{0, '\0', 0, '\0', '\0'}
	};

	KK_header hDb[3] = {
		{TYPE_INT, "obj_id", 0, '\0', '\0',},
		{TYPE_VARCHAR, "name", 0, '\0', '\0',},
		{0, '\0', 0, '\0', '\0'}
	};

	KK_header hDb_obj[3] = {
		{TYPE_INT, "db_id", 0, '\0', '\0',},
		{TYPE_INT, "artifact_id", 0, '\0', '\0',},
		{0, '\0', 0, '\0', '\0'}
	};


	KK_header hUser[4] = {
		{TYPE_INT, "obj_id", 0, '\0', '\0',},
		{TYPE_VARCHAR, "username", 0, '\0', '\0',},
		{TYPE_INT, "password", 0, '\0', '\0',},
		{0, '\0', 0, '\0', '\0'}
	};

	KK_header hGroup[3] = {
		{TYPE_INT, "obj_id", 0, '\0', '\0',},
		{TYPE_VARCHAR, "name", 0, '\0', '\0',},
		{0, '\0', 0, '\0', '\0'}
	};

	KK_header hRight[6] = {
		{TYPE_INT, "obj_id", 0, '\0', '\0',},
		{TYPE_VARCHAR, "name", 0, '\0', '\0',},
		{TYPE_INT, "group_id", 0, '\0', '\0',},
		{TYPE_INT, "artifact_id", 0, '\0', '\0',},
		{TYPE_INT, "right_type", 0, '\0', '\0',},
		{0, '\0', 0, '\0', '\0'}
	};

	for( i = 0; i < 4; i++) {
		KK_memset_int(hRelation[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hRelation[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hRelation[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for( i = 0; i < 4; i++) {
		KK_memset_int(hAttribute[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hAttribute[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hAttribute[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for( i = 0; i < 6; i++) {
		KK_memset_int(hIndex[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hIndex[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hIndex[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for( i = 0; i < 3; i++) {
		KK_memset_int(hView[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hView[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hView[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for( i = 0; i < 7; i++) {
		KK_memset_int(hSequence[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hSequence[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hSequence[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for( i = 0; i < 4; i++) {
		KK_memset_int(hFunction[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hFunction[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hFunction[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for( i = 0; i < 3; i++) {
		KK_memset_int(hFunction_arguments[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hFunction_arguments[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hFunction_arguments[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for( i = 0; i < 5; i++) {
		KK_memset_int(hTrigger[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hTrigger[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hTrigger[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for( i = 0; i < 2; i++) {
		KK_memset_int(hDb[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hDb[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hDb[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for( i = 0; i < 2; i++) {
		KK_memset_int(hDb_obj[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hDb_obj[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hDb_obj[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for( i = 0; i < 3; i++) {
		KK_memset_int(hUser[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hUser[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hUser[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for( i = 0; i < 2; i++) {
		KK_memset_int(hGroup[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hGroup[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hGroup[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	for( i = 0; i < 5; i++) {
		KK_memset_int(hRight[i].integrity, FREE_INT, MAX_CONSTRAINTS);
		memset(hRight[i].constr_name, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_NAME);
		memset(hRight[i].constr_code, FREE_CHAR, MAX_CONSTRAINTS * MAX_CONSTR_CODE);
	}

	if( DEBUG )
		printf( "KK_init_system_catalog: Creating new segments...\n" );

	relation = KK_new_segment("KK_relation", SEGMENT_TYPE_SYSTEM_TABLE, hRelation);
	attribute = KK_new_segment("KK_attribute", SEGMENT_TYPE_SYSTEM_TABLE, hAttribute);
	index = KK_new_segment("KK_index", SEGMENT_TYPE_SYSTEM_TABLE, hIndex);
	view = KK_new_segment("KK_view", SEGMENT_TYPE_SYSTEM_TABLE, hView);
	sequence = KK_new_segment("KK_sequence", SEGMENT_TYPE_SYSTEM_TABLE, hSequence);
	function = KK_new_segment("KK_function", SEGMENT_TYPE_SYSTEM_TABLE, hFunction);
	function_arguments = KK_new_segment("KK_function_arguments", SEGMENT_TYPE_SYSTEM_TABLE, hFunction_arguments);
	trigger = KK_new_segment("KK_trigger", SEGMENT_TYPE_SYSTEM_TABLE, hTrigger);
	db = KK_new_segment("KK_db", SEGMENT_TYPE_SYSTEM_TABLE, hDb);
	db_obj = KK_new_segment("KK_db_obj", SEGMENT_TYPE_SYSTEM_TABLE, hDb_obj);
	user = KK_new_segment("KK_user", SEGMENT_TYPE_SYSTEM_TABLE, hUser);
	group = KK_new_segment("KK_group", SEGMENT_TYPE_SYSTEM_TABLE, hGroup);
	right = KK_new_segment("KK_right", SEGMENT_TYPE_SYSTEM_TABLE, hRight);

	if( DEBUG )
		printf( "KK_init_system_catalog: Segments created!\n" );

	if(EXIT_SUCCESS == KK_init_system_tables_catalog(relation, attribute, index, view, sequence, function, function_arguments, trigger, db, db_obj, user, group, right))
	{
		printf( "KK_init_system_catalog: System catalog initialized!\n" );
		return EXIT_SUCCESS;
	}
	else {
	    printf("KK_init_system_catalog: ERROR. KK_init_system_tables_catalog(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d) failed.\n", relation, attribute, index, view, sequence, function, function_arguments, trigger, db, db_obj, user, group, right);
		return EXIT_ERROR;
	}
}

/** 
 @author Markus Schatten
 
 Deletes a block by a given block address (resets the header and data)

 @param address - address of the block to be deleted

 @return returns EXIT_SUCCESS if deletion successful, else EXIT_ERROR
 */
int KK_delete_block( int address )
{
	register int i, j, k;
	
	KK_header head[ MAX_ATTRIBUTES ];
	KK_tuple_dict tuple_dict[ DATA_BLOCK_SIZE ];
	unsigned char data[ DATA_BLOCK_SIZE * DATA_ENTRY_SIZE ];
	
	for( i = 0; i < MAX_ATTRIBUTES; i++ )
	{
		head[ i ].type = FREE_INT;
		for( j = 0; j < MAX_ATT_NAME; j++ )
		{
			head[ i ].att_name[ j ] = FREE_CHAR;
		}
		for( j = 0; j < MAX_CONSTRAINTS; j++ )
		{
			head[ i ].integrity[ j ] = FREE_INT;
			for( k = 0; k < MAX_CONSTR_NAME; k++ )
			{
				head[ i ].constr_name[ j ][ k ] = FREE_CHAR;
			}
			for( k = 0; k < MAX_CONSTR_CODE; k++ )
			{
				head[ i ].constr_code[ j ][ k ] = FREE_CHAR;
			}
		}
	}

	for( i = 0; i < DATA_BLOCK_SIZE; i++ )
	{
		tuple_dict[ i ].type = FREE_INT;
		tuple_dict[ i ].address = FREE_INT;
		tuple_dict[ i ].size = FREE_INT;		
	}
	for( i = 0; i < DATA_BLOCK_SIZE * DATA_ENTRY_SIZE; i++ )
	{
		data[ i ] = FREE_CHAR;
	}
	
	KK_block * block = KK_read_block ( address );
	block->type = BLOCK_TYPE_FREE;
	block->chained_with = NOT_CHAINED;
	block->free_space = DATA_BLOCK_SIZE * DATA_ENTRY_SIZE * sizeof( int );
	memcpy( block->header, head, sizeof( *head ) );
	memcpy( block->tuple_dict, tuple_dict, sizeof( *tuple_dict ) );
	memcpy( block->data, data, sizeof( *data ) );	
	
	if( KK_write_block ( block ) == EXIT_SUCCESS )
	{
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
int KK_delete_extent(int begin, int end)
{
	int address;
	for(address = begin; address < end + 1; address++ )
    {
        if (KK_delete_block(address) == NULL)
        {
            return EXIT_ERROR;
        }
    }
	return ( EXIT_SUCCESS );
}

/**
 @author Markus Schatten

*/
int KK_init_disk_manager()
{
	int size_in_mb = DB_FILE_SIZE;
	float size = 1024 * 1024 * size_in_mb / sizeof( KK_block );
	
	printf( "KK_init_disk_manager: Initializing disk manager...\n\n" );	
	if( DEBUG )
	{
		printf( "KK_init_disk_manager: Block size is: %d\n", sizeof( KK_block ) );
		printf( "KK_init_disk_manager: We need %d blocks for %d MiB\n", ( int ) size, size_in_mb );
	}
	
	if( KK_init_db_file( ( int ) size ) == EXIT_SUCCESS )
	{
		if( KK_init_system_catalog() == EXIT_SUCCESS )
		{
			printf( "KK_init_disk_manager: Disk manager initialized!\n\n" );
			return EXIT_SUCCESS;
		}
		printf( "KK_init_disk_manager: ERROR. System catalog initialization failed!\n" );
		return( EXIT_ERROR );
	}
	printf( "KK_init_disk_manager: ERROR. DB file initialization failed!" );
	return( EXIT_ERROR );
}








/**
function for geting addresses of some table 
@return structure table_addresses witch contains start and end adress of table extents, 
when form and to are 0 you are on the end of addresses
@param table - table that you search
*/
table_addresses * get_table_addresses ( char * table)
{
	//promjentiti temp_block u block i odati ispred catalog_block->
	//KK_mem_block *catalog_block = (KK_mem_block *) malloc(sizeof(KK_mem_block));
	//printf("\n GET TEABLE ADDTESES SPACE TUUU0");
	KK_block *temp_block = (KK_block *) malloc(sizeof(KK_block));
	//printf("te %s", table);
	//catalog_block = KK_get_block( 0 );
	//printf("\n GET TEABLE ADDTESES SPACE TUUU1");
	temp_block = KK_read_block(0);
	int trazi=1;
	int i=0;
	int data_adr=0;
	int data_size=0;
	int data_type=0;
	char name_sys[100];
	int address_sys;
	int free=0;
	
	printf("\nTražim systemsku tablicu relacija \n");
	for(i;i<DATA_BLOCK_SIZE;)
	{
		
		//trazi=0;
		free=0;
		for(free;free<100;free++)
			name_sys[free]='\0';
		data_adr=temp_block->tuple_dict[i].address;
		//printf("\n adr: %d",data_adr);
		data_size=temp_block->tuple_dict[i].size;
		data_type=temp_block->tuple_dict[i].type;
		memcpy(name_sys,temp_block->data+data_adr,data_size);
		printf("\n adr: %s",name_sys);
		i++;
		data_adr=temp_block->tuple_dict[i].address;
		data_size=temp_block->tuple_dict[i].size;
		data_type=temp_block->tuple_dict[i].type;
		memcpy(&address_sys,temp_block->data+data_adr,data_size);
		if(strcmp(name_sys,"KK_relation")==0)
		{	
			printf("\npronasao adresu relacijeske sys tablice: %d \n",address_sys);
			trazi=0;
			break;
		}
		i++;
		
		printf("\naaaaa");
	}
	
	//trazi=1;
	i=0;
	//catalog_block = KK_get_block( address_sys );
	temp_block=KK_read_block(address_sys);
	table_addresses * addresses = (table_addresses *) malloc(sizeof(table_addresses));
	free=0;	
	for(free;free<200;free++)
	{	
		addresses->address_from[free] = 0;
		addresses->address_to[free] = 0;
	}
	//return addresses;
	char name[200];
	int address_from;
	int address_to;
	int j=0;
	for(i;i<DATA_BLOCK_SIZE;)
	{
		free=0;
		//free the variable name
		for(free;free<200;free++)
			name[free]='\0';

		i++;
		//printf("\ntu sam\n");
		data_adr=temp_block->tuple_dict[i].address;
		data_size=temp_block->tuple_dict[i].size;
		data_type=temp_block->tuple_dict[i].type;
		memcpy(name,temp_block->data+data_adr,data_size);
		//printf("table: %s, name: %s, brojac: %d, addres: %d, size: %d",table,name,i,data_adr,data_size);
		i++;
		data_adr=temp_block->tuple_dict[i].address;
		data_size=temp_block->tuple_dict[i].size;
		data_type=temp_block->tuple_dict[i].type;
		memcpy(&address_from,temp_block->data+data_adr,data_size);
		i++;
		data_adr=temp_block->tuple_dict[i].address;
		data_size=temp_block->tuple_dict[i].size;
		data_type=temp_block->tuple_dict[i].type;
		memcpy(&address_to,temp_block->data+data_adr,data_size);
		i++;
		//printf("\n table: %s, name: %s,",table, name);
		if(strcmp(name,table)==0) //možda neka funkcija tu ide a ne običan =
		{	
			addresses->address_from[j]= address_from; //možda i neka funkcija
			addresses->address_to[j]= address_to;
			j++;
			printf("\npronasao adrese tražene tablice: %d , %d \n",address_from, address_to);	
		}
		if(strcmp(name,"")==0)
		{
			//printf("nema ničeg");
			trazi=0;
		}
		
	}
	//free( temp_block);
	//printf("aaaaaaaaaaaaaaaaa");
	//free( temp_block ); 
	return addresses;
}


int find_free_space ( table_addresses * addresses )
{
	//printf("\n FIND FREE SPACE TUUU1");
	//KK_mem_block *mem_block = (KK_mem_block *) malloc(sizeof(KK_mem_block));
	KK_block *temp_block = (KK_block *) malloc(sizeof(KK_block));
	//printf("\n FIND FREE SPACE TUUU1");
	int from=0,to=0,j=0,i=0;
	//return 100;
	printf("\n Searching for block that has free space<500 \n");
	for (j=0;j<200;j++)
	{
		if(addresses->address_from!=0)
		{
			from=addresses->address_from[j];
			to=addresses->address_to[j];
			for(i=from;i<=to;i++)
			{
				temp_block = KK_read_block( i );
				int free_space_on=temp_block->free_space;
				printf("FRRE SPACE %d",temp_block->free_space);
				if(free_space_on < 2000)
				{
				//	printf("TU SAM");
					return i; 
				}
			}
		}
		else break;
	}	
	
	int adr = 122; 
	//KK_initialize_new_ekstent(i); //funkcija ne postoji koja znači inicjalizira novi prostor ekstenta, spozapiše sve što treba i vrati početnu adresu ekstenta u koji ja mogu dalje zapisivati tablicu;
	//i je adresa bloka koji sadrži tu tablicu pa da se može samo kopirati heder
	free( temp_block );
	return adr;
}