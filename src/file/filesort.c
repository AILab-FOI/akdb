/**
@file filsort.c Provides functions for file sorting
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
#include "filesort.h"

/**
 * @author Unknown
 * @brief Function returns total number of headers in the block
 * @return number of attribute in header (0 - MAX_ATTRIBUTES). USE in tuple_dict[num]...
 */
int Ak_get_total_headers(AK_block *iBlock) {
    register int i;
    for (i = 0; i < MAX_ATTRIBUTES; i++) {
        if (strcmp((char *) iBlock->header[i].att_name, "") == 0) //if there is no more attributes
            return i;
    }
    return i;
}

/**
 * @author Unknown
 * @brief Function returns number of header in the block which to sort
 * @return number of attribute in header (0 - MAX_ATTRIBUTES). USE in tuple_dict[num]...
 */
int Ak_get_header_number(AK_block *iBlock, char *attribute_name) {
    register int i;

    for (i = 0; i < MAX_ATTRIBUTES; i++) {
        if (!strcmp((char *) iBlock->header[i].att_name, attribute_name)) {
            return i;
        } else if (!strcmp((char *) iBlock->header[i].att_name, "/0")) {
            printf("get_header_number: ERROR: in this table does not exist atribute: %s", attribute_name);
            return EXIT_ERROR;
        }
    }
    printf("get_header_number: ERROR: in this table does not exist atribute: %s", attribute_name);
    return EXIT_ERROR;
}

/**
 * @author Unknown
 * @brief Function returns tuples number in block
 * @return tuples number in block
 */
int Ak_get_num_of_tuples(AK_block *iBlock) {
    int i = 0;
    int kraj = 1;
    while (kraj) {
        i++;
        if (iBlock->tuple_dict[ i ].size == 0) {
            kraj = 0;
        }
    }

    if (i == 0)
        return 0;

    int max_header_num = Ak_get_total_headers(iBlock);
    return (i / max_header_num);
}
/*
AK_mem_block *get_next_block(int num) {
    AK_mem_block *mem_block = (AK_mem_block *) AK_malloc(sizeof (AK_mem_block));
    mem_block = (AK_mem_block *) AK_get_block(num);

    //AK_block *temp_block = AK_read_block(num); // tu poslije komentirati
    //memcpy(mem_block->block, temp_block, sizeof(AK_block));
    return mem_block;
}*/
/*
 * @author Unknown
 * @brief Function sorts segment
 * @return No return value.
 */
