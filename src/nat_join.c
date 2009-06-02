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

void crate_spoj_block_header(int one,int two,char * new_table,list_op * att_root)
{
	KK_block * temp_block;
	KK_header header[MAX_ATTRIBUTES];
	element_op some_element;
	temp_block=KK_read_block(one);
	int next_header=1;
	int search_elem;
	int head=0;
	int new_head=0;
	int s_copy;
	while(next_header)
	{
		s_copy=1;
		if(strcmp(temp_block->header[head].att_name,"")!=0)
		{
			search_elem=1;
			some_element=GetFirstelementOp(att_root);
			while(search_elem)
			{
				if(strcmp(some_element->attribute_name,temp_block->header[head].att_name)==0)
				{
					s_copy=0;
				}
				some_element=GetNextelementOp(some_element);
				if(some_element==0)
				{
					search_elem=0;
				}
			}
			if(s_copy)
			{
				memcpy( & header[new_head], & temp_block->header[head], 
						sizeof( temp_block->header[head] ) );
				printf("Copy attribute headrer: %s",header[new_head].att_name);
				new_head++;	
			}
			head++;	
		}
		else
		{
			next_header=0;
		}
		
	}	
	temp_block=KK_read_block(two);
	next_header=1;
	head=0;
	while(next_header)
	{
		if(strcmp(temp_block->header[head].att_name,"")!=0)
		{
			
			memcpy( & header[new_head], & temp_block->header[head], 
				sizeof( temp_block->header[head] ) );
			printf("Copy attribute headrer2: %s",header[new_head].att_name);
			new_head++;	
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

void merge_block_spoj(list * row_root,list * row_root_insert, KK_block * temp_block,list_op * att_root, char * new_table)
{
	printf("\n COPYING SPOJ...");
	element some_element;
	list * row_root_insert2 =  (list *) malloc( sizeof(list) );
	InitializeList(row_root_insert2);
	int next;
	int head;//printf("\n tu sam 12 \n");
	int search_elem;
	int entry_data[200];
	int something_to_copy=1;
	int i;
	int free;
	int size;
	int not_in_list=1;
	int overflov;
	//return 0;
//	printf("\n tu sam \n");
	for (i=0;i<DATA_BLOCK_SIZE; ) 
	{//going through headers i=i+head	
		next=1;
		head=0;
		something_to_copy=1;
		//printf("\n tu sam 10 \n");
		//kopitanje stare liste
		
		search_elem=1;
		some_element=GetFirstElement(row_root_insert);
		//printf("\n tu sam 12 \n");
		while(search_elem)
		{
			
			InsertNewElementForUpdate(some_element->type,some_element->data,
							new_table,some_element->attribute_name, row_root_insert2,0);		
			//printf("AAAAAAAA");
			some_element=GetNextElement(some_element);
			if(some_element==0)
			{
				search_elem=0;
			}
		}
		
		while(next)
		{
			if(strcmp(temp_block->header[head].att_name,"")!=0)
			{
				//printf("\n tu sam 11 \n");
				search_elem=1;
				some_element=GetFirstElement(row_root);
				//printf("\n tu sam 12 \n");
				while(search_elem)
				{
				//	printf("\n tu sam 13 \n");
				//	printf("\n %s %d %s",some_element->attribute_name,temp_block->tuple_dict[i].size,temp_block->header[head].att_name);
					size=temp_block->tuple_dict[i].size;
					overflov=temp_block->tuple_dict[i].size+temp_block->tuple_dict[i].address;
					printf("\n overflov: %d, %d",overflov,i);
					
					if((strcmp(some_element->attribute_name,temp_block->header[head].att_name)==0)&&(size!=0)
&&(overflov<(temp_block->free_space+1))&&(overflov>-1))
					{			
						free=0;
					
						for(free;free<200;free++)
							entry_data[free]='\0';
					//	printf("\ntu sam 144\n");
					//	printf("\n Data: , %d, %d,%d,\n",temp_block->tuple_dict[i].address,temp_block->tuple_dict[i].size,temp_block->free_space);
						memcpy(entry_data,
						temp_block->data+temp_block->tuple_dict[i].address,
							temp_block->tuple_dict[i].size);
						//printf("\n Data: %s, %d, %d,%d,\n",entry_data,temp_block->tuple_dict[i].address,temp_block->tuple_dict[i].size,temp_block->free_space);
						if(strcmp(some_element->data,entry_data)!=0)
						{
							something_to_copy=0;
							printf("TU SAM 555");
						}
						not_in_list=0;
					}
					else if((size==0)||(overflov>(temp_block->free_space+1))||(overflov<-1))
					{	
						something_to_copy=0;	
					}

					some_element=GetNextElement(some_element);
					if(some_element==0)
					{
						search_elem=0;
					}
					
					
				}
				
				size=temp_block->tuple_dict[i].size;
				overflov=temp_block->tuple_dict[i].size+temp_block->tuple_dict[i].address;
				
				//printf("\n tu sam 21 \n");
				if((not_in_list==1)&&(size!=0)&&(overflov<temp_block->free_space+1)&&(overflov>-1))
				{	
		
				//	printf("\n tu sam 22 \n");
					free=0;
					for(free;free<200;free++)
						entry_data[free]='\0';
				//	printf("\n tu sam 23 \n");
				//	printf("\n Data2: %s, %d, %d,%d,\n",entry_data,temp_block->tuple_dict[i].address,temp_block->tuple_dict[i].size,temp_block->free_space);
					memcpy(entry_data,
						temp_block->data+temp_block->tuple_dict[i].address,
							temp_block->tuple_dict[i].size);
					//printf("\n \nEntry: %s", entry_data);
					//printf("\ntu sam 24\n");
					InsertNewElementForUpdate(temp_block->tuple_dict[i].type,entry_data,
						new_table,temp_block->header[head].att_name, row_root_insert2,0);
					element neki;
					neki=GetFirstElement(row_root_insert2);
					printf("NEKI: %s,%d,%s,%s,",neki->data,neki->type,neki->attribute_name,neki->table);
				}				
				not_in_list=1;
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
			insert_row(row_root_insert2);
			printf("UBACUJEM");	
		}
		DeleteAllElements(row_root_insert2);
	}
	printf("ALO");
}

void copy_blocks_spoj(KK_block * temp_block,KK_block * temp_block2,list_op * att_root,char * new_table)
{
	printf("\n COPYING SPOJ...");
	list *row_root =  (list *) malloc( sizeof(list) );
	list *row_root_insert =  (list *) malloc( sizeof(list) );
	//printf("\n tu sam");
	InitializeList(row_root);
	InitializeList(row_root_insert);
	//printf("\n tu sam");
	element_op some_element;
	
	int next;
	int head;//printf("\n tu sam 12 \n");
	int search_elem;
	int entry_data[200];
	int something_to_copy=0;
	int i;
	int free;
	int size;
	int not_in_list=1;
	int overflov;
//	printf("\n tu sam \n");
	for (i=0;i<DATA_BLOCK_SIZE; ) 
	{//going through headers i=i+head	
		next=1;
		head=0;
		something_to_copy=0;
		//printf("\n tu sam 10 \n");
		while(next)
		{
			if(strcmp(temp_block->header[head].att_name,"")!=0)
			{
				//printf("\n tu sam 11 \n");
				search_elem=1;
				some_element=GetFirstelementOp(att_root);
				//printf("\n tu sam 12 \n");
				while(search_elem)
				{
					//printf("\n tu sam 13 \n");
					//printf("\n %s %d %s",some_element->attribute_name,temp_block->tuple_dict[i].size,temp_block->header[head].att_name);
					size=temp_block->tuple_dict[i].size;
					overflov=temp_block->tuple_dict[i].size+temp_block->tuple_dict[i].address;
					//printf("\n overflov: %d",overflov);
					
					if((strcmp(some_element->attribute_name,temp_block->header[head].att_name)==0)&&(size!=0)
&&(overflov<(temp_block->free_space+1))&&(overflov>-1))
					{			
						free=0;
					
						for(free;free<200;free++)
							entry_data[free]='\0';
						
						//printf("\n Data: %s, %d, %d,%d,\n",entry_data,temp_block->tuple_dict[i].address,temp_block->tuple_dict[i].size,temp_block->free_space);
						memcpy(entry_data,
							temp_block->data+temp_block->tuple_dict[i].address,
								temp_block->tuple_dict[i].size);
						printf("\n \nEntry: %s", entry_data);
						//printf("",);
						InsertNewElementForUpdate(temp_block->tuple_dict[i].type,entry_data,
							new_table,some_element->attribute_name, row_root,0);

						InsertNewElementForUpdate(temp_block->tuple_dict[i].type,entry_data,
							new_table,some_element->attribute_name, row_root_insert,0);
						element neki;
						neki=GetFirstElement(row_root);
						printf("NEKI: %s ",neki->data);
						something_to_copy=1;
						not_in_list=0;
					}

					some_element=GetNextelementOp(some_element);
					if(some_element==0)
					{
						search_elem=0;
					}
					
					
				}

				size=temp_block->tuple_dict[i].size;
				overflov=temp_block->tuple_dict[i].size+temp_block->tuple_dict[i].address;
				
				//printf("\n tu sam 21 \n");
				if((not_in_list==1)&&(size!=0)&&(overflov<temp_block->free_space+1)&&(overflov>-1))
				{	
					
					//printf("\n tu sam 22 \n");
					free=0;
					for(free;free<200;free++)
						entry_data[free]='\0';
					//printf("\n tu sam 23 \n");
					//printf("\n Data: %s, %d, %d,%d,\n",entry_data,old_block->tuple_dict[i].address,old_block->tuple_dict[i].size,old_block->free_space);
					memcpy(entry_data,
						temp_block->data+temp_block->tuple_dict[i].address,
							temp_block->tuple_dict[i].size);
					//printf("\n \nEntry: %s", entry_data);
					//printf("\ntu sam 24\n");
					InsertNewElementForUpdate(temp_block->tuple_dict[i].type,entry_data,
						new_table,temp_block->header[head].att_name, row_root_insert,0);
					element neki;
					neki=GetFirstElement(row_root_insert);
					printf("NEKI: %s,%d,%s,%s,",neki->data,neki->type,neki->attribute_name,neki->table);
				}				
				not_in_list=1;
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
			//insert_row(row_root_insert);
			merge_block_spoj(row_root,row_root_insert,temp_block2,att_root,new_table);
			printf("UBACUJEM");
			DeleteAllElements(row_root);
			DeleteAllElements(row_root_insert);
		}
	}
}

int KK_spoj(list_op * att_root,char * old_table_one, char * old_table_two, char * new_table)
{
	table_addresses * addresses_one;
	table_addresses * addresses_two;
	
//	printf("\n TUUU0");

	if (DEBUG)
		printf("Create table: %s, from: %s, %s",new_table,old_table_one,old_table_two);
	
	addresses_one=get_table_addresses(old_table_one);
	addresses_two=get_table_addresses(old_table_two);
			
	if((addresses_one->address_from[0]!=0)&&(addresses_two->address_from[0]!=0))
	{
		crate_spoj_block_header(addresses_one->address_from[0],addresses_two->address_from[0],new_table,att_root);
		printf("\nSpoj: start copying data");	

			//table_addresses * addresses_new = (table_addresses *) malloc(sizeof(table_addresses));
			//addresses_new=get_table_addresses(new_table);

			//KK_mem_block *mem_block = (KK_mem_block *) malloc(sizeof(KK_mem_block));
			KK_block *temp_block;
			KK_block *temp_block2;
	
			int from=0,to=0,j=0,i=0;
			int from2=0,to2=0,j2=0,i2=0;
			for (j=0;j<200;j++)
			{
				from=addresses_one->address_from[j];
				if(from!=0)
				{
					printf("\n Spoj: copy ekstent1: %d", j);
			
					to=addresses_one->address_to[j];
					for(i=from;i<=to;i++)
					{
						printf("\n Spoj: copy block1: %d",i);
						//mem_block = KK_get_block( i );
						temp_block=KK_read_block( i );
						//traženje sad u bloku zapis  i brisanje
						//copy_block_projekcija(temp_block,att_root,new_table);
						if(temp_block->free_space!=0)
						{
						for (j2=0;j2<200;j2++)
						{
							from2=addresses_two->address_from[j2];
							if(from2!=0)
							{
								printf("\n Spoj: copy ekstent2: %d", j2);
			
								to2=addresses_two->address_to[j2];
								for(i2=from2;i2<=to2;i2++)
								{
									printf("\n Spoj: copy block2: %d",i2);
									//mem_block = KK_get_block( i );
									temp_block2=KK_read_block( i2 );
									//traženje sad u bloku zapis  i brisanje
								if(temp_block2->free_space!=0)
								{
									copy_blocks_spoj(temp_block,temp_block2,att_root,new_table);
}
								}
							}
							else break;
						}
						}
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

void op_spoj_test()
{
	printf("\n STRATING prirodni spoj \n");

	list_op *att_root =  (list_op *) malloc( sizeof(list_op) );
	InitializelistOp(att_root);
	InsertNewelementOp("testna","Ime",att_root);
	element_op some_element;
	some_element = GetFirstelementOp(att_root);
	InsertNewelementOp("testna","Prezime",some_element);
	KK_spoj(att_root,"testna","testna_druga","testna_spoj");
	free( att_root );
}
