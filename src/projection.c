/**
@file op_projekcija.c Provides functions for file input/output
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
#include "projection.h"
#include "dbman.h"
#include "memoman.h"
#include "string.h"
#include "configuration.h"
#include "auxiliary.h"
#include "fileio.h"


///START GLOBAL FUNCTIONS

//Alocate empty list_op
void InitializelistOp(list_op *L)
{
	L->next = 0;
}
 
//returns frst list_op element_opint type;
element_op GetFirstelementOp(list_op *L)
{
	return L->next;
}

//returns last list_op element_op
element_op GetLastelementOp(list_op *L)
{
	list_op *Currentelement_op;
	Currentelement_op = L;
	while (Currentelement_op->next) 
		Currentelement_op = Currentelement_op->next;
	if(Currentelement_op!=L)
		return Currentelement_op;
	else 
		return 0;
}

//returns next element_op of given current element_op 
element_op GetNextelementOp(element_op Currentelement_op)
{
	if (Currentelement_op->next == 0) {
//		cout << "element_op ne postoji!" <<endl;
		return 0;
	} else {
		list_op *Nextelement_op;
		Nextelement_op = Currentelement_op->next;
		return Nextelement_op;
	}
}

//returns previous element_op of given current element_op, must alsa give list_op as second parameter
element_op GetPreviouselementOp(element_op Currentelement_op, list_op *L)
{
	list_op *Previouselement_op;
	Previouselement_op = L;
	while (Previouselement_op->next != 0 && Previouselement_op->next != Currentelement_op)
		Previouselement_op = Previouselement_op->next;
	if (Previouselement_op->next != 0 && Previouselement_op!=L) {
		return Previouselement_op;
	} else {
//		cout << "element_op ne postoji!" <<endl;
		return 0;
	}
}

//Retruns number positon of some element_op 
int GetPositionOfelementOp(element_op Searchedelement_op, list_op *L)
{
	list_op *Currentelement_op;
	int i = 0;
	Currentelement_op = L;
	while (Currentelement_op->next != 0 && Currentelement_op != Searchedelement_op) {
		Currentelement_op = Currentelement_op->next;
		i++;
	}
	return i;
}

///Delete element_op in list_op
void DeleteelementOp(element_op Deletedelement_op, list_op *L)
{
	element_op Previouselement_op = GetPreviouselementOp(Deletedelement_op,L);
		if(Previouselement_op!=0)
		{	
			Previouselement_op->next = Deletedelement_op->next;
		}
		else
		{
			L->next=Deletedelement_op->next;
		}
	free(Deletedelement_op);
}

//delete all list_op element_ops
void DeleteAllelementsOp(list_op *L)
{
	list_op *Currentelement_op = L, *Deletedelement_op=L->next;	
	while (Currentelement_op->next != 0) {
		Currentelement_op->next = Deletedelement_op->next;
		//clear(Deletedelement_op);
		free(Deletedelement_op);
		Deletedelement_op = Currentelement_op->next;
	}	
}

///END GLOBAL FUNCTIONS

///START SPECIAL FUNCTIONS FOR WORK WITH row_element_op_structure

///Insert new element_op after some element_op, to insert on first place give list_op as before element_op
void InsertNewelementOp(char * table, char * attribute_name, element_op elementBefore)
{
//	printf("\n TUUU0");
	list_op *newelement_op = (list_op *) malloc( sizeof(list_op) );
//	printf("\n TUUU0");
	memcpy(newelement_op->table, table, strlen(table));
	newelement_op->table[strlen(table)]='\0';
//	printf("\n TUUU0");
	memcpy(newelement_op->attribute_name, attribute_name, strlen(attribute_name));
	newelement_op->attribute_name[strlen(attribute_name)]='\0';

//	printf("\n TUUU0");
	newelement_op->next = elementBefore->next;
	elementBefore->next = newelement_op;
}

///END SPECIAL FUNCTIONS 

void temp_crate_table(char * table, KK_header * header, int type_segment)
{
	KK_block * sys_block = ( KK_block * ) malloc ( sizeof( KK_block ) );
	sys_block=KK_read_block(1);
	int first_block;
	first_block=KK_new_segment(table, type_segment, header);
//	KK_block * block = ( KK_block * ) malloc ( sizeof( KK_block ) );	
/*	KK_header * header_name = ( KK_header * ) malloc ( sizeof( KK_header ) );
	header_name = KK_create_header( "Ime", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	KK_header * header_surname = ( KK_header * ) malloc ( sizeof( KK_header ) );
	header_surname = KK_create_header( "Prezime", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR ) ;
	
	block->address = 80;
	block->type = BLOCK_TYPE_NORMAL;
	block->chained_with = NOT_CHAINED;
	block->free_space = 0; ///using as an address for the first free space in block->dat

	memcpy( & block->header[0], header_name, sizeof( * header_name ) );
	memcpy( & block->header[1], header_surname, sizeof( * header_surname ) );

	KK_write_block( block );
	block->address = 81;
	KK_write_block( block );
	block->address = 82;
	KK_write_block( block );
	block->address = 83;
	KK_write_block( block );
*/
	printf("First block adress of new segmet: %d",first_block);	
	
	int broj=8;
	KK_insert_entry(sys_block, TYPE_INT, &broj, 8 );
	KK_insert_entry(sys_block, TYPE_VARCHAR, table, 9 );
	//broj=80;
	broj=first_block;	
	KK_insert_entry(sys_block, TYPE_INT, &broj, 10 );
	//broj=83	
	broj=first_block+19;
	KK_insert_entry(sys_block, TYPE_INT, &broj, 11 );
	
	KK_write_block( sys_block );	
}