void AK_sort_segment(char *table_name, char *attr) {
    //initialize variables
        register int i, j, n, q;
    int blocks_addr[MAX_NUM_OF_BLOCKS]; 		//array with block addresses
    int temp_blocks_addr[MAX_NUM_OF_BLOCKS];	//array with temporary block addresse
    int num_blocks = 0;							//number of blocks in segment
	int num_extents = 0;						//number of extents in segment
    int num_temp_blocks = 0;					//number of temporary blocks in segment
    //---------------------------------------------------------------------------------
	
    //Get number of extents and number of blocks for given table
	table_addresses *addresses = (table_addresses *) AK_get_table_addresses(table_name);
	
    for (i = 0; addresses->address_from[i] != 0; i++) {
        for (j = addresses->address_from[i]; j <= addresses->address_to[i]; j++) {
            blocks_addr[num_blocks] = j;
            num_blocks++;
        }
        num_extents++;
    }
	//---------------------------------------------------------------------------------

	Ak_dbg_messg(HIGH, FILE_MAN, "There are: %i blocks in segment %s, first block at %i\n", num_blocks, table_name, blocks_addr[0]);

    //create the new temp segment with size equal to the size of the original segment
    char temp_segment[MAX_VARCHAR_LENGTH];
    memset(temp_segment, '\0', MAX_VARCHAR_LENGTH);
	strcat(temp_segment, "SORT_TEMP_HELP_");
    strcat(temp_segment, table_name);

    AK_header *head = AK_get_header(table_name);
    AK_initialize_new_segment(temp_segment, SEGMENT_TYPE_TABLE, head);
	
    //init new table extent
    for (i = 1; i < num_extents; i++) {
        AK_init_new_extent(temp_segment, SEGMENT_TYPE_TABLE);
    }
	//---------------------------------------------------------------------------------
    
	//search for all blocks in segment
    addresses = (table_addresses *) AK_get_table_addresses(temp_segment);

    for (i = 0; addresses->address_from[i] != 0; i++) {
        for (j = addresses->address_from[i]; j <= addresses->address_to[i]; j++) {
            temp_blocks_addr[num_temp_blocks] = j;
            num_temp_blocks++;
        }
    }
	//---------------------------------------------------------------------------------

	Ak_dbg_messg(HIGH, FILE_MAN, "There are: %i blocks in segment %s, first block at %i\n", num_temp_blocks, temp_segment, temp_blocks_addr[0]);

    //SORTING ...
    char x[DATA_ROW_SIZE]; 		//needed data
    char y[DATA_ROW_SIZE]; 		//data needed for comparison
    char data[DATA_TUPLE_SIZE]; //data to be copied

    int r1, r2;
    
	//array index of the current block address
	int id, id2, id_temp;
	
	//position
    int cache1_pos, cache2_pos, cacheTemp_pos;
	
	id = id2 = id_temp = cache1_pos = cache2_pos = cacheTemp_pos = 0;
    
	AK_mem_block * cache1 = (AK_mem_block*) AK_get_block(blocks_addr[id]);
	AK_mem_block * cache2 = (AK_mem_block*) AK_malloc(sizeof(AK_mem_block));//AK_get_block(blocks_addr[id2]); //or AK_malloc as before
	AK_mem_block * cacheTemp = (AK_mem_block*) AK_get_block(temp_blocks_addr[id_temp]);
	
    int num_tuples_cache1 = Ak_get_num_of_tuples(cache1->block);
	int num_tuples_cache2 = 0;
	
	//get total number of headers and the number of header used to sort segment
	int num_headers = Ak_get_total_headers(cache1->block);
	int num_sort_header = Ak_get_header_number(cache1->block, attr);

	int cacheTemp_size = 0; //cacheTemp block capacity tracking

    //return (0);

    //ako je glavni = 1 onda se sortirani blokovi nalaze u blocks_addr
    //ako je glavni = 0 onda se sortirani podaci nalaze u temp_blocks_adrr
    int glavni = 1;

    for (n = 1; n < num_blocks; n = n * 2) {
        for (q = 0; q < num_blocks / 2; q += n) {
            id = q; 			//pokazivac na block u prvom dijelu
            id2 = q + n; 		//pokazivac na block u drugom dijelu
            r1 = q + n; 		//granična vrijednost blokova u 1 dijelu
            r2 = q + n + n; 	//granična vrijednost blokova u 2 dijelu

			Ak_dbg_messg(HIGH, FILE_MAN, "q: %i ,  n: %i  , r1: %i   , r2: %i\n", q, n, r1, r2);

            cache1 = (AK_mem_block*) AK_get_block(blocks_addr[id]); //read first block
            num_tuples_cache1 = Ak_get_num_of_tuples(cache1->block);
            Ak_dbg_messg(HIGH, FILE_MAN, "read block at: %i , block number: %i\n", id, blocks_addr[id]);
            //id++;

            cache2 = (AK_mem_block*) AK_get_block(blocks_addr[id2]); //read second block
            num_tuples_cache2 = Ak_get_num_of_tuples(cache2->block);
            Ak_dbg_messg(HIGH, FILE_MAN, "read block at: %i , block number: %i\n", id2, blocks_addr[id2]);
            //id2++;

            cacheTemp = (AK_mem_block*) AK_get_block(temp_blocks_addr[id_temp]); //read temp block for write
			Ak_dbg_messg(HIGH, FILE_MAN, "read block at: %i , temp block number: %i\n", id_temp, temp_blocks_addr[id_temp]);
            //id_temp++;

			int type, address, size;
			type = address = size = 0;
			
            while ((id < r1) && (id2 < r2)) {
				int address = cache1->block->tuple_dict[cache1_pos * num_headers + num_sort_header].address;
				int size = cache1->block->tuple_dict[cache1_pos * num_headers + num_sort_header].size;
				
                //first data
                memset(x, '\0', MAX_VARCHAR_LENGTH); //DATA_ROW_SIZE
                memcpy(x, cache1->block->data + address, size);
				
				address = cache2->block->tuple_dict[cache2_pos * num_headers + num_sort_header].address;
				size = cache2->block->tuple_dict[cache2_pos * num_headers + num_sort_header].size;

                //second data
				printf("\nData: %i\n", cache2->block->tuple_dict[2].address);
				if (cache2->block->last_tuple_dict_id == 0) {
					return;
				}

				memset(y, '\0', MAX_VARCHAR_LENGTH);
				memcpy(y, cache2->block->data + address, size);
				
                Ak_dbg_messg(HIGH, FILE_MAN, ">> tuples: %s , %s   \n", x, y);

                //comparison (x < y)
                if (strcmp(x, y) <= 0) {
                    Ak_dbg_messg(HIGH, FILE_MAN, "the lower tuple is: %s\n", x);

                    //check if data can fit in the block. If can't load the new temp block.
                    for (i = 0; i < num_headers; i++) {
                        cacheTemp_size += cache1->block->tuple_dict[i + (cache1_pos * num_headers)].size;
                    }

                    if ((cacheTemp->block->AK_free_space + cacheTemp_size) >= DATA_BLOCK_SIZE) {
						id_temp++;
						cacheTemp->dirty = BLOCK_DIRTY;
                        cacheTemp = (AK_mem_block*) AK_get_block(temp_blocks_addr[id_temp]);
                        cacheTemp_pos = 0;
                    }
					//---------------------------------------------------------------------------------

                    //copy data to the temp block
                    for (i = 0; i < num_headers; i++) {
						address = cache1->block->tuple_dict[i + (cache1_pos * num_headers)].address;
						size = cache1->block->tuple_dict[i + (cache1_pos * num_headers)].size;
                        type = cache1->block->tuple_dict[i + (cache1_pos * num_headers)].type;
						
						memset(data, '\0', MAX_VARCHAR_LENGTH); // DATA_TUPLE_SIZE
                        memcpy(data, cache1->block->data + address, size);
                        AK_insert_entry(cacheTemp->block, type, data, cacheTemp_pos);
                        cacheTemp_pos++;
                    }
                    cache1_pos++;

                    //ako se je pročitao zadnji podatak iz cache1->block
                    if (cache1_pos > num_tuples_cache1) {
                        id++;

                        //ako to nije bio zadnji blok u prolazu
                        if (id < r1) {
                            cache1 = (AK_mem_block*) AK_get_block(blocks_addr[id]); // učitaj slijedeći blok
                            num_tuples_cache1 = Ak_get_num_of_tuples(cache1->block); //koliko ima zapisa?
                            cache1_pos = 0;
                        }
                    }
                }//end: if(strcmp(x,y) <= 0)

                else { //x >= y
                    Ak_dbg_messg(HIGH, FILE_MAN, "ELSE the lower tuple is: %s\n", y); //x before changes

                    //check if data can fit in the block. If can't load the new temp block.
                    for (i = 0; i < num_headers; i++) {
                        cacheTemp_size += cache2->block->tuple_dict[i + (cache2_pos * num_headers)].size;
                    }

                    if ((cacheTemp->block->AK_free_space + cacheTemp_size) >= DATA_BLOCK_SIZE) {
						id_temp++;
						cacheTemp->dirty = BLOCK_DIRTY;
                        cacheTemp = (AK_mem_block*) AK_get_block(temp_blocks_addr[id_temp]);
                        cacheTemp_pos = 0;
                    }
					//---------------------------------------------------------------------------------

                    //copy data to the temp block
                    for (i = 0; i < num_headers; i++) {
						address = cache2->block->tuple_dict[i + (cache2_pos * num_headers)].address;
						size = cache2->block->tuple_dict[i + (cache2_pos * num_headers)].size;
						type = cache2->block->tuple_dict[i + (cache2_pos * num_headers)].type;
						
                        memset(data, '\0', MAX_VARCHAR_LENGTH); //DATA_TUPLE_SIZE
                        memcpy(data, cache2->block->data + address, size);
                        AK_insert_entry(cacheTemp->block, type, data, cacheTemp_pos);
                        cacheTemp_pos++;
                    }
                    cache2_pos++;

                    //ako se je pročitao zadnji podatak iz cache2->block
                    if (cache2_pos > num_tuples_cache2) {
                        id2++;

                        //ako to nije bio zadnji blok u prolazu
                        if (id2 < r2) {
                            cache2 = (AK_mem_block*) AK_get_block(blocks_addr[id2]); // učitaj slijedeći blok
                            num_tuples_cache2 = Ak_get_num_of_tuples(cache2->block);
                            cache2_pos = 0; //postavi se na početak učitanog bloka
                        }
                    }
                } //end: else x>=y
            } // end: while( id < r1  &&  id2 < r2 )

            if (id == r1) {
                while (id2 < r2) {
					//if all data from the 1th partition copied and there is more data in 2th partition 
                    Ak_dbg_messg(HIGH, FILE_MAN, "there is more data to copy from the 2th partition");

                    //check if data can fit in the block. If can't load the new temp block.
                    for (i = 0; i < num_headers; i++) {
                        cacheTemp_size += cache2->block->tuple_dict[i + (cache2_pos * num_headers)].size;
                    }

                    if ((cacheTemp->block->AK_free_space + cacheTemp_size) >= DATA_BLOCK_SIZE) { // ako je veća od zadane veličine onda treba učitati novi temp blok
						id_temp++;
						cacheTemp->dirty = BLOCK_DIRTY;
                        cacheTemp = (AK_mem_block*) AK_get_block(temp_blocks_addr[id_temp]);
                        cacheTemp_pos = 0;
                    }
					//---------------------------------------------------------------------------------
					
                    //copy data to the temp block
                    for (i = 0; i < num_headers; i++) {
						address = cache2->block->tuple_dict[i + (cache2_pos * num_headers)].address;
						size = cache2->block->tuple_dict[i + (cache2_pos * num_headers)].size;
						type = cache2->block->tuple_dict[i + (cache2_pos * num_headers)].type;
						
                        memset(data, '\0', MAX_VARCHAR_LENGTH);  //DATA_TUPLE_SIZE
                        memcpy(data, cache2->block->data + address, size);
                        AK_insert_entry(cacheTemp->block, type, data, cacheTemp_pos);
                        cacheTemp_pos++;
                    }
                    cache2_pos++;

                    //ako se je pročitao zadnji podatak iz cache2->block
                    if (cache2_pos > num_tuples_cache2) {
                        id2++;

                        //ako to nije bio zadnji blok u prolazu
                        if (id2 < r2) {
                            cache2 = (AK_mem_block*) AK_get_block(blocks_addr[id2]); // učitaj slijedeći blok
                            num_tuples_cache2 = Ak_get_num_of_tuples(cache2->block);
                            cache2_pos = 0;
                        }
                    }
                } //end: while(id2 < r2)
            }//end: if(id == r1)
			
			//if ( id2 == r2 )
            else {
                Ak_dbg_messg(HIGH, FILE_MAN, "there is more data to copy from the 1th partition");

                while (id < r1) {
                    //check if data can fit in the block. If can't load the new temp block.
                    for (i = 0; i < num_headers; i++) {
                        cacheTemp_size += cache1->block->tuple_dict[i + (cache1_pos * num_headers)].size;
                    }

                    if ((cacheTemp->block->AK_free_space + cacheTemp_size) >= DATA_BLOCK_SIZE) {
						id_temp++;
						cacheTemp->dirty = BLOCK_DIRTY;
                        cacheTemp = (AK_mem_block*) AK_get_block(temp_blocks_addr[id_temp]);
                        cacheTemp_pos = 0;
                    }
					//---------------------------------------------------------------------------------
					
                    //copy data to the temp block
                    for (i = 0; i < num_headers; i++) {
						address = cache1->block->tuple_dict[i + (cache1_pos * num_headers)].address;
						size = cache1->block->tuple_dict[i + (cache1_pos * num_headers)].size;
						type = cache1->block->tuple_dict[i + (cache1_pos * num_headers)].type;
						
                        memset(data, '\0', MAX_VARCHAR_LENGTH); //DATA_TUPLE_SIZE
                        memcpy(data, cache1->block->data + address, size);
                        AK_insert_entry(cacheTemp->block, type, data, cacheTemp_pos);
                        cacheTemp_pos++;
                    }
                    cache1_pos++;

                    //ako se je pročitao zadnji podatak iz cache1->block
                    if (cache1_pos > num_tuples_cache1) {
                        id++;

                        //ako to nije bio zadnji blok u prolazu
                        if (id < r1) {
                            cache1 = (AK_mem_block*) AK_get_block(blocks_addr[id]); // učitaj slijedeći blok
                            num_tuples_cache1 = Ak_get_num_of_tuples(cache1->block); //koliko ima zapisa?
                            cache1_pos = 0; //postavi se na početak učitanog bloka
                        }
                    }
                } //end: while (id < r1)
            } //end: else
        } //end: for (q = 0; q < num_blocks / 2 ; q = 1 + n )

        //zamjena polja sa blokovima
		int swap_blocks_addr[MAX_NUM_OF_BLOCKS];		//
		int num_swap_blocks = 0;						//
        
		// podaci su u blocks_addr
        if (glavni) {
            glavni = 0;
            printf("\n\nmain is segment\n\n");

            for (i = 0; i < num_blocks; i++) {
                swap_blocks_addr[i] = blocks_addr[i];
                blocks_addr[i] = temp_blocks_addr[i];
                temp_blocks_addr[i] = swap_blocks_addr[i];
            }

            num_swap_blocks = num_blocks;
            num_blocks = num_temp_blocks;
            num_temp_blocks = num_swap_blocks;
        } else {
            printf("\n\nmain is temp\n\n");
            glavni = 1;

            for (i = 0; i < num_blocks; i++) {
                swap_blocks_addr[i] = temp_blocks_addr[i];
                temp_blocks_addr[i] = blocks_addr[i];
                blocks_addr[i] = swap_blocks_addr[i];
            }

            num_swap_blocks = num_temp_blocks;
            num_temp_blocks = num_blocks;
            num_blocks = num_swap_blocks;
        }
    } // end : for(n = 1; n < num_blocks; n = n * 2 )
}
/*
 * @author Unknown
 * @brief Function resets block
 * @param block block to be resetted
 * @return No return value
 */
