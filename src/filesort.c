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
 * @brief total number of headers in the block
 * @return returns num - number of attribute in header (0 - MAX_ATTRIBUTES). USE in tuple_dict[num]...
 */
int get_total_headers(AK_block *iBlock) {
    register int i;
    for (i = 0; i < MAX_ATTRIBUTES; i++) {
        if (strcmp((char *) iBlock->header[i].att_name, "") == 0) //if there is no more attributes
            return i;
    }
    return i;
}

/**
 * @brief number of header in the block which to sort
 * @return returns num - number of attribute in header (0 - MAX_ATTRIBUTES). USE in tuple_dict[num]...
 */
int get_header_number(AK_block *iBlock, char *attribute_name) {
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
 * @brief get tuples number in block
 * @return int
 */
int get_num_of_tuples(AK_block *iBlock) {
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

    int max_header_num = get_total_headers(iBlock);
    return (i / max_header_num);
}

AK_mem_block *get_next_block(int num) {
    AK_mem_block *mem_block = (AK_mem_block *) malloc(sizeof (AK_mem_block));
    mem_block = (AK_mem_block *) AK_get_block(num);

    //AK_block *temp_block = AK_read_block(num); // tu poslije komentirati
    //memcpy(mem_block->block, temp_block, sizeof(AK_block));
    return mem_block;
}

void sort_segment(char *table_name, char *atr_name) {
    table_addresses *addresses = (table_addresses *) get_table_addresses(table_name);

    register int i, j, n, q, k, num_extents;
    int ORG_blokovi[MAX_NUM_OF_BLOCKS];
    int TEMP_blokovi[MAX_NUM_OF_BLOCKS];
    int POMOCNI_blokovi[MAX_NUM_OF_BLOCKS];
    int ORG_brojac = 0;
    int TEMP_brojac = 0;
    int POMOCNI_brojac = 0;
    int ORG_tuples = 0;
    int TEMP_tuples = 0;
    num_extents = 0;

    //Get number of extents and number of blocks for given table
    for (i = 0;; i++) {
        if (addresses->address_from[i] == 0)
            break;

        for (j = addresses->address_from[i]; j <= addresses->address_to[i]; j++) {
            ORG_blokovi[ORG_brojac] = j;
            ORG_brojac++;
        }
        num_extents++;
    }

    if (DEBUG_SORT)
        printf("There are: %i blocks in segment %s, 1. block = %i\n", ORG_brojac, table_name, ORG_blokovi[0]);

    //create the new temp segment which size is equal to the size of the original segment
    char *temp_segment = (char*) malloc(MAX_VARCHAR_LENGHT * sizeof (char));
    strcat(temp_segment, "SORT_TEMP_HELP_");
    strcat(temp_segment, table_name); //npr: SORT_TEMP_PESONS this is the name of temp segment

    AK_block * Hblock = (AK_block *) AK_read_block(ORG_blokovi[0]);
    AK_header *head = (AK_header*) malloc(sizeof (AK_header));
    memcpy(&head, Hblock->header, sizeof (AK_header));

    AK_initialize_new_segment(temp_segment, SEGMENT_TYPE_TABLE, head);

    //init new table extent
    for (i = 1; i < num_extents; i++) {
        AK_init_new_extent(temp_segment, SEGMENT_TYPE_TABLE);
    }

    //search for all blocks in segment
    addresses = (table_addresses *) get_table_addresses(temp_segment);
    TEMP_brojac = 0;

    for (i = 0;; i++) {
        if (addresses->address_from[i] == 0)
            break;
        for (j = addresses->address_from[i]; j <= addresses->address_to[i]; j++) {
            TEMP_blokovi[TEMP_brojac] = j;
            TEMP_brojac++;
        }
    }

    if (DEBUG_SORT)
        printf("There are: %i blocks in segment %s, 1. block = %i\n", TEMP_brojac, temp_segment, TEMP_blokovi[0]);

    //SORTING ...

    char x[DATA_ROW_SIZE]; //bas podatak koji nas zanima
    char y[DATA_ROW_SIZE]; //bas podatak s kojim se uspoređuje
    char data[DATA_TUPLE_SIZE]; //svaki podatak koji se mora kopirati

    int r1, r2;

    AK_mem_block * iCache1 = (AK_mem_block*) malloc(sizeof (AK_mem_block));

    int BR_org1 = 0;
    int iCache1_position = 0;
    iCache1 = get_next_block(ORG_blokovi[BR_org1]);

    int num_tuples_iCache1 = get_num_of_tuples(iCache1->block);

    //posto se headeri isti kod bilo kojeg bloka onda je svejedno u kojem se uzmu ti podaci
    int sort_header_num = get_header_number(iCache1->block, atr_name); //broj headera po kojem se sortira
    int max_header_num = get_total_headers(iCache1->block); //ukupni broj headera za jedan zapis

    AK_mem_block * iCache2 = (AK_mem_block*) malloc(sizeof (AK_mem_block));
    int BR_org2 = 0;
    int iCache2_position = 0;
    //iCache2 = get_next_block(ORG_blokovi[BR_org2]);
    int num_tuples_iCache2 = 0;

    return (0);
    //u taj tek zapisujemo
    AK_mem_block * iCacheTemp = (AK_mem_block*) malloc(sizeof (AK_mem_block));
    int BR_temp = 0;
    int iCacheTemp_position_td = 0; //for tuple dict
    int iCacheTemp_size = 0; // zauzetost iCacheTemp bloka
    iCacheTemp = get_next_block(TEMP_blokovi[BR_temp]);

    // ako je glavni = 1 onda se sortirani blokovi nalaze u adrese_blokova_ORG
    // ako je glavni = 0 onda se sortirani podaci nalaze u adrese_blokova_TEMP
    int glavni = 1;

    for (n = 1; n < ORG_brojac; n = n * 2) {
        for (q = 0; q < ORG_brojac / 2; q += n) {
            BR_org1 = q; //pokazivac na block u prvom dijelu
            BR_org2 = q + n; //pokazivac na block u drugom dijelu
            r1 = q + n; //granična vrijednost blokova u 1 dijelu
            r2 = q + n + n; //granična vrijednost blokova u 2 dijelu

            if (DEBUG_SORT)
                printf("q: %i ,  n: %i  , r1: %i   , r2: %i\n", q, n, r1, r2);

            iCache1 = get_next_block(ORG_blokovi[BR_org1]); //read first block
            num_tuples_iCache1 = get_num_of_tuples(iCache1->block);

            if (DEBUG_SORT)
                printf("procitao sam na poziciji: %i , broj bloka: %i\n", BR_org1, ORG_blokovi[BR_org1]);
            //BR_org1++;

            iCache2 = get_next_block(ORG_blokovi[BR_org2]); //read second block
            num_tuples_iCache2 = get_num_of_tuples(iCache2->block);

            if (DEBUG_SORT)
                printf("procitao sam na poziciji: %i , broj bloka: %i\n", BR_org2, ORG_blokovi[BR_org2]);
            //BR_org2++;

            iCacheTemp = get_next_block(TEMP_blokovi[BR_temp]); //read temp block for write
            if (DEBUG_SORT)
                printf("procitao sam na poziciji: %i , broj temp bloka: %i\n", BR_temp, TEMP_blokovi[BR_temp]);
            //BR_temp++;

            while ((BR_org1 < r1) && (BR_org2 < r2)) {
                //PODATAK PRVI
                memset(x, '\0', DATA_ROW_SIZE);
                memcpy(x, iCache1->block->data + iCache1->block->tuple_dict[iCache1_position * max_header_num + sort_header_num].address,
                        iCache1->block->tuple_dict[iCache1_position * max_header_num + sort_header_num].size);

                //PODATAK DRUGI
                memset(y, '\0', DATA_ROW_SIZE);
                memcpy(y, iCache2->block->data + iCache2->block->tuple_dict[iCache2_position * max_header_num + sort_header_num].address,
                        iCache2->block->tuple_dict[iCache2_position * max_header_num + sort_header_num].size);

                if (DEBUG_SORT)
                    printf(" ------------- slogovi: %s , %s   \n", x, y);

                //usporedba
                if (strcmp(x, y) <= 0) { // x<y
                    if (DEBUG_SORT)
                        printf("                         manji je: %s\n", x);

                    //DA LI JE TEMP BLOCK PUN?
                    //izracun slijedece velicine zapisa
                    for (i = 0; i < max_header_num; i++) {
                        iCacheTemp_size += iCache1->block->tuple_dict[i + (iCache1_position * max_header_num)].size;
                    }

                    //da li podatak stane u blok?
                    // ako je veća od zadane veličine onda treba učitati novi temp blok
                    if ((iCacheTemp->block->free_space + iCacheTemp_size) > 4999) {
                        iCacheTemp->dirty = BLOCK_DIRTY;
                        BR_temp++;
                        iCacheTemp = get_next_block(TEMP_blokovi[BR_temp]);
                        iCacheTemp_position_td = 0;
                    }

                    //KOPIRANJE PODATAKA U TEMP BLOCK
                    for (i = 0; i < max_header_num; i++) {
                        memset(data, '\0', DATA_TUPLE_SIZE);
                        memcpy(data, iCache1->block->data + iCache1->block->tuple_dict[i + (iCache1_position * max_header_num)].address,
                                iCache1->block->tuple_dict[i + (iCache1_position * max_header_num)].size);
                        AK_insert_entry(iCacheTemp->block, iCache1->block->tuple_dict[i + (iCache1_position * max_header_num)].type, data, iCacheTemp_position_td);
                        iCacheTemp_position_td++;
                    }
                    //end: for (i = 0; i < max_header_num; i++)
                    iCache1_position++;

                    //ako se je pročitao zadnji podatak iz iCache1->block
                    if (iCache1_position > num_tuples_iCache1) {
                        BR_org1++; //pokazivač na slijedeći blok za čitanje

                        //ako to nije bio zadnji blok u prolazu
                        if (BR_org1 < r1) {
                            iCache1 = get_next_block(ORG_blokovi[BR_org1]); // učitaj slijedeći blok
                            num_tuples_iCache1 = get_num_of_tuples(iCache1->block); //koliko ima zapisa?
                            iCache1_position = 0; //postavi se na početak učitanog bloka
                        } //end: if( BR_org1 < r1 )
                    } // end: if(iCache1_position > num_tuples_iCache1 )
                }//end: if(strcmp(x,y) <= 0) x<y

                else { //x>=y
                    if (DEBUG_SORT)
                        printf("                      ELSE   manji je: %s\n", x);

                    //DA LI JE TEMP BLOCK PUN?
                    //izracun slijedece velicine zapisa
                    for (i = 0; i < max_header_num; i++) {
                        iCacheTemp_size += iCache2->block->tuple_dict[i + (iCache2_position * max_header_num)].size;
                    }

                    //da li podatak stane u blok?
                    if ((iCacheTemp->block->free_space + iCacheTemp_size) > 4999) { // ako je veća od zadane veličine onda treba učitati novi temp blok
                        iCacheTemp->dirty = BLOCK_DIRTY;
                        BR_temp++;
                        iCacheTemp = get_next_block(TEMP_blokovi[BR_temp]);
                        iCacheTemp_position_td = 0;
                    }

                    //KOPIRANJE PODATAKA U TEMP BLOCK
                    for (i = 0; i < max_header_num; i++) {
                        memset(data, '\0', DATA_TUPLE_SIZE);
                        memcpy(data, iCache2->block->data + iCache2->block->tuple_dict[i + (iCache2_position * max_header_num)].address,
                                iCache2->block->tuple_dict[i + (iCache2_position * max_header_num)].size);
                        AK_insert_entry(iCacheTemp->block, iCache2->block->tuple_dict[i + (iCache2_position * max_header_num)].type, data, iCacheTemp_position_td);
                        iCacheTemp_position_td++;
                    } //end: for (i = 0; i < max_header_num; i++)
                    iCache2_position++;

                    //ako se je pročitao zadnji podatak iz iCache2->block
                    if (iCache2_position > num_tuples_iCache2) {
                        BR_org2++; //pokazivač na slijedeći blok za čitanje

                        //ako to nije bio zadnji blok u prolazu
                        if (BR_org2 < r2) {
                            iCache2 = get_next_block(ORG_blokovi[BR_org2]); // učitaj slijedeći blok
                            num_tuples_iCache2 = get_num_of_tuples(iCache2->block);
                            iCache2_position = 0; //postavi se na početak učitanog bloka
                        } //end: if( BR_org2 < r2 )
                    } //end: if(iCache2_position > num_tuples_iCache2 )
                } //end: else x>=y
            } // end: while( BR_org1 < r1  &&  BR_org2 < r2 )

            //ako su svi podaci iz prvog skupa kopirani tj. u drugom skupu je još ostalo podataka
            if (BR_org1 == r1) {
                while (BR_org2 < r2) {
                    if (DEBUG_SORT)
                        printf("                      ima još podataka u 2. dijelu");

                    //DA LI JE TEMP BLOCK PUN?
                    //izracun slijedece velicine zapisa
                    for (i = 0; i < max_header_num; i++) {
                        iCacheTemp_size += iCache2->block->tuple_dict[i + (iCache2_position * max_header_num)].size;
                    }

                    //da li podatak stane u blok?
                    if ((iCacheTemp->block->free_space + iCacheTemp_size) > 4999) { // ako je veća od zadane veličine onda treba učitati novi temp blok
                        iCacheTemp->dirty = BLOCK_DIRTY;
                        BR_temp++;
                        iCacheTemp = get_next_block(TEMP_blokovi[BR_temp]);
                        iCacheTemp_position_td = 0;
                    }


                    //KOPIRANJE PODATAKA U TEMP BLOCK
                    for (i = 0; i < max_header_num; i++) {
                        memset(data, '\0', DATA_TUPLE_SIZE);
                        memcpy(data, iCache2->block->data + iCache2->block->tuple_dict[i + (iCache2_position * max_header_num)].address,
                                iCache2->block->tuple_dict[i + (iCache2_position * max_header_num)].size);
                        AK_insert_entry(iCacheTemp->block, iCache2->block->tuple_dict[i + (iCache2_position * max_header_num)].type, data, iCacheTemp_position_td);
                        iCacheTemp_position_td++;
                    } //end: for (i = 0; i < max_header_num; i++)
                    iCache2_position++;

                    //ako se je pročitao zadnji podatak iz iCache2->block
                    if (iCache2_position > num_tuples_iCache2) {
                        BR_org2++; //pokazivač na slijedeći blok za čitanje

                        //ako to nije bio zadnji blok u prolazu
                        if (BR_org2 < r2) {
                            iCache2 = get_next_block(ORG_blokovi[BR_org2]); // učitaj slijedeći blok
                            num_tuples_iCache2 = get_num_of_tuples(iCache2->block);
                            iCache2_position = 0; //postavi se na početak učitanog bloka
                        } //end: if( BR_org2 < r2 )
                    }
                } //end: while(BR_org2 < r2)
            }//end: if(BR_org1 == r1)

                //if ( BR_org2 == r2 )  u prvom skupu još postoje podaci
            else {
                if (DEBUG_SORT)
                    printf("                 ima još podataka u 1. dijelu i njih kopiram");

                while (BR_org1 < r1) {

                    //DA LI JE TEMP BLOCK PUN?
                    //izracun slijedece velicine zapisa
                    for (i = 0; i < max_header_num; i++) {
                        iCacheTemp_size += iCache1->block->tuple_dict[i + (iCache1_position * max_header_num)].size;
                    }

                    //da li podatak stane u blok?
                    if ((iCacheTemp->block->free_space + iCacheTemp_size) > 4999) { // ako je veća od zadane veličine onda treba učitati novi temp blok
                        iCacheTemp->dirty = BLOCK_DIRTY;
                        BR_temp++;
                        iCacheTemp = get_next_block(TEMP_blokovi[BR_temp]);
                        iCacheTemp_position_td = 0; //postavljamo se na početak bloka jer smo učitali novi blok
                    }

                    //KOPIRANJE PODATAKA U TEMP BLOCK
                    for (i = 0; i < max_header_num; i++) {
                        memset(data, '\0', DATA_TUPLE_SIZE);
                        memcpy(data, iCache1->block->data + iCache1->block->tuple_dict[i + (iCache1_position * max_header_num)].address,
                                iCache1->block->tuple_dict[i + (iCache1_position * max_header_num)].size);
                        AK_insert_entry(iCacheTemp->block, iCache1->block->tuple_dict[i + (iCache1_position * max_header_num)].type, data, iCacheTemp_position_td);
                        iCacheTemp_position_td++;
                    } //end: for (i = 0; i < max_header_num; i++)
                    iCache1_position++;

                    //ako se je pročitao zadnji podatak iz iCache1->block
                    if (iCache1_position > num_tuples_iCache1) {
                        BR_org1++; //pokazivač na slijedeći blok za čitanje

                        //ako to nije bio zadnji blok u prolazu
                        if (BR_org1 < r1) {
                            iCache1 = get_next_block(ORG_blokovi[BR_org1]); // učitaj slijedeći blok
                            num_tuples_iCache1 = get_num_of_tuples(iCache1->block); //koliko ima zapisa?
                            iCache1_position = 0; //postavi se na početak učitanog bloka
                        } //end: if( BR_org1 < r1 )
                    } // end: if(iCache1_position > num_tuples_iCache1 )
                } //end: while(BR_org1 < r1)
            } //end: else
        } //end: for(q = 0; q < ORG_brojac / 2 ; q = 1 + n )

        //zamjena polja sa blokovima

        // podaci su u adrese_blokova_ORG
        if (glavni) {
            glavni = 0;
            printf("\n\n\n glavni je segment\n\n\n");

            for (i = 0; i < ORG_brojac; i++) {
                POMOCNI_blokovi[i] = ORG_blokovi[i];
                ORG_blokovi[i] = TEMP_blokovi[i];
                TEMP_blokovi[i] = POMOCNI_blokovi[i];
            }

            POMOCNI_brojac = ORG_brojac;
            ORG_brojac = TEMP_brojac;
            TEMP_brojac = POMOCNI_brojac;
        } else {
            printf("\n\n\n glavni je temp\n\n\n");
            glavni = 1;

            for (i = 0; i < ORG_brojac; i++) {
                POMOCNI_blokovi[i] = TEMP_blokovi[i];
                TEMP_blokovi[i] = ORG_blokovi[i];
                ORG_blokovi[i] = POMOCNI_blokovi[i];
            }

            POMOCNI_brojac = TEMP_brojac;
            TEMP_brojac = ORG_brojac;
            ORG_brojac = POMOCNI_brojac;
        }
    } // end : for(n=1; n < ORG_brojac; n = n * 2 )
}

int reset_block(AK_block * iBlock) {
    register int i, j, k;

    AK_header prazni_head[ MAX_ATTRIBUTES ];
    AK_tuple_dict prazni_tuple_dict[ DATA_BLOCK_SIZE ];
    unsigned char prazni_data[ DATA_BLOCK_SIZE * DATA_ENTRY_SIZE ];

    for (i = 0; i < MAX_ATTRIBUTES; i++) {
        prazni_head[i].type = FREE_INT;
        for (j = 0; j < MAX_ATT_NAME; j++) {
            prazni_head[i].att_name[j] = FREE_CHAR;
        }
        for (j = 0; j < MAX_CONSTRAINTS; j++) {
            prazni_head[i].integrity[j] = FREE_INT;
            for (k = 0; k < MAX_CONSTR_NAME; k++) {
                prazni_head[i].constr_name[j][k] = FREE_CHAR;
            }
            for (k = 0; k < MAX_CONSTR_CODE; k++) {
                prazni_head[i].constr_code[j][k] = FREE_CHAR;
            }
        }
    }

    for (i = 0; i < DATA_BLOCK_SIZE; i++) {
        prazni_tuple_dict[i].type = FREE_INT;
        prazni_tuple_dict[i].address = FREE_INT;
        prazni_tuple_dict[i].size = FREE_INT;
    }

    for (i = 0; i < DATA_BLOCK_SIZE * DATA_ENTRY_SIZE; i++) {
        prazni_data[i] = FREE_CHAR;
    }

    iBlock->type = BLOCK_TYPE_FREE;
    iBlock->chained_with = NOT_CHAINED;
    iBlock->free_space = 0;

    memcpy(iBlock->header, prazni_head, sizeof (*prazni_head));
    memcpy(iBlock->tuple_dict, prazni_tuple_dict, sizeof ( *prazni_tuple_dict));
    memcpy(iBlock->data, prazni_data, sizeof (*prazni_data));
}

/**
@author Bakoš Nikola
@version v1.0
Sorts the given block
@param iBlock - block
 */
void block_sort(AK_block * iBlock, char * atr_name) {
    register int i, j, k, n, t, q;

    char x[DATA_ROW_SIZE]; //bas podatak koji nas zanima
    char y[DATA_ROW_SIZE]; //bas podatak s kojim se uspoređuje

    AK_block * cTemp1 = (AK_block*) malloc(sizeof (AK_block));
    cTemp1 = (AK_block *) AK_read_block(15);

    int tip = 0;
    int broj_td;

    AK_block *cTemp2 = (AK_block*) malloc(sizeof (AK_block));
    cTemp2 = (AK_block *) AK_read_block(16);

    AK_header *block_header = (AK_header *) malloc(sizeof (AK_header));
    memcpy(block_header, iBlock->header, sizeof (AK_header));

    int sort_header_num = get_header_number(iBlock, atr_name); //broj headera po kojem se sortira
    //tu provjeriti još funkciju
    int max_header_num = get_total_headers(iBlock); //ukupni broj headera za jedan zapis
    int num_tuples = get_num_of_tuples(iBlock);

    if (DEBUG_SORT)
        printf("\n tu sam: %i, %i", sort_header_num, num_tuples);

    unsigned char data[MAX_VARCHAR_LENGHT]; //it was 2000 before MAX_VARCHAR_LENGHT
    int ubr1 = num_tuples / 2; //ukupni broj 1. polovice
    int ubr2 = num_tuples - ubr1;

    for (n = 1; n < num_tuples; n = n * 2) {
        reset_block(cTemp1);
        reset_block(cTemp2);
        //n=1;

        broj_td = 0;
        //write a half in the first temp block
        for (t = 0; t < ubr1; t++) {
            if (DEBUG)
                printf("block_sort: 1) sada sam na %i / %i\n", t, num_tuples);

            for (i = 0; i < max_header_num; i++) {
                memset(data, '\0', MAX_VARCHAR_LENGHT);
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
            memset(data, '\0', MAX_VARCHAR_LENGHT);
            memcpy(data, cTemp1->data + cTemp1->tuple_dict[i].address, cTemp1->tuple_dict[i].size);
            printf(" %s ", data);
        }
        printf("\n\n");

        //upis druge polovice u drugi temp blok
        broj_td = 0;
        for (; t < num_tuples; t++) {
            if (DEBUG)
                printf("block_sort: 2) sada sam na %i / %i\n", t, num_tuples);

            for (i = 0; i < max_header_num; i++) {
                memset(data, '\0', MAX_VARCHAR_LENGHT);
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
            memset(data, '\0', MAX_VARCHAR_LENGHT);
            memcpy(data, cTemp2->data + cTemp2->tuple_dict[i].address, cTemp2->tuple_dict[i].size);
            printf(" %s ", data);
        }
        printf("\n\n");

        //Start sorting
        reset_block(iBlock);

        if (DEBUG_SORT)
            printf("                      , size: %i\n", iBlock->free_space);

        memcpy(iBlock->header, block_header, sizeof (AK_header));

        int br1, br2;
        broj_td = br1 = br2 = j = k = 0;

        for (q = 0; q < ubr1; q += n) {
            printf("\nfor (q = 0; q < num_tuples/2; q += n ) q=%i , j=%i   ,  k=%i  , br1=%i  , br2=%i\n", q, j, k, br1, br2);
            for (j = k = 0;;) {
                memset(x, '\0', DATA_ROW_SIZE);
                memcpy(x, cTemp1->data + cTemp1->tuple_dict[br1 * max_header_num + sort_header_num].address,
                        cTemp1->tuple_dict[br1 * max_header_num + sort_header_num].size);
                //x[cTemp1->tuple_dict[br1 * max_header_num + sort_header_num].size - 1]="\0";

                memset(y, '\0', DATA_ROW_SIZE);
                memcpy(y, cTemp2->data + cTemp2->tuple_dict[br2 * max_header_num + sort_header_num ].address,
                        cTemp2->tuple_dict[br2 * max_header_num + sort_header_num].size);
                //y[cTemp2->tuple_dict[br2 * max_header_num + sort_header_num].size - 1]="\0";

                if (DEBUG)
                    printf("slogovi: %s , %s   , head: %i \n", x, y, sort_header_num);

                //comparison
                if (strcmp(x, y) <= 0) {
                    if (DEBUG)
                        printf("manji je: %s\n", x);

                    //insert data
                    for (i = 0; i < max_header_num; i++) {
                        memset(data, '\0', MAX_VARCHAR_LENGHT);
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
                        memcpy(x, cTemp1->data + cTemp1->tuple_dict[br1 * max_header_num + sort_header_num].address,
                                cTemp1->tuple_dict[br1 * max_header_num + sort_header_num].size);
                        //x[cTemp1->tuple_dict[br1 * max_header_num + sort_header_num].size]="\0";
                    } else {
                        printf("\n\nbreak j<n\n\n");
                        break;
                    }
                } else {
                    if (DEBUG)
                        printf("manji je: %s\n", y);

                    //insert data
                    for (i = 0; i < max_header_num; i++) {
                        memset(data, '\0', MAX_VARCHAR_LENGHT);
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
                        memcpy(y, cTemp2->data + cTemp2->tuple_dict[br2 * max_header_num + sort_header_num].address,
                                cTemp2->tuple_dict[ br2 * max_header_num + sort_header_num].size);
                        //y[cTemp1->tuple_dict[br2 * max_header_num + sort_header_num].size]="\0";
                    } else {
                        printf("\n\nbreak k<n\n\n");
                        break;
                    }
                }
            }

            printf("\n\njoš podataka if(j<n) j=%i , n=%i, br1=%i, ubr1=%i\n\n", k, n, br1, ubr1);
            if (j < n && br1 < ubr1) { //još ima podataka u cTemp1
                if (!(br1 < ubr1))
                    break;

                //insert data
                for (i = 0; i < max_header_num; i++) {
                    memset(data, '\0', MAX_VARCHAR_LENGHT);
                    memcpy(data, cTemp1->data + cTemp1->tuple_dict[i + (br1 * max_header_num)].address,
                            cTemp1->tuple_dict[i + (br1 * max_header_num)].size);
                    tip = cTemp1->tuple_dict[i + (br1 * max_header_num)].type;
                    AK_insert_entry(iBlock, tip, data, broj_td);
                    broj_td += 1;
                }
                br1++;
                j++;
            }

            printf("\n\njoš podataka if(k<n) k=%i , n=%i, br2=%i, ubr2=%i\n\n", k, n, br2, ubr2);
            if (k < n && br2 < ubr2) { //još ima podataka u cTemp2
                if (!(br2 < ubr2))
                    break;
                //insert data
                for (i = 0; i < max_header_num; i++) {
                    memset(data, '\0', MAX_VARCHAR_LENGHT);
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
                    memset(data, '\0', MAX_VARCHAR_LENGHT);
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
                    memset(data, '\0', MAX_VARCHAR_LENGHT);
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
            memset(data, '\0', MAX_VARCHAR_LENGHT);
            memcpy(data, iBlock->data + iBlock->tuple_dict[i].address, iBlock->tuple_dict[i].size);
            printf(" %s ", data);
        }
        printf("\n\n");
    }
    reset_block(cTemp1);
    reset_block(cTemp2); //tu treba sad još spremiti

    AK_write_block(cTemp1); //ali mislim da samo ide dirty bit
    AK_write_block(cTemp2);
}

extern int address_of_tempBlock = 0;

void filesort_test() {
    printf("filesort_test: Present!\n");
    sort_segment("testna", "Prezime");
}
