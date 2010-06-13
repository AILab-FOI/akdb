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
 @author Markus Schatten, Matija Šestak(revised)

 Initializes the global cache memory (variable db_cache)

 @return EXIT_SUCCESS if the cache memory has been initialized, EXIT_ERROR otherwise
*/
int AK_cache_malloc()
{
	register int i;

	if( ( db_cache = ( AK_db_cache * ) malloc ( sizeof( AK_db_cache ) ) ) == NULL )
	{
		exit( EXIT_ERROR );
	}
	db_cache->next_replace = 0;
	for( i = 0; i < MAX_CACHE_MEMORY; i++ )
	{
		db_cache->cache[ i ] = ( AK_mem_block * ) malloc ( sizeof( AK_mem_block ) );
                db_cache->cache[ i ]->block = ( AK_block * ) malloc( sizeof( AK_block ));
		if( ( AK_cache_block( i, db_cache->cache[ i ] ) ) == EXIT_ERROR )
		{
			exit( EXIT_ERROR );
		}
		//printf( "Cached block %d with address %d\n", i,  &db_cache->cache[ i ]->block->address );
	}

	return EXIT_SUCCESS;
}

/**
 @author Dejan Sambolić

 Initializes the global redo log memory (variable redo_log)

 @return EXIT_SUCCESS if the redo log memory has been initialized, EXIT_ERROR otherwise
*/
int AK_redo_log_malloc()
{
	if( ( redo_log = ( AK_redo_log * ) malloc ( sizeof( AK_redo_log ) ) ) == NULL)
	{
		exit( EXIT_ERROR );
	}
	return EXIT_SUCCESS;
}

/**
 @author Matija Novak

 Initializes the global query memory (variable query_mem)

 @return EXIT_SUCCESS if the query memory has been initialized, EXIT_ERROR otherwise
*/
int AK_query_mem_malloc()
{
	if( DEBUG )
		printf("AK_query_mem_malloc: Start query_mem_malloc\n");
	/// allocate memory for global variable query_mem
	if( (query_mem = ( AK_query_mem * ) malloc ( sizeof( AK_query_mem) ) ) == NULL )
	{
		printf( "AK_query_mem_malloc: ERROR. Cannot allocate query memory \n");
		exit( EXIT_ERROR );
	}

	/// allocate memory for variable query_mem_lib which is used in query_mem->parsed
	AK_query_mem_lib * query_mem_lib;
	if(( query_mem_lib = ( AK_query_mem_lib *) malloc(sizeof(AK_query_mem_lib)) ) == NULL )
	{
		printf( "AK_query_mem_malloc: ERROR. Cannot allocate query library memory \n");
		exit( EXIT_ERROR );
	}

	/// allocate memory for variable query_mem_dict which is used in query_mem->dictionary
	AK_query_mem_dict * query_mem_dict;
	if( (query_mem_dict = ( AK_query_mem_dict *) malloc(sizeof(AK_query_mem_dict)) ) == NULL )
	{
		printf( "AK_query_mem_malloc: ERROR. Cannot allocate query dictionary memory \n");
		exit( EXIT_ERROR );
	}

	/// allocate memory for variable query_mem_result which is used in query_mem->result
	AK_query_mem_result * query_mem_result;
	if(( query_mem_result = (AK_query_mem_result *) malloc(sizeof(AK_query_mem_result)) ) == NULL )
	{
		printf( "  AK_query_mem_malloc: ERROR. Cannot allocate query result memory \n");
		exit( EXIT_ERROR );
	}

	/// allocate memory for variable tuple_dict which is used in query_mem->dictionary->dictionary[]
	AK_tuple_dict * tuple_dict = (AK_tuple_dict *) malloc(sizeof(AK_tuple_dict));
	if(( tuple_dict = (AK_tuple_dict *) malloc(sizeof(AK_tuple_dict)) ) == NULL )
	{
		printf( "  AK_query_mem_malloc: ERROR. Cannot allocate tuple dictionary memory \n");
		exit( EXIT_ERROR );
	}


	memcpy(query_mem_dict->dictionary,tuple_dict,sizeof(* tuple_dict));

	query_mem->parsed = query_mem_lib;
	query_mem->dictionary = query_mem_dict;
	query_mem->result = query_mem_result;

/*	wrong way because we don't have data only adress which must be written in query_mem variables
	memcpy(query_mem->parsed, query_mem_lib, sizeof(* query_mem_lib));
	memcpy(query_mem->dictionary,query_mem_dict,sizeof(* query_mem_dict));
	memcpy(query_mem->result,query_mem_result,sizeof(* query_mem_result));*/

	if( DEBUG )
		printf("AK_query_mem_malloc: Success!\n");
	return EXIT_SUCCESS;
}