void Ak_reset_block(AK_block * block) {
    register int i, j, k;

    AK_header empty_header[ MAX_ATTRIBUTES ];
    AK_tuple_dict empty_tuple_dict[ DATA_BLOCK_SIZE ];
    unsigned char empty_data[ DATA_BLOCK_SIZE * DATA_ENTRY_SIZE ];

    for (i = 0; i < MAX_ATTRIBUTES; i++) {
        empty_header[i].type = FREE_INT;
        for (j = 0; j < MAX_ATT_NAME; j++) {
            empty_header[i].att_name[j] = FREE_CHAR;
        }
        for (j = 0; j < MAX_CONSTRAINTS; j++) {
            empty_header[i].integrity[j] = FREE_INT;
            for (k = 0; k < MAX_CONSTR_NAME; k++) {
                empty_header[i].constr_name[j][k] = FREE_CHAR;
            }
            for (k = 0; k < MAX_CONSTR_CODE; k++) {
                empty_header[i].constr_code[j][k] = FREE_CHAR;
            }
        }
    }

    for (i = 0; i < DATA_BLOCK_SIZE; i++) {
        empty_tuple_dict[i].type = FREE_INT;
        empty_tuple_dict[i].address = FREE_INT;
        empty_tuple_dict[i].size = FREE_INT;
    }

    for (i = 0; i < DATA_BLOCK_SIZE * DATA_ENTRY_SIZE; i++) {
        empty_data[i] = FREE_CHAR;
    }

    block->type = BLOCK_TYPE_FREE;
    block->chained_with = NOT_CHAINED;
    block->AK_free_space = 0;

    memcpy(block->header, empty_header, sizeof (*empty_header));
    memcpy(block->tuple_dict, empty_tuple_dict, sizeof ( *empty_tuple_dict));
    memcpy(block->data, empty_data, sizeof (*empty_data));
}

