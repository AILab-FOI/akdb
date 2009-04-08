/**
@file memoman.c Defines functions for the memory manager of
 Kalashnikov DB
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

#include "memoman.h"

/**
Initializes the global cache memory (variable db_cache)

@return EXIT_SUCCESS if the cache memory has been initialized, EXIT_ERROR otherwise
*/
int KK_cache_malloc()
{
	return EXIT_SUCCESS;
}

/**
Initializes the global redo log memory (variable redo_log)

@return EXIT_SUCCESS if the redo log memory has been initialized, EXIT_ERROR otherwise
*/
int KK_redo_log_malloc()
{
	return EXIT_SUCCESS;
}

/**
Initializes the global query memory (variable query_mem)

@return EXIT_SUCCESS if the query memory has been initialized, EXIT_ERROR otherwise
*/
int KK_query_mem_malloc()
{
	printf("Start query_mem_malloc");
	//allocate memory for global variable query_mem
	if( (query_mem = ( KK_query_mem * ) malloc ( sizeof( KK_query_mem) ) ) == NULL )
	{
		printf( " KK_query_mem: ERROR. Cannot allocate memory \n");
		exit( EXIT_ERROR );
	}

	//allocate memory for variable query_mem_lib which is used in query_mem->parsed
	KK_query_mem_lib * query_mem_lib;
	if(( query_mem_lib = (KK_query_mem_lib *) malloc(sizeof(KK_query_mem_lib)) ) == NULL )
	{
		printf( " KK_query_mem_lib: ERROR. Cannot allocate memory \n");
		exit( EXIT_ERROR );
	}

	//allocate memory for variable query_mem_dict which is used in query_mem->dictionary
	KK_query_mem_dict * query_mem_dict;
	if( (query_mem_dict = ( KK_query_mem_dict *) malloc(sizeof(KK_query_mem_dict)) ) == NULL )
	{
		printf( "  KK_query_mem_dict: ERROR. Cannot allocate memory \n");
		exit( EXIT_ERROR );
	}

	//allocate memory for variable query_mem_result which is used in query_mem->result
	KK_query_mem_result * query_mem_result;
	if(( query_mem_result = (KK_query_mem_result *) malloc(sizeof(KK_query_mem_result)) ) == NULL )
	{
		printf( "  KK_query_mem_result: ERROR. Cannot allocate memory \n");
		exit( EXIT_ERROR );
	}

	//allocate memory for variable tuple_dict which is used in query_mem->dictionary->dictionary[]
	KK_tuple_dict * tuple_dict = (KK_tuple_dict *) malloc(sizeof(KK_tuple_dict));
	if(( tuple_dict = (KK_tuple_dict *) malloc(sizeof(KK_tuple_dict)) ) == NULL )
	{
		printf( "  KK_tuple_dict: ERROR. Cannot allocate memory \n");
		exit( EXIT_ERROR );
	}

	memcpy(query_mem_dict->dictionary,tuple_dict,sizeof(* tuple_dict));

	query_mem->parsed = query_mem_lib;
	query_mem->dictionary = query_mem_dict;
	query_mem->result = query_mem_result;

/*	wrong way becouse we don't have data only adress which must be written in query_mem variables
	memcpy(query_mem->parsed, query_mem_lib, sizeof(* query_mem_lib));
	memcpy(query_mem->dictionary,query_mem_dict,sizeof(* query_mem_dict));
	memcpy(query_mem->result,query_mem_result,sizeof(* query_mem_result));*/

	printf("END; Success query_mem_malloc");
	return EXIT_SUCCESS;
}

/**
Initializes memory manager (cache, redo log and query memory)

@return EXIT_SUCCESS if the query memory manager has been initialized, EXIT_ERROR otherwise
*/
int KK_memoman_init()
{
    if(KK_cache_malloc() == EXIT_ERROR) {
        printf( "KK_memoman_init: ERROR. KK_cache_malloc() failed.\n");
        return EXIT_ERROR;
    }

    if(KK_redo_log_malloc() == EXIT_ERROR) {
        printf( "KK_memoman_init: ERROR. KK_redo_log_malloc() failed.\n");
        return EXIT_ERROR;
    }

    if(KK_query_mem_malloc() == EXIT_ERROR) {
        printf( "KK_memoman_init: ERROR. KK_query_mem_malloc() failed.\n");
        return EXIT_ERROR;
    }

	return EXIT_SUCCESS;
}

