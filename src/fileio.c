/**
@file fileio.c Provides functions for file input/output
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
 17 */
#include "fileio.h"
#include "dbman.h"
#include "memoman.h"
#include "string.h"


table_addresses * get_table_addresses(char * table)
{
	KK_mem_block *catalog_block = (KK_mem_block *) malloc(sizeof(KK_mem_block));
	//printf("te %s", table);
	catalog_block = KK_get_block( 0 );
	int trazi=1;
	int i=0;
	int data_adr=0;
	int data_size=0;
	int data_type=0;
	char name_sys[100];
	char address_sys[20];
	while(trazi)
	{
		
		trazi=0;
		printf("\ntu sam\n");
		data_adr=catalog_block->block->tuple_dict[i].address;
		data_size=catalog_block->block->tuple_dict[i].size;
		data_type=catalog_block->block->tuple_dict[i].type;
		memcpy(name_sys,catalog_block->block->data+data_adr,data_size);
		i++;
		data_adr=catalog_block->block->tuple_dict[i].address;
		data_size=catalog_block->block->tuple_dict[i].size;
		data_type=catalog_block->block->tuple_dict[i].type;
		memcpy(address_sys,catalog_block->block->data+data_adr,data_size);
		if(strcmp(name_sys,"KK_relation")==0)
		{	
			printf("\npronasao adresu relacijeske sys tablice %s \n",address_sys);
			trazi=0;
		}
	}
	
	trazi=1;
	i=0;
	catalog_block = KK_get_block( address_sys );
	table_addresses * addresses = (table_addresses *) malloc(sizeof(table_addresses));
	char name[200];
	char address_from[200];
	char address_to[200];
	int j=0;
	while(trazi)
	{
		i++;
		printf("\ntu sam\n");
		data_adr=catalog_block->block->tuple_dict[i].address;
		data_size=catalog_block->block->tuple_dict[i].size;
		data_type=catalog_block->block->tuple_dict[i].type;
		memcpy(name,catalog_block->block->data+data_adr,data_size);
		i++;
		data_adr=catalog_block->block->tuple_dict[i].address;
		data_size=catalog_block->block->tuple_dict[i].size;
		data_type=catalog_block->block->tuple_dict[i].type;
		memcpy(address_from,catalog_block->block->data+data_adr,data_size);
		i++;
		data_adr=catalog_block->block->tuple_dict[i].address;
		data_size=catalog_block->block->tuple_dict[i].size;
		data_type=catalog_block->block->tuple_dict[i].type;
		memcpy(address_to,catalog_block->block->data+data_adr,data_size);
		i++;
		if(name==table) //možda neka funkcija tu ide a ne običan =
		{	
			addresses->address_from[j]= (int) address_from; //možda i neka funkcija
			addresses->address_to[j]= (int) address_from;
			j++;
			printf("\npronasao adrese tražene tablice %s \n",address_sys);
		}
		if(name=="\0")
		{
			trazi=0;
		}
	}
	return addresses;
}

int find_free_space(table_addresses * addresses)
{
	KK_mem_block *mem_block = (KK_mem_block *) malloc(sizeof(KK_mem_block));
	int from=0,to=0,j=0,i=0;
	for (j=0;j<200;j++)
	{
		if(addresses->address_from!=0)
		{
			from=addresses->address_from[j];
			to=addresses->address_to[j];
			for(i=from;i<=to;i++)
			{
				 mem_block = KK_get_block( i );
				if(mem_block->block->free_space<4000);
				{
					return i; 
				}
			}
		}
		else break;
	}	
	
	int adr = 13; 
	//KK_initialize_new_ekstent(i); //funkcija ne postoji koja znači inicjalizira novi prostor ekstenta, spozapiše sve što treba i vrati početnu adresu ekstenta u koji ja mogu dalje zapisivati tablicu;
	//i je adresa bloka koji sadrži tu tablicu pa da se može samo kopirati heder
	return adr;
}

