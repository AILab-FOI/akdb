/**
@file select.c Provides functions for SELECT relational operator
 */

#include "select.h"
#include "../mm/memoman.h"

/**
 * @author Renata Mesaros
 * @brief Function that implements SELECT relational operator
 * @param srcTable - original table that is used for selection
 * @param destTable - table that contains the result
 * @param condition - condition for selection
 * @return EXIT_SUCCESS if cache result in memory and print table else break 
 */
//int AK_select(char *srcTable,char *destTable,AK_list *attributes,AK_list *condition){
int AK_select(char *srcTable,char *destTable,struct list_node *attributes,struct list_node *condition){
	///calling the relational operator for filtering according to given condition
	AK_PRO;
	AK_selection(srcTable,destTable,condition);

	
	

	///help table for the final result
	char *helptable="help_table";


	table_addresses *src_addr=(table_addresses *) AK_get_table_addresses(destTable);
	int startAddress = src_addr->address_from[0];

	AK_block *temp_block = (AK_block *) AK_read_block(startAddress);


	//AK_list_elem list_attributes;
	struct list_node *list_attributes;
	
	AK_header header[MAX_ATTRIBUTES];
 	memset(header, 0, sizeof( AK_header ) * MAX_ATTRIBUTES);

	int new_head=0;
	int head_num[6]={7,7,7,7,7,7};
	int head_counter=0;

	///new header for the resulting table
	int head = 0;


	///going through the header of the table of subscore
	///making a new header for the final result from the selected ones from the subscore
	while(strcmp(temp_block->header[head].att_name, "") != 0) {

		list_attributes = AK_First_L2(attributes);
		int create=0;
		while(list_attributes!=NULL){
			if(strcmp(temp_block->header[head].att_name,list_attributes->data)==0){
				head_num[head_counter++]=head;  ///the ordinal number of the selected attribute
				create=1;
				break;
			}
			else list_attributes=list_attributes->next;
		}
		if(create){
			memcpy(&header[new_head], &temp_block->header[head], sizeof (temp_block->header[head]));
			new_head++;
		}
		head++;
	}
	AK_initialize_new_segment(helptable, SEGMENT_TYPE_TABLE, header);
	

	
	AK_free(temp_block);

	struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));

    int i, j, k, l, type, size, address;
    char data[MAX_VARCHAR_LENGTH];

int b = 0;
printf("\n\n\n");

	for (i = 0; src_addr->address_from[i] != 0; i++) {

			for (j = src_addr->address_from[i]; j < src_addr->address_to[i]; j++) {
				AK_mem_block *temp = (AK_mem_block *) AK_get_block(j);
				if (temp->block->last_tuple_dict_id == 0)
						break;

for (k = 0; k < DATA_BLOCK_SIZE;k+=5) {
	if (temp->block->tuple_dict[k].type == FREE_INT)
						break;

			int gl=0;

			for(l=0;l<5;l++){
				int write=0;
				///if the attribute number is in the selected list, write it in the resulting table
				while(head_num[b] != 7) {
							if(head_num[b++]==l){write=1;break;}
					}
					if(write==1){
						type = temp->block->tuple_dict[l + k].type;
						size = temp->block->tuple_dict[l + k].size;

						address = temp->block->tuple_dict[l + k].address;
						memcpy(data, &(temp->block->data[address]), size);
						data[size] = '\0';
						AK_Insert_New_Element(type, data, helptable, header[gl++].att_name, row_root);


					}
					b=0;
				}
				AK_insert_row(row_root);
				AK_DeleteAll_L3(&row_root);
	}
	
}}

  AK_print_table(helptable);
	
	/**CACHE RESULT IN MEMORY**/

	AK_cache_result(srcTable,temp_block,header);
	AK_free(temp_block);
	AK_EPI;
	return EXIT_SUCCESS;

}


/**
 * @author Renata Mesaros
 * @brief Function for testing the implementation
 */
TestResult AK_select_test(){
	AK_PRO;


	printf("\n\n\n ***** SELECT RELATIONAL OPERATOR ***** \n\n\n");
	
	///list of attributes which will be in the result of selection

	struct list_node *attributes = (struct list_node *) AK_malloc(sizeof (struct list_node));
	AK_Init_L3(&attributes);

	///list of elements which represent the condition for selection

	struct list_node *condition = (struct list_node *) AK_malloc(sizeof (struct list_node));
	AK_Init_L3(&condition);


	char *num = "2005";

	char *srcTable="student";
	char *destTable="select_result";
	
	AK_InsertAtEnd_L3(TYPE_ATTRIBS, "firstname", sizeof ("firstname"), attributes);
	AK_InsertAtEnd_L3(TYPE_ATTRIBS, "year", sizeof ("year"), attributes);
	AK_InsertAtEnd_L3(TYPE_ATTRIBS, "weight", sizeof ("weight"), attributes);

	AK_InsertAtEnd_L3(TYPE_ATTRIBS, "year", sizeof ("year"), condition);
	AK_InsertAtEnd_L3(TYPE_INT, num, sizeof (int), condition);
	AK_InsertAtEnd_L3(TYPE_OPERATOR, "<", sizeof ("<"), condition);

	printf("\n SELECT firstname,year,weight FROM student WHERE year<2005;\n\n");

	AK_select(srcTable, destTable, attributes, condition);
	
	AK_DeleteAll_L3(&attributes);
	AK_free(attributes);

	
	AK_DeleteAll_L3(&condition);
	AK_free(condition);
	AK_EPI;

	return TEST_result(0,0);
}