/**
 @author Miroslav Policki

 Initializes memory manager (cache, redo log and query memory)

 @return EXIT_SUCCESS if the query memory manager has been initialized, EXIT_ERROR otherwise
*/
int AK_memoman_init()
{
	printf( "AK_memoman_init: Initializing memory manager...\n" );

	if(AK_cache_malloc() == EXIT_ERROR)
	{
		printf( "AK_memoman_init: ERROR. AK_cache_malloc() failed.\n");
		return EXIT_ERROR;
	}

	if(AK_redo_log_malloc() == EXIT_ERROR)
	{
		printf( "AK_memoman_init: ERROR. AK_redo_log_malloc() failed.\n");
		return EXIT_ERROR;
	}

	if(AK_query_mem_malloc() == EXIT_ERROR) {
		printf( "AK_memoman_init: ERROR. AK_query_mem_malloc() failed.\n");
		return EXIT_ERROR;
	}

	printf( "AK_memoman_init: Memory manager initialized...\n" );

	return ( EXIT_SUCCESS );
}

/**
 @author Nikola Bakoš, Matija Šestak(revised)

 Caches block into memory.

 @param num block number (address)
 @param mem_block address of memmory block

 @return EXIT_SUCCESS if the block has been successfully read into memory, EXIT_ERROR otherwise
*/
int AK_cache_block( int num, AK_mem_block * mem_block )
{
	AK_block * block_cache;
	unsigned long timestamp;

	/// read the block from the given address
	block_cache = (AK_block *) AK_read_block( num );

        memcpy( mem_block->block, block_cache, sizeof( AK_block ));
	mem_block->dirty = BLOCK_CLEAN; /// set dirty bit in mem_block struct

	timestamp = clock();  /// get the timestamp
	mem_block->timestamp_read = timestamp; /// set timestamp_read
	mem_block->timestamp_last_change = timestamp; /// set timestamp_last_change

	free( block_cache );

	return (EXIT_SUCCESS); /// if all is succesfull
}


/**
 @author Tomislav Fotak, updated by Matija Šestak

 Reads a block from memory. If the block is cached returns the cached block. Else uses
 AK_cache_block to read the block to cache and then returns it.

 @param num block number (address)

 @return segment start address
*/
AK_mem_block * AK_get_block( int num )
{
    int i = 0;
    int pos;
    int oldest_block = db_cache->next_replace;
    int second_oldest = 0;
    int found_in_cache = 0;
    int first_free_mem_block = -1;
    int block_written = 0;

    AK_mem_block *cached_block;
    AK_block *data_block;

    while( i < MAX_CACHE_MEMORY ){
        if( db_cache->cache[i]->timestamp_read == -1 ){
            first_free_mem_block = i;
            break;
        }
        if( db_cache->cache[i]->block->address == num ){
            found_in_cache = 1;
            pos = i;
            cached_block = db_cache->cache[i];
            break;
        }
        i++;
    }
    if( !found_in_cache ){
        if( first_free_mem_block != -1 ){
            if ( AK_cache_block (num, db_cache->cache[ first_free_mem_block ] ) == EXIT_SUCCESS )
                cached_block = db_cache->cache[first_free_mem_block];
        }else{
                if ( db_cache->cache[oldest_block]->dirty == BLOCK_DIRTY ){
                        data_block = (AK_block *) db_cache->cache[oldest_block]->block;
                        block_written = AK_write_block ( data_block );
                        /// if block form cache can not be writed to DB file -> EXIT_ERROR
                        if ( block_written != EXIT_SUCCESS ){
                                exit ( EXIT_ERROR );
                        }
                }
                if ( AK_cache_block (num, db_cache->cache[ oldest_block ] ) == EXIT_SUCCESS )
                    cached_block = db_cache->cache[ oldest_block ];
            }
    }
    else{
        if ( db_cache->cache[pos]->dirty == BLOCK_DIRTY ){
            data_block = (AK_block *) db_cache->cache[pos]->block;
            block_written = AK_write_block ( data_block );
            /// if block form cache can not be writed to DB file -> EXIT_ERROR
            if ( block_written != EXIT_SUCCESS ){
                    exit ( EXIT_ERROR );
            }
        }
   }

    int min = 0;
    i = 1;
    while( i < MAX_CACHE_MEMORY ){
        if( db_cache->cache[i]->timestamp_read != -1 ){
            if( db_cache->cache[i]->timestamp_read < db_cache->cache[ min ]->timestamp_read )
                min = i;
        }
        i++;
    }
    db_cache->next_replace = min;
    return cached_block;
}

