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
#include <string.h>  //ARCHIVE LOG
#include <stdlib.h>
// Global configuration
#include "aux/constants.h"
#include "aux/configuration.h"

// Disk management
#include "dm/dbman.h"

// Memory management
#include "mm/memoman.h"

// File management
#include "file/fileio.h"
#include "file/files.h"
#include "file/filesearch.h"
#include "file/filesort.h"
#include "file/table.h"

//Logging
#include "rec/archive_log.h" //ARCHIVE LOG

// Indices
#include "file/idx/hash.h"
#include "file/idx/btree.h"
#include "file/idx/bitmap.h"

// Relational operators
#include "rel/difference.h"
#include "rel/intersect.h"
#include "rel/nat_join.h"
#include "rel/theta_join.h"
#include "rel/projection.h"
#include "sql/rename.h"
#include "rel/selection.h"
#include "rel/union.h"
#include "rel/aggregation.h"

//Command
#include "sql/command.h"

// Query processing
#include "opti/query_optimization.h"

// Constraints
#include "sql/cs/reference.h"
#include "rel/expression_check.h"


//Other
#include "sql/trigger.h"
#include "aux/iniparser.h"
#include "sql/privileges.h"
#include "trans/transaction.h"
#include "rec/redo_log.h"
#include "aux/observable.h"
/**
Main program function

@return EXIT_SUCCESS if successful, EXIT_ERROR otherwise
*/
int main()
{
	printf( "KALASHNIKOV DB - STARTING\n\n" );
	AK_inflate_config();
	printf("db_file: %s\n", DB_FILE);

	if( AK_init_disk_manager() == EXIT_SUCCESS )
	{
		if( AK_memoman_init() == EXIT_SUCCESS )
		{
			/* component test area --- begin */
			AK_create_test_tables();
			
//			files_test();
//			fileio_test();
//			filesearch_test();
//			filesort_test();

//			rel_eq_projection_test();
//			rel_eq_selection_test();
//			rel_eq_assoc_test();
//			rel_eq_comut_test();
//			query_optimization_test();
//
//			table_test();
//
//			op_selection_test();
//			AK_op_selection_test_redolog();
//			op_projection_test();
//			op_join_test();
//			op_theta_join_test();
//			op_intersect_test();
//			op_union_test();
//			op_difference_test();
//			op_rename_test();
//			op_product_test();

//			btree_test();
//			hash_test();
//			bitmap_test(); 	//needs complete rewrite, currently doesn't work correctly
//
//			reference_test();
//
//			trigger_test();
//
//			aggregation_test();
//
//			memoman_test();
//
//			constraint_between_test();
//			id_test();
//			null_test();
//			privileges_test();
//          AK_unique_test();

			/* component test area --- end */
                        
			if ( AK_flush_cache() == EXIT_SUCCESS ){
                            
				printf( "\nEverything was fine!\nBye =)\n" );
                    //            pthread_exit(NULL);
                                return ( EXIT_SUCCESS );
			}
		}
		printf( "ERROR. Failed to initialize memory manager\n" );
		return ( EXIT_ERROR );
	}
	printf( "ERROR. Failed to initialize disk manager\n" );
	return ( EXIT_ERROR );
        
}