/**
  * @author Bakoš Nikola
  * @version v1.0
  * @brief Function sorts the given block
  * @param iBlock block to be sorted
  * @return No return value
 */
void AK_block_sort(AK_block * iBlock, char * atr_name) {
    register int i, j, k, n, t, q;

    char x[DATA_ROW_SIZE]; //bas podatak koji nas zanima
    char y[DATA_ROW_SIZE]; //bas podatak s kojim se uspoređuje

    AK_block * cTemp1 = (AK_block*) AK_malloc(sizeof (AK_block));
    cTemp1 = (AK_block *) AK_read_block(15);

    int tip = 0;
    int broj_td;

    AK_block *cTemp2 = (AK_block*) AK_malloc(sizeof (AK_block));
    cTemp2 = (AK_block *) AK_read_block(16);

    AK_header *block_header = (AK_header *) AK_malloc(sizeof (AK_header));
    memcpy(block_header, iBlock->header, sizeof (AK_header));

    int num_sort_header = Ak_get_header_number(iBlock, atr_name); //broj headera po kojem se sortira
    //tu provjeriti još funkciju
    int max_header_num = Ak_get_total_headers(iBlock); //ukupni broj headera za jedan zapis
    int num_tuples = Ak_get_num_of_tuples(iBlock);

    Ak_dbg_messg(HIGH, FILE_MAN, "\n tu sam: %i, %i", num_sort_header, num_tuples);

    unsigned char data[MAX_VARCHAR_LENGTH]; //it was 2000 before MAX_VARCHAR_LENGHT
    int ubr1 = num_tuples / 2; //ukupni broj 1. polovice
    int ubr2 = num_tuples - ubr1;

    for (n = 1; n < num_tuples; n = n * 2) {
        Ak_reset_block(cTemp1);
        Ak_reset_block(cTemp2);
        //n=1;

        broj_td = 0;
        //write a half in the first temp block
        for (t = 0; t < ubr1; t++) {
            Ak_dbg_messg(HIGH, FILE_MAN, "block_sort: 1) sada sam na %i / %i\n", t, num_tuples);

            for (i = 0; i < max_header_num; i++) {
                memset(data, '\0', MAX_VARCHAR_LENGTH);
                memcpy(data, iBlock->data + iBlock->tuple_dict[i + (t * max_header_num)].address,
                        iBlock->tuple_dict[ i + (t * max_header_num) ].size);
                AK_insert_entry(cTemp1, iBlock->tuple_dict[i + (t * max_header_num)].type, data, broj_td);
                broj_td++;
            }
        }

        memcpy(cTemp1->header, block_header, sizeof (AK_header));
        AK_write_block(cTemp1); //tu treba nekaj napraviti
        printf("   \n\n               in cTemp1        data, brtd: %i\n", broj_td);

        for (i = 0; i < broj_td; i++) {
            memset(data, '\0', MAX_VARCHAR_LENGTH);
            memcpy(data, cTemp1->data + cTemp1->tuple_dict[i].address, cTemp1->tuple_dict[i].size);
            printf(" %s ", data);
        }
        printf("\n\n");

        //upis druge polovice u drugi temp blok
        broj_td = 0;
        for (; t < num_tuples; t++) {
            Ak_dbg_messg(HIGH, FILE_MAN, "block_sort: 2) sada sam na %i / %i\n", t, num_tuples);

            for (i = 0; i < max_header_num; i++) {
                memset(data, '\0', MAX_VARCHAR_LENGTH);
                memcpy(data, iBlock->data + iBlock->tuple_dict[i + (t * max_header_num)].address,
                        iBlock->tuple_dict[i + (t * max_header_num)].size);
                //printf ("                          podatak: %s      na: %i  adresa: %i\n", podatak, broj_td, iBlock->tuple_dict[ i + (t * max_header_num) ].address);
                AK_insert_entry(cTemp2, iBlock->tuple_dict[i + (t * max_header_num)].type, data, broj_td);
                broj_td++;
            }
        }
        memcpy(cTemp2->header, block_header, sizeof (AK_header));
        if (AK_write_block(cTemp2) == EXIT_ERROR) //tu isto treba nekaj napraviti
            printf("cTemp2 writing error\n");

        printf("   \n\n               in cTemp2        data, brtd: %i\n", broj_td);
        for (i = 0; i < broj_td; i++) {
            memset(data, '\0', MAX_VARCHAR_LENGTH);
            memcpy(data, cTemp2->data + cTemp2->tuple_dict[i].address, cTemp2->tuple_dict[i].size);
            printf(" %s ", data);
        }
        printf("\n\n");

        //Start sorting
        Ak_reset_block(iBlock);

        Ak_dbg_messg(HIGH, FILE_MAN, "                      , size: %i\n", iBlock->AK_free_space);

        memcpy(iBlock->header, block_header, sizeof (AK_header));

        int br1, br2;
        broj_td = br1 = br2 = j = k = 0;

        for (q = 0; q < ubr1; q += n) {
            printf("\nfor (q = 0; q < num_tuples/2; q += n ) q=%i , j=%i   ,  k=%i  , br1=%i  , br2=%i\n", q, j, k, br1, br2);
            for (j = k = 0;;) {
                memset(x, '\0', DATA_ROW_SIZE);
                memcpy(x, cTemp1->data + cTemp1->tuple_dict[br1 * max_header_num + num_sort_header].address,
                        cTemp1->tuple_dict[br1 * max_header_num + num_sort_header].size);
                //x[cTemp1->tuple_dict[br1 * max_header_num + num_sort_header].size - 1]="\0";

                memset(y, '\0', DATA_ROW_SIZE);
                memcpy(y, cTemp2->data + cTemp2->tuple_dict[br2 * max_header_num + num_sort_header ].address,
                        cTemp2->tuple_dict[br2 * max_header_num + num_sort_header].size);
                //y[cTemp2->tuple_dict[br2 * max_header_num + num_sort_header].size - 1]="\0";

                Ak_dbg_messg(HIGH, FILE_MAN, "slogovi: %s , %s   , head: %i \n", x, y, num_sort_header);

                //comparison
                if (strcmp(x, y) <= 0) {
                    Ak_dbg_messg(HIGH, FILE_MAN, "manji je: %s\n", x);

                    //insert data
                    for (i = 0; i < max_header_num; i++) {
                        memset(data, '\0', MAX_VARCHAR_LENGTH);
                        memcpy(data, cTemp1->data + cTemp1->tuple_dict[i + (br1 * max_header_num)].address,
                                cTemp1->tuple_dict[i + (br1 * max_header_num)].size);

                        tip = cTemp1->tuple_dict[i + (br1 * max_header_num)].type;
                        AK_insert_entry(iBlock, tip, data, broj_td);
                        broj_td += 1;
                    }
                    br1++;
                    j++;

                    if (j < n && br1 < ubr1) { //još ima podataka u cTemp1
                        printf("\nif(j<n){    //još ima podataka u cTemp1  j=%i , n=%i\n", j, n);
                        memset(x, '\0', DATA_ROW_SIZE);
                        memcpy(x, cTemp1->data + cTemp1->tuple_dict[br1 * max_header_num + num_sort_header].address,
                                cTemp1->tuple_dict[br1 * max_header_num + num_sort_header].size);
                        //x[cTemp1->tuple_dict[br1 * max_header_num + num_sort_header].size]="\0";
                    } else {
                        printf("\n\nbreak j<n\n\n");
                        break;
                    }
                } else {
                    Ak_dbg_messg(HIGH, FILE_MAN, "manji je: %s\n", y);

                    //insert data
                    for (i = 0; i < max_header_num; i++) {
                        memset(data, '\0', MAX_VARCHAR_LENGTH);
                        memcpy(data, cTemp2->data + cTemp2->tuple_dict[i + (br2 * max_header_num)].address,
                                cTemp2->tuple_dict[i + (br2 * max_header_num)].size);
                        tip = cTemp2->tuple_dict[i + (br2 * max_header_num)].type;
                        AK_insert_entry(iBlock, tip, data, broj_td);
                        broj_td += 1;
                    }
                    br2++;
                    k++;

                    if (k < n && br2 < ubr2) { //još ima podataka u cTemp2
                        printf("\nif(k<n){    //još ima podataka u cTemp1  k=%i , n=%i\n", k, n);
                        memset(y, '\0', DATA_ROW_SIZE);
                        memcpy(y, cTemp2->data + cTemp2->tuple_dict[br2 * max_header_num + num_sort_header].address,
                                cTemp2->tuple_dict[ br2 * max_header_num + num_sort_header].size);
                        //y[cTemp1->tuple_dict[br2 * max_header_num + num_sort_header].size]="\0";
                    } else {
                        printf("\n\nbreak k<n\n\n");
                        break;
                    }
                }
            }

            printf("\n\nmore data if(j<n) j=%i , n=%i, br1=%i, ubr1=%i\n\n", k, n, br1, ubr1);
            if (j < n && br1 < ubr1) { //još ima podataka u cTemp1
                if (!(br1 < ubr1))
                    break;

                //insert data
                for (i = 0; i < max_header_num; i++) {
                    memset(data, '\0', MAX_VARCHAR_LENGTH);
                    memcpy(data, cTemp1->data + cTemp1->tuple_dict[i + (br1 * max_header_num)].address,
                            cTemp1->tuple_dict[i + (br1 * max_header_num)].size);
                    tip = cTemp1->tuple_dict[i + (br1 * max_header_num)].type;
                    AK_insert_entry(iBlock, tip, data, broj_td);
                    broj_td += 1;
                }
                br1++;
                j++;
            }

            printf("\n\nmore data if(k<n) k=%i , n=%i, br2=%i, ubr2=%i\n\n", k, n, br2, ubr2);
            if (k < n && br2 < ubr2) { //još ima podataka u cTemp2
                if (!(br2 < ubr2))
                    break;
                //insert data
                for (i = 0; i < max_header_num; i++) {
                    memset(data, '\0', MAX_VARCHAR_LENGTH);
                    memcpy(data, cTemp2->data + cTemp2->tuple_dict[i + (br2 * max_header_num)].address,
                            cTemp2->tuple_dict[i + (br2 * max_header_num)].size);
                    tip = cTemp2->tuple_dict[i + (br2 * max_header_num)].type;
                    AK_insert_entry(iBlock, tip, data, broj_td);
                    broj_td += 1;
                }
                br2++;
                k++;
            }

            //ako je jedna od datoteka prazna
            for (; j < n; j++) {
                if (!(br1 < ubr1))
                    break;
                //insert data
                printf("\n\n for(; j<n; j++)  br1=%i    //insert data brtd=%i\n\n", br1, broj_td);
                for (i = 0; i < max_header_num; i++) {
                    memset(data, '\0', MAX_VARCHAR_LENGTH);
                    memcpy(data, cTemp1->data + cTemp1->tuple_dict[i + (br1 * max_header_num)].address,
                            cTemp1->tuple_dict[i + (br1 * max_header_num)].size);
                    tip = cTemp1->tuple_dict[i + (br1 * max_header_num)].type;
                    AK_insert_entry(iBlock, tip, data, broj_td);
                    broj_td += 1;
                }
                br1++;
            }

            for (; k < n; k++) {
                if (!(br2 < ubr2))
                    break;
                //insert data
                printf("\n\n for(; k<n; k++)  br2=%i   //insert data brtd=%i\n\n", br2, broj_td);
                for (i = 0; i < max_header_num; i++) {
                    memset(data, '\0', MAX_VARCHAR_LENGTH);
                    memcpy(data, cTemp2->data + cTemp2->tuple_dict[i + (br2 * max_header_num)].address,
                            cTemp2->tuple_dict[i + (br2 * max_header_num)].size);
                    tip = cTemp2->tuple_dict[i + (br2 * max_header_num)].type;
                    AK_insert_entry(iBlock, tip, data, broj_td);
                    broj_td += 1;
                }
                br2++;
            }
        }
        printf("\n\n KORAK ZAVRSIO \n\n");
        AK_write_block(iBlock); //tu treba nekaj drugo
        printf("   \n\n               in iBlock        data, brtd: %i\n", broj_td);

        for (i = 0; i < broj_td; i++) {
            memset(data, '\0', MAX_VARCHAR_LENGTH);
            memcpy(data, iBlock->data + iBlock->tuple_dict[i].address, iBlock->tuple_dict[i].size);
            printf(" %s ", data);
        }
        printf("\n\n");
    }
    Ak_reset_block(cTemp1);
    Ak_reset_block(cTemp2); //tu treba sad još spremiti

    AK_write_block(cTemp1); //ali mislim da samo ide dirty bit
    AK_write_block(cTemp2);
}

//extern int address_of_tempBlock = 0;
/*
 * @author Unknown
 * @brief Function for file sorting
 * @return No return value
 */
void Ak_filesort_test() {
    printf("filesort_test: Present!\n");
    AK_sort_segment("student", "year");
	AK_print_table("SORT_TEMP_HELP_student");
		
	/*Sort block
	table_addresses * addresses = (table_addresses *)get_table_addresses("student");
	int address = addresses->address_from[0];
	AK_block *temp_block = (AK_block *)AK_read_block(address);
	AK_block_sort(temp_block, "lastname");
	AK_print_table("student");
	*/
}