/**
 * @author Matija Šestak.
 * @brief  Function re-read all the blocks from disk
 * @result int-EXIT_SUCCESS
 */
int AK_refresh_cache(){
    int i;
    AK_mem_block *mem_block;
    for( i = 0; i < MAX_CACHE_MEMORY; i++ ){
        mem_block = db_cache->cache[i];
        AK_block *temp_block;
        temp_block = (AK_block*) AK_read_block( mem_block->block->address);
        memcpy( mem_block->block, temp_block, sizeof( AK_block));
    }
    return EXIT_SUCCESS;
}

/** 	@author Matija Novak, updated by Matija Šestak(function now uses caching), modified and renamed by Mislav Čakarić
	function for geting addresses of some table
	@return structure table_addresses witch contains start and end adresses of table extents,
	when form and to are 0 you are on the end of addresses
	@param table - table name that you search for
*/
table_addresses * get_segment_addresses ( char * segmentName, int segmentType )
{
	AK_mem_block *mem_block;

	mem_block = (AK_mem_block *) AK_get_block(0);

	int i=0;
	int data_adr=0;
	int data_size=0;
	int data_type=0;
	char name_sys[MAX_ATT_NAME];
        char *sys_table;
	int address_sys;
	int free2=0;//var to clear char variable

        switch(segmentType){
            case SEGMENT_TYPE_TABLE:
                sys_table="AK_relation";
                if(DEBUG)
                    printf("get_segment_addresses: Serching for AK_relation table \n");
                break;
            case SEGMENT_TYPE_INDEX:
                sys_table="AK_index";
                if(DEBUG)
                    printf("get_segment_addresses: Serching for AK_index table \n");
                break;
        }


	for(i=0;i<DATA_BLOCK_SIZE;i++)
	{//going throught headers

		free2=0;
		for(free2=0;free2<MAX_ATT_NAME;free2++)
			name_sys[free2]='\0';

		if(mem_block->block->tuple_dict[i].address == FREE_INT)
			break;
		data_adr=mem_block->block->tuple_dict[i].address;
		data_size=mem_block->block->tuple_dict[i].size;
		data_type=mem_block->block->tuple_dict[i].type;
		memcpy(name_sys,mem_block->block->data+data_adr,data_size);

		i++;
		data_adr=mem_block->block->tuple_dict[i].address;
		data_size=mem_block->block->tuple_dict[i].size;
		data_type=mem_block->block->tuple_dict[i].type;
		memcpy(&address_sys,mem_block->block->data+data_adr,data_size);

		if(strcmp(name_sys,sys_table)==0)
		{
			if(DEBUG)
				printf("get_segment_addresses: Found the address of the %s table: %d \n",sys_table,address_sys);
			break;
		}
		i++;
	}

	mem_block= (AK_mem_block *) AK_get_block(address_sys);
	table_addresses * addresses = (table_addresses *) malloc(sizeof(table_addresses));

	free2=0;
	for(free2=0;free2<MAX_EXTENTS_IN_SEGMENT;free2++)
	{//intialize address structure
		addresses->address_from[free2] = 0;
		addresses->address_to[free2] = 0;
	}

	char name[MAX_VARCHAR_LENGHT];
	int address_from;
	int address_to;
	int j=0;

	for(i=0;i<DATA_BLOCK_SIZE;i++)
	{
            if( mem_block->block->tuple_dict[i].type == FREE_INT )
                break;
            i++;
            memcpy( name, &(mem_block->block->data[mem_block->block->tuple_dict[i].address]), mem_block->block->tuple_dict[i].size );
            name[ mem_block->block->tuple_dict[i].size] = '\0';

            i++;
            memcpy( &address_from, &(mem_block->block->data[mem_block->block->tuple_dict[i].address]),mem_block->block->tuple_dict[i].size);

            i++;
            memcpy( &address_to, &(mem_block->block->data[mem_block->block->tuple_dict[i].address]),mem_block->block->tuple_dict[i].size);

		if(strcmp(name,segmentName)==0)
		{//if found the table that addresses we need
			addresses->address_from[j]= address_from;
			addresses->address_to[j]= address_to;
			j++;
			if(DEBUG)
				printf("get_segment_addresses(%s): Found addresses of searching segment: %d , %d \n", name, address_from, address_to);
		}
            if(segmentType==SEGMENT_TYPE_INDEX)
                i+=2;
	}
	return addresses;
}