int insert_row(list *row_root)
{
	if(DEBUG){ 
		printf("Start inserting data");
	}
	element some_element;
	some_element=GetFirstElement(row_root);
	char table[100];
	memcpy(&table,some_element->table,strlen(some_element->table));
	printf("te %s", table);

	int adr_to_write;
	adr_to_write=find_free_space(get_table_addresses(&table));

	KK_mem_block *mem_block = (KK_mem_block *) malloc(sizeof(KK_mem_block));
	mem_block = KK_get_block(adr_to_write);
	
	int unosi=1; //used to run while until all heders are inserted
	int type; //type od entry data
	char entry_data[2000]; 
	int size;	//size of entry data
	int id=-1; //id tuple dict in which is inserted next data
	int head=0; //index of header which is curently inserted
	int search_tuple_id, search_elem;//serch for tuple dict id and searc for data in list 
	while(unosi)
	{
		search_tuple_id=1;
		while(search_tuple_id)
		{
			id++;
			if(mem_block->block->tuple_dict[id].size==0)
			{
				search_tuple_id=0;
			}
		}
		
		if(mem_block->block->header[head].att_name!="\0")//opet možda treba funkcija neka za usporedbu znakova
		{ 
		
			search_elem=1;
			some_element=GetFirstElement(row_root);
			while(search_elem)
			{
				if(some_element->attribute_name==mem_block->block->header[head].att_name)
				{//isto funkcija za usporedbu
					type=some_element->type;
					memcpy(entry_data,some_element->data,strlen(some_element->data));
					search_elem=0;
				}
				else
				{
					some_element=GetNextElement(some_element);
					if(some_element==0)
					{
						memcpy(entry_data,"null",strlen("null"));
						type=TYPE_VARCHAR;
						search_elem=0;
					}
					
				}
			}
		
			memcpy(mem_block->block->data+mem_block->block->free_space, entry_data, strlen( entry_data));		
			mem_block->block->tuple_dict[id].address=mem_block->block->free_space;
			mem_block->block->free_space+=strlen(entry_data);
			mem_block->block->tuple_dict[id].type=type;
			mem_block->block->tuple_dict[id].size = strlen(entry_data);
			if( DEBUG )
				printf("Insert;y: Insert data: %s Size of data: %d\n", entry_data, strlen(entry_data));
			head++;
		}
		else
		{
			unosi=0;
		}
	}
	return EXIT_SUCCESS;
	
}

void delete_row_from_block(KK_mem_block *mem_block, list *row_root)
{
	int next=1; //moving through headers
	int head=0; //counting headers
	int i; //go through all tuple_dicts
	int delete=1; //if can delete gorup of tuple dict which are in same row of table
	int search_elem=1; //search throug list elemnts if contains some header
	char entry_data[2000]; //entry data when haeader is found in list which is copied to compare with data in block
	int exits_equal_attrib=0; //if we fund at least one header in list
	element some_element; //element of list
	for (i=0;i<DATA_BLOCK_SIZE; )
	{//going through headers i=i+head	
		next=1;
		head=0;
		while(next)
		{
			if(mem_block->block->header[head].att_name!="\0")//opet možda treba funkcija neka za usporedbu znakova
			{
				search_elem=1;
				some_element=GetFirstElement(row_root);
				while(search_elem)
				{
					if(some_element->attribute_name==mem_block->block->header[head].att_name)
					{//isto funkcija za usporedbu
						exits_equal_attrib=1;
						memcpy(&entry_data,
							mem_block->block->data+mem_block->block->tuple_dict[i].address,
							mem_block->block->tuple_dict[i].size);
						if(entry_data!=some_element->data)
						{//funkcija usporedbe
							delete=0;
						}
						search_elem=0;
					}
					else
					{
						some_element=GetNextElement(some_element);
						if(some_element==0)
						{
							search_elem=0;
						}
					
					}
				}
				head++;	
			} 
			else
			{
				next=0;
			}
			
		}
		
		if((exits_equal_attrib==1) && (delete=1))
		{	
			int j=0;
			for(j=i;j<i+head;j++)
			{	
				int k=mem_block->block->tuple_dict[j].address;
				for(k;k<k+mem_block->block->tuple_dict[j].size;k++)
					memcpy(mem_block->block->data+k,"0",1);
				mem_block->block->tuple_dict[j].size=0;
				mem_block->block->tuple_dict[j].type=0;
				mem_block->block->tuple_dict[j].address=0;
			}
		}
		i+=head;
	}
}

