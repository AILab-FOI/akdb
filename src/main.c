/**
@file main.c Main program file
*/
/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
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

#include <stdio.h>


#include "configuration.h"
#include "memoman.h"

/**
Main program function

@return EXIT_SUCCESS if successful, EXIT_ERROR otherwise
*/
int main()
{
	printf( "Калашникова DB - STARTING\n\n" );

	int size_in_mb = DB_FILE_SIZE;
	float size = 1024 * 1024 * size_in_mb / sizeof( KK_block );
	
	//db_file_size = size;
	
	printf( "We need %d blocks for %d MiB\n", ( int ) size, size_in_mb );

	
	KK_init_db_file( ( int ) size );
	
	KK_block * block = KK_read_block( 55 );

	block->header[ 0 ].integrity[ 0 ] = 23;	
	block->type = BLOCK_TYPE_NORMAL;

	KK_write_block ( block );

	free( block );
	block = KK_read_block( 55 );
	
	printf( "Integrity: %d\nType: %d\nChained with: %d\nSize: %d\n", block->header[ 0 ].integrity[ 0 ], block->type, block->chained_with, sizeof( *block ) );
	
//nbakos test of function KK_cache_block(int block_address)
	
//	int address_of_block = 55;
//	if( KK_cache_block( address_of_block,  ) == EXIT_SUCCESS)
//		printf("\nnbakos test: uspjesno kesiran blok: % ",address_of_block);
//	else{
//		printf("\nnbakos test: ERROR. NEuspjesno kesiran blok: % ",address_of_block);	
//	}
	
//end of KK_cache_block(int block_address) test
	
	

//nbakos test funkcije: int KK_new_extent( int start_address, int old_size, int ekstent_type, KK_header *header )
	/*
	int address_of_extend=0;
	KK_header *zaglavlje = ( KK_header * ) malloc ( sizeof( KK_header ) );
	zaglavlje->integrity[ 0 ] = 23;
	address_of_extend = KK_new_extent(0,0,0, zaglavlje );
	printf( "\nnbakos test: adresa extenda=%d\n",address_of_extend );
	*/
//nbakos end of KK_new_extent test
	
	
	
	
	//tomislavfotak test funkcije: KK_new_segment(char* name, int type, KK_header *header)
	//usput tesitra i funkciju KK_new_extent koju koristi :-)
/*	KK_header header;
	char ime = 'T';
	int adresaSegmenta;
	header.type = 1;
	header.integrity[0] = 23;
	adresaSegmenta = KK_new_segment(&ime, SEGMENT_TYPE_TABLE, &header);
	if(adresaSegmenta != 0)
		printf ("Jej, alocirao sam segment s pocetnom adresom %d :-)\n", adresaSegmenta);*/
	//nikako neće alocirati blokove 41 - 56 (nemam pojma zašto)
	
	
	//matnovak test funkcije: KK_init_system_tables_catalog( int relation, int attribute, 
								//	int index, int view, int sequence, int function, int function_arguments, 
								//  int trigger, int db, int db_obj, int user, int group, int right)
	
	KK_init_system_tables_catalog(1,2,3,4,5,6,7,8,9,10,11,12,13);
	KK_block  * catalog_block = KK_read_block( 0 );
	
	printf( "\nAtribut 1: %s\n Type: %d\n Integrity: %d\n Constr_name: %s\n", (char *) catalog_block->header[ 0 ].att_name, catalog_block->header[ 0 ].type ,  catalog_block->header[ 0 ].integrity[1],  (char *) catalog_block->header[ 0 ]. constr_name[0] );
	printf( "\nAtribut 2: %s\n Type: %d\n Integrity: %d\n Constr_name: %s\n", (char *) catalog_block->header[ 1 ].att_name, catalog_block->header[ 1 ].type ,  catalog_block->header[ 1 ].integrity[1],  (char *) catalog_block->header[ 1 ]. constr_name[0] );
	printf( "\nBlock: %d\n Type: %d\n Chained with: %d\n Free_space: %d\n", catalog_block->address, catalog_block->type ,  catalog_block->chained_with,  catalog_block->free_space);
	
	int i;
	for(i=0;i<26;i++) 
		
	{
		printf( "\nTupleDict%d \n Type: %d\n Address: %d\n Size: %d \n",i,catalog_block->tuple_dict[i].type, catalog_block->tuple_dict[i].address, catalog_block->tuple_dict[i].size);
	}
		
	char neki[143];
	i=0;
	int j=0;
	for(i; i < catalog_block->free_space;i++)
	{
		j=0;
		memcpy(&neki, catalog_block->data+i, 1);
		printf("\nZnak i:%d rj:%s", i,neki);
	}
		
	
/*	Try to write in alocated memory place
	 
	KK_query_mem_malloc();
	query_mem->parsed->next_replace = 1;
	printf("%d",query_mem->parsed->next_replace);
	*/
	//matnovak end test
	
	
	
		
	printf( "\nBye =)\n" );
	return ( EXIT_SUCCESS );
}
