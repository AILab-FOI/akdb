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

/**
Main program function

@return EXIT_SUCCESS if successful, EXIT_ERROR otherwise
*/
int main()
{
	printf( "Калашникова DB - STARTING\n\n" );


	if( KK_init_disk_manager() == EXIT_SUCCESS )
	{
		if( KK_memoman_init() == EXIT_SUCCESS )
		{

			/* component test area --- begin */

			btree_test();


			hash_test();


			files_test();


			


//			filesearch_test();


			fileio_test();
			filesort_test();


			/* component test area --- end */

			printf( "\nEverything was fine!\nBye =)\n" );
			return ( EXIT_SUCCESS );
		}
		printf( "ERROR. Failed to initialize memory manager\n" );
		return ( EXIT_ERROR );
	}
	printf( "ERROR. Failed to initialize disk manager\n" );
	return ( EXIT_ERROR );
}