/** 	@author Mislav Čakarić
	function for geting addresses of some table
	@return structure table_addresses witch contains start and end adresses of table extents,
	when form and to are 0 you are on the end of addresses
	@param table - table name that you search for
*/
table_addresses * get_table_addresses ( char *table){
    return get_segment_addresses ( table, SEGMENT_TYPE_TABLE );
}

/** 	@author Mislav Čakarić
	function for geting addresses of some index
	@return structure table_addresses witch contains start and end adresses of table extents,
	when form and to are 0 you are on the end of addresses
	@param index - index name that you search for
*/
table_addresses * get_index_addresses ( char * index){
    return get_segment_addresses ( index, SEGMENT_TYPE_INDEX );
}

/** 	@author Matija Novak, updated by Matija Šestak( function now uses caching)
	function to find free space in some block betwen block addresses
	function made for insert_row()
	@param address - addresses of extents
	@returns int - address of the block to write in
*/
int find_free_space ( table_addresses * addresses )
{
	AK_mem_block *mem_block;
	int from=0,to=0,j=0,i=0;

	if(DEBUG)
		printf("find_free_space: Searching for block that has free space < 500 \n");

	for (j=0;j<MAX_EXTENTS_IN_SEGMENT;j++)
	{//searching extent
		if(addresses->address_from != 0)
		{
			from=addresses->address_from[j];
			to=addresses->address_to[j];

			for(i=from;i<=to;i++)
			{//searching block
				mem_block = (AK_mem_block *) AK_get_block( i );
				int free_space_on=mem_block->block->free_space;
				if(DEBUG)
					printf("find_free_space: FREE SPACE %d\n",mem_block->block->free_space);
				if((free_space_on < MAX_FREE_SPACE_SIZE)&&
					(mem_block->block->last_tuple_dict_id<MAX_LAST_TUPLE_DICT_SIZE_TO_USE))
				{//found free block to write
					return i;
				}
			}
		}
		else break;
	}

	//I cant call function from memoman must consider another solution to place these functions
	int adr = -1;

	//need to create new extent
	return adr;
}

