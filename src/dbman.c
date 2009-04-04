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
//#include "memoman.h"

/**
Initializes a new database file named DB_FILE

@param size size of new file in in blocks
@return EXIT_SUCCESS if the file has been written to disk, EXIT_ERROR otherwise

*/
int KK_init_db_file( int size )
{
	register int i, j, k;
	
	KK_header head[ MAX_ATTRIBUTES ];
	KK_tuple_dict tuple_dict[ DATA_BLOCK_SIZE ];
	int data[ DATA_BLOCK_SIZE ][ DATA_ENTRY_SIZE ];
	KK_block * block = ( KK_block * ) malloc ( sizeof( KK_block ) );
	
	db_file_size = size;
		
	if( ( db = fopen( DB_FILE, "wb" ) ) == NULL ) {
		printf( "KK_init_db_file: ERROR. Cannot open db file %s.\n", DB_FILE );
		exit( EXIT_ERROR );
	}
	

	printf( "Initializing DB file..."
		"\nPlease be patient, this can take several minutes depending "
		"\non disk performance.\n" );
	

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
		for( j = 0; j < DATA_ENTRY_SIZE; j++ )
		{
			data[ i ][ j ] = FREE_INT;
		}
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
			exit(EXIT_ERROR);
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
			
			if( nAlocated_blocks !=0 && ( req_free_space == nAlocated_blocks ) ) /// if requested space for extent is OK
			{
				break;	/// exit loop
			}
		}
	}
	int success = 0;    /// var for chack of number of blocks written
	for( j = first_addr_of_extent; j < ( first_addr_of_extent + nAlocated_blocks ); j++)
	{
		block = KK_read_block( j );   	/// read block
		memcpy( block->header, header, sizeof( *header ) ); /// copy header information	
		block->type = BLOCK_TYPE_NORMAL; /// set the block type
		
		if( KK_write_block( block ) == EXIT_SUCCESS ) /// if write of block succeded increase var success, else nothing
		{
			success++;
		}
		
		free(block);  /// free the block
	}
	
	if( success != nAlocated_blocks ) /// if some blocks are not succesfully alocated, whitch means the extend alocation has FAILED
	{
		printf( "KK_new_extent: ERROR. Cannot allocate extend %d\n", first_addr_of_extent );
		exit( EXIT_ERROR );
	}
	
	return(first_addr_of_extent);

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
	int segment_start_addr = 1; //start adress for segment because we can not allocate segment in block 0
	int allocated_extents = 0; //number of allocated extents
	int i; //counter
	KK_block *block;
	int current_extent_start_addr;
	
	
	for ( i = segment_start_addr; i <= db_file_size; i++ )
	{
		// if there is max number of extent allocated, exit FOR loop
		if( allocated_extents == MAX_EXTENTS )
			break;
		
		// check if the block is free
		block = KK_read_block(i);
		
		if ( block->type == BLOCK_TYPE_FREE )
		{
			current_extent_start_addr = KK_new_extent( i, 0, type, header ); //allocate new extent
			
			// if extent is successfully allocated, increment number of allocated extents and move to
			// next block after allocated extent, else move for INITIAL_EXTENT_SIZE blocks, so in that way get
			// either first block of new extent or some block in this extent which will not be free
			if ( current_extent_start_addr != 0 )
			{
				allocated_extents++;
				i += INITIAL_EXTENT_SIZE - 1;
			}
			else
			{
				i += INITIAL_EXTENT_SIZE - 1;
			}
		}
	}
	
	if ( allocated_extents < MAX_EXTENTS )
	{
		printf("KK_new_extent: WARNING! Segment is allocated with less than %d extents.\nIt contains %d extents.\n", MAX_EXTENTS, allocated_extents);
		exit(EXIT_ERROR);
	}
	
	return (EXIT_SUCCESS);
}


//MN function for creating header
KK_header * KK_create_header(char * name, int type, int integrity, char * constr_name, char * contr_code ) 
{
	KK_header * catalog_header = ( KK_header * ) malloc ( sizeof( KK_header ) );
	printf("\nHeder: %s, %d",name, strlen(name));
	catalog_header->type = type;
	memcpy(catalog_header->att_name,name, strlen(name));
	int i=0;
	int j=0;
	int k=0;
	for (i=0;i<MAX_CONSTRAINTS;i++)
	{
		catalog_header->integrity[ i ] = integrity;
		for (j=0;j<MAX_CONSTR_NAME;j++)
		{			
			catalog_header->constr_name[ i ][ j ]= constr_name;
		}
		for (k=0;k<MAX_CONSTR_CODE;k++)
		{
			catalog_header->constr_code[ i ][ k ]=contr_code;
		}
	}
	
	return ( catalog_header );
}

