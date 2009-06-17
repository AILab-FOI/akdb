/**
@file difference.c Provides functions for relational database difference operation
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

#include "fileio.h"
#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include "dbman.h"
#include "memoman.h"
#include <string.h>
#include "configuration.h"


int KK_difference(char *table1, char *table2, char *new_table)
{
	register int i, j, k;
	int num_attr_t1 = 0, num_attr_t2 = 0; //number of attributes in table
	int adr1[MAX_EXTENTS_IN_SEGMENT], adr2[MAX_EXTENTS_IN_SEGMENT];
	int brojac1 = 0, brojac2 = 0;
	
	//get table addresses
	table_addresses *address1 = (table_addresses*) get_table_addresses(table1);
	table_addresses *address2 = (table_addresses*) get_table_addresses(table2);
	
	//get all table addresses
	for (i=0; ;i++){
		if(address1->address_from[i] == 0) break;
		for(j = address1->address_from[i]; j <= address1->address_to[i]; j++){
			adr1[brojac1] = j;
			brojac1++;

		}
	}
	
	for (i=0; ;i++){
		if(address2->address_from[i] == 0) break;
		for(j = address2->address_from[i]; j <= address2->address_to[i]; j++){
			adr2[brojac2] = j;
			brojac2++;
		}
	}
	
	//attribute list for difference
	KK_block * iBlock = (KK_block*) KK_read_block(adr1[0]);
	KK_block * iBlock2 = (KK_block*) KK_read_block(adr2[0]);
	
	//check for relation shemas
	for(i = 0; i < MAX_ATTRIBUTES; i++){
		if(strcmp((char *)iBlock->header[i].att_name,"\0") == 0) //if there is no more attributes
			break;
		else
			num_attr_t1++;
	}

	
	for(i = 0; i < MAX_ATTRIBUTES; i++){
		if(strcmp((char *)iBlock2->header[i].att_name,"\0") == 0) //if there is no more attributes
			break;
		else
			num_attr_t2++;
	}
	
	if(num_attr_t1 != num_attr_t2){
		printf("DIFFERENCE ERROR: Not same number of attributes! \n");
		return EXIT_ERROR;
	
	for(i = 0; i < MAX_ATTRIBUTES; i++)
	{
		if(strcmp((char *)iBlock->header[i].att_name, (char *)iBlock2->header[i].att_name) != 0)
		{
			printf("DIFFERENCE ERROR: Relation shemas are not same! \n");
			return EXIT_ERROR;
		}
	}
	
	//initialize new segment
	KK_header *iHeader = (KK_header *) malloc(sizeof(KK_header));
	memcpy(iHeader, iBlock->header, sizeof(KK_header));
	KK_initialize_new_segment(new_table, SEGMENT_TYPE_TABLE ,iHeader);
	free(iHeader);
	
	char podatak1[MAX_VARCHAR_LENGHT];
	char podatak2[MAX_VARCHAR_LENGHT];
	int pozicija_block1=0, pozicija_block2=0;
	
	int free=0;
	int zapisano =0;
	int razlicito=0;
	int broj_jednakih=0;
	i = 0;
	list * row_root = (list *) malloc( sizeof(list) );
	
	while(i < brojac1)
	{ 
		iBlock = (KK_block*) KK_read_block(adr1[i]); //read block from first table

		i++;
		int imaJosElemenata=0; //indicates if there are more elements in block

		while(iBlock->free_space > imaJosElemenata)
		{
			j=0;
			while(j < brojac2)
			{ //go to second table
				iBlock2 = (KK_block*) KK_read_block(adr2[j]);

				int imaJosElemenata2=0;

				while(iBlock2->free_space > imaJosElemenata2)
				{
					//through the header
					for(k = 0; k < num_attr_t1; k++)
					{
						for(free=0;free<MAX_VARCHAR_LENGHT;free++)
						{
							podatak1[free]=FREE_CHAR;
							podatak2[free]=FREE_CHAR;
						}
						
						//copy content from tuble dict to podatak[]
						memcpy(podatak1,
							   iBlock->data + iBlock->tuple_dict[pozicija_block1 * num_attr_t1 +k].address,
							   iBlock->tuple_dict[pozicija_block1 * num_attr_t1 +k].size);

						memcpy(podatak2,
							   iBlock2->data + iBlock2->tuple_dict[pozicija_block2 * num_attr_t1 + k ].address,
							   iBlock2->tuple_dict[pozicija_block2 * num_attr_t1 + k ].size);

						imaJosElemenata2 = iBlock2->tuple_dict[pozicija_block2 * num_attr_t1 + k ].address + iBlock2->tuple_dict[pozicija_block2 * num_attr_t1 + k ].size;
						
						if(strcmp(podatak1,podatak2)!=0)
						{ //if they are different
							razlicito = 1;
							break;
						}
					}
					// next position in second table
					pozicija_block2++;

					//if there is a difference between tuple_dicts
					if(razlicito == 1)
					{
						DeleteAllElements(row_root); //remove row from further action
						
						for(k = 0; k < num_attr_t1; k++){//through the header
						
							for(free=0;free<MAX_VARCHAR_LENGHT;free++){
								podatak1[free]=FREE_CHAR;
							}
							
							//copy tuple_dict to new block
							memcpy(podatak1,
								   iBlock->data + iBlock->tuple_dict[pozicija_block1 * num_attr_t1 +k].address,
								   iBlock->tuple_dict[pozicija_block1 * num_attr_t1 +k].size);
							
							imaJosElemenata = iBlock->tuple_dict[pozicija_block1 * num_attr_t1 + k ].address + iBlock->tuple_dict[pozicija_block1 * num_attr_t1 + k ].size;
							
							InsertNewElementForUpdate(iBlock->tuple_dict[pozicija_block1 * num_attr_t1 + k ].type, &podatak1, new_table, iBlock->header[ k ].att_name, row_root, 0);
							
						}
						
						insert_row(row_root);
						zapisano=1;
						break;
					}	
				}	
				if(zapisano) break;
			}
			pozicija_block1++; //move to next position in block
		}
	}
	return EXIT_SUCCESS;
}