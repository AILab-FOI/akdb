/**
@file auxiliary.c Provides auxiliary functions
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
#include "auxiliary.h"

/** 
 @author Miroslav Policki
 
 Returns size in bytes of the provided database type

 @param iDB_type database data type (defined in configuration.h)
 @param szVarchar if iDB_type == TYPE_VARCHAR, pointer to the string, otherwise unused

 @return size of provided data type in bytes if provided data type is valid, else 0
*/
size_t KK_type_size(int iDB_type, char *szVarchar) {
    switch(iDB_type) {
        case TYPE_INTERNAL:
            return sizeof(int);
        case TYPE_INT:
            return sizeof(int);
        case TYPE_FLOAT:
            return sizeof(double);
        case TYPE_NUMBER:
            return sizeof(double);
        case TYPE_VARCHAR:
            return (size_t) strlen(szVarchar);
        case TYPE_DATE:
            return sizeof(int);
        case TYPE_DATETIME:
            return sizeof(int);
        case TYPE_TIME:
            return sizeof(int);
        case TYPE_BLOB:
            return sizeof(void *);
        case TYPE_BOOL:
            return (size_t) 1;
        default:
            return (size_t) 0;
    }
}


/**
function for geting addresses of some table 
@return structure table_addresses witch contains start and end adress of table extents, 
when form and to are 0 you are on the end of addresses
@param table - table that you search
*/
table_addresses * get_table_addresses ( char * table)
{
	//promjentiti temp_block u block i odati ispred catalog_block->
	//KK_mem_block *catalog_block = (KK_mem_block *) malloc(sizeof(KK_mem_block));
	//printf("\n GET TEABLE ADDTESES SPACE TUUU0");
	KK_block *temp_block = (KK_block *) malloc(sizeof(KK_block));
	//printf("te %s", table);
	//catalog_block = KK_get_block( 0 );
	//printf("\n GET TEABLE ADDTESES SPACE TUUU1");
	temp_block=KK_read_block(0);
	int trazi=1;
	int i=0;
	int data_adr=0;
	int data_size=0;
	int data_type=0;
	char name_sys[100];
	int address_sys;
	int free=0;
	
	printf("\nTražim systemsku tablicu relacija \n");
	for(i;i<DATA_BLOCK_SIZE;)
	{
		
		//trazi=0;
		free=0;
		for(free;free<100;free++)
			name_sys[free]='\0';
		data_adr=temp_block->tuple_dict[i].address;
		//printf("\n adr: %d",data_adr);
		data_size=temp_block->tuple_dict[i].size;
		data_type=temp_block->tuple_dict[i].type;
		memcpy(name_sys,temp_block->data+data_adr,data_size);
		printf("\n adr: %s",name_sys);
		i++;
		data_adr=temp_block->tuple_dict[i].address;
		data_size=temp_block->tuple_dict[i].size;
		data_type=temp_block->tuple_dict[i].type;
		memcpy(&address_sys,temp_block->data+data_adr,data_size);
		if(strcmp(name_sys,"KK_relation")==0)
		{	
			printf("\npronasao adresu relacijeske sys tablice: %d \n",address_sys);
			trazi=0;
			break;
		}
		i++;
		
		printf("\naaaaa");
	}
	
	//trazi=1;
	i=0;
	//catalog_block = KK_get_block( address_sys );
	temp_block=KK_read_block(address_sys);
	table_addresses * addresses = (table_addresses *) malloc(sizeof(table_addresses));
	free=0;	
	for(free;free<200;free++)
	{	
		addresses->address_from[free] = 0;
		addresses->address_to[free] = 0;
	}
	//return addresses;
	char name[200];
	int address_from;
	int address_to;
	int j=0;
	for(i;i<DATA_BLOCK_SIZE;)
	{
		free=0;
		//free the variable name
		for(free;free<200;free++)
			name[free]='\0';

		i++;
		//printf("\ntu sam\n");
		data_adr=temp_block->tuple_dict[i].address;
		data_size=temp_block->tuple_dict[i].size;
		data_type=temp_block->tuple_dict[i].type;
		memcpy(name,temp_block->data+data_adr,data_size);
		//printf("table: %s, name: %s, brojac: %d, addres: %d, size: %d",table,name,i,data_adr,data_size);
		i++;
		data_adr=temp_block->tuple_dict[i].address;
		data_size=temp_block->tuple_dict[i].size;
		data_type=temp_block->tuple_dict[i].type;
		memcpy(&address_from,temp_block->data+data_adr,data_size);
		i++;
		data_adr=temp_block->tuple_dict[i].address;
		data_size=temp_block->tuple_dict[i].size;
		data_type=temp_block->tuple_dict[i].type;
		memcpy(&address_to,temp_block->data+data_adr,data_size);
		i++;
		printf("\n table: %s, name: %s,",table, name);
		if(strcmp(name,table)==0) //možda neka funkcija tu ide a ne običan =
		{	
			addresses->address_from[j]= address_from; //možda i neka funkcija
			addresses->address_to[j]= address_to;
			j++;
			printf("\npronasao adrese tražene tablice: %d , %d \n",address_from, address_to);	
		}
		if(strcmp(name,"")==0)
		{
			//printf("nema ničeg");
			trazi=0;
		}
		
	}
	//free( temp_block);
	//printf("aaaaaaaaaaaaaaaaa");
	//free( temp_block ); 
	return addresses;
}


int find_free_space ( table_addresses * addresses )
{
	//printf("\n FIND FREE SPACE TUUU1");
	//KK_mem_block *mem_block = (KK_mem_block *) malloc(sizeof(KK_mem_block));
	KK_block *temp_block = (KK_block *) malloc(sizeof(KK_block));
	//printf("\n FIND FREE SPACE TUUU1");
	int from=0,to=0,j=0,i=0;
	//return 100;
	printf("\n Searching for block that has free space<500 \n");
	for (j=0;j<200;j++)
	{
		if(addresses->address_from!=0)
		{
			from=addresses->address_from[j];
			to=addresses->address_to[j];
			for(i=from;i<=to;i++)
			{
				temp_block = KK_read_block( i );
				int free_space_on=temp_block->free_space;
				printf("FRRE SPACE %d",temp_block->free_space);
				if(free_space_on < 4500)
				{
				//	printf("TU SAM");
					return i; 
				}
			}
		}
		else break;
	}	
	
	int adr = 122; 
	//KK_initialize_new_ekstent(i); //funkcija ne postoji koja znači inicjalizira novi prostor ekstenta, spozapiše sve što treba i vrati početnu adresu ekstenta u koji ja mogu dalje zapisivati tablicu;
	//i je adresa bloka koji sadrži tu tablicu pa da se može samo kopirati heder
	free( temp_block );
	return adr;
}