//MN funciton for inserting entry in tuple_dict and data
void KK_insert_entry(KK_block * block_address, int type, char * entry_data, int i )
{
	KK_tuple_dict * catalog_tuple_dict = (KK_tuple_dict *) malloc (sizeof( KK_tuple_dict ));
	
	printf("\nTU: %s Size of: %d", entry_data, strlen(entry_data));
	
	memcpy(block_address->data+block_address->free_space, entry_data, strlen( entry_data));
	
	catalog_tuple_dict->address=block_address->free_space;
	
	block_address->free_space+=1;
	//no need for "+strlen(entry_data)" while "+1" is like "new line" 
	
	catalog_tuple_dict->type=type;
	catalog_tuple_dict->size = strlen(entry_data);
	
	memcpy(& block_address->tuple_dict[i],catalog_tuple_dict, sizeof(* catalog_tuple_dict));
}

//MN function initialises the sytem table katalog
int KK_init_system_tables_catalog( int relation, int attribute, int index, int view, int sequence, int function, int function_arguments, 
								  int trigger, int db, int db_obj, int user, int group, int right)
{	
	printf("tu sam");
	KK_block * catalog_block = ( KK_block * ) malloc ( sizeof( KK_block ) );
	//first header attribute of catalog_block
	KK_header * catalog_header_name  =  ( KK_header * ) malloc ( sizeof( KK_header ) );
	catalog_header_name = KK_create_header( "Name", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	//second attribute of catalog_block
	KK_header * catalog_header_address = ( KK_header * ) malloc ( sizeof( KK_header ) );
	catalog_header_address = KK_create_header( "Address", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	
	//initialize other elements of block
	catalog_block->address = 0;
	catalog_block->type = BLOCK_TYPE_NORMAL;
	catalog_block->chained_with = NOT_CHAINED;
	catalog_block->free_space = 0; //using as an adrees for the last free_space of data

	
	//merge catalog_heder with heders created before
	memcpy( & catalog_block->header[0], catalog_header_name, sizeof( * catalog_header_name ) );
	memcpy( & catalog_block->header[1], catalog_header_address, sizeof( * catalog_header_address ) );
	KK_tuple_dict tuple_dict[13];
	memcpy(catalog_block->tuple_dict, tuple_dict, sizeof( *tuple_dict ) );
	
	char buf[5];
	//insert data and tuple_dict in block
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_relation", 0 );
	//convert int to char
	sprintf(buf,"%d",relation);
	KK_insert_entry(catalog_block, TYPE_INT, buf, 1 );
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_attribute", 2 );
	sprintf(buf,"%d",attribute);
	KK_insert_entry(catalog_block, TYPE_INT, buf, 3 );
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_index", 4 );
	sprintf(buf,"%d",index);
	KK_insert_entry(catalog_block, TYPE_INT, buf, 5 );
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_view", 6 );
	sprintf(buf,"%d",view);
	KK_insert_entry(catalog_block, TYPE_INT, buf, 7 );
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_sequence", 8 );
	sprintf(buf,"%d",sequence);
	KK_insert_entry(catalog_block, TYPE_INT, buf, 9 );
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_function",10 );
	sprintf(buf,"%d",function);
	KK_insert_entry(catalog_block, TYPE_INT, buf, 11 );
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_function_arguments", 12 );
	sprintf(buf,"%d",function_arguments);
	KK_insert_entry(catalog_block, TYPE_INT, buf, 13 );
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_trigger", 14 );
	sprintf(buf,"%d",trigger);
	KK_insert_entry(catalog_block, TYPE_INT, buf, 15 );
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_db", 16 );
	sprintf(buf,"%d",db);
	KK_insert_entry(catalog_block, TYPE_INT, buf, 17 );
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_db_obj", 18 );
	sprintf(buf,"%d",db_obj);
	KK_insert_entry(catalog_block, TYPE_INT, buf, 19 );
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_user", 20 );
	sprintf(buf,"%d",user);
	KK_insert_entry(catalog_block, TYPE_INT, buf, 21 );
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_group", 22 );
	sprintf(buf,"%d",group);
	KK_insert_entry(catalog_block, TYPE_INT, buf, 23 );
	KK_insert_entry(catalog_block, TYPE_VARCHAR, "KK_right", 24 );
	sprintf(buf,"%d",right);
	KK_insert_entry(catalog_block, TYPE_INT, buf, 25 );
		
	//call function for writing the block on the first place in the file (ie. first block is on position zero)
	if (KK_write_block(catalog_block))
	{
		return EXIT_SUCCESS;
	}
	else
	{
		return EXIT_ERROR;
	}
}



