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


#include "rename.h"

void rename_test()
{
	printf( "rename_test: Present!\n" );
	KK_rename("testna","Prezime","preim_test","novoPrezime");
}

int KK_rename(char *old_table_name, char *old_attr, char *new_table_name, char *new_attr){
	table_addresses *adrese = get_table_addresses(old_table_name);
	int tab_adrese[MAX_NUM_OF_BLOCKS];
	int num_extents=0, brojac = 0;
	register int i,j;
	//SEARCH FOR ALL BLOCKS IN SEGMENT
	for (i=0; ;i++){
		if(adrese->address_from[i] == 0) break;
		for(j = adrese->address_from[i]; j <= adrese->address_to[i]; j++){
			tab_adrese[brojac] = j;
			brojac++;
		}
		num_extents++;
	}
	
	KK_header iHeader[MAX_ATTRIBUTES];
	KK_block * iBlock = KK_read_block(tab_adrese[0]);
	memcpy(&iHeader, iBlock->header, sizeof(iBlock->header));
	
	for(i = 0; i < MAX_ATTRIBUTES; i++){
		if(!strcmp(iHeader[i].att_name , old_attr)){
			printf("imena atributa su ista!\n");
			memcpy(iHeader[i].att_name, new_attr, strlen(new_attr));
			break;
		}
		else if(!strcmp(iHeader[i].att_name , "/0")){		//if there is no more attributes
			printf("KK_op_preimenovanje: ERROR: in this table does not exist atribute: %s", old_attr);
			return (EXIT_ERROR);
		}
	}
	
	KK_initialize_new_segment(new_table_name,1,&iHeader);
	for (i=1; i<num_extents; i++){
		KK_init_new_extent(new_table_name, 1);		//init new table extent 
	}
	table_addresses *adrese_new = get_table_addresses(new_table_name);
	int new_tab_adrese[MAX_NUM_OF_BLOCKS];
	brojac = 0;
	//SEARCH FOR ALL BLOCKS IN NEW INITIALIZED SEGMENT
	for (i=0; ;i++){
		if(adrese->address_from[i] == 0) break;
		for(j = adrese->address_from[i]; j <= adrese->address_to[i]; j++){
			new_tab_adrese[brojac] = j;
			brojac++;
		}
	}
	
	//PREIMENOVANJE
	KK_block * block1;
	KK_block * block2;
	
	for(i=0; i< brojac; i++){
		block1 = KK_read_block(tab_adrese[i]);
		block2 = KK_read_block(new_tab_adrese[i]);
		
		memcpy(block2,block1,sizeof(block1));				//kopiram cijeli stari blok u novi
		block2->address = new_tab_adrese[i];
		memcpy(block2->header, iHeader, sizeof(*iHeader)); 	// u novom mijenjam samo header
		KK_write_block(block2);								//zapisem novi block
		
	}
	free(block1);
	free(block2);
	
	return EXIT_SUCCESS;
	
	
}
