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
#include "nat_join.h"
#include "projection.h"
#include "dbman.h"
#include "memoman.h"
#include "string.h"
#include "configuration.h"
#include "auxiliary.h"
#include "fileio.h"

/** 	@author Matija Novak
	function makes header for the new table and call the function to create the segment
	@param one - address of the block of the first table
	@param two - address of the block of the second table
	@param new_table - name of the join table
	@param att_root - attributes on which we make nat_join
	@result void
*/
void crate_spoj_block_header(int one,int two,char * new_table,list_op * att_root)
{
	KK_block * temp_block;
	KK_header header[MAX_ATTRIBUTES];
	element_op some_element;
	temp_block= (KK_block *) KK_read_block(one);

	int next_header=1; //boolean var to indicate there are more headers
	int search_elem;
	int head=0;//counter of the heads 
	int new_head=0; //counter of heads to write
	int s_copy;//indicate if we copy these header or not

	while(next_header)
	{//going throug headers
		s_copy=1;
		if(strcmp(temp_block->header[head].att_name,"")!=0)
		{//if there are more headers

			search_elem=1;
			some_element= (element_op) GetFirstelementOp(att_root);

			while(search_elem)
			{//searches elements in list
				if(strcmp(some_element->attribute_name,temp_block->header[head].att_name)==0)
				{//if is element on wich we make join skip it
					s_copy=0;
				}
	
				some_element= (element_op) GetNextelementOp(some_element);
				if(some_element==0)
				{//more elements in list
					search_elem=0;
				}
			}
			if(s_copy)
			{//if is header to copy
				memcpy( & header[new_head], & temp_block->header[head], 
						sizeof( temp_block->header[head] ) );
				if(DEBUG)
					printf(" NAT join: Copy attribute headrer: %s",header[new_head].att_name);
				new_head++;//counter of new headers
			}
			head++;	//go to next header
		}
		else
		{//no more headers in table 1
			next_header=0;
		}
		
	}	

	free(temp_block);
	temp_block= (KK_block *) KK_read_block(two);
	next_header=1;
	head=0;

	while(next_header)
	{//going throught headers of table 2
		if(strcmp(temp_block->header[head].att_name,"")!=0)
		{//copy all headers if some exist
			
			memcpy( & header[new_head], & temp_block->header[head], 
				sizeof( temp_block->header[head] ) );
			if(DEBUG)
				printf("NAT join: Copy attribute headrer2: %s",header[new_head].att_name);
			//next old and new header
			new_head++;	
			head++;	
		}
		else
		{//no more headers in table 2
			next_header=0;
		}
	}
	
	temp_crate_table(new_table, header, SEGMENT_TYPE_TABLE);
	free( temp_block );	
}

