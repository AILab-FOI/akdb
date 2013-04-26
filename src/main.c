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
#include "auxi/constants.h"
#include "auxi/configuration.h"

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
#include "auxi/iniparser.h"
#include "sql/privileges.h"
#include "trans/transaction.h"
#include "rec/redo_log.h"
#include "auxi/observable.h"

void test()
{
    int ans=-1;
    AK_create_test_tables();
    
    while(ans)
    {
        system("clear");
        printf("Choose test:\n\n");
        printf(" 1. %-30s", "Ak_files_test()");
        printf(" 2. %-30s", "Ak_fileio_test()");
        printf(" 3. %-30s", "Ak_filesearch_test()");
        printf("\n");
        printf(" 4. %-30s", "Ak_filesort_test()");
        printf(" 5. %-30s", "AK_rel_eq_projection_test()");
        printf(" 6. %-30s", "AK_rel_eq_selection_test()");
        printf("\n");
        printf(" 7. %-30s", "AK_rel_eq_assoc_test()");
        printf(" 8. %-30s", "rel_eq_comut_test()");
        printf(" 9. %-30s", "AK_query_optimization_test()");
        printf("\n");
        printf("10. %-30s", "AK_table_test()");
        printf("11. %-30s", "AK_op_selection_test()");
        printf("12. %-30s", "AK_op_selection_test_redolog()");
        printf("\n");
        printf("13. %-30s", "AK_op_projection_test()");
        printf("14. %-30s", "AK_op_join_test()");
        printf("15. %-30s", "AK_op_theta_join_test()");
        printf("\n");
        printf("16. %-30s", "AK_op_intersect_test()");
        printf("17. %-30s", "AK_op_union_test()");
        printf("18. %-30s", "AK_op_difference_test()");
        printf("\n");
        printf("19. %-30s", "AK_op_rename_test()");
        printf("20. %-30s", "AK_op_product_test()");
        printf("21. %-30s", "Ak_btree_test()");
        printf("\n");
        printf("22. %-30s", "Ak_hash_test()");
        printf("23. %-30s", "Ak_bitmap_test()");
        printf("24. %-30s", "AK_reference_test()");
        printf("\n");
        printf("25. %-30s", "AK_trigger_test()");
        printf("26. %-30s", "Ak_aggregatin_test()");
        printf("27. %-30s", "AK_memoman_test()");
        printf("\n");
        printf("28. %-30s", "Ak_constraint_between_test()");
        printf("29. %-30s", "Ak_id_test()");
        printf("30. %-30s", "AK_null_test()");
        printf("\n");
        printf("31. %-30s", "AK_privileges_test()");
        printf("32. %-30s", "AK_unique_test()");
        printf("\n\n");
        printf(" 0. %-30s", "Exit");
        printf("\n\n");
        printf("Test: ");
        scanf("%d", &ans);
        while(ans<0 && ans>32)
        {
            printf("\nTest: ");
            scanf("%d", &ans);
        }
        switch (ans) {
            case 0:
                break;
            case 1:
                Ak_files_test();
                break;
            case 2:
                Ak_fileio_test();
                break;
            case 3:
                Ak_filesearch_test();
                break;
            case 4:
                Ak_filesort_test();
                break;
            case 5:
                AK_rel_eq_projection_test();
                break;
            case 6:
                AK_rel_eq_selection_test();
                break;
            case 7:
                AK_rel_eq_assoc_test();
                break;
            case 8:
                rel_eq_comut_test();
                break;
            case 9:
                //AK_query_optimization_test();
                printf("Error with test!");
                break;
            case 10:
                AK_table_test();
                break;
            case 11:
                AK_op_selection_test();
                break;
            case 12:
                AK_op_selection_test_redolog();
                break;
            case 13:
                AK_op_projection_test();
                break;
            case 14:
                AK_op_join_test();
                break;
            case 15:
                AK_op_theta_join_test();
                break;
            case 16:
                Ak_op_intersect_test();
                break;
            case 17:
                AK_op_union_test();
                break;
            case 18:
                Ak_op_difference_test();
                break;
            case 19:
                AK_op_rename_test();
                break;
            case 20:
                AK_op_product_test();
                break;
            case 21:
                Ak_btree_test();
                break;
            case 22:
                Ak_hash_test();
                break;
            case 23:
                Ak_bitmap_test();
                break;
            case 24:
                AK_reference_test();
                break;
            case 25:
                AK_trigger_test();
                break;
            case 26:
                Ak_aggregation_test();
                break;
            case 27:
                AK_memoman_test();
                break;
            case 28:
                Ak_constraint_between_test();
                break;
            case 29:
                Ak_id_test();
                break;
            case 30:
                AK_null_test();
                break;
            case 31:
                AK_privileges_test();
                break;
            case 32:
                AK_unique_test();
                break;
            default:
                break;
        }
        if(ans)
        {
            printf("\n\nPress Enter to continue:");
            getchar();
            getchar();
        }
    }

}

/**
Main program function

@return EXIT_SUCCESS if successful, EXIT_ERROR otherwise
*/
int main(int argc, char * argv[])
{
	printf( "KALASHNIKOV DB - STARTING\n\n" );
	AK_inflate_config();
	printf("db_file: %s\n", DB_FILE);

	if( AK_init_disk_manager() == EXIT_SUCCESS )
	{
		if( AK_memoman_init() == EXIT_SUCCESS )
		{
            /* component test area --- begin */
            if((argc == 2) && !strcmp(argv[1], "test"))
            {
                test();
            }
            /*component test area --- end */
            
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