int delete_row(list *row_root)
{
	element some_element;
	some_element=GetFirstElement(row_root);
	char table[100];
	memcpy(&table,some_element->table,strlen(some_element->table));
	printf("te %s", table);

	table_addresses * addresses = (table_addresses *) malloc(sizeof(table_addresses));
	addresses=get_table_addresses(&table);

	KK_mem_block *mem_block = (KK_mem_block *) malloc(sizeof(KK_mem_block));
	int from=0,to=0,j=0,i=0;
	for (j=0;j<200;j++)
	{
		if(addresses->address_from!=0)
		{
			from=addresses->address_from[j];
			to=addresses->address_to[j];
			for(i=from;i<=to;i++)
			{
				mem_block = KK_get_block( i );
				
				//traženje sad u bloku zapis  i brisanje
				delete_row_from_block(mem_block, row_root);
			}
		}
		else break;
	}	
	return EXIT_SUCCESS;	
}


void update_row()
{

}


void fileio_test()
{
/*OVO JE UBITI SVE VEĆ SPREMNO*/
	//zapis tablice testna
	KK_block * block = ( KK_block * ) malloc ( sizeof( KK_block ) );	
	KK_header * header_red_br  =  ( KK_header * ) malloc ( sizeof( KK_header ) );
	header_red_br = KK_create_header( "Redni_broj", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	KK_header * header_name = ( KK_header * ) malloc ( sizeof( KK_header ) );
	header_name = KK_create_header( "Ime", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	KK_header * header_surname = ( KK_header * ) malloc ( sizeof( KK_header ) );
	header_surname = KK_create_header( "Prezime", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	
	block->address = 10;
	block->type = BLOCK_TYPE_NORMAL;
	block->chained_with = NOT_CHAINED;
	block->free_space = 0; ///using as an address for the first free space in block->dat

	memcpy( & block->header[0], header_red_br, sizeof( * header_red_br ) );
	memcpy( & block->header[1], header_name, sizeof( * header_name ) );
	memcpy( & block->header[2], header_surname, sizeof( * header_surname ) );

	KK_write_block( block );

	//upis u sistemski katalog relacije
	KK_block * block2 = ( KK_block * ) malloc ( sizeof( KK_block ) );	
	header_red_br = KK_create_header( "obj_id", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	header_name = KK_create_header( "Name", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	header_surname = KK_create_header( "address", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	KK_header * header_surname2 = ( KK_header * ) malloc ( sizeof( KK_header ) );
	header_surname2 = KK_create_header( "addressKraj", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	
	block2->address = 1;
	block2->type = BLOCK_TYPE_NORMAL;
	block2->chained_with = NOT_CHAINED;
	block2->free_space = 0; ///using as an address for the first free space in block->dat

	memcpy( & block2->header[0], header_red_br, sizeof( * header_red_br ) );
	memcpy( & block2->header[1], header_name, sizeof( * header_name ) );
	memcpy( & block2->header[2], header_surname, sizeof( * header_surname ) );
	memcpy( & block2->header[2], header_surname2, sizeof( * header_surname ) );

	KK_insert_entry(block2, TYPE_INT, "1", 0 );
	KK_insert_entry(block2, TYPE_VARCHAR, "testna_druga", 1 );
	KK_insert_entry(block2, TYPE_INT, "20", 2 );
	KK_insert_entry(block2, TYPE_INT, "22", 3 );

	KK_insert_entry(block2, TYPE_INT, "2", 4 );
	KK_insert_entry(block2, TYPE_VARCHAR, "testna", 5 );
	KK_insert_entry(block2, TYPE_INT, "10", 6 );
	KK_insert_entry(block2, TYPE_INT, "10", 7 );


	KK_write_block( block2 );
/*OVO JE UBITI SVE VEĆ SPREMNO*/
//priprema podataka radi viši sloj
	list *row_root =  (list *) malloc( sizeof(list) );
	InitializeList(row_root);
	InsertNewElement(TYPE_INT,"1","testna","redni_broj",row_root);
	InsertNewElement(TYPE_VARCHAR,"Matija","testna","Ime",row_root);
	InsertNewElement(TYPE_VARCHAR,"Novak","testna","Prezime",row_root);

	insert_row(row_root); //prvi poziv funkcije koja je moja
	
	//priprema podataka radi viši sloj
	DeleteAllElements(row_root);
	InsertNewElement(TYPE_INT,"2","testna","redni_broj",row_root);
	InsertNewElement(TYPE_VARCHAR,"Nikola","testna","Ime",row_root);
	InsertNewElement(TYPE_VARCHAR,"Bakoš","testna","Prezime",row_root);

	insert_row(row_root); //drugi poziv funkcije koja je moja
	
	DeleteAllElements(row_root);
	InsertNewElement(TYPE_INT,"1","testna","redni_broj",row_root);
	delete_row(row_root);

	DeleteAllElements(row_root);
	InsertNewElement(TYPE_INT,"2","testna","redni_broj",row_root);
	InsertNewElement(TYPE_VARCHAR,"Nikol","testna","Ime",row_root);
	InsertNewElement(TYPE_VARCHAR,"Bako","testna","Prezime",row_root);
	update_row(row_root);

	//ispis elementa
	element some_element;
	some_element=GetFirstElement(row_root);
	printf("\nTip: %d ",some_element->type);
	printf("\nTablica: %s ",some_element->table);
	printf("\nAtribut: %s ",some_element->attribute_name);
	printf("\nData: %s ",some_element->data);

/*LIST TEST	
	int a;
	list *ListaKorjen =  (list *) malloc( sizeof(list) );
	element elementListe;
	InitializeList(ListaKorjen);
	printf("tu %d",(int) ListaKorjen);
	printf("tu %d",(int) GetFirstElement(ListaKorjen));

	InsertNewElement(1,ListaKorjen);
	elementListe=GetFirstElement(ListaKorjen);
	printf("tu %d",(int) GetFirstElement(ListaKorjen));
	printf("tu %d",elementListe->type);

	InsertNewElement(2,elementListe);
	printf("tu %d",elementListe->type);

	elementListe=GetNextElement(GetFirstElement(ListaKorjen));
	printf("tu %d",elementListe->type);

	elementListe=GetPreviousElement(elementListe,ListaKorjen);
	printf("tu %d",elementListe->type);

	InsertNewElement(3,GetPreviousElement(GetLastElement(ListaKorjen),ListaKorjen));
	elementListe=GetNextElement(elementListe);
	printf("tu %d",elementListe->type);

	printf("tu %d", (int) GetPositionOfElement(elementListe,ListaKorjen));
	printf("tu %d", (int) GetPreviousElement(GetFirstElement(ListaKorjen),ListaKorjen));
	printf("tu %d", (int) GetNextElement(GetLastElement(ListaKorjen)));

	DeleteElement(elementListe,ListaKorjen);
	elementListe=GetNextElement(GetFirstElement(ListaKorjen));
	printf("tu %d",elementListe->type);

	elementListe=GetFirstElement(ListaKorjen);
	DeleteElement(elementListe,ListaKorjen);
	elementListe=GetFirstElement(ListaKorjen);
	printf("tu %d",(int) elementListe);
	elementListe=GetLastElement(ListaKorjen);
	printf("tu %d",(int) elementListe);

	InsertNewElement(1,ListaKorjen);
	elementListe=GetFirstElement(ListaKorjen);
	printf("tu %d",(int) elementListe);
	InsertNewElement(3,ListaKorjen);
	elementListe=GetFirstElement(ListaKorjen);
	printf("tu %d",(int) elementListe);
	elementListe=GetLastElement(ListaKorjen);
	printf("tu %d",(int) elementListe);

	DeleteAllElements(ListaKorjen);
	elementListe=GetFirstElement(ListaKorjen);
	printf("tu %d", (int) elementListe);
	elementListe=GetLastElement(ListaKorjen);
	printf("tu %d",(int) elementListe);
	printf( "fileio_test: Present!\n" );
*/
}