/** 	@author Matija Novak
	function searches the second block and when found matches with the first one makes a join and write row to join table
	@param row_root - list of values from first table to be marged with table 2
	@param row_root_insert - list of values from first table to be inserted into nat_join table
	@param temp_block - block from second tabl to be merged
	@param att_root - attributes on which we make nat_join
	@param new_table - name of the naj_join table
	@result void
*/
void merge_block_spoj(list * row_root,list * row_root_insert, KK_block * temp_block,list_op * att_root, char * new_table)
{
	if(DEBUG)
		printf("\n MERGE NAT JOIN...");
	element some_element;
	list * row_root_insert2 =  (list *) malloc( sizeof(list) );
	InitializeList(row_root_insert2);

	int next; //boolean var if there are more headers
	int head; //counter of the headers
	int search_elem; //boolean var if there is more elements in list
	char entry_data[MAX_VARCHAR_LENGHT];
	int something_to_copy=1; //if there is something to copy
	int i; //counter of tuple_dicts
	int free2;//clearing varchar varijable
	int size;
	int not_in_list=1;
	int overflov;

	for (i=0;i<DATA_BLOCK_SIZE; ) 
	{//going through tuple_dict of the secnond table	
		next=1;
		head=0;
		something_to_copy=1;
		search_elem=1;

		some_element= (element) GetFirstElement(row_root_insert);

		while(search_elem)
		{//make a copy of insert row list of the first table
			InsertNewElementForUpdate(some_element->type,some_element->data,
							new_table,some_element->attribute_name, row_root_insert2,0);		
			some_element=(element)GetNextElement(some_element);

			if(some_element==0)
			{//if there is more elements in list
				search_elem=0;
			}
		}
		
		while(next)
		{//going throught headers of the secnod table
			if(strcmp(temp_block->header[head].att_name,"")!=0)
			{//there are more headers
				search_elem=1;
				some_element=(element)GetFirstElement(row_root);

				while(search_elem)
				{//going throug list of elements to compare
					size=temp_block->tuple_dict[i].size;
					overflov=temp_block->tuple_dict[i].size+temp_block->tuple_dict[i].address;
					
					if((strcmp(some_element->attribute_name,temp_block->header[head].att_name)==0)&&(size!=0)
&&(overflov<(temp_block->free_space+1))&&(overflov>-1))
					{//if ist an element in list, ad if its ok data, and size not null			
						free2=0;
						for(free2;free2<MAX_VARCHAR_LENGHT;free2++)
							entry_data[free2]='\0';
					
						memcpy(entry_data,
						temp_block->data+temp_block->tuple_dict[i].address,
							temp_block->tuple_dict[i].size);
					
						if(strcmp(some_element->data,entry_data)!=0)
						{//if merge data is not equal
							something_to_copy=0; //dont copy these set of tuple_dicts
						}
						not_in_list=0;
					}
					else if((size==0)||(overflov>(temp_block->free_space+1))||(overflov<-1))
					{	
						something_to_copy=0;	
					}

					some_element= (element) GetNextElement(some_element);
					if(some_element==0)
					{//if there are more elements to compare for merge
						search_elem=0;
					}
					
					
				}
				
				size=temp_block->tuple_dict[i].size;
				overflov=temp_block->tuple_dict[i].size+temp_block->tuple_dict[i].address;
				
				if((not_in_list==1)&&(size!=0)&&(overflov<temp_block->free_space+1)&&(overflov>-1))
				{//if its not header which is in compare list then I can copy these tuple dicts becouse it not exist in the insert_row_list2
					free2=0;
					for(free2;free2<MAX_VARCHAR_LENGHT;free2++)
						entry_data[free2]='\0';
			
					memcpy(entry_data,
						temp_block->data+temp_block->tuple_dict[i].address,
							temp_block->tuple_dict[i].size);
					
					//insert data from second table to insert_list
					InsertNewElementForUpdate(temp_block->tuple_dict[i].type,entry_data,
						new_table,temp_block->header[head].att_name, row_root_insert2,0);
				}				
				not_in_list=1;
				head++;	//next header
				i++;//next tuple dict
			} 
			else
			{//no more headers
				next=0;
			} 
		}
		
		if(something_to_copy)
		{//if these set is one that is passes merge then insert the list of data to join table
			insert_row(row_root_insert2);	
		}
		DeleteAllElements(row_root_insert2);
	}
	free(row_root_insert2);
}

/** 	@author Matija Novak
	function gous through block of the first table and copies data tah it need to join then calls a merge function to
	merge with secnond table
	@param temp_block - block of the first zable
	@param temp_block - block of the second join table
	@param att_root - attributes on which we make nat_join
	@param new_table - name of the naj_join table
	@result void
*/
void copy_blocks_spoj(KK_block * temp_block,KK_block * temp_block2,list_op * att_root,char * new_table)
{
	if(DEBUG)
		printf("\n COPYING NAT JOIN");

	list *row_root =  (list *) malloc( sizeof(list) );
	list *row_root_insert =  (list *) malloc( sizeof(list) );
	InitializeList(row_root);
	InitializeList(row_root_insert);
	element_op some_element;
	
	//same use as in merge function only for the first table
	int next;
	int head;
	int search_elem;
	char entry_data[MAX_VARCHAR_LENGHT];
	int something_to_copy=0;
	int i;
	int free2;
	int size;
	int not_in_list=1;
	int overflov;

	for (i=0;i<DATA_BLOCK_SIZE; ) 
	{//going through tuple_dict of the first table block	
		next=1;
		head=0;
		something_to_copy=0;

		while(next)
		{//going throug headers
			if(strcmp(temp_block->header[head].att_name,"")!=0)
			{//there are more headers?
				search_elem=1;
				some_element=(element_op)GetFirstelementOp(att_root);

				while(search_elem)
				{//going throug list of elements on which we merge
					size=temp_block->tuple_dict[i].size;
					overflov=temp_block->tuple_dict[i].size+temp_block->tuple_dict[i].address;
					
					if((strcmp(some_element->attribute_name,temp_block->header[head].att_name)==0)&&(size!=0)
&&(overflov<(temp_block->free_space+1))&&(overflov>-1))
					{//if we found an element tahat we need, and its ok we copy it			
						free2=0;
						for(free2;free2<MAX_VARCHAR_LENGHT;free2++)
							entry_data[free2]='\0';
						
						memcpy(entry_data,
							temp_block->data+temp_block->tuple_dict[i].address,
								temp_block->tuple_dict[i].size);
						//insert element into list on which we compare
						InsertNewElementForUpdate(temp_block->tuple_dict[i].type,entry_data,
							new_table,some_element->attribute_name, row_root,0);
						//insert element into list which we insert into join_table toether with second table data 
						InsertNewElementForUpdate(temp_block->tuple_dict[i].type,entry_data,
							new_table,some_element->attribute_name, row_root_insert,0);
						
						something_to_copy=1;
						not_in_list=0;
					}

					some_element= (element_op) GetNextelementOp(some_element);
					if(some_element==0)
					{//more elements?
						search_elem=0;
					}
					
					
				}

				size=temp_block->tuple_dict[i].size;
				overflov=temp_block->tuple_dict[i].size+temp_block->tuple_dict[i].address;
				
				if((not_in_list==1)&&(size!=0)&&(overflov<temp_block->free_space+1)&&(overflov>-1))
				{//copy element which is not for merge only for insert
					
					free2=0;
					for(free2;free2<MAX_VARCHAR_LENGHT;free2++)
						entry_data[free2]='\0';

					memcpy(entry_data,
						temp_block->data+temp_block->tuple_dict[i].address,
							temp_block->tuple_dict[i].size);
	
					InsertNewElementForUpdate(temp_block->tuple_dict[i].type,entry_data,
						new_table,temp_block->header[head].att_name, row_root_insert,0);
				}				
				not_in_list=1;
				head++;	//next header
				i++;//next tuple dict
			} 
			else
			{//no more eheaders
				next=0;
			} 
		}
		if(something_to_copy)
		{//if there is data to copy
			//merege data with second table
			merge_block_spoj(row_root,row_root_insert,temp_block2,att_root,new_table);
			DeleteAllElements(row_root);
			DeleteAllElements(row_root_insert);
		}
	}
	free(row_root);
	free(row_root_insert);
}

