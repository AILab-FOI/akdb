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


