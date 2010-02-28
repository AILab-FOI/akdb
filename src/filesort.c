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
#include <string.h>
#include "configuration.h"
#include "dbman.h"
#include "memoman.h"
#include "auxiliary.h"
#include "filesort.h"
#include "files.h"
//#include "fileio.h"



//funkcije upravljanja

/**
total number of headers in block
@return returns num - number of attribute in header (0 - MAX_ATTRIBUTES). USE in tuple_dict[num]...
*/
int get_total_headers(KK_block *iBlock){
	register int i;
	for(i = 0; i < MAX_ATTRIBUTES; i++){
		if(strcmp((char *)iBlock->header[i].att_name,"")==0) //if there is no more attributes
			return i;
	}
	return i;

}


/**
number of header in block witch to sort
@return returns num - number of attribute in header (0 - MAX_ATTRIBUTES). USE in tuple_dict[num]...
*/
int get_header_number(KK_block *iBlock, char *attribute_name){
	register int i;
	if(DEBUG_SORT)
		printf("\natribut: %s\n", attribute_name);
	for(i = 0; i < MAX_ATTRIBUTES; i++){
		if(!strcmp((char *)iBlock->header[i].att_name , attribute_name)){
			return i;
		}
		else if(!strcmp((char *)iBlock->header[i].att_name , "/0")){		//if there is no more attributes
			printf("get_header_number: ERROR: in this table does not exist atribute: %s", attribute_name);
			return (EXIT_ERROR);
		}
	}
	printf("get_header_number: ERROR: in this table does not exist atribute: %s", attribute_name);
	return (EXIT_ERROR);
}

/**
vrati broj zapisa u bloku

*/
int get_num_of_tuples(KK_block *iBlock){

	int i=0;
	int kraj = 1;
	while( kraj ) {
		i ++;
		if( iBlock->tuple_dict[ i ].size == 0 ){
			kraj = 0;
		}	
	}
	
	if(i == 0)
		return 0;
	
	int max_header_num = get_total_headers(iBlock);
	return (i / max_header_num);
}

/**
ucita slijedeci block s obzirom da funkcija za to još ne radi. Ova služi kao pomoćna a kasnije i za lakšu zamjenu
*/

KK_mem_block * get_next_block(int num){

	KK_mem_block * iCache = (KK_mem_block * )malloc(sizeof(KK_mem_block));
	//iCache=KK_get_block(num);

	KK_block * iBlock = KK_read_block(num); // tu poslije komentirati

	memcpy(iCache->block, iBlock , sizeof (KK_block));  // i ovo zakomentirati

	return iCache;

}







extern int address_of_tempBlock = 0;
void filesort_test(){
	printf( "filesort_test: Present!\n" );
	
	
	sort_segment("testna", "Prezime");
}


