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
#include <stdio.h>
#include <stdlib.h>
#include "fileio.h"
#include "dbman.h"
#include "memoman.h"
#include "string.h"
#include "configuration.h"
#include "auxiliary.h"


///START GLOBAL FUNCTIONS TO WORK WITH LIST

//Alocate empty list
void InitializeList(list *L)
{
	L->next = 0;
}
 
//returns frst list elementint type;
element GetFirstElement(list *L)
{
	return L->next;
}

//returns last list element
element GetLastElement(list *L)
{
	list *CurrentElement;
	CurrentElement = L;
	while (CurrentElement->next) 
		CurrentElement = CurrentElement->next;
	if(CurrentElement!=L)
		return CurrentElement;
	else 
		return 0;
}

//returns next element of given current element 
element GetNextElement(element CurrentElement)
{
	if (CurrentElement->next == 0) {
//		cout << "Element ne postoji!" <<endl;
		return 0;
	} else {
		list *NextElement;
		NextElement = CurrentElement->next;
		return NextElement;
	}
}

//returns previous element of given current element, must alsa give list as second parameter
element GetPreviousElement(element CurrentElement, list *L)
{
	list *PreviousElement;
	PreviousElement = L;
	while (PreviousElement->next != 0 && PreviousElement->next != CurrentElement)
		PreviousElement = PreviousElement->next;
	if (PreviousElement->next != 0 && PreviousElement!=L) {
		return PreviousElement;
	} else {
//		cout << "Element ne postoji!" <<endl;
		return 0;
	}
}

//Retruns number positon of some element 
int GetPositionOfElement(element SearchedElement, list *L)
{
	list *CurrentElement;
	int i = 0;
	CurrentElement = L;
	while (CurrentElement->next != 0 && CurrentElement != SearchedElement) {
		CurrentElement = CurrentElement->next;
		i++;
	}
	return i;
}

///Delete element in list
void DeleteElement(element DeletedElement, list *L)
{
	element PreviousElement = GetPreviousElement(DeletedElement,L);
		if(PreviousElement!=0)
		{	
			PreviousElement->next = DeletedElement->next;
		}
		else
		{
			L->next=DeletedElement->next;
		}
	free(DeletedElement);
}

//delete all list elements
void DeleteAllElements(list *L)
{
	list *CurrentElement = L, *DeletedElement=L->next;	
	while (CurrentElement->next != 0) {
		CurrentElement->next = DeletedElement->next;
		//clear(DeletedElement);
		free(DeletedElement);
		DeletedElement = CurrentElement->next;
	}	
}


///END GLOBAL FUNCTIONS

///START SPECIAL FUNCTIONS FOR WORK WITH row_element_structure

///Insert new element after some element, to insert on first place give list as before element
void InsertNewElement(int newtype, void * data, char * table, char * attribute_name, element ElementBefore)
{
	list *newElement = (list *) malloc( sizeof(list) );
	newElement->type = newtype;
	memcpy(newElement->data, data, KK_type_size(newtype, data));
/*	int free=0;
	for(free;free< MAX_ATT_NAME;free++)
			newElement->table[free]='\0';*/
	memcpy(newElement->table, table, strlen(table));
	newElement->table[strlen(table)]='\0';
	memcpy(newElement->attribute_name, attribute_name, strlen(attribute_name));
	newElement->attribute_name[strlen(attribute_name)]='\0';
	if(newtype==TYPE_VARCHAR)
	{
		newElement->data[KK_type_size(newtype, data)]='\0';
	}
	newElement->constraint=0;
	newElement->next = ElementBefore->next;
	ElementBefore->next = newElement;
}

void InsertNewElementForUpdate(int newtype, void * data, char * table, char * attribute_name, element ElementBefore, int newconstraint)
{
	list *newElement = (list *) malloc( sizeof(list) );
	newElement->type = newtype;
	memcpy(newElement->data, data, KK_type_size(newtype, data));
/*	int free=0;
	for(free;free< MAX_ATT_NAME;free++)
			newElement->table[free]='\0';*/
	//printf("\n u hederu: %s",table);
	memcpy(newElement->table, table, strlen(table));
	//printf("\n u hederu: %s",newElement->table);
	newElement->table[strlen(table)]='\0';
	//printf("\n u hederu: %s",newElement->table);
	memcpy(newElement->attribute_name, attribute_name, strlen(attribute_name));
	newElement->attribute_name[strlen(attribute_name)]='\0';
	if(newtype==TYPE_VARCHAR)
	{
		newElement->data[KK_type_size(newtype, data)]='\0';
	}
	newElement->constraint = newconstraint;
	newElement->next = ElementBefore->next;
	ElementBefore->next = newElement;
}
///END SPECIAL FUNCTIONS row_element_structure


