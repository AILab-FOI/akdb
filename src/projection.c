/**
@file projection.c Provides functions for relational projection operation
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

#include "projection.h"


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
                element_op Currentelement_op = L->next;
		int i = 0;
                do{
                    if(Currentelement_op == Searchedelement_op) break;
                    Currentelement_op = Currentelement_op->next;
                    i++;
		}while (Currentelement_op);
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

/** 
 * @brief  Temporaly function to create table, and insert entry to the system_relation catalog
 * @author Matija Novak, updated by Dino Laktašić
 * @param table - table name
 * @param header - AK_header of the new table
 * @param type_segment - type of the new segment
 * @return void
*/
void AK_temp_create_table(char *table, AK_header *header, int type_segment) {
	AK_block *sys_block = (AK_block *)malloc(sizeof(AK_block));
	sys_block = (AK_block *)AK_read_block(1);

	int startAddress = AK_initialize_new_segment(table, type_segment, header);
	/*
	if (startAddress == EXIT_ERROR) {
		return EXIT_ERROR; 
	}
	*/
	
	if (DEBUG)	
		printf("temp_create_table: First block address of the new segmet: %d", startAddress);	
	
	//AK_create_header(table, type_segment, header->type, header->integrity, header->constr_name, header->constr_code);
	int broj = 8;
	//insert object_id
	AK_insert_entry(sys_block, TYPE_INT, &broj, 8 );
	//insert table name	
	AK_insert_entry(sys_block, TYPE_VARCHAR, table, 9 );
	//insert start address	
	broj = startAddress;	
	AK_insert_entry(sys_block, TYPE_INT, &broj, 10 );	
	//insert end address
	broj = startAddress + 19;
	AK_insert_entry(sys_block, TYPE_INT, &broj, 11 );
		
	AK_write_block(sys_block);
	free(sys_block);
}

/** 
 * @brief  Function to create a new header for the projection table
 * @author Matija Novak, rewrited and optimized by Dino Laktašić to support AK_list 
 * @param old_block_add - address of the block from which we copy headers we need
 * @param dstTable - name of the new table
 * @param att - list of the attributes which should the projeciton table contain
 * @return void
*/
void AK_create_block_header(int old_block, char *dstTable, AK_list *att) {
	AK_block *temp_block = (AK_block *)malloc(sizeof(AK_block));
	temp_block = (AK_block *) AK_read_block(old_block);
	
	AK_list_elem list_elem;
	AK_header header[MAX_ATTRIBUTES];
	
	int head = 0; 		//counter of the headers
	int new_head = 0; 	//counter of the new headers created for the projection table

	while (strcmp(temp_block->header[head].att_name, "") != 0) {
		list_elem = (AK_list_elem)FirstL(att);
		
		while (list_elem != NULL) {
			//if header is found than copy header 
			if (strcmp(list_elem->data, temp_block->header[head].att_name) == 0) {
				//make a copy of the header needed for projection
				memcpy(&header[new_head], &temp_block->header[head], sizeof(temp_block->header[head]));
				
				if (DEBUG) 
					printf("Copy attribute header: %s\n", header[new_head].att_name);
				
				new_head++;	
			}
			list_elem = list_elem->next;
		}
		head++;	
	}
	
	memset(header + new_head, '\0', MAX_ATTRIBUTES - new_head);
	//AK_new_segment(dstTable, SEGMENT_TYPE_TABLE, header);
	
	free(temp_block);
	//return AK_initialize_new_segment(dstTable, SEGMENT_TYPE_TABLE, header);
	AK_temp_create_table(dstTable, header, SEGMENT_TYPE_TABLE);
}

