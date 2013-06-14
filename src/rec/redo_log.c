/**
 @file redo_log.c Provides redolog functions.
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include "redo_log.h"

/**
 * @author Krunoslav Bilić
 * @brief Function used to clean outdated redolog entries
 * @param no param.
 * @return No return value.
 */
void AK_clean_redolog() {
	/*int element = 0;

	//redo_log = (struct redo_log *) realloc(redo_log, MAX_REDO_LOG_MEMORY);

	printf("%ld velicina\n", sizeof(redo_log));
//sdads
	// poništavanje elemenata liste koji su outdated
	for (element = 0; element < MAX_REDO_LOG_MEMORY; element++) {    	

		redo_log->redo_log_cache = (AK_mem_block *) malloc(sizeof (AK_mem_block));
    	redo_log->expr = (AK_list *) malloc(sizeof (AK_list));

		if (redo_log->redo_log_cache[element]->timestamp_read != 0) {
			table_addresses *addresses = (table_addresses*) AK_get_table_addresses(redo_log->expr[element]->next->table);
			AK_mem_block* memBlock = (AK_mem_block *) malloc(sizeof(AK_mem_block));

			memBlock = (AK_mem_block *) AK_get_block(addresses->address_from[0]);

			if (memBlock->timestamp_last_change	> redo_log->redo_log_cache[element]->timestamp_last_change) {
				if (memBlock->dirty == 1){
					memcpy(redo_log->redo_log_cache[element], memBlock, sizeof(AK_mem_block));
				}else{
				redo_log->redo_log_cache[element] = NULL;
				}
			}
		}
	}

	// popravljanje liste
	for (element = 0; element < MAX_REDO_LOG_MEMORY; element++){

		if (redo_log->redo_log_cache[element] == NULL){
			//kopiranje sljedeceg elementa na mjesto praznog elementa
			redo_log->redo_log_cache[element] = (AK_mem_block *) malloc(sizeof(AK_mem_block));
			redo_log->redo_log_cache[element]->block = (AK_block *) malloc (sizeof(AK_block));
			redo_log->expr[element]->next = (AK_list_elem) malloc(sizeof(struct list_elem));

			if (element < MAX_REDO_LOG_MEMORY){
				memcpy(redo_log->redo_log_cache[element], redo_log->redo_log_cache[element+1], sizeof(AK_mem_block));
				memcpy(redo_log->expr[element], redo_log->expr[element+1], sizeof(AK_list));

				//brisanje elementa poslje
				redo_log->redo_log_cache[element+1] = (AK_mem_block *)  malloc(sizeof(AK_mem_block));
				redo_log->redo_log_cache[element+1]->block = (AK_block *) malloc (sizeof(AK_block));
				redo_log->redo_log_cache[element+1]->dirty = 0;

				redo_log->expr[element+1] =  (AK_list *) malloc (sizeof (AK_list));
				redo_log->expr[element+1]->next = (AK_list *) malloc (sizeof(AK_list));

				//sljedeci za izbacivanje je
				redo_log->next_replace = element-1;

				element = element-1;
			}

		}

	}*/
}

/**
 * @author Krunoslav Bilić
 * @brief Function used to add new element to redolog
 * @param memory block containing new element and affiliated expression
 * @return EXIT_SUCCESS if input is successful.
 */

int AK_add_to_redolog(char *srcTableName, AK_list *expr) {
    return EXIT_FAILURE;
	int element = 0;
	int size = 0;
	AK_mem_block* memBlock;

    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(srcTableName);

    if((memBlock = (AK_mem_block *) malloc(sizeof ( AK_mem_block))) == NULL){
     	return EXIT_FAILURE;
     }

    memBlock = (AK_mem_block *)AK_get_block(addresses->address_from[0]);

    size = (redo_log->next_replace + 1)*sizeof(AK_mem_block) + (redo_log->next_replace + 1)*sizeof(AK_list) + sizeof(int);

    if(redo_log->next_replace == MAX_REDO_LOG_ENTRIES || size > MAX_REDO_LOG_MEMORY){

	    //pozvati archive
	    //element = redo_log->next_replace;

	    memcpy(redo_log->redo_log_cache[element], memBlock, sizeof(AK_mem_block));
	    AK_copy_L(expr, redo_log->expr[element]);

	    Ak_dbg_messg(LOW, REDO, "\nAK_add_to_redolog: RedoLog dump");
	    if (REDO == 1) {AK_printout_redolog();}
	    return EXIT_SUCCESS;

    }else{
	    // dodavanje na sljedece prazno mjesto
	    for (element = 0; element < MAX_REDO_LOG_ENTRIES; element++){

	    	if (redo_log->redo_log_cache[element]->timestamp_last_change == 0){
	    		memcpy(redo_log->redo_log_cache[element], memBlock, sizeof(AK_mem_block));
	    		AK_copy_L(expr, redo_log->expr[element]);

	    		redo_log->next_replace = element + 1;
	    		Ak_dbg_messg(LOW, REDO, "\nAK_add_to_redolog: RedoLog dump");
	    		if (REDO == 1) {AK_printout_redolog();}
	    		return EXIT_SUCCESS;
	    	}
	    }
	}
}

/**
 * @author Krunoslav Bilić
 * @brief Function used to test whether redo log contains expression or not
 * @param expression to be tested
 * @return EXIT_SUCCESS if test is successful, otherwise EXIT_FAILURE
 */
int AK_is_in_redolog(AK_list *expr) {
    return EXIT_FAILURE;
	int element = 0;

	// ciscenje ukoliko je doslo do promjene stanja
	AK_clean_redolog();


	// provjerava da li trazeni upit postoji u redologu
	for (element = 0; element < MAX_REDO_LOG_MEMORY; element++) {
		if (AK_compare_L(redo_log->expr[element], expr) == EXIT_SUCCESS) {
			return EXIT_SUCCESS;
		}
	}
	return EXIT_FAILURE;
}

/**
 * @author Krunoslav Bilić
 * @brief Function prints out the content of redolog memory
 * @return No return value.
 */
void AK_printout_redolog(){
	int len[3200];
	int element = 0;

	for (element = 0; element != MAX_REDO_LOG_MEMORY; element++) {

		if (redo_log->expr[element]->next->type != 0) {
			printf("\n%d :", element);
			AK_print_row(len, redo_log->expr[element]);
			printf("table : %s",redo_log->expr[element]->next->table);
			printf("\n\n");
		}

	}
}


// Test redo_log is in selection.c