void crate_block_header(int old_block_add,char * new_table,list_op * att_root)
{
	KK_block * temp_block = ( KK_block * ) malloc ( sizeof( KK_block ) );
	KK_header header[MAX_ATTRIBUTES];
	element_op some_element;
	temp_block=KK_read_block(old_block_add);
	int next_header=1;
	int search_elem;
	int head=0;
	int new_head=0;
	while(next_header)
	{
		if(strcmp(temp_block->header[head].att_name,"")!=0)
		{
			search_elem=1;
			some_element=GetFirstelementOp(att_root);
			while(search_elem)
			{
				if(strcmp(some_element->attribute_name,temp_block->header[head].att_name)==0)
				{
					memcpy( & header[new_head], & temp_block->header[head], 
						sizeof( temp_block->header[head] ) );
					printf("Copy attribute headrer: %s",header[new_head].att_name);
					new_head++;	
				}
				some_element=GetNextelementOp(some_element);
				if(some_element==0)
				{
					search_elem=0;
				}
			}
			head++;	
		}
		else
		{
			next_header=0;
		}
	}	

	temp_crate_table(new_table, header, SEGMENT_TYPE_TABLE);
	free( temp_block );
}

void copy_block_projekcija(KK_block * old_block, list_op * att_root,char * new_table)
{
	printf("\n COPYING...");
	list *row_root =  (list *) malloc( sizeof(list) );
	//printf("\n tu sam");
	InitializeList(row_root);
	//printf("\n tu sam");
	element_op some_element;
	
	int next;
	int head;
	int search_elem;
	int entry_data[200];
	int something_to_copy=0;
	int i;
	int free;
	int size;
//	printf("\n tu sam \n");
	for (i=0;i<DATA_BLOCK_SIZE; ) 
	{//going through headers i=i+head	
		next=1;
		head=0;
		something_to_copy=0;
	//	printf("\n tu sam 10 \n");
		while(next)
		{
			if(strcmp(old_block->header[head].att_name,"")!=0)
			{
			//	printf("\n tu sam 11 \n");
				search_elem=1;
				some_element=GetFirstelementOp(att_root);
			//	printf("\n tu sam 12 \n");
				while(search_elem)
				{
					//printf("\n %s %d %s",some_element->attribute_name,old_block->tuple_dict[i].size,old_block->header[head].att_name);
					size=old_block->tuple_dict[i].size;
					int overflov=old_block->tuple_dict[i].size+old_block->tuple_dict[i].address;
					//printf("\noverflov: %d",overflov);
					if((strcmp(some_element->attribute_name,old_block->header[head].att_name)==0)&&(size!=0)
&&(overflov<old_block->free_space+1)&&(overflov>-1))
					{	
					
						free=0;
						for(free;free<200;free++)
							entry_data[free]='\0';
						
						//printf("\n Data: %s, %d, %d,%d,\n",entry_data,old_block->tuple_dict[i].address,old_block->tuple_dict[i].size,old_block->free_space);
						memcpy(entry_data,
							old_block->data+old_block->tuple_dict[i].address,
								old_block->tuple_dict[i].size);
						printf("\n \nEntry: %s", entry_data);
						//printf("",);
						InsertNewElementForUpdate(old_block->tuple_dict[i].type,entry_data,
							new_table,some_element->attribute_name, row_root,0);
						element neki;
						neki=GetFirstElement(row_root);
						printf("NEKI: %s ",neki->data);
						something_to_copy=1;
					}
					
					some_element=GetNextelementOp(some_element);
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
		if(something_to_copy)
		{
			insert_row(row_root);
			printf("UBACUJEM");
			DeleteAllElements(row_root);
		}
	}
}

int KK_projekcija(list_op * att_root,char * new_table)
{
	element_op some_element;
	table_addresses * addresses = (table_addresses *) malloc(sizeof(table_addresses));
	
//	printf("\n TUUU0");

	some_element=GetFirstelementOp(att_root);
//	printf("\n TUUU1");
	char table[MAX_ATT_NAME];
	memcpy(&table,some_element->table,strlen(some_element->table));	
	table[strlen(some_element->table)]='\0';
	if (DEBUG)
		printf("Create table: %s, from: ",new_table,table);
	
	addresses=get_table_addresses(table);
			
	if(addresses->address_from[0]!=0)
	{
		crate_block_header(addresses->address_from[0],new_table,att_root);
		printf("\nPorojekcija: start copying data");	

			//table_addresses * addresses_new = (table_addresses *) malloc(sizeof(table_addresses));
			//addresses_new=get_table_addresses(new_table);

			//KK_mem_block *mem_block = (KK_mem_block *) malloc(sizeof(KK_mem_block));
			KK_block *temp_block = (KK_block *) malloc(sizeof(KK_block));
	
			int from=0,to=0,j=0,i=0;
			for (j=0;j<200;j++)
			{
				from=addresses->address_from[j];
				if(from!=0)
				{
					printf("\n Projekcija: copy ekstent: %d", j);
			
					to=addresses->address_to[j];
					for(i=from;i<=to;i++)
					{
						printf("\n Projekcija: copy block: %d",i);
						//mem_block = KK_get_block( i );
						temp_block=KK_read_block( i );
						//traženje sad u bloku zapis  i brisanje
						copy_block_projekcija(temp_block,att_root,new_table);
					}
				}
				else break;
			}	
			return EXIT_SUCCESS;	
			
	}
	else
	{	
		printf("KK_projekcija: Table from which I must read not exist");
		return 0;
	}
	

}

void op_projekcija_test()
{
	printf("\n \n \n OP_PROJEKTICJA_TEST\n");	

	list_op *att_root =  (list_op *) malloc( sizeof(list_op) );
	InitializelistOp(att_root);
	InsertNewelementOp("testna","Ime",att_root);
	element_op some_element;
	some_element = GetFirstelementOp(att_root);
	//InsertNewelementOp("testna","Prezime",some_element);
	KK_projekcija(att_root,"testna_projekcija ");
		
}
