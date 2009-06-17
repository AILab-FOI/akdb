/**
@file union.c Provides functions for relational database union operation
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

int KK_union(char *table1, char *table2, char *new_table)
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
	
	//attribute list for union
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
	
	if(sum_attr1 != sum_attr2){
		printf("UNION ERROR: Not same number of attributes! \n");
		return EXIT_ERROR;
	
	for(i = 0; i < MAX_ATTRIBUTES; i++)
	{
		if(strcmp((char *)iBlock1->header[i].att_name, (char *)iBlock2->header[i].att_name) != 0)
		{
			printf("UNION ERROR: Relation shemas are not same! \n");
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
}