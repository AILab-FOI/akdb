/**
@file rename.c Provides functions for header atributes renaming
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


#include "intersect.h"


void intersect_test()
{
	printf( "\nintersect_test: Present!\n\n\n" );
	AK_intersect("testna","testna","INTERSECT_table");
}


int AK_intersect(char *table1, char *table2, char *new_table){
	register int i,j,k;
	int brojac1=0, brojac2=0, brojac3=0, sum_attr1=0, sum_attr2 = 0;
	int adr1[MAX_EXTENTS_IN_SEGMENT];
	int adr2[MAX_EXTENTS_IN_SEGMENT];
	table_addresses *address1 = (table_addresses*) get_table_addresses(table1);
	table_addresses *address2 = (table_addresses*) get_table_addresses(table2);
	
//	intersect_attr atributes[MAX_ATTRIBUTES];
	
	for(i=0; i<MAX_ATTRIBUTES; i++){
//		atributes[i].type = FREE_INT;
//		atributes[i].att_name = FREE_CHAR;
	}
	
	
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

	
	//uzeti header iz prve tablice i to je lista atributa po kojima se radi presjek
	AK_block * iBlock = (AK_block*) AK_read_block(adr1[0]);

	
	for(i = 0; i < MAX_ATTRIBUTES; i++){
		if(strcmp((char *)iBlock->header[i].att_name,"\0") == 0) //if there is no more attributes
			break;
		else{
			sum_attr1++;
	//		atributes[i].type = iBlock->header[i].type;
	//		atributes[i].att_name= iBlock->header[i].att_name;
		}
	}
	//free(iBlock);
	
	AK_block * iBlock2 = (AK_block*) AK_read_block(adr2[0]);
	
	for(i = 0; i < MAX_ATTRIBUTES; i++){
		if(strcmp((char *)iBlock2->header[i].att_name,"\0") == 0) //if there is no more attributes
			break;
		else{
			sum_attr2++;
		}
	}
	//free(iBlock2);
	
	if(sum_attr1 != sum_attr2){
		printf("INTERSECT ERROR: Not same number of attributes! \n");
		return EXIT_ERROR;
	}
	
	for(i=0;i<MAX_ATTRIBUTES;i++){
		if(strcmp((char *)iBlock->header[i].type, (char *)iBlock2->header[i].type)!=0){
			printf("INTERSECT ERROR: Attributes are not of the same type!");
			return EXIT_ERROR;
		}
	}
	
	for(i = 0; i < MAX_ATTRIBUTES; i++)
	{
		if(strcmp((char *)iBlock->header[i].att_name, (char *)iBlock2->header[i].att_name) != 0)
		{
			printf("INTERSECT ERROR: Relation shemas are not same! \n");
			return EXIT_ERROR;
		}
	}
	
	//initialize new segment
	AK_header *iHeader = (AK_header *) malloc(sizeof(AK_header));
	memcpy(iHeader, iBlock->header, sizeof(AK_header));
	AK_initialize_new_segment(new_table, SEGMENT_TYPE_TABLE ,iHeader);
	free(iHeader);
	
	
	char podatak1[MAX_VARCHAR_LENGHT];
	char podatak2[MAX_VARCHAR_LENGHT];
	int pozicija_block1=0, pozicija_block2=0;
	
	//INTERSECT
	int free=0;
	int zapisano =0;
	int poklapanje=0;
	int broj_jednakih=0;
	i = 0;
	list * row_root = (list *) malloc( sizeof(list) );
	
	while(i < brojac1){ //through the blocks of table 1
		iBlock = (AK_block*) AK_read_block(adr1[i]);
printf("\ntu sam: %i\n",adr1[i]);

		i++;
		int jos_u_data=0;



		while(iBlock->free_space > jos_u_data){ //if it is not the end of block
			j=0;
			while(j < brojac2){ //through the blocks of table 2
				iBlock2 = (AK_block*) AK_read_block(adr2[j]);
printf("\ntu sam2: %i\n",adr2[j]);

				int jos_u_data2=0;
				poklapanje =0;
				while(iBlock2->free_space > jos_u_data2){ //if it is not the end of block
					for(k = 0; k < sum_attr1; k++){//through the header
						
						for(free=0;free<MAX_VARCHAR_LENGHT;free++){
							podatak1[free]=FREE_CHAR;
							podatak2[free]=FREE_CHAR;
						}
						
						memcpy(podatak1,
							   iBlock->data + iBlock->tuple_dict[pozicija_block1 * sum_attr1 +k].address,
							   iBlock->tuple_dict[pozicija_block1 * sum_attr1 +k].size);
						if(INTERSECT_DEBUG)						
							printf("\npodatak 1 : %s     na poziciji: %i\n",podatak1, iBlock->tuple_dict[pozicija_block1 * sum_attr1 +k].address);

						memcpy(podatak2,
							   iBlock2->data + iBlock2->tuple_dict[pozicija_block2 * sum_attr1 + k ].address,
							   iBlock2->tuple_dict[pozicija_block2 * sum_attr1 + k ].size);
						
						if(INTERSECT_DEBUG)						
							printf("\npodatak 2 : %s     na poziciji: %i\n",podatak2, iBlock2->tuple_dict[pozicija_block2 * sum_attr2 +k].address);


						jos_u_data2 = iBlock2->tuple_dict[pozicija_block2 * sum_attr1 + k ].address + iBlock2->tuple_dict[pozicija_block2 * sum_attr1 + k ].size;
						
						if(strcmp(podatak1,podatak2)==0){ //jednaki su
							poklapanje++;
						}
						else{
							poklapanje=0;
							break;
						}
					
						

					}//end: through the header
					pozicija_block2++;

					if(INTERSECT_DEBUG)
						printf("\npoklapanje : %i      sum_atr1: %i\n",poklapanje, sum_attr1);

					if(poklapanje == sum_attr1){ //ako se po svim atributima poklapalo
					//zapisati red u novu tablicu jer se poklapaju
						broj_jednakih++;
						DeleteAllElements(row_root);
						//InsertNewElement(TYPE_INT,&broj_jednakih,"testna","Redni_broj",row_root);
						
						for(k = 0; k < sum_attr1; k++){//through the header
						
							for(free=0;free<MAX_VARCHAR_LENGHT;free++){
								podatak1[free]=FREE_CHAR;
							}
							
							memcpy(podatak1,
								   iBlock->data + iBlock->tuple_dict[pozicija_block1 * sum_attr1 +k].address,
								   iBlock->tuple_dict[pozicija_block1 * sum_attr1 +k].size);
							
							jos_u_data = iBlock->tuple_dict[pozicija_block1 * sum_attr1 + k ].address + iBlock->tuple_dict[pozicija_block1 * sum_attr1 + k ].size;
							
							InsertNewElementForUpdate(iBlock->tuple_dict[pozicija_block1 * sum_attr1 + k ].type, &podatak1, new_table, iBlock->header[ k ].att_name, row_root, 0);
							
						}//end: through the header
						
						insert_row(row_root);
						zapisano=1;
						break;
					}
					
				
				}
				
				if(zapisano)
					break;
				
				
			}//end: through the blocks of table 2
			
			pozicija_block1++;
		}
	}//end: through the blocks of table 1
	
	
	
	
	

}