/** 	@author Matija Novak
	function to make nat_join betwen two tables on some attributes
	@param old_table_one - name of the frst table to join
	@param old_table_two - name of the second table to join
	@param att_root - attributes on which we make nat_join
	@param new_table - name of the naj_join table
	@result if succes returns EXIT_SUCCESS
*/
int KK_spoj(list_op * att_root,char * old_table_one, char * old_table_two, char * new_table)
{
	table_addresses * addresses_one;
	table_addresses * addresses_two;
	
	if (DEBUG)
		printf("NAT JOIN: Create table: %s, from: %s, %s",new_table,old_table_one,old_table_two);
	
	addresses_one=(table_addresses *)get_table_addresses(old_table_one);
	addresses_two=(table_addresses *)get_table_addresses(old_table_two);
			
	if((addresses_one->address_from[0]!=0)&&(addresses_two->address_from[0]!=0))
	{//if there is some table one and two
		//make a nat_join table
		crate_spoj_block_header(addresses_one->address_from[0],addresses_two->address_from[0],new_table,att_root);

		KK_block *temp_block;
		KK_block *temp_block2;
	
		int from=0,to=0,j=0,i=0;
		int from2=0,to2=0,j2=0,i2=0;
		for (j=0;j<MAX_EXTENTS_IN_SEGMENT;j++)
		{//going through extests of the first table
			from=addresses_one->address_from[j];
			if(from!=0)
			{	
				if(DEBUG)
					printf("\n Nat Join: copy ekstent1: %d", j);
		
				to=addresses_one->address_to[j];
				for(i=from;i<=to;i++)
				{//going through bloks of the first table
					if(DEBUG)
						printf("\n NAt join: copy block1: %d",i);
					
					temp_block=(KK_block *) KK_read_block( i );
					
					if(temp_block->free_space!=0)
					{//if its some data in block
						for (j2=0;j2<MAX_EXTENTS_IN_SEGMENT;j2++)
						{//going through extests of the second table
							from2=addresses_two->address_from[j2];
							if(from2!=0)
							{
								if(DEBUG)
									printf("\n Nat join: copy ekstent2: %d", j2);
		
								to2=addresses_two->address_to[j2];
								for(i2=from2;i2<=to2;i2++)
								{//going through bloks of the second table
									if(DEBUG)	
										printf("\n Nat join: copy block2: %d",i2);
									
									temp_block2= (KK_block *) KK_read_block( i2 );
									
								  if(temp_block2->free_space!=0){
								   copy_blocks_spoj(temp_block,temp_block2,att_root,new_table);}
								}
							}
							else break;
						}
					}
				}
			}
			else break;
		}	
		free(addresses_one);
		free(addresses_two);
		free(temp_block);
		free(temp_block2);
		return EXIT_SUCCESS;	
			
	}
	else
	{	
		printf("KK_spoj: Table from which I must read not exist");
		free(addresses_one);
		free(addresses_two);
		return 0;
	}
	
}

//test function of nat join must have table testa with atributes Ime and Prezime and table testna_druga also with the same attributes
void op_spoj_test()
{
	printf("\n STRATING NAT JOIN \n");

	list_op *att_root =  (list_op *) malloc( sizeof(list_op) );
	InitializelistOp(att_root);

	InsertNewelementOp("testna","Ime",att_root);
	element_op some_element;
	some_element = (element_op) GetFirstelementOp(att_root);

	InsertNewelementOp("testna","Prezime",some_element);
	KK_spoj(att_root,"testna","testna_druga","testna_spoj");
	
	free( att_root );
}