void sort_segment(char * table_name, char * atr_name){
	table_addresses * adrese = get_table_addresses(table_name);
	
	int ORG_blokovi[MAX_NUM_OF_BLOCKS];
	int TEMP_blokovi[MAX_NUM_OF_BLOCKS];
	int POMOCNI_blokovi[MAX_NUM_OF_BLOCKS];
	register int i,j,n,q,k, num_extents;
	int ORG_brojac=0;
	int TEMP_brojac=0;
	int POMOCNI_brojac=0;
	int ORG_tuples=0;
	int TEMP_tuples=0;
	num_extents=0;

	//SEARCH FOR ALL BLOCKS IN SEGMENT
	for (i=0; ;i++){
		if(adrese->address_from[i] == 0) break;
		for(j = adrese->address_from[i]; j <= adrese->address_to[i]; j++){
			ORG_blokovi[ORG_brojac] = j;
			ORG_brojac++;
		}
		num_extents++;
	}
	if(DEBUG_SORT)
		printf("Ukupno ima: %i blokova u segmentu %s     1. blok=%i\n",ORG_brojac,table_name,ORG_blokovi[0]);
	
	
	//CREATE NEW TEMP SEGMENT WITCH SIZE IS EQUAL TO SIZE OF ORGINAL SEGMENT
	char *temp_segment;
	strcat(temp_segment,"SORT_TEMP_HELP_");
	strcat(temp_segment,table_name);		//npr: SORT_TEMP_PESONS this is the name of temp segment
	
	KK_block * Hblock = KK_read_block(ORG_blokovi[0]);
	KK_header *head = (KK_header*) malloc (sizeof(KK_header));
	memcpy(&head,Hblock->header,sizeof(KK_header));
	
	KK_initialize_new_segment(temp_segment,1,head);
	
	for (i=1; i<num_extents; i++){
		KK_init_new_extent(temp_segment, 1);		//init new table extent 
		
	}
	
	//SEARCH FOR ALL BLOCKS IN SEGMENT
	adrese = get_table_addresses(temp_segment);
	TEMP_brojac=0;
	for (i=0; ;i++){
		if(adrese->address_from[i] == 0) break;
		for(j = adrese->address_from[i]; j <= adrese->address_to[i]; j++){
			TEMP_blokovi[TEMP_brojac] = j;
			TEMP_brojac++;
		}
	}
	if(DEBUG_SORT)
		printf("Ukupno ima: %i blokova u segmentu %s     1. blok=%i\n",TEMP_brojac,temp_segment,TEMP_blokovi[0]);
	
	
	//SORTING ...
	
	char x[DATA_ROW_SIZE]; //bas podatak koji nas zanima
	char y[DATA_ROW_SIZE]; //bas podatak s kojim se uspoređuje
	char podatak[DATA_TUPLE_SIZE];  //svaki podatak koji se mora kopirati

	int r1, r2, free;
	
	
	KK_mem_block * iCache1 = (KK_mem_block*) malloc (sizeof(KK_mem_block));
	

	int BR_org1 = 0;
	int iCache1_position = 0;
	iCache1 = get_next_block(ORG_blokovi[BR_org1]);
	
	
	int num_tuples_iCache1 = get_num_of_tuples( iCache1->block ); 

	
			//posto se headeri isti kod bilo kojeg bloka onda je svejedno u kojem se uzmu ti podaci
	int sort_header_num = get_header_number(iCache1->block, atr_name);    //broj headera po kojem se sortira
	
	int max_header_num = get_total_headers( iCache1->block );				//ukupni broj headera za jedan zapis
	
	KK_mem_block * iCache2 = (KK_mem_block*) malloc (sizeof(KK_mem_block));
	int BR_org2 = 0;
	int iCache2_position=0;  
	//iCache2 = get_next_block(ORG_blokovi[BR_org2]);
	int num_tuples_iCache2 = 0;
	
	
	return(0);	
	//u taj tek zapisujemo
	KK_mem_block * iCacheTemp = (KK_mem_block*) malloc (sizeof(KK_mem_block));
	int BR_temp=0;
	int iCacheTemp_position_td=0; //for tuple dict
	int iCacheTemp_size = 0;   // zauzetost iCacheTemp bloka
    iCacheTemp = get_next_block(TEMP_blokovi[BR_temp]);
	
	// ako je glavni = 1 onda se sortirani blokovi nalaze u adrese_blokova_ORG
	// ako je glavni = 0 onda se sortirani podaci nalaze u adrese_blokova_TEMP
	int glavni = 1;
	
	for(n=1; n < ORG_brojac; n = n * 2 ){
		
		
		for(q = 0; q < ORG_brojac / 2 ; q += n ){
			BR_org1=q;		//pokazivac na block u prvom dijelu
			BR_org2=q+n;		//pokazivac na block u drugom dijelu
			r1=q+n;		//granična vrijednost blokova u 1 dijelu
			r2=q+n+n; 	//granična vrijednost blokova u 2 dijelu
			
			if(DEBUG_SORT)
				printf("q: %i ,  n: %i  , r1: %i   , r2: %i\n",q,n,r1,r2);
			
			iCache1 = get_next_block(ORG_blokovi[BR_org1]);  //read first block
			num_tuples_iCache1 = get_num_of_tuples( iCache1->block );
			
			if(DEBUG_SORT)
				printf("procitao sam na poziciji: %i , broj bloka: %i\n",BR_org1, ORG_blokovi[BR_org1]);
			//BR_org1++;
			
			iCache2 = get_next_block(ORG_blokovi[BR_org2]);  //read second block
			num_tuples_iCache2 = get_num_of_tuples( iCache2->block );
			
			if(DEBUG_SORT)
				printf("procitao sam na poziciji: %i , broj bloka: %i\n",BR_org2, ORG_blokovi[BR_org2]);
			//BR_org2++;
			
			iCacheTemp = get_next_block(TEMP_blokovi[BR_temp]);   //read temp block for write
			if(DEBUG_SORT)
				printf("procitao sam na poziciji: %i , broj temp bloka: %i\n",BR_temp, TEMP_blokovi[BR_temp]);
			//BR_temp++;
			
			while ( ( BR_org1 < r1 )  &&  ( BR_org2 < r2 ) ){
				
				//PODATAK PRVI
				//čišćenje varijable
					for(free=0; free < DATA_ROW_SIZE; free++)
						x[free]='\0';
				//uzimanje podatka iz iCache1
				memcpy(x,
					   iCache1->block->data + iCache1->block->tuple_dict[ iCache1_position * max_header_num + sort_header_num ].address,
					   iCache1->block->tuple_dict[ iCache1_position * max_header_num + sort_header_num ].size);
				
				//PODATAK DRUGI
				//čišćenje varijable
					for(free=0; free < DATA_ROW_SIZE; free++)
						y[free]='\0';
				//uzimanje podatka iz iCache1
				memcpy(y,
					   iCache2->block->data + iCache2->block->tuple_dict[ iCache2_position * max_header_num + sort_header_num ].address,
					   iCache2->block->tuple_dict[ iCache2_position * max_header_num + sort_header_num ].size);
				
				if(DEBUG_SORT)
					printf(" ------------- slogovi: %s , %s   \n",x,y);
				
				//usporedba
				if(strcmp(x,y) <= 0){		// x<y
					if(DEBUG_SORT)
						printf("                         manji je: %s\n",x);
	
//DA LI JE TEMP BLOCK PUN?
					//izracun slijedece velicine zapisa
					for (i = 0; i < max_header_num; i++){
						iCacheTemp_size += iCache1->block->tuple_dict[ i + (iCache1_position * max_header_num) ].size;
					}
					
					//da li podatak stane u blok?
					if( ( iCacheTemp->block->free_space + iCacheTemp_size ) > 4999 ){  // ako je veća od zadane veličine onda treba učitati novi temp blok
						iCacheTemp->dirty = BLOCK_DIRTY;
						BR_temp++;
						iCacheTemp = get_next_block(TEMP_blokovi[BR_temp]);
						iCacheTemp_position_td = 0;
						
					}
					
					//KOPIRANJE PODATAKA U TEMP BLOCK
					for (i = 0; i < max_header_num; i++){
							for( free = 0 ; free < DATA_TUPLE_SIZE ; free++ )
								podatak[free]='\0';
						
						memcpy(podatak,
							   iCache1->block->data + iCache1->block->tuple_dict[ i + (iCache1_position * max_header_num) ].address,
							   iCache1->block->tuple_dict[ i + (iCache1_position * max_header_num) ].size);
						
						
						KK_insert_entry(iCacheTemp->block, 
										iCache1->block->tuple_dict[ i + (iCache1_position * max_header_num) ].type, 
										podatak, 
										iCacheTemp_position_td );
						
						iCacheTemp_position_td++;
						
					} 
					//end: for (i = 0; i < max_header_num; i++)
					
					iCache1_position++;
					
					if(iCache1_position > num_tuples_iCache1 ){			// ako se je pročitao zadnji podatak iz iCache1->block
						BR_org1++;  //pokazivač na slijedeći blok za čitanje
						
						if( BR_org1 < r1 ){		// ako to nije bio zadnji blok u prolazu
							iCache1 = get_next_block(ORG_blokovi[BR_org1]);  // učitaj slijedeći blok
							num_tuples_iCache1 = get_num_of_tuples( iCache1->block );  //koliko ima zapisa?
							iCache1_position = 0; 	//postavi se na početak učitanog bloka
						} 
						//end: if( BR_org1 < r1 )	
						
					}
					// end: if(iCache1_position > num_tuples_iCache1 )
					
				}
				//end: if(strcmp(x,y) <= 0) x<y
				
				else{ //x>=y
					if(DEBUG_SORT)
						printf("                      ELSE   manji je: %s\n",x);
					
//DA LI JE TEMP BLOCK PUN?
					//izracun slijedece velicine zapisa
					for (i = 0; i < max_header_num; i++){
						iCacheTemp_size += iCache2->block->tuple_dict[ i + (iCache2_position * max_header_num) ].size;
					}
					
					//da li podatak stane u blok?
					if( ( iCacheTemp->block->free_space + iCacheTemp_size ) > 4999 ){  // ako je veća od zadane veličine onda treba učitati novi temp blok
						iCacheTemp->dirty = BLOCK_DIRTY;
						BR_temp++;
						iCacheTemp = get_next_block(TEMP_blokovi[BR_temp]);
						iCacheTemp_position_td = 0;
					}
					

					//KOPIRANJE PODATAKA U TEMP BLOCK
					for (i = 0; i < max_header_num; i++){
							for( free = 0 ; free < DATA_TUPLE_SIZE ; free++ )
								podatak[free]='\0';
						
						memcpy(podatak,
							   iCache2->block->data + iCache2->block->tuple_dict[ i + (iCache2_position * max_header_num) ].address,
							   iCache2->block->tuple_dict[ i + (iCache2_position * max_header_num) ].size);
						
						
						KK_insert_entry(iCacheTemp->block, 
										iCache2->block->tuple_dict[ i + (iCache2_position * max_header_num) ].type, 
										podatak, 
										iCacheTemp_position_td );
						
						iCacheTemp_position_td++;
						
						
					} 
					//end: for (i = 0; i < max_header_num; i++)
					
					iCache2_position++;
					
					if(iCache2_position > num_tuples_iCache2 ){			// ako se je pročitao zadnji podatak iz iCache2->block
						BR_org2++;  //pokazivač na slijedeći blok za čitanje
						
						if( BR_org2 < r2 ){		// ako to nije bio zadnji blok u prolazu
							iCache2 = get_next_block(ORG_blokovi[BR_org2]);  // učitaj slijedeći blok
							num_tuples_iCache2 = get_num_of_tuples( iCache2->block ); 
							iCache2_position = 0; 	//postavi se na početak učitanog bloka
						} 
						//end: if( BR_org2 < r2 )	
						
					}
					// end: if(iCache2_position > num_tuples_iCache2 )

				} 
				// end: else x>=y
							
			}
			// end: while( BR_org1 < r1  &&  BR_org2 < r2 )
			
			
			
			if( BR_org1 == r1 ){//ako su svi podaci iz prvog skupa kopirani tj. u drugom skupu je još ostalo podataka
				
				while( BR_org2 < r2 ){
					
					if(DEBUG_SORT)
						printf("                      ima još podataka u 2. dijelu");
					
//DA LI JE TEMP BLOCK PUN?
					//izracun slijedece velicine zapisa
					for (i = 0; i < max_header_num; i++){
						iCacheTemp_size += iCache2->block->tuple_dict[ i + (iCache2_position * max_header_num) ].size;
					}
					
					//da li podatak stane u blok?
					if( ( iCacheTemp->block->free_space + iCacheTemp_size ) > 4999 ){  // ako je veća od zadane veličine onda treba učitati novi temp blok
						iCacheTemp->dirty = BLOCK_DIRTY;
						BR_temp++;
						iCacheTemp = get_next_block(TEMP_blokovi[BR_temp]);
						iCacheTemp_position_td = 0;
					}
					

					//KOPIRANJE PODATAKA U TEMP BLOCK
					for (i = 0; i < max_header_num; i++){
							for( free = 0 ; free < DATA_TUPLE_SIZE ; free++ )
								podatak[free]='\0';
						
						memcpy(podatak,
							   iCache2->block->data + iCache2->block->tuple_dict[ i + (iCache2_position * max_header_num) ].address,
							   iCache2->block->tuple_dict[ i + (iCache2_position * max_header_num) ].size);
						
						
						KK_insert_entry(iCacheTemp->block, 
										iCache2->block->tuple_dict[ i + (iCache2_position * max_header_num) ].type, 
										podatak, 
										iCacheTemp_position_td );
						
						iCacheTemp_position_td++;
						
						
					} 
				//end: for (i = 0; i < max_header_num; i++)
					
					iCache2_position++;
					
					if(iCache2_position > num_tuples_iCache2 ){			// ako se je pročitao zadnji podatak iz iCache2->block
						BR_org2++;  //pokazivač na slijedeći blok za čitanje
						
						if( BR_org2 < r2 ){		// ako to nije bio zadnji blok u prolazu
							iCache2 = get_next_block(ORG_blokovi[BR_org2]);  // učitaj slijedeći blok
							num_tuples_iCache2 = get_num_of_tuples( iCache2->block ); 
							iCache2_position = 0; 	//postavi se na početak učitanog bloka
						} 
						//end: if( BR_org2 < r2 )	
						
					}
			
				} 
				//end: while(BR_org2 < r2)
			
				
			
				
			} 
			//end: if(BR_org1 == r1)
			
			
			else{// if ( BR_org2 == r2 )  u prvom skupu još postoje podaci
	
				if(DEBUG_SORT)
					printf("                 ima još podataka u 1. dijelu i njih kopiram");
				
				while( BR_org1 < r1 ){
					
//DA LI JE TEMP BLOCK PUN?
					//izracun slijedece velicine zapisa
					for (i = 0; i < max_header_num; i++){
						iCacheTemp_size += iCache1->block->tuple_dict[ i + (iCache1_position * max_header_num) ].size;
					}
					
					//da li podatak stane u blok?
					if( ( iCacheTemp->block->free_space + iCacheTemp_size ) > 4999 ){  // ako je veća od zadane veličine onda treba učitati novi temp blok
						iCacheTemp->dirty = BLOCK_DIRTY;
						BR_temp++;
						iCacheTemp = get_next_block(TEMP_blokovi[BR_temp]);
						iCacheTemp_position_td = 0;			//postavljamo se na početak bloka jer smo učitali novi blok
						
					}
						
					
					//KOPIRANJE PODATAKA U TEMP BLOCK
					for (i = 0; i < max_header_num; i++){
							for( free = 0 ; free < DATA_TUPLE_SIZE ; free++ )
								podatak[free]='\0';
						
						memcpy(podatak,
							   iCache1->block->data + iCache1->block->tuple_dict[ i + (iCache1_position * max_header_num) ].address,
							   iCache1->block->tuple_dict[ i + (iCache1_position * max_header_num) ].size);
						
						
						KK_insert_entry(iCacheTemp->block, 
										iCache1->block->tuple_dict[ i + (iCache1_position * max_header_num) ].type, 
										podatak, 
										iCacheTemp_position_td );
						
						iCacheTemp_position_td++;
						
					} 
						//end: for (i = 0; i < max_header_num; i++)
					
					iCache1_position++;
					
					if(iCache1_position > num_tuples_iCache1 ){			// ako se je pročitao zadnji podatak iz iCache1->block
						BR_org1++;  //pokazivač na slijedeći blok za čitanje
						
						if( BR_org1 < r1 ){		// ako to nije bio zadnji blok u prolazu
							iCache1 = get_next_block(ORG_blokovi[BR_org1]);  // učitaj slijedeći blok
							num_tuples_iCache1 = get_num_of_tuples( iCache1->block );  //koliko ima zapisa?
							iCache1_position = 0; 	//postavi se na početak učitanog bloka
						} 
						//end: if( BR_org1 < r1 )	
						
					}
						// end: if(iCache1_position > num_tuples_iCache1 )
					
				} 
				//end: while(BR_org1 < r1)
				
			} 
			//end: else
			
		}	
		
		//end: for(q = 0; q < ORG_brojac / 2 ; q = 1 + n )
		
		//zamjena polja sa blokovima
		
		if(glavni){ // podaci su u adrese_blokova_ORG
			glavni = 0;
			printf("\n\n\n glavni je segment\n\n\n");
			for (i=0; i< ORG_brojac; i++) {
				POMOCNI_blokovi[i] = ORG_blokovi[i];
				ORG_blokovi[i] = TEMP_blokovi[i];
				TEMP_blokovi[i] = POMOCNI_blokovi[i];
				
			}
			POMOCNI_brojac = ORG_brojac;
			ORG_brojac = TEMP_brojac;
			TEMP_brojac = POMOCNI_brojac;
		}
		else{
				printf("\n\n\n glavni je temp\n\n\n");
				glavni = 1 ;
			for(i=0; i< ORG_brojac; i++){
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




int reset_block( KK_block * iBlock )
{
	register int i, j, k;
	
	KK_header prazni_head[ MAX_ATTRIBUTES ];
	KK_tuple_dict prazni_tuple_dict[ DATA_BLOCK_SIZE ];
	unsigned char prazni_data[ DATA_BLOCK_SIZE * DATA_ENTRY_SIZE ];
	
	for( i = 0; i < MAX_ATTRIBUTES; i++ )
	{
		prazni_head[ i ].type = FREE_INT;
		for( j = 0; j < MAX_ATT_NAME; j++ )
		{
			prazni_head[ i ].att_name[ j ] = FREE_CHAR;
		}
		for( j = 0; j < MAX_CONSTRAINTS; j++ )
		{
			prazni_head[ i ].integrity[ j ] = FREE_INT;
			for( k = 0; k < MAX_CONSTR_NAME; k++ )
			{
				prazni_head[ i ].constr_name[ j ][ k ] = FREE_CHAR;
			}
			for( k = 0; k < MAX_CONSTR_CODE; k++ )
			{
				prazni_head[ i ].constr_code[ j ][ k ] = FREE_CHAR;
			}
		}
	}

	for( i = 0; i < DATA_BLOCK_SIZE; i++ )
	{
		prazni_tuple_dict[ i ].type = FREE_INT;
		prazni_tuple_dict[ i ].address = FREE_INT;
		prazni_tuple_dict[ i ].size = FREE_INT;		
	}
	for( i = 0; i < DATA_BLOCK_SIZE * DATA_ENTRY_SIZE; i++ )
	{
		prazni_data[ i ] = FREE_CHAR;
	}
	
	iBlock->type = BLOCK_TYPE_FREE;
	iBlock->chained_with = NOT_CHAINED;
	iBlock->free_space = 0;
	memcpy( iBlock->header, prazni_head, sizeof( *prazni_head ) );
	memcpy( iBlock->tuple_dict, prazni_tuple_dict, sizeof( *prazni_tuple_dict ) );
	memcpy( iBlock->data, prazni_data, sizeof( *prazni_data ) );	
	
}





/**
@author Bakoš Nikola
@version v1.0
Sorts the given block
@param iBlock - block
*/


void block_sort(KK_block * iBlock, char * atr_name){
	register int t, n, j, k, q, i;
	
	char x[DATA_ROW_SIZE]; //bas podatak koji nas zanima
	char y[DATA_ROW_SIZE]; //bas podatak s kojim se uspoređuje
	int tip=0;
	KK_block * cTemp1 = (KK_block*) malloc (sizeof(KK_block));
	cTemp1 = KK_read_block(15);
	int free;
	int broj_td;
	
	KK_block * cTemp2 = (KK_block*) malloc (sizeof(KK_block));
	cTemp2 = KK_read_block(16);
	
	KK_header * block_header = (KK_header * ) malloc (sizeof(KK_header));
	memcpy(block_header,  iBlock->header,  sizeof(KK_header));
	
	int sort_header_num = get_header_number(iBlock, atr_name);    //broj headera po kojem se sortira
	
//tu provjeriti još funkciju	
	int max_header_num = get_total_headers( iBlock );				//ukupni broj headera za jedan zapis
	
	
	int num_tuples = get_num_of_tuples( iBlock ); 
	if(DEBUG_SORT)
		printf ("\n tu sam: %i, %i", sort_header_num, num_tuples);
	
	unsigned char podatak[2000];

	int ubr1= num_tuples/2;				//ukupni broj 1. polovice
	int ubr2= num_tuples - ubr1;
	
	for( n = 1; n < num_tuples; n = n * 2 ){
		
		
		reset_block(cTemp1);						//RESET BLOKOVA
		reset_block(cTemp2);
	//n=1;
		
		broj_td=0;
		//upis polovice u prvi temp blok
		for( t = 0; t < ubr1; t++ ){
			if(DEBUG)
				printf("block_sort: 1) sada sam na %i / %i\n", t, num_tuples );

			for (i = 0; i < max_header_num; i++){
				free=0;
							for(free;free<2000;free++)
								podatak[free]='\0';
				memcpy(podatak,
					   iBlock->data + iBlock->tuple_dict[ i + (t * max_header_num) ].address,
					   iBlock->tuple_dict[ i + (t * max_header_num) ].size);
				//printf ("                          podatak: %s      na: %i \n", podatak, broj_td);
				KK_insert_entry(cTemp1, iBlock->tuple_dict[ i + (t * max_header_num) ].type, podatak, broj_td );
				broj_td+=1;					
			}
			
								
		}
		memcpy(cTemp1->header, block_header, sizeof(KK_header));
		KK_write_block(cTemp1);				//tu treba nekaj napraviti
		printf ("   \n\n               u cTemp1        podatak, brtd: %i\n",broj_td);
		
		for(i=0;i<broj_td; i++){
			free=0;
							for(free;free<2000;free++)
								podatak[free]='\0';
			memcpy(podatak,
					   cTemp1->data + cTemp1->tuple_dict[ i ].address,
					   cTemp1->tuple_dict[ i ].size);
				printf (" %s ", podatak);
		}
		printf("\n\n");
		
		
		//upis druge polovice u drugi temp blok
		broj_td=0;
		for(; t <  num_tuples; t++ ){
			if(DEBUG)
				printf("block_sort: 2) sada sam na %i / %i\n", t, num_tuples );

			for (i = 0; i < max_header_num; i++){
				free=0;
							for(free;free<2000;free++)
								podatak[free]='\0';
				memcpy(podatak,
					   iBlock->data + iBlock->tuple_dict[ i + (t * max_header_num) ].address,
					   iBlock->tuple_dict[ i + (t * max_header_num) ].size);
				//printf ("                          podatak: %s      na: %i  adresa: %i\n", podatak, broj_td, iBlock->tuple_dict[ i + (t * max_header_num) ].address);
				
				KK_insert_entry(cTemp2, iBlock->tuple_dict[ i + (t * max_header_num) ].type, podatak, broj_td );
				broj_td+=1;						
			}
			
								
		}
		memcpy(cTemp2->header, block_header, sizeof(KK_header));
		if ( KK_write_block(cTemp2) == EXIT_ERROR)			//tu isto treba nekaj napraviti
			printf("cTemp2 GREŠKA UPISA\n");
		printf ("   \n\n               u cTemp2        podatak, brtd: %i\n",broj_td);
		for(i=0;i<broj_td; i++){
			free=0;
							for(free;free<2000;free++)
								podatak[free]='\0';
				memcpy(podatak,
					   cTemp2->data + cTemp2->tuple_dict[ i ].address,
					   cTemp2->tuple_dict[ i ].size);
				printf (" %s ", podatak);
		}
		printf("\n\n");
		
		
		//TU POCNE SORTIRANJE
		
		
		reset_block(iBlock);			//RESET BLOKA

		if(DEBUG_SORT)
			printf("                      , velicina: %i\n", iBlock->free_space);
		
		memcpy(iBlock->header,  block_header,  sizeof(KK_header));
		
		
		broj_td=0;
		j=k=0;
		int br1=0;  int br2=0;
		
		for (q = 0; q < ubr1; q += n ){
			
			printf("\nfor (q = 0; q < num_tuples/2; q += n ) q=%i , j=%i   ,  k=%i  , br1=%i  , br2=%i\n", q,j,k,br1,br2);
			
				
			
			for(j=k=0; ; ){
				
						free=0;
							for(free;free<DATA_ROW_SIZE;free++)
								x[free]='\0';
				memcpy(x,
					   cTemp1->data + cTemp1->tuple_dict[ br1 * max_header_num + sort_header_num ].address,
					   cTemp1->tuple_dict[br1 * max_header_num + sort_header_num].size);
				//x[cTemp1->tuple_dict[br1 * max_header_num + sort_header_num].size - 1]="\0";
				
				free=0;
							for(free;free<DATA_ROW_SIZE;free++)
								y[free]='\0';
				memcpy(y,
					   cTemp2->data + cTemp2->tuple_dict[br2 * max_header_num + sort_header_num ].address,
					   cTemp2->tuple_dict[br2 * max_header_num + sort_header_num].size);
				//y[cTemp2->tuple_dict[br2 * max_header_num + sort_header_num].size - 1]="\0";
				
				if(DEBUG)
					printf("slogovi: %s , %s   , head: %i \n",x,y,sort_header_num);
				
				//usporedba
				if(strcmp(x,y)<=0){
					if(DEBUG)
						printf("manji je: %s\n",x);
					
					//unosi podatke
					for (i = 0; i < max_header_num; i++){
						free=0;
							for(free;free<2000;free++)
								podatak[free]='\0';
						memcpy(podatak,
							   cTemp1->data + cTemp1->tuple_dict[ i + (br1 * max_header_num) ].address,
							   cTemp1->tuple_dict[ i + (br1 * max_header_num) ].size);
						
						tip = cTemp1->tuple_dict[ i + (br1 * max_header_num) ].type;
						KK_insert_entry(iBlock, tip, podatak, broj_td );
						broj_td+=1;						
					}
					br1++;
					j++;
					
					if(j<n && br1<ubr1){    //još ima podataka u cTemp1
						printf("\nif(j<n){    //još ima podataka u cTemp1  j=%i , n=%i\n",j,n);
						
						free=0;
							for(free;free<DATA_ROW_SIZE;free++)
								x[free]='\0';
						memcpy(x,
					   			cTemp1->data + cTemp1->tuple_dict[ br1 * max_header_num + sort_header_num ].address,
					   			cTemp1->tuple_dict[br1 * max_header_num + sort_header_num].size);
						//x[cTemp1->tuple_dict[br1 * max_header_num + sort_header_num].size]="\0";
						
					}
					else{
						printf("\n\nbreak j<n\n\n");
						break;
					}
					
				}
				else{
					if(DEBUG)
						printf("manji je: %s\n",y);
					
					//unosi podatke
					for (i = 0; i < max_header_num; i++){
						free=0;
							for(free;free<2000;free++)
								podatak[free]='\0';
						
						memcpy(podatak,
							   cTemp2->data + cTemp2->tuple_dict[ i + (br2 * max_header_num) ].address,
							   cTemp2->tuple_dict[ i + (br2 * max_header_num) ].size);
						tip = cTemp2->tuple_dict[ i + (br2 * max_header_num) ].type;
						KK_insert_entry(iBlock, tip, podatak, broj_td );
						broj_td+=1;						
					}
					br2++;
					k++;
					
					if(k<n && br2<ubr2){    //još ima podataka u cTemp2
						printf("\nif(k<n){    //još ima podataka u cTemp1  k=%i , n=%i\n",k,n);
						free=0;
							for(free;free<DATA_ROW_SIZE;free++)
								y[free]='\0';
						memcpy(y,
					   			cTemp2->data + cTemp2->tuple_dict[ br2 * max_header_num + sort_header_num ].address,
					   			cTemp2->tuple_dict[ br2 * max_header_num + sort_header_num].size);
						//y[cTemp1->tuple_dict[br2 * max_header_num + sort_header_num].size]="\0";
						
					}
					else{
						printf("\n\nbreak k<n\n\n");
						break;
					}
					
				
				}
				
			}
			printf("\n\njoš podataka if(j<n) j=%i , n=%i, br1=%i, ubr1=%i\n\n",k,n,br1,ubr1);
				if(j<n && br1<ubr1){    //još ima podataka u cTemp1
						if(!(br1<ubr1)) break;
						//unosi podatke
					for (i = 0; i < max_header_num; i++){
						free=0;
							for(free;free<2000;free++)
								podatak[free]='\0';
						memcpy(podatak,
							   cTemp1->data + cTemp1->tuple_dict[ i + (br1 * max_header_num) ].address,
							   cTemp1->tuple_dict[ i + (br1 * max_header_num) ].size);
						tip = cTemp1->tuple_dict[ i + (br1 * max_header_num) ].type;
						KK_insert_entry(iBlock, tip, podatak, broj_td );
						broj_td+=1;						
					}
					br1++;
					j++;
						
				}
				printf("\n\njoš podataka if(k<n) k=%i , n=%i, br2=%i, ubr2=%i\n\n",k,n,br2,ubr2);
				if(k<n && br2<ubr2){    //još ima podataka u cTemp2
						if (!(br2<ubr2)) break;
						//unosi podatke
					for (i = 0; i < max_header_num; i++){
						free=0;
							for(free;free<2000;free++)
								podatak[free]='\0';
						memcpy(podatak,
							   cTemp2->data + cTemp2->tuple_dict[ i + (br2 * max_header_num) ].address,
							   cTemp2->tuple_dict[ i + (br2 * max_header_num) ].size);
						tip = cTemp2->tuple_dict[ i + (br2 * max_header_num) ].type;
						KK_insert_entry(iBlock, tip, podatak, broj_td );
						broj_td+=1;						
					}
					br2++;
					k++;
						
				}
			
			
			
			//ako je jedna od datoteka prazna
			for(; j<n; j++){
				if (!(br1<ubr1)) break;
				//unosi podatke
				printf("\n\n for(; j<n; j++)  br1=%i    //unosi podatke  brtd=%i\n\n",br1,broj_td);
					for (i = 0; i < max_header_num; i++){
						free=0;
							for(free;free<2000;free++)
								podatak[free]='\0';
						memcpy(podatak,
							   cTemp1->data + cTemp1->tuple_dict[ i + (br1 * max_header_num) ].address,
							   cTemp1->tuple_dict[ i + (br1 * max_header_num) ].size);
						tip = cTemp1->tuple_dict[ i + (br1 * max_header_num) ].type;
						KK_insert_entry(iBlock, tip, podatak, broj_td );
						broj_td+=1;						
				
					}
				br1++;
			
			}
			
			for(; k<n; k++){
				if (!(br2<ubr2)) break;
				//unosi podatke
				printf("\n\n for(; k<n; k++)  br2=%i   //unosi podatke  brtd=%i\n\n",br2,broj_td);
					for (i = 0; i < max_header_num; i++){
						free=0;
							for(free;free<2000;free++)
								podatak[free]='\0';
						memcpy(podatak,
							   cTemp2->data + cTemp2->tuple_dict[ i + (br2 * max_header_num) ].address,
							   cTemp2->tuple_dict[ i + (br2 * max_header_num) ].size);
						tip = cTemp2->tuple_dict[ i + (br2 * max_header_num) ].type;
						KK_insert_entry(iBlock, tip, podatak, broj_td );
						broj_td+=1;						
				
					}
				br2++;
			}
		
		
		}

	printf("\n\n KORAK ZAVRSIO \n\n");
	KK_write_block(iBlock);					//tu treba nekaj drugo
		
		printf ("   \n\n               u iBlock        podatak, brtd: %i\n",broj_td);
		for(i=0;i<broj_td; i++){
			free=0;
							for(free;free<2000;free++)
								podatak[free]='\0';
			memcpy(podatak,
					   iBlock->data + iBlock->tuple_dict[ i ].address,
					   iBlock->tuple_dict[ i ].size);
				printf (" %s ", podatak);
		}
		printf("\n\n");
	}
	
	//brisanje svega
	
	reset_block(cTemp1);
	reset_block(cTemp2);				//tu treba sad još spremiti 
	
	KK_write_block(cTemp1);					// ali mislim da samo ide dirty bit
	KK_write_block(cTemp2);
	
}