/**
 @author Nikola Bakoš, updated by Matija Šestak (function now uses caching)

 Extends the segment
 @param table_name name of segment to extent
 @param extent_type type of extent (can be one of:
	SEGMENT_TYPE_SYSTEM_TABLE,
	SEGMENT_TYPE_TABLE,
	SEGMENT_TYPE_INDEX,
	SEGMENT_TYPE_TRANSACTION,
	SEGMENT_TYPE_TEMP
 @return address of new extent, otherwise EXIT_ERROR

*/
int AK_init_new_extent ( char *table_name , int extent_type){
	table_addresses *adrese;
	adrese = (table_addresses *) get_segment_addresses(table_name, SEGMENT_TYPE_TABLE);
	int adr_bloka = adrese->address_from[1];
	int old_size=0;
        int block_written;
        char *sys_table;

	//promjentiti temp_block = mem_block->block
	AK_mem_block *mem_block = (AK_mem_block *) malloc(sizeof(AK_mem_block));
	mem_block = AK_get_block(adr_bloka); // bilo koji blok tablice, samo da se dobije header iz njega

	//AK_block *temp_block = (AK_block *) malloc(sizeof(AK_block));
	//		temp_block = (AK_block *) AK_read_block(adr_bloka);


	int velicina=0;
	register int i=0;
	for(i=0; i<MAX_EXTENTS_IN_SEGMENT; i++){
		if(adrese->address_from[i] == 0)		//ako smo prošli cijelo polje
			break;
		velicina = adrese->address_to[i] - adrese->address_from[i];
		if(velicina > old_size)			//trazim najveći extent
			old_size = velicina;
	}

	old_size += 1;
	int pocetna_adr = 0;
	if ( (pocetna_adr = AK_new_extent(1, old_size, extent_type, mem_block->block->header)) == EXIT_ERROR){
		printf("AK_init_new_extent: Could not alocate new extent\n");
		return EXIT_ERROR;
	}
	if(DEBUG)
		printf("AK_init_new_extent: pocetna_adr=%i, old_size=%i, extent_type=%i\n",pocetna_adr, old_size, extent_type);

	int zavrsna_adr = pocetna_adr;

	float RESIZE_FACTOR = 0;

		switch(extent_type)
		{
			case SEGMENT_TYPE_TABLE:
				RESIZE_FACTOR = EXTENT_GROWTH_TABLE;
                                sys_table="AK_relation";
				break;
			case SEGMENT_TYPE_INDEX:
				RESIZE_FACTOR = EXTENT_GROWTH_INDEX;
                                sys_table="AK_index";
				break;
			case SEGMENT_TYPE_TRANSACTION:
				RESIZE_FACTOR = EXTENT_GROWTH_TRANSACTION;
                                sys_table="AK_relation";//TO-DO
				break;
			case SEGMENT_TYPE_TEMP:
				RESIZE_FACTOR = EXTENT_GROWTH_TEMP;
                                sys_table="AK_relation";//TO-DO
				break;
		}

		zavrsna_adr = pocetna_adr + ( old_size + old_size * RESIZE_FACTOR );
		mem_block= (AK_mem_block *) AK_get_block( 0 );



	char name_sys[13];
	int address_sys;
	int free=0;
	if(DEBUG)
		printf("\nAK_init_new_extent: Searching for system table of relations \n");

	for(i=0;i<DATA_BLOCK_SIZE;)
	{

		free=0;
		for(free=0;free<100;free++)
			name_sys[free]='\0';

		memcpy(name_sys,
			   mem_block->block->data + mem_block->block->tuple_dict[i].address,
			   mem_block->block->tuple_dict[i].size );

		printf("\n adresa: %s",name_sys);
		i++;

		memcpy(&address_sys,
			   mem_block->block->data + mem_block->block->tuple_dict[i].address,
			   mem_block->block->tuple_dict[i].size);

		if(strcmp(name_sys, sys_table)==0)
		{	if(DEBUG)
				printf("\nAK_init_new_extent: Pronasao adresu relacijeske sys tablice: %d \n",address_sys);
			break;
		}
		i++;

	}
        mem_block->dirty=BLOCK_DIRTY;

	//adresa relacijske sistemske tablice je pronađena
	// zapisati u sistemski katalog relacije

		//mem_block = AK_get_block( address_sys );

		mem_block=(AK_mem_block *)AK_get_block(address_sys);						//tu zamjena

	//trazi mjesto za slijedeci unos u sis katalogu
	int id=0;
	int nadjeno=1;
	while ( nadjeno )
	{
		id++; //to je vrijednost gdje se pise
		if( mem_block->block->tuple_dict[id].size == 0 )
		{if ( block_written != EXIT_SUCCESS ){
            printf("AK_read_block: ERROR! Cannot write block from cache to DB File.\n");
            exit ( EXIT_ERROR );
    }
			nadjeno=0;
		}
	}


	//to je kaj unesem nutra
		int obj_id=2;
	if(DEBUG)
		printf("unosim: %d , %s, %i, %i", obj_id, table_name, pocetna_adr, zavrsna_adr);
	AK_insert_entry(mem_block->block, TYPE_INT, &obj_id, id );
	AK_insert_entry(mem_block->block, TYPE_VARCHAR, table_name, id + 1 );
	AK_insert_entry(mem_block->block, TYPE_INT, &pocetna_adr, id + 2 );
	AK_insert_entry(mem_block->block, TYPE_INT, &zavrsna_adr, id + 3);
        mem_block->dirty=BLOCK_DIRTY;
	return pocetna_adr;
}

/**
 * @brief flush memory blocks to disk file
 * @author Matija Šestak
 * @param void
 * @result int - EXIT_SUCCESS
*/
int AK_flush_cache(){
    int i = 0;
    AK_block *data_block;
    int block_written;
    while( i < MAX_CACHE_MEMORY ){
        if ( db_cache->cache[i]->dirty == BLOCK_DIRTY ){
            data_block = (AK_block *) db_cache->cache[i]->block;
            block_written = AK_write_block ( data_block );
            /// if block form cache can not be writed to DB file -> EXIT_ERROR
            if ( block_written != EXIT_SUCCESS ){
                    exit ( EXIT_ERROR );
            }
        }
        i++;
    }
    return EXIT_SUCCESS;
}


void memoman_test(){

}