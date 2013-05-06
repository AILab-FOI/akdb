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
#include "file/test.h"

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
#include "rel/product.h"
#include "rel/sequence.h"

//Command
#include "sql/command.h"

// Query processing
#include "opti/query_optimization.h"

// Constraints
#include "sql/cs/reference.h"
#include "sql/cs/between.h"
#include "sql/cs/nnull.h"
#include "sql/cs/unique.h"
#include "rel/expression_check.h"
#include "sql/drop.h"
#include "sql/cs/check_constraint.h"


//Other
#include "sql/trigger.h"
#include "auxi/iniparser.h"
#include "sql/privileges.h"
#include "trans/transaction.h"
#include "rec/redo_log.h"
#include "auxi/observable.h"
#include "sql/view.h"

void help();
void show_test();
void choose_test();
//void run_test();

typedef struct {
    char name[40];
    void (*func)(void);
} function;

function fun[] = {
        
        {"Ak_bitmap_test", &Ak_bitmap_test},
        {"Ak_btree_test", &Ak_btree_test},
        {"Ak_constraint_between_test", &Ak_constraint_between_test},
        {"Ak_fileio_test", &Ak_fileio_test},
        {"Ak_files_test", &Ak_files_test},
        {"Ak_filesearch_test", &Ak_filesearch_test},
        {"Ak_filesort_test", &Ak_filesort_test},
        {"Ak_hash_test", &Ak_hash_test},
        {"Ak_aggregation_test", &Ak_aggregation_test},
        {"Ak_id_test", &Ak_id_test},
        {"AK_memoman_test", &AK_memoman_test},
        {"AK_null_test", &AK_null_test},
        {"AK_op_difference_test", &Ak_op_difference_test},
        {"AK_op_intersect_test", &Ak_op_intersect_test},
        {"AK_op_join_test", &AK_op_join_test},
        {"AK_op_product_test", &AK_op_product_test},
        {"AK_op_projection_test", &AK_op_projection_test},
        {"AK_op_rename_test", &AK_op_rename_test},
        {"AK_op_selection_test", &AK_op_selection_test},
        {"AK_op_selection_test_redolog", &AK_op_selection_test_redolog},
        {"AK_op_theta_join_test", &AK_op_theta_join_test},
        {"AK_op_union_test", &AK_op_union_test},
        {"AK_rel_eq_comut_test", &AK_rel_eq_comut_test},
        {"AK_privileges_test", &AK_privileges_test},
        {"AK_reference_test", &AK_reference_test},
        {"AK_rel_eq_assoc_test", &AK_rel_eq_assoc_test},
        {"AK_rel_eq_projection_test", &AK_rel_eq_projection_test},
        {"AK_rel_eq_selection_test", &AK_rel_eq_selection_test},
        {"AK_table_test", &AK_table_test},
        {"AK_trigger_test", &AK_trigger_test},
        {"AK_unique_test", &AK_unique_test},
        {"AK_tarjan_test", &AK_tarjan_test},
        {"AK_observable_test", &AK_observable_test},
        {"AK_drop_test", &AK_drop_test},
        {"AK_sequence_test", &AK_sequence_test},
        {"AK_function_test", &AK_function_test},
        {"AK_view_test", &AK_view_test},
        {"AK_check_constraint_test", &AK_check_constraint_test},
        {"AK_transaction_test", &AK_test_Transaction},
        {"AK_observable_pattern_test", &AK_observable_test}
    };

/**
Main program function

@return EXIT_SUCCESS if successful, EXIT_ERROR otherwise
*/
int main(int argc, char * argv[])
{
    qsort(fun, sizeof(fun)/sizeof(fun[0]), (int)sizeof(fun[0]), (void*)strcasecmp);
    if((argc == 2) && !strcmp(argv[1], "help"))
        help();
    else if((argc == 3) && !strcmp(argv[1], "test") && !strcmp(argv[2], "show"))
        show_test();
    else
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
                choose_test();
            }
            else if((argc == 3) && !strcmp(argv[1], "test"))
            {
                int ans;
                ans = (int)strtol(argv[2], NULL, 10)-1;
                AK_create_test_tables();
                fun[ans].func();
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
    return(EXIT_SUCCESS);
}

void help()
{
    printf("Usage: akdb [option]\n");
    printf("Available commands:\n");
    printf("help - displays help\n");
    printf("test [test_id] - run akdb in testing mode\n");
    printf("test show - displays available tests\n");
}
void show_test()
{
    int i=0;
    
    int m = sizeof(fun)/sizeof(fun[0]);
    
    printf("Choose test:\n\n");
    while(i<m)
    {
        printf("%2d. %-32s", i+1, fun[i].name);
        i++;
        if(!(i%3))
            printf("\n");
    }
    printf("\n\n");
    printf(" 0. %-30s", "Exit");
    printf("\n\n");
}

void choose_test()
{
    int ans=-1;
    AK_create_test_tables();
    while(ans)
    {
        printf("\n\n\n");
        show_test();
        printf("Test: ");
        scanf("%d", &ans);
        while(ans<0 && ans>32)
        {

            printf("\nTest: ");
            scanf("%d", &ans);
        }
        if(ans)
        {
            ans--;
            fun[ans].func();
            printf("\n\nPress Enter to continue:");
            getchar();
            getchar();
            ans++;
        }
    }
}
