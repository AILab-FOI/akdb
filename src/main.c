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

	printf( "We need %d blocks for %d MiB\n", ( int ) size, size_in_mb );

	KK_init_db_file( ( int ) size );
	
	KK_block * block = KK_read_block( 55 );

	block->header[ 0 ].integrity[ 0 ] = 23;	
	block->type = BLOCK_TYPE_NORMAL;

	KK_write_block ( block );

	free( block );
	block = KK_read_block( 55 );
	
	printf( "Integrity: %d\nType: %d\nChained with: %d\nSize: %d\n", block->header[ 0 ].integrity[ 0 ], block->type, block->chained_with, sizeof( *block ) );

	printf( "\nBye =)\n" );
	
	//nbakos test funkcije: int KK_new_extent( int start_address, int old_size, int ekstent_type, KK_header *header )
	int address_of_extend=0;
	KK_header *header;
	header->integrity[ 0 ] = 23;
	address_of_extend = KK_new_extent(0,0,0, header );
	printf( "\nnbakos test: adresa extenda=%d\n",address_of_extend );
	return ( EXIT_SUCCESS );
}
