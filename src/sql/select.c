#include "select.h"

int AK_select(char *srcTable,char *destTable,AK_list *atributi,AK_list *uvjet){

AK_selection(srcTable,destTable,uvjet);

	char *helptable="help_table";
		
	
	table_addresses *src_addr=(table_addresses *) AK_get_table_addresses(destTable);
	int startAddress = 	src_addr->address_from[0];
	
	AK_block *temp_block = (AK_block *) AK_read_block(startAddress);
	

	AK_list_elem attr_liste;
	AK_header header[MAX_ATTRIBUTES];
 	memset(header, 0, sizeof( AK_header ) * MAX_ATTRIBUTES);

	int new_head=0;
	int brglave[6]={7,7,7,7,7,7};
	int brjgl=0;
	
	//kreiranje zaglavlja nove tablice
	int head;
	while(strcmp(temp_block->header[head].att_name,"")!=0){
			
		attr_liste=(AK_list_elem) Ak_First_L(atributi);
		int kreiraj=0;
		while(attr_liste!=NULL){
			if(strcmp(temp_block->header[head].att_name,attr_liste->data)==0){
				//napravi zaglavlje
				brglave[brjgl++]=head; //biljezi redni broj atributa
				kreiraj=1;
				break;
			}
			else attr_liste=attr_liste->next;
		}
		if(kreiraj){
			memcpy(&header[new_head], &temp_block->header[head], sizeof (temp_block->header[head]));
			new_head++;
		}
		head++;
	}
	AK_initialize_new_segment(helptable, SEGMENT_TYPE_TABLE, header);

	free(temp_block);
	
	//////////////////////////////
	//kopiranje potrebnih stupaca
	
	AK_list* row_root = (AK_list *) malloc(sizeof (AK_list));
	//	Ak_Init_L(row_root);
			
		
   // int head;
    int something_to_copy = 0;
    int overflow;
    int not_in_list = 1;
    int i, j, k, l, type, size, address;
    char data[MAX_VARCHAR_LENGTH];
	AK_list_elem list_elem;
	
int b;	
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
				int upisi=0;
				while(brglave[b]!=7) {
							if(brglave[b++]==l){upisi=1;break;}
					}
					if(upisi==1){
						type = temp->block->tuple_dict[l + k].type;
						size = temp->block->tuple_dict[l + k].size;
						
						address = temp->block->tuple_dict[l + k].address;
						memcpy(data, &(temp->block->data[address]), size);
						data[size] = '\0';
						Ak_Insert_New_Element(type, data, helptable, header[gl++].att_name, row_root);
						
						
					}
					b=0;
				}
				Ak_insert_row(row_root);
				Ak_DeleteAll_L(row_root);
	}
}}
  
  AK_print_table(helptable);
	//}
	free(temp_block);
	return EXIT_SUCCESS;

}




void AK_select_test(){
printf("\n\n\n ***** SELECT RELATIONAL OPERATOR ***** \n\n\n");

AK_list *atributi = (AK_list *) malloc(sizeof (AK_list));
Ak_Init_L(atributi);

AK_list *uvjet = (AK_list *) malloc(sizeof (AK_list));
Ak_Init_L(uvjet);

int num = 2005;

char *srcTable="student";
char *destTable="select_result";

Ak_InsertAtEnd_L(TYPE_ATTRIBS, "firstname", sizeof ("firstname"), atributi);
Ak_InsertAtEnd_L(TYPE_ATTRIBS, "year", sizeof ("year"), atributi);
Ak_InsertAtEnd_L(TYPE_ATTRIBS, "weight", sizeof ("weight"), atributi);

Ak_InsertAtEnd_L(TYPE_ATTRIBS, "year", sizeof ("year"), uvjet);
Ak_InsertAtEnd_L(TYPE_INT, &num, sizeof (int), uvjet);
Ak_InsertAtEnd_L(TYPE_OPERATOR, "<", sizeof ("<"), uvjet);

printf("\n SELECT firstname,year,weight FROM student WHERE year<2005;\n\n");

AK_select(srcTable,destTable,atributi,uvjet);
Ak_DeleteAll_L(atributi);
free(atributi);

Ak_DeleteAll_L(uvjet);
free(uvjet);

}