/**
Caches block into memory.

@param num block number (address)
@return EXIT_SUCCESS if the block has been successfully read into memory, EXIT_ERROR otherwise
*/
int KK_cache_block( int num )
{
	KK_mem_block * mem_block;
    KK_block * block_cache;

	//alocation of KK_mem_block
	if( ( mem_block = ( KK_mem_block * ) malloc ( sizeof( KK_mem_block ) ) ) == NULL )
	{
		printf( " KK_mem_block: ERROR. Cannot allocate memory \n");
		return (EXIT_ERROR);
	}
	//alocation of KK_block
	if( ( block_cache = ( KK_block * ) malloc ( sizeof( KK_block ) ) ) == NULL )
	{
		printf( " KK_block: ERROR. Cannot allocate memory \n");
		return (EXIT_ERROR);
	}

	//read the block from the given address
    block_cache = KK_read_block( num );

	memcpy( &mem_block->block, block_cache, sizeof( *block_cache ) ); // copy pointer to given block
	mem_block->dirty = BLOCK_CLEAN; //set dirty bit in mem_block struct

    int timestamp = clock();  //get the timestamp
    mem_block->timestamp_read = timestamp; //set timestamp_read
    mem_block->timestamp_last_change = timestamp; //set timestamp_last_change

	return (EXIT_SUCCESS); //if all is succesfull
}


/**
Reads a block from memory. If the block is cached returns the cached block. Else uses
KK_cache_block to read the block to cache and then returns it.

@param num block number (address)
@return the block given if succesfull, EXIT_ERROR otherwise
*/
KK_mem_block * KK_read_block( int num )
{
	int i = 0; //counter
	bool found_in_cache = false;
	int first_free_mem_block = -1;
	int oldest_block = &db_cache.next_replace;
	int get_second_oldest = 0; // if block will be removed from cache, then second oldest should be
								// marked in db_cache as the one that will be replaced next
	KK_mem_block *cached_block; // cached memory block
	KK_block *data_block;
	int block_writed;

	while ( i < MAX_CACHE_MEMORY )
	{
		//if block is cached returns block
		if ( &db_cache.cache[i]->block.address == num )
		{
			cached_block = (KK_mem_block*) db_cache.cache[i];
			found_in_cache = true;
		}

		//get first free memory block for possible block caching
		//checking by timestamp of last block reading
		if ( first_free_mem_block == -1 )
		{
			//assume that free block has timestamp_read set to -1
			if ( &db_cache.cache[i].timestamp_read == -1 )
				first_free_mem_block = i;
		}

		// get second oldest block index in db_cache
		if ( (&db_cache.cache[i].timestamp_read > &db_cache.cache[oldset_block].timestamp_read) &&
			(&db_cache.cache[i].timestamp_read < &db_cache.cache[get_second_oldest].timestamp_read) )
			get_second_oldest = i;

		i++;
	}

	if ( !found_in_cache )
	{
		if ( first_free_mem_block != -1 )
		{
			// assume that function KK_cahce_block returns (KK_mem_block*)
			// and ih takes two arguments (block_number, allocation_address)
			cached_block = (KK_mem_block*) KK_cache_block ( num, db_cache.cache[first_free_mem_block] );
		}
		else
		{
			if ( &db_cache.cache[oldest_block].dirty == BLOCK_DIRTY )
			{
				data_block = db_cache.cache[oldest_block]->block;
				block_writed = KK_write_block ( data_block );
				// if block form cache can not be writed to DB file -> EXIT_ERROR
				if ( block_writed != EXIT_SUCCESS )
				{
					printf("KK_write_block: ERROR! Cannot write block from cache to DB File.\n");
					exit ( EXIT_ERROR );
				}
				// if block is written to DB file, set next_replace to second oldest
				&db_cache.next_replace = get_second_oldest;
			}
			cached_block = (KK_mem_block*) KK_cache_block ( num, db_cache.cache[oldest_block] );
		}
	}

	return ( cached_block );
}