int insert_row_to_block(list *row_root, KK_block *temp_block)
{
	element some_element;	
	int unosi=1; //used to run while until all heders are inserted
	int type; //type od entry data
	unsigned char entry_data[200]; 
	int size;	//size of entry data
	int id=-1; //id tuple dict in which is inserted next data
	int head=0; //index of header which is curently inserted
	int search_tuple_id, search_elem;//serch for tuple dict id and searc for data in list 
//	printf("tu1 ");
	while(unosi)
	{
		search_tuple_id=1;
		while(search_tuple_id)
		{
			id++;
			if(temp_block->tuple_dict[id].size==0)
			{
				search_tuple_id=0;
			}
		}
		
		printf("\n pozicija za pisanje %d, heder_att_name %s",id,temp_block->header[head].att_name);
		if(strcmp(temp_block->header[head].att_name,"")!=0)//opet možda treba funkcija neka za usporedbu znakova
		{ 
		
			search_elem=1;
			some_element=GetFirstElement(row_root);
			//printf("\n tražim element u listi");
			while(search_elem)
			{
				//printf("\nsome_element->attribute_name %s", some_element->attribute_name);
				if((strcmp(some_element->attribute_name,temp_block->header[head].att_name)==0)&&(some_element->constraint==0))
				{//isto funkcija za usporedbu
					
					int free=0;
					//free the variable name
					for(free;free<200;free++)
						entry_data[free]='\0';
					type=some_element->type;
					printf("\nINSERT BLOCK: DATA: %s",some_element->data);
					memcpy(entry_data,some_element->data,KK_type_size(type,some_element->data));
					//printf("\nnašao %s, vrijednsot: %d", some_element->attribute_name, entry_data );
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
			//printf("\nDATA: %d",entry_data);
			memcpy(temp_block->data+temp_block->free_space, entry_data,KK_type_size(type,entry_data));		
			temp_block->tuple_dict[id].address=temp_block->free_space;
			temp_block->free_space+=KK_type_size(type,entry_data);
			temp_block->tuple_dict[id].type=type;
			temp_block->tuple_dict[id].size = KK_type_size(type,entry_data);
			//temp_block->tuple_dict[id+1].size=0;
			memcpy(entry_data,temp_block->data+temp_block->tuple_dict[id].address,temp_block->tuple_dict[id].size);
			//printf("\nDATA: %s",entry_data);
			if( DEBUG )
				printf("\nInsert: Insert data: %s Size of data: %d\n", entry_data, KK_type_size(type,entry_data));
			head++;
		}
		else
		{
			//printf("tu9 ");
			unosi=0;
		}
	}
	temp_block->last_tuple_dict_id=id;
	//printf("tu10 ");
	return EXIT_SUCCESS; 
}

int insert_row(list *row_root)
{
	if(DEBUG){ 
		printf("\n \n \n Start inserting data");
	}
	element some_element;
//	printf("\n TUUU0");
	some_element=GetFirstElement(row_root);
//	printf("\n TUUU1");
	char table[100];
	int free2=0;
	for(free2;free2<100;free2++)
		table[free2]='\0';
	memcpy(&table,some_element->table,strlen(some_element->table));
//	printf("\n TUUU2");
	printf("\n Insert into table: %s", table);
	int adr_to_write;
//	printf("\n TUUU3");
	adr_to_write=find_free_space(get_table_addresses(&table));

	printf("\n Insert into block on adress: %d",adr_to_write);
	
	//KK_mem_block *mem_block = (KK_mem_block *) malloc(sizeof(KK_mem_block));
//	printf("\n TUUU4");
	KK_block *temp_block = (KK_block *) malloc(sizeof(KK_block));
//	printf("\n TUUU5");
	temp_block=KK_read_block(adr_to_write);
//	printf("\n PRIJE POZIVA \n");
	//mem_block = KK_get_block(adr_to_write);
	printf("INSERT: DATA: %s",some_element->data);
	int end=insert_row_to_block(row_root, temp_block);
//	printf("\n POSLIJE POZIVA \n");
	KK_write_block(temp_block);
	//free(temp_block);
	free( temp_block );
	return end;
	
	//return EXIT_SUCCESS;
	
}
/**
@param what -if 0 then update, if 1 then delete
*/
void update_delete_row_from_block(KK_block *temp_block, list *row_root, int what)
{
	int next=1; //moving through headers
	int head=0; //counting headers
	int i; //go through all tuple_dicts
	int delete=1; //if can delete gorup of tuple dict which are in same row of table
	int search_elem=1; //search throug list elemnts if contains some header
	unsigned char entry_data[200]; //entry data when haeader is found in list which is copied to compare with data in block
	int exits_equal_attrib=0; //if we fund at least one header in list
	element some_element; //element of list
	int difrent_varchar_exist=0;//to now on update if extist varchar that is not the same so that i must delete/insert the entry
	int free=0;
//	printf("tu");
//OBAVEZNO STAVITI NA CJELU VELIČINU
	for (i=0;i<DATA_BLOCK_SIZE; ) 
	{//going through headers i=i+head	
		next=1;
		head=0;
		//printf("\n for: %d", i);
		
		while(next)
		{//printf("\n while next: %d, header: %s", next,temp_block->header[head].att_name);
			if(strcmp(temp_block->header[head].att_name,"")!=0)//opet možda treba funkcija neka za usporedbu znakova
			{
				
				search_elem=1;
				some_element=GetFirstElement(row_root);
				while(search_elem)
				{//printf("\n while search element");
//printf("\n Lista at %s, heder at %s",some_element->attribute_name ,temp_block->header[head].att_name);
					if((strcmp(some_element->attribute_name,temp_block->header[head].att_name)==0)&&(some_element->constraint==1))
					{//isto funkcija za usporedbu
						
						free=0;
						//free the variable name
						for(free;free<200;free++)
							entry_data[free]='\0';

						exits_equal_attrib=1;
						//printf("\n isti heder");
						memcpy(entry_data,
							temp_block->data+temp_block->tuple_dict[i].address,
								temp_block->tuple_dict[i].size);
					//	printf("\n Data:%d, %s, %d, %d",i,entry_data,temp_block->tuple_dict[i].address,temp_block->tuple_dict[i].size);
					/*	int prvi;
						 memcpy(&prvi,some_element->data,4);
						int drugi = entry_data;
					         memcpy(&drugi,entry_data,4);
						printf("\n entry_data: %d,some_element_data: %d",drugi, prvi);*/
						
						if(strcmp(entry_data,some_element->data)!=0)
						{//funkcija usporedbe
							//printf("\n nije identičan podatak: %s, %s",entry_data,some_element->data);
							delete=0;
						}
						else
						{
							printf("\n identičan podatak: %s, %s",entry_data,some_element->data);
						}
						
						//search_elem=0;
					}
					int type=temp_block->tuple_dict[i].type;
					//printf("\n   update: %s,%s,%d,%d,%d,%d",some_element->attribute_name,temp_block->header[head].att_name,some_element->constraint,what,difrent_varchar_exist,type);

					if((strcmp(some_element->attribute_name,temp_block->header[head].att_name)==0)&&(some_element->constraint==0)&&(what==0)&&(difrent_varchar_exist==0)&&(type==TYPE_VARCHAR))
					{//update provjera dali postoji varchar koji se mora mjenjat
						printf("Usporedba varchara");
						free=0;
						for(free;free<200;free++)
							entry_data[free]='\0';
	
						memcpy(entry_data,
							temp_block->data+temp_block->tuple_dict[i].address,
								temp_block->tuple_dict[i].size);

						//if(strcmp(entry_data,some_element->data)!=0)
						//printf("duljine,%d,%d",strlen(entry_data),strlen(some_element->data));
						if(strlen(entry_data)!=strlen(some_element->data))
						{//funkcija usporedbe
							printf("\n postoji različiti varchar podatak");
							difrent_varchar_exist=1;
						}
						
					}					
										
					//printf("tu");
					some_element=GetNextElement(some_element);
					if(some_element==0)
					{
						search_elem=0;
					}
					
					
				}
				head++;	
				i++;
			} 
			else
			{
				next=0;
			}
			
			
		}
		//printf("za brisanje: %d,%d",exits_equal_attrib,delete);
		//printf("fgagf");
		if(what==1)
		{//delete
		//	printf("aaaaa");
			printf("\n Delete row");
			if((exits_equal_attrib==1) && (delete==1))
			{	
				int j=0;
				for(j=i-head;j<i;j++)
				{//printf("\nfor brisanje: %d", j);	
					int k=temp_block->tuple_dict[j].address;
					int l=k+temp_block->tuple_dict[j].size;	
					printf("\nod: %d, do: %d \n",k,l);			
					for(k;k<l;k++)
					{
						char prazan='\0';
						memcpy(temp_block->data+k,&prazan,1);
						//return 0;
						printf("brojac, %d",k);
					}
				
					temp_block->tuple_dict[j].size=0;
					temp_block->tuple_dict[j].type=0;
					temp_block->tuple_dict[j].address=0;
				}
		//		printf("head: %d, i: %d",head,i);
				//return 0;
			}
		}
		else
		{//update
			printf("\nUpdate row");
			
			//prije provjeriti dali ima varchar koji se mora mjenjat u tom redu
			//tu sad if ili biše pa piše ili mjenja
			if((exits_equal_attrib==1) && (delete==1))
			{	
				int j=0;
				int up_type;
				char up_entry[200];
				for(j=i-head;j<i;j++)
				{//printf("\nfor brisanje: %d", j);	
					if(difrent_varchar_exist==1)
					{
								

						//pretraga starih elemenata
						search_elem=1;
						some_element=GetFirstElement(row_root);
						int exist_new_data=0;
						while(search_elem)
						{
						
						if((strcmp(some_element->attribute_name,temp_block->header[j%head].att_name)==0)
							&&(some_element->constraint==0))
							{
					printf("Prepisivanje: element: %s ,heder: %s",some_element->attribute_name,
							temp_block->header[j%head].att_name);
								exist_new_data=1;
							}
							some_element=GetNextElement(some_element);
							if(some_element==0)
							{
								search_elem=0;
							}

						}
						
						if(exist_new_data==0)
						{
							free=0;
							for(free;free<200;free++)
								up_entry[free]='\0';
							memcpy(up_entry,temp_block->data+temp_block->tuple_dict[j].address,
								temp_block->tuple_dict[j].size);
							some_element=GetFirstElement(row_root);
							InsertNewElementForUpdate(temp_block->tuple_dict[j].type,up_entry,
								 some_element->table,temp_block->header[j%head].att_name,
									some_element,0)	;
						}
					
							//risanje
						int k=temp_block->tuple_dict[j].address;
						int l=k+temp_block->tuple_dict[j].size;	
						printf("\nod: %d, do: %d \n",k,l);			
						for(k;k<l;k++)
						{
							char prazan='\0';
							memcpy(temp_block->data+k,&prazan,1);
							//return 0;
							printf("brojac, %d",k);
						}
				
						temp_block->tuple_dict[j].size=0;
						temp_block->tuple_dict[j].type=0;
						temp_block->tuple_dict[j].address=0;

						
						
					}
					else
					{
						free=0;
						for(free;free<200;free++)
							up_entry[free]='\0';
						memcpy(up_entry,
							temp_block->data+temp_block->tuple_dict[j].address,
								temp_block->tuple_dict[j].size);
						search_elem=1;
						some_element=GetFirstElement(row_root);
						while(search_elem)
						{
						//printf("\nmodulo %d",j%head);
						if((strcmp(some_element->attribute_name,temp_block->header[j%head].att_name)==0)
							&&(some_element->constraint==0))
							{
							//	printf("tuuuu");
								if(strcmp(up_entry,some_element->data)!=0)
								{//printf("2 %s,%s",up_entry,some_element->data);
									memcpy(temp_block->data+temp_block->tuple_dict[j].address,
									some_element->data,
								        temp_block->tuple_dict[j].size);	
								}	
							}
							some_element=GetNextElement(some_element);
							if(some_element==0)
							{
								search_elem=0;
							}
						}
							
					}	
/*				
					int k=temp_block->tuple_dict[j].address;
				
					int l=k+temp_block->tuple_dict[j].size;	
					printf("\nod: %d, do: %d \n",k,l);			
					for(k;k<l;k++)
					{
						char prazan='\0';
						memcpy(temp_block->data+k,&prazan,1);
						//return 0;
						printf("brojac, %d",k);
					}
				
					temp_block->tuple_dict[j].size=0;
					temp_block->tuple_dict[j].type=0;
					temp_block->tuple_dict[j].address=0;
*/
				}
				if(difrent_varchar_exist==1)
				{
					insert_row_to_block(row_root,temp_block);
				}
		//		printf("head: %d, i: %d",head,i);
				//return 0;
			}


			
		}
		difrent_varchar_exist=0;
		delete=1;
		exits_equal_attrib=0;
		//i+=head;
	}
	//write();
}

int delete_update_segment(list *row_root, int delete)
{
	element some_element;
	some_element=GetFirstElement(row_root);
	char table[100];
	
	strcpy(&table,some_element->table);
	printf("\n \n \n table to delete_update from: %s, izvor %s", table, some_element->table);

	table_addresses * addresses = (table_addresses *) malloc(sizeof(table_addresses));
	addresses=get_table_addresses(&table);

	//KK_mem_block *mem_block = (KK_mem_block *) malloc(sizeof(KK_mem_block));
	KK_block *temp_block = (KK_block *) malloc(sizeof(KK_block));
	
	int from=0,to=0,j=0,i=0;
	for (j=0;j<200;j++)
	{
		from=addresses->address_from[j];
		if(from!=0)
		{
			printf("\n delete_update ekstent: %d", j);
			
			to=addresses->address_to[j];
			for(i=from;i<=to;i++)
			{
				printf("\n delete_update block: %d",i);
				//mem_block = KK_get_block( i );
				temp_block=KK_read_block( i );
				//traženje sad u bloku zapis  i brisanje
				update_delete_row_from_block(temp_block,row_root,delete);
				KK_write_block(temp_block);
				
			}
		}
		else break;
	}	
	return EXIT_SUCCESS;	
}

int delete_row(list *row_root)
{	
	delete_update_segment(row_root, 1);
	return EXIT_SUCCESS;
}

int update_row(list *row_root)
{
	delete_update_segment(row_root, 0);	
	return EXIT_SUCCESS;	
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
	block->free_space = 0;
	block->last_tuple_dict_id=0; ///using as an address for the first free space in block->dat

	memcpy( & block->header[0], header_red_br, sizeof( * header_red_br ) );
	memcpy( & block->header[1], header_name, sizeof( * header_name ) );
	memcpy( & block->header[2], header_surname, sizeof( * header_surname ) );

	KK_write_block( block );
	block->address = 11;
	KK_write_block( block );
	block->address = 12;
	KK_write_block( block );
	block->address = 13;
	KK_write_block( block );

	header_red_br = KK_create_header( "Adresa", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	header_name = KK_create_header( "Ime", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	header_surname = KK_create_header( "Prezime", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	
	block->address = 20;
	block->type = BLOCK_TYPE_NORMAL;
	block->chained_with = NOT_CHAINED;
	block->free_space = 0;
	block->last_tuple_dict_id=0; ///using as an address for the first free space in block->dat

	memcpy( & block->header[0], header_red_br, sizeof( * header_red_br ) );
	memcpy( & block->header[1], header_name, sizeof( * header_name ) );
	memcpy( & block->header[2], header_surname, sizeof( * header_surname ) );

	KK_write_block( block );
	block->address = 21;	
	KK_write_block( block );
	block->address = 22;
	KK_write_block( block );
	block->address = 23;
	KK_write_block( block );


	//upis u sistemski katalog relacije
	KK_block * block2 = ( KK_block * ) malloc ( sizeof( KK_block ) );	
	header_red_br = KK_create_header( "obj_id", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	header_name = KK_create_header( "Name", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	header_surname = KK_create_header( "start_addres", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	KK_header * header_surname2 = ( KK_header * ) malloc ( sizeof( KK_header ) );
	header_surname2 = KK_create_header( "end_addres", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	
	block2->address = 1;
	block2->type = BLOCK_TYPE_NORMAL;
	block2->chained_with = NOT_CHAINED;
	block2->free_space = 0; ///using as an address for the first free space in block->dat

	memcpy( & block2->header[0], header_red_br, sizeof( * header_red_br ) );
	memcpy( & block2->header[1], header_name, sizeof( * header_name ) );
	memcpy( & block2->header[2], header_surname, sizeof( * header_surname ) );
	memcpy( & block2->header[3], header_surname2, sizeof( * header_surname ) );
	
	block2=KK_get_block(1);
	//printf("\naaaaaaaaaaaaaaa");
	//printf("\n RELACIJA: %d,%s",block2->address,block2->header[2].att_name);
	int broj;
	broj=1;
	KK_insert_entry(block2, TYPE_INT, &broj, 0 );
	KK_insert_entry(block2, TYPE_VARCHAR, "testna_druga", 1 );
	broj=20;
	KK_insert_entry(block2, TYPE_INT, &broj, 2 );
	broj=22;
	KK_insert_entry(block2, TYPE_INT, &broj, 3 );
	
	broj=2;
	KK_insert_entry(block2, TYPE_INT, &broj, 4 );
	KK_insert_entry(block2, TYPE_VARCHAR, "testna", 5 );
	broj=10;
	KK_insert_entry(block2, TYPE_INT, &broj, 6 );
	broj=13;
	KK_insert_entry(block2, TYPE_INT, &broj, 7 );

	int cisti;
	KK_write_block( block2 );
	block2=KK_read_block(10);
	
	for(cisti=0;cisti<DATA_BLOCK_SIZE;cisti++)
	{
		block2->tuple_dict[cisti].size=0;
	}
	KK_write_block( block2 );
	block2=KK_read_block(11);
	
	for(cisti=0;cisti<DATA_BLOCK_SIZE;cisti++)
	{
		block2->tuple_dict[cisti].size=0;
	}
	KK_write_block( block2 );
	block2=KK_read_block(12);
	
	for(cisti=0;cisti<DATA_BLOCK_SIZE;cisti++)
	{
		block2->tuple_dict[cisti].size=0;
	}
	KK_write_block( block2 );
	block2=KK_read_block(13);
	
	for(cisti=0;cisti<DATA_BLOCK_SIZE;cisti++)
	{
		block2->tuple_dict[cisti].size=0;
	}
	KK_write_block( block2 );
	

	//return 0;
/*OVO JE UBITI SVE VEĆ SPREMNO*/
//priprema podataka radi viši sloj
	list *row_root =  (list *) malloc( sizeof(list) );
	InitializeList(row_root);
	broj=1;
	element some_element;
	InsertNewElement(TYPE_INT,&broj,"testna","Redni_broj",row_root);
	some_element=GetFirstElement(row_root);
	//printf("\nTip: %d ",some_element->type);
	//printf("\nTablica: %s ",some_element->table);
	//printf("\nAtribut: %s ",some_element->attribute_name);
	int aa;
	memcpy(&aa,some_element->data,4);
	//printf("\nData: %d ",aa);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Matija","testna","Ime",row_root,0);
	some_element=GetFirstElement(row_root);
	//printf("\nData: %s ",some_element->data);
	InsertNewElement(TYPE_VARCHAR,"Novak","testna","Prezime",row_root);
	some_element=GetFirstElement(row_root);
	//printf("\nData: %s ",some_element->data);

	insert_row(row_root); //prvi poziv funkcije koja je moja
	
	//priprema podataka radi viši sloj
	DeleteAllElements(row_root);
	broj=2;
	InsertNewElement(TYPE_INT,&broj,"testna","Redni_broj",row_root);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Nikola","testna","Ime",row_root,0);
	InsertNewElement(TYPE_VARCHAR,"Bakoš","testna","Prezime",row_root);
	some_element=GetFirstElement(row_root);
	//printf("\n \n \n tu %d",(int) some_element);
	insert_row(row_root); //drugi poziv funkcije koja je moja

	broj=3;
	InsertNewElement(TYPE_INT,&broj,"testna","Redni_broj",row_root);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Matija","testna","Ime",row_root,0);
	InsertNewElement(TYPE_VARCHAR,"Bakoš","testna","Prezime",row_root);
	some_element=GetFirstElement(row_root);
	//printf("\n \n \n tu %d",(int) some_element);
	insert_row(row_root); //drugi poziv funkcije koja je moja
	int i;
	for (i=0;i<10;i++)
	{
		broj=i;
		printf("\n BROJ %d\n",i);
		InsertNewElement(TYPE_INT,&broj,"testna","Redni_broj",row_root);
		InsertNewElementForUpdate(TYPE_VARCHAR,"Maja","testna","Ime",row_root,0);
		InsertNewElement(TYPE_VARCHAR,"Vacenovski","testna","Prezime",row_root);
		some_element=GetFirstElement(row_root);
		//printf("\n \n \n tu %d",(int) some_element);
		insert_row(row_root); //drugi poziv funkcije koja je moja
	}
	DeleteAllElements(row_root);
	broj=1;
	InsertNewElementForUpdate(TYPE_INT,&broj,"testna","Redni_broj",row_root,1);
	delete_row(row_root);

	DeleteAllElements(row_root);
	broj=1;
	InsertNewElementForUpdate(TYPE_INT,&broj,"testna","Redni_broj",row_root,1);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Nikola","testna","Ime",row_root,0);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Bakoš","testna","Prezime",row_root,0);
	update_row(row_root);

	DeleteAllElements(row_root);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Matija","testna","Ime",row_root,1);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Pajdo","testna","Prezime",row_root,0);
	update_row(row_root);

	DeleteAllElements(row_root);
	broj=3;
	InsertNewElementForUpdate(TYPE_INT,&broj,"testna","Redni_broj",row_root,1);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Slonic","testna","Ime",row_root,0);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Marko","testna","Prezime",row_root,0);
	update_row(row_root);
	//ispis elementa
	
	DeleteAllElements(row_root);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Slonic","testna","Ime",row_root,1);
	delete_row(row_root);	

	DeleteAllElements(row_root);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Maja","testna","Ime",row_root,1);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Mihi","testna","Ime",row_root,0);
	update_row(row_root);	

	some_element=GetFirstElement(row_root);

	DeleteAllElements(row_root);
	InsertNewElement(TYPE_VARCHAR,"Brace Radica 13","testna_druga","Adresa",row_root);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Nikola","testna_durga","Ime",row_root,0);
	InsertNewElement(TYPE_VARCHAR,"Bakoš","testna_druga","Prezime",row_root);
	some_element=GetFirstElement(row_root);
	//printf("\n \n \n tu %d",(int) some_element);
	insert_row(row_root); //drugi poziv funkcije koja je moja

	DeleteAllElements(row_root);
	InsertNewElement(TYPE_VARCHAR,"Kalnička 54","testna_druga","Adresa",row_root);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Mihi","testna_durga","Ime",row_root,0);
	InsertNewElement(TYPE_VARCHAR,"Vacenovski","testna_druga","Prezime",row_root);
	some_element=GetFirstElement(row_root);
	//printf("\n \n \n tu %d",(int) some_element);
	insert_row(row_root); //drugi poziv funkcije koja je moja

	DeleteAllElements(row_root);
	InsertNewElement(TYPE_VARCHAR,"Neka","testna_druga","Adresa",row_root);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Maja","testna_durga","Ime",row_root,0);
	InsertNewElement(TYPE_VARCHAR,"Vacenovski","testna_druga","Prezime",row_root);
	some_element=GetFirstElement(row_root);
	//printf("\n \n \n tu %d",(int) some_element);
	insert_row(row_root); //drugi poziv funkcije koja je moja

	DeleteAllElements(row_root);
	broj=133;
	InsertNewElement(TYPE_INT,&broj,"testna","Redni_broj",row_root);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Maja","testna","Ime",row_root,0);
	InsertNewElement(TYPE_VARCHAR,"Vacenovski","testna","Prezime",row_root);
	some_element=GetFirstElement(row_root);
	//printf("\n \n \n tu %d",(int) some_element);
	insert_row(row_root); //drugi poziv funkcije koja je moja

	DeleteAllElements(row_root);
	broj=1334;
	InsertNewElement(TYPE_INT,&broj,"testna","Redni_broj",row_root);
	InsertNewElementForUpdate(TYPE_VARCHAR,"Matija","testna","Ime",row_root,0);
	InsertNewElement(TYPE_VARCHAR,"Novak","testna","Prezime",row_root);
	some_element=GetFirstElement(row_root);
	//printf("\n \n \n tu %d",(int) some_element);
	insert_row(row_root); //drugi poziv funkcije koja je moja
	
	
	//printf("\nTip: %d ",some_element->type);
	//printf("\nTablica: %s ",some_element->table);
	//printf("\nAtribut: %s ",some_element->attribute_name);
	//printf("\nData: %s ",some_element->data);
	
	//počitisti memorijske lokacije jer se ponovo koriste.
	//printf("tu2 %d",(int) some_element);

/*LIST TEST	memcpy(temp_block->data+temp_block->free_space, entry_data,KK_type_size(type,entry_data));
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