/** @brief  Function for copying the data from old table block to the new projection table
	@author Matija Novak, rewrited and optimized by Dino Laktašić to support AK_list
	@param old_block - block from which we copy data
	@param dstTable - name of the new table
	@param att - list of the attributes which should the projeciton table contain
	@result void
*/
void AK_copy_block_projection(AK_block *old_block, AK_list *att, char *dstTable) {
	//if(DEBUG) 
	//	printf("\nCOPYING PROJECTION DATA FROM BLOCK...\n");

	element row_root = (element)malloc(sizeof(list));
	InitializeList(row_root);

	AK_list_elem list_elem;
	
	int i; 						//tuple_dict counter
	int head;					//head counter
	int something_to_copy;		//boolean variable to indicate if there is data to copy in these set of tuple_dicts
	int size; 					//current tuple_dict data size
	
	char data[MAX_VARCHAR_LENGHT]; //data to copy
	
	//iterate through all tuple_dicts in block
	for (i = 0; i < DATA_BLOCK_SIZE;) {
		head = something_to_copy = 0;
		
		//if (old_block->tuple_dict[i].type == FREE_INT)
		//	break;
			
		while (strcmp(old_block->header[head].att_name, "") != 0) {
			list_elem = (AK_list_elem)FirstL(att);

			while (list_elem != NULL) {
				size = old_block->tuple_dict[i].size;
				//used to check if the data is correct
				int overflow = old_block->tuple_dict[i].size + old_block->tuple_dict[i].address;

				//if the data is what we need, if the size is not null, and data is correct	
				if ((strcmp(list_elem->data, old_block->header[head].att_name) == 0) && (size != 0)
					&& (overflow < old_block->free_space + 1) && (overflow > -1)) {
					
					memset(data, 0, MAX_VARCHAR_LENGHT);
					memcpy(data, old_block->data + old_block->tuple_dict[i].address, old_block->tuple_dict[i].size);
					
					//insert element to list to be inserted into new table
					InsertNewElement(old_block->tuple_dict[i].type, data, dstTable, list_elem->data, row_root); //ForUpdate 0
					something_to_copy = 1;
				}
				list_elem = list_elem->next;
			}
			head++;
			i++;
		}
		
		//write row to the porojection table
		if (something_to_copy) {
			if(DEBUG) 
				printf("\nInsert row to projection table.\n");
				
			insert_row(row_root);
			DeleteAllElements(row_root);
		}
	}
	
	free(row_root);
}

/**
*@brief  Function makes a projection of some table
*@author Matija Novak, rewrited and optimized by Dino Laktašić, now support cacheing
*@param att - list of atributes on which we make projection
*@param dstTable - table name for projection table
*@return EXIT_SUCCESS if continues succesfuly, when not EXIT_ERROR
*/
int AK_projection(char *srcTable, char *dstTable, AK_list *att) {
	//geting the table addresses from table on which we make projection
	table_addresses *src_addr = (table_addresses *) get_table_addresses(srcTable);
			
	if (src_addr->address_from[0] != 0) {
		//create new segmenet for the projection table
		AK_create_block_header(src_addr->address_from[0], dstTable, att);

		if (DEBUG) {
			printf("\nTABLE %s CREATED from %s!\n", dstTable, srcTable);
			printf("\nAK_projection: start copying data\n");	
		}
		
		int startAddress = 0, i = 0, j;

		//for each extent that contains blocks needed for projection
		for (i = 0; src_addr->address_from[i] != 0; i++) {  //MAX_EXTENTS_IN_SEGMENT
			startAddress = src_addr->address_from[i];
	
			if (startAddress != 0) {
				if (DEBUG) {
					printf("\nAK_projection: copy extent: %d\n", i);
				}
			
				//for each block in extent
				for (j = startAddress; j <= src_addr->address_to[i]; j++) {
					AK_mem_block *temp = (AK_mem_block *) AK_get_block(j);
					//AK_block *temp = (AK_block *) AK_read_block(j);
								
					if (temp->block->last_tuple_dict_id == 0) {
						free(temp);
						break;
					}
					
					if (DEBUG) 
						printf("\nAK_projection: copy block: %d\n", j);
					
					//get projection tuples from block
					AK_copy_block_projection(temp->block, att, dstTable);
					//
				}
			}
			else break;
		}
		free(src_addr);
		
		if (DEBUG) 
			printf("PROJECTION_TEST_SUCCESS\n");
		return EXIT_SUCCESS;	
	} else {	
		if (DEBUG) 
			printf("\n AK_projection: Table doesn't exist!");
		return EXIT_ERROR;
	}
}

//test function for projection must exist table testna whith ime and prezime as atibutes
void op_projekcija_test() {
	printf( "\n********** PROJECTION TEST **********\n\n");	
	
	AK_list *att = (AK_list *)malloc(sizeof(AK_list));
	InitL(att);

	InsertAtEndL(TYPE_ATTRIBS, "firstname", sizeof("firstname"), att);
	InsertAtEndL(TYPE_ATTRIBS, "lastname", sizeof("lastname"), att);

	AK_projection("student", "projection_test", att);
	AK_print_table("projection_test");
	
	DeleteAllL(att);
	free(att);
}
