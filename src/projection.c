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


///START GLOBAL FUNCTIONS for work with list for relational operators

	/**	@author Matija Novak     
		alocate empty list
		@param list_op - root of the list_op
		@result void
	*/
	void InitializelistOp(list_op *L)
	{
		L->next = 0;
	}
	 
	/** 	@author Matija Novak
		Get the frst list element
		@param list_op - root of the list_op
		@result element_op first element of the list
	*/	
	element_op GetFirstelementOp(list_op *L)
	{
		return (element_op) L->next;
	}

	/** 	@author Matija Novak
		Get the last list element
		@param list_op - root of the list_op
		@result element_op last element of the list
	*/		
	element_op GetLastelementOp(list_op *L)
	{
		list_op *Currentelement_op;
		Currentelement_op = L;
		while (Currentelement_op->next) 
			Currentelement_op = (element_op) Currentelement_op->next;
		if(Currentelement_op!=L)
			return (element_op) Currentelement_op;
		else 
			return 0;
	}


	/** 	@author Matija Novak
		Gets the next list element of an given element
		@param Currenetelelemnt_op - some element of the list (list_op) form which we want the next element
		@result element_op - next element of given element_op
	*/		
	element_op GetNextelementOp(element_op Currentelement_op)
	{
		if (Currentelement_op->next == 0) {
			return 0;
		} else {
			list_op *Nextelement_op;
			Nextelement_op = (element_op) Currentelement_op->next;
			return (element_op) Nextelement_op;
		}
	}

	/** 	@author Matija Novak
		Get the previous element of some element in the list
		@param Currentelelemnt_op - element of which we want the previous element
		@param L - root of the list 
		@result element_op - previous element of the element that we give as first parameter
	*/	
	element_op GetPreviouselementOp(element_op Currentelement_op, element_op L)
	{
		element_op Previouselement_op;
		Previouselement_op = L;
		while ((Previouselement_op->next != 0) && ((element_op) Previouselement_op->next != Currentelement_op))
			Previouselement_op = (element_op) Previouselement_op->next;
		if (Previouselement_op->next != 0 && Previouselement_op!=L) {
			return (element_op) Previouselement_op;
		} else {
			return 0;
		}
	}

	/** 	@author Matija Novak
		Get the posititn of given elelment
		@param Searchelement_op - element which posititon we search for
		@param L - root of the list
		@result returns the posititon number of some elelemnt
	*/	
	int GetPositionOfelementOp(element_op Searchedelement_op, list_op *L)
	{
		list_op *Currentelement_op;
		int i = 0;
		Currentelement_op = L;
		while (Currentelement_op->next != 0 && Currentelement_op != Searchedelement_op) {
			Currentelement_op = (list_op *) Currentelement_op->next;
			i++;
		}
		return i;
	}

	/** 	@author Matija Novak
		Delete given elelment from the list
		@param Deletedelement_op - element which we delete
		@param L - root of the list
		@result void
	*/	
	void DeleteelementOp(element_op Deletedelement_op, list_op *L)
	{
		element_op Previouselement_op = (element_op) GetPreviouselementOp(Deletedelement_op,L);
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

	
	/** 	@author Matija Novak
		Delete all elelments from the list
		@param L - root of the list
		@result void
	*/
	void DeleteAllelementsOp(list_op *L)
	{
		list_op *Currentelement_op = L;
		list_op *Deletedelement_op= (list_op *) L->next;	
		while (Currentelement_op->next != 0) {
			Currentelement_op->next = Deletedelement_op->next;
			free(Deletedelement_op);
			Deletedelement_op = (list_op *) Currentelement_op->next;
		}	
	}

///END GLOBAL FUNCTIONS

///START SPECIAL FUNCTIONS FOR WORK WITH row_element_op_structure
	
	/** 	@author Matija Novak
		Inserts new element_op after some element_op, to insert on first place give list_op as before element_op
		@param table - table name
		@param attribute_name - attribute name
		@param element_op - element after we which insert the new element 
		@result void
	*/	
	void InsertNewelementOp(char * table, char * attribute_name, element_op elementBefore)
	{
		list_op *newelement_op = (list_op *) malloc( sizeof(list_op) );
		memcpy(newelement_op->table, table, strlen(table));
		newelement_op->table[strlen(table)]='\0';
		memcpy(newelement_op->attribute_name, attribute_name, strlen(attribute_name));
		newelement_op->attribute_name[strlen(attribute_name)]='\0';
		newelement_op->next = elementBefore->next;
		elementBefore->next = newelement_op;
	}

///END SPECIAL FUNCTIONS 

/** 	@author Matija Novak
	temporaly function to create table, and insert entry to the system_relation catalog
	@param table - table name
	@param header - KK_header of the new table
	@param type_segment - type of the new segment
	@result void
*/
void temp_crate_table(char * table, KK_header * header, int type_segment)
{
	KK_block * sys_block = ( KK_block * ) malloc ( sizeof( KK_block ) );
	sys_block= (KK_block *) KK_read_block(1);
	
	int first_block;
	//create new segment
	first_block= (int) KK_new_segment(table, type_segment, header);
	if(DEBUG)	
		printf("temp_create_table: First block adress of new segmet: %d",first_block);	
	
	int broj=8;
	//insert object_id
	KK_insert_entry(sys_block, TYPE_INT, &broj, 8 );
	//insert table name	
	KK_insert_entry(sys_block, TYPE_VARCHAR, table, 9 );
	//insett start address	
	broj=first_block;	
	KK_insert_entry(sys_block, TYPE_INT, &broj, 10 );	
	//insert end address
	broj=first_block+19;
	KK_insert_entry(sys_block, TYPE_INT, &broj, 11 );
		
	KK_write_block( sys_block );
	free(sys_block);	
}


/** 	@author Matija Novak
	function create a new heder for the projection table
	@param old_block_add - address of the block from which we copy headers we need
	@param new_table - name of the new table
	@param att_root - list of the attributes which should the projeciton table contain
	@result void
*/
void crate_block_header(int old_block_add,char * new_table,list_op * att_root)
{
	KK_block * temp_block = ( KK_block * ) malloc ( sizeof( KK_block ) );
	
	KK_header header[MAX_ATTRIBUTES];
	element_op some_element;
	temp_block= (KK_block *) KK_read_block(old_block_add);
	int next_header=1; //boolean var to new if there is more headers
	int search_elem; //boolean var to new if we have more elements in list
	int head=0; //counter of the headers
	int new_head=0; //counter of the new headers created for the projection table

	while(next_header)
	{//goes through headers
		if(strcmp(temp_block->header[head].att_name,"")!=0)
		{//if we read all headers

			search_elem=1;
			some_element=GetFirstelementOp(att_root);

			while(search_elem)
			{//goes through list elements
				if(strcmp(some_element->attribute_name,temp_block->header[head].att_name)==0)
				{//if we found the header we need to copy 
					
					//make a copy of the header we need for projection
					memcpy( & header[new_head], & temp_block->header[head], 
						sizeof( temp_block->header[head] ) );

					if(DEBUG)
						printf("Copy attribute headrer: %s",header[new_head].att_name);
					new_head++;	
				}
				some_element= (element_op) GetNextelementOp(some_element);
				if(some_element==0)
				{//if there are more elements in list
					search_elem=0;
				}
			}
			//go to next header
			head++;	
		}
		else
		{
			//there are no more headers
			next_header=0;
		}
	}	

	temp_crate_table(new_table, header, SEGMENT_TYPE_TABLE);
	free( temp_block );
}

/** 	@author Matija Novak
	function copies the data from old table block to the new projection table
	@param old_block - block from which we copy data
	@param new_table - name of the new table
	@param att_root - list of the attributes which should the projeciton table contain
	@result void
*/
void copy_block_projekcija(KK_block * old_block, list_op * att_root,char * new_table)
{
	if(DEBUG)	
		printf("\n COPYING PROJECTION DATA FORM BLOCK...");

	list *row_root =  (list *) malloc( sizeof(list) );
	InitializeList(row_root);

	element_op some_element;
	int next;//boolea var to indicate there are no more headers
	int head;//counter of the head we are on
	int search_elem;  //booolean var to indicate end of the list elements in row_root list
	char entry_data[MAX_VARCHAR_LENGHT]; //data to copy
	int something_to_copy=0; //boolean varijable to indicate if there is data to copy in these set of tuple_dicts
	int i; //counter of the tuple_dicts
	int free2; //varijable to free used string varajable
	int size; //site of the data read from tuple_dict

	for (i=0;i<DATA_BLOCK_SIZE; ) 
	{//going through tuple_dicts
		next=1;
		head=0;
		something_to_copy=0;

		while(next)
		{//going thtorught headers
			if(strcmp(old_block->header[head].att_name,"")!=0)
			{//if there are more headers
				search_elem=1;
				some_element=GetFirstelementOp(att_root);

				while(search_elem)
				{//going throught row_root list elements

					size=old_block->tuple_dict[i].size;
					//used to check if the data is correct
					int overflov=old_block->tuple_dict[i].size+old_block->tuple_dict[i].address;

					if((strcmp(some_element->attribute_name,old_block->header[head].att_name)==0)&&(size!=0)
&&(overflov<old_block->free_space+1)&&(overflov>-1))
					{//if the data is what we need, if the size is not null, and data is correct	
					
						free2=0;
						for(free2;free2<MAX_VARCHAR_LENGHT;free2++)
							entry_data[free2]='\0';
				
						//copy data
						memcpy(entry_data,
							old_block->data+old_block->tuple_dict[i].address,
								old_block->tuple_dict[i].size);
						
						//insert element to list to be inserted into new table
						InsertNewElementForUpdate(old_block->tuple_dict[i].type,entry_data,
							new_table,some_element->attribute_name, row_root,0);
						
						something_to_copy=1;
					}
					
					some_element= (element_op) GetNextelementOp(some_element);
					if(some_element==0)
					{//if there are more list elements
						search_elem=0;
					}
					
					
				}
				head++; //go to next header	
				i++; //go to next tuple dict
			} 
			else
			{//there are no more headers
				next=0;
			} 
		}
		if(something_to_copy)
		{//write one row of elements to the porjection table
			if(DEBUG)
				printf("Insert one row to projection table");
			insert_row(row_root);
			DeleteAllElements(row_root);
		}
	}
	free(row_root);
	//free(att_root); not here 
	//free(old_block); not here
}

/**
Function makes a projection of some table
@param att_root - list of atributes on which we make projection
@param new_table - table name for projection table
@raturn EXIT_SUCCESS if continues succesfuly, when not EXIT_ERROR
@author Matija Novak
*/
int KK_projekcija(list_op * att_root, char * new_table)
{
	element_op some_element;
	table_addresses * addresses;
	
	some_element= (element_op) GetFirstelementOp(att_root);

	///name of the table from which we read
	char table[MAX_ATT_NAME];
	memcpy(&table,some_element->table,strlen(some_element->table));	
	table[strlen(some_element->table)]='\0';

	if (DEBUG)
		printf("\n Create table: %s, from: ",new_table,table);
	
	///geting the table addresses from table on which we make projection
	addresses= (table_addresses *) get_table_addresses(table);
			
	if(addresses->address_from[0]!=0)
	{
		///create new segmenet for the projection table
		crate_block_header(addresses->address_from[0],new_table,att_root);

		if(DEBUG)
			printf("\n KK_Porojekcija: start copying data");	

			//KK_mem_block *mem_block;
			KK_block *temp_block;
	
			int from=0,to=0,j=0,i=0;
			///Going through blocks and make the projection
			for (j=0;j<MAX_EXTENTS_IN_SEGMENT;j++)
			{
				from=addresses->address_from[j];
				if(from!=0)
				{
					if(DEBUG)
						printf("\n Projekcija: copy ekstent: %d", j);
			
					to=addresses->address_to[j];
					for(i=from;i<=to;i++)
					{
						if(DEBUG)
							printf("\n Projekcija: copy block: %d",i);
						
						//mem_block = KK_get_block( i );
						//temp_block= &mem_block->block;
						temp_block = (KK_block *) KK_read_block( i );
						///making the projection from one block of the table  
						copy_block_projekcija(temp_block,att_root,new_table);
						free(temp_block);
					}
				}
				else break;
			}
	
			free(addresses);
			if(DEBUG)
				printf("\n PROJECTION_TEST_SUCCESS \n");

			return EXIT_SUCCESS;	
			
	}
	else
	{	
		if(DEBUG)
			printf("\n KK_projekcija: Table from which I must read dose not exist");
		return 0;
	}
	

}

//test function for projection must exist table testna whith ime and prezime as atibutes
void op_projekcija_test()
{
	printf("\n PROJECTION_TEST \n");	

	list_op *att_root =  (list_op *) malloc( sizeof(list_op) );
	InitializelistOp(att_root);

	InsertNewelementOp("testna","Ime",att_root);
	element_op some_element;
	some_element = GetFirstelementOp(att_root);
	InsertNewelementOp("testna","Prezime",some_element);

	KK_projekcija(att_root,"testna_projekcija ");
	
	DeleteAllelementsOp(att_root);
	free( att_root );
}
