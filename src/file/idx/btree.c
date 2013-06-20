/**
@file btree.c Header file that provides functions for BTree indices
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

#include "btree.h"

/**
  * @author Anđelko Spevec
  * @brief Function for creating new btree index on integer attribute in table
  * @param tblName - name of the table on which we are creating index
  * @param attributes - attribute on which we are creating index
  * @param indexName - name of the index
  * NOTES: 1.tablica treba biti sortirana po atributu po kojem radimo indeks, to
    treba ispitati s filesort.c ali trenutno postoje neke greške u tom dijelu. Pa 
    kada se to ispravi treba to ispitivanje uključiti u funkciju AK_btree_create()
    2.dodati upisivanje i čitanje iz više blokova
    3.MERGE zadnjeg leafa s predzadnjim - treba dodati (prvo implementacija inserta)
 */
int AK_btree_create(char *tblName, AK_list *attributes, char *indexName){
	int i,n,exist;
	table_addresses *addresses = (table_addresses*) AK_get_table_addresses(tblName);
	int num_attr = AK_num_attr(tblName);
	while(addresses->address_from[ i ]){
		printf("\nAdresa TABLICE je od %u do %u \n",(addresses->address_from[ i ]),(addresses->address_to[ i ]));
		i++;
	}
	AK_header *table_header = (AK_header *)AK_get_header(tblName);
    	AK_list_elem attribute = (AK_list_elem) Ak_First_L(attributes);
	AK_header i_header[ MAX_ATTRIBUTES ];
	AK_header* temp;
    	n = 0;
	while (attribute != 0) {
		exist = 0;
		for (i = 0; i < num_attr; i++) {
		    if (strcmp((table_header + i)->att_name, attribute->data) == 0) {
			printf("Attribute %s exist in table, found on position: %d\n", (table_header + i)->att_name, i);
			Ak_dbg_messg(HIGH, INDICES, "Attribute %s exist in table, found on position: %d\n", (table_header + i)->att_name, i);
			exist = 1;
			temp = (AK_header*) AK_create_header((table_header + i)->att_name, (table_header + i)->type, FREE_INT, FREE_CHAR, FREE_CHAR);
			memcpy(i_header + n, temp, sizeof ( AK_header));
			n++;
			if ((table_header)->type != TYPE_INT) {
			    printf("Unsupported data type for bree index! Only int!");
			    return EXIT_ERROR;
			}
		    }
		}
		if (!exist) {
		    printf("Atribut %s ne postoji u tablici", attribute->data);
		    return EXIT_ERROR;
		}
		attribute = attribute->next;
	} 
	/* imamo samo jedan atribut pa je n=0 zauzelo svoje varijable te se n povecao na n=1. krecuci od n+1 na dalje
	postavljamo MAX=10 na 10-1 =9 sljedecih atributa na 0 !  */
	memset(i_header + n, 0, MAX_ATTRIBUTES - n);

	//inicijalizacija novog segmenta na adresi
	int startAddress = AK_initialize_new_segment(indexName, SEGMENT_TYPE_INDEX, i_header);
	if (startAddress != EXIT_ERROR)
		printf("\nINDEX %s CREATED!\n", indexName);
	//test print pocetna i zadnja adresa indeksa
        int r;
	table_addresses *addIndex = (table_addresses*) AK_get_index_addresses(indexName);
	while(addIndex->address_from[ r ]){
		printf("\nAdresa INDEKSA je od %u do %u \n",(addIndex->address_from[ r ]),(addIndex->address_to[ r ]));
		r++;
	}
	AK_block *block = (AK_block*) AK_read_block(startAddress);
	//pronalazak adrese bloka koji ima slobodnog mjesta za pisanje
	int adr_to_write = (int) AK_find_free_space(AK_get_index_addresses(indexName));
	printf("\n U bloku na adresi %u postoji mjesta za upisivanje",adr_to_write);
	printf("\n Iskoristeno mjesta za zapis: %u",(block->free_space));
	printf("\n ------------------------------------------------------------\n\n");
	//odrediti broj listova i čvorova-----------------------------------------------------------------------
	int number_el = AK_get_num_records(tblName);	
	printf("\n Broj redaka u tablici je: %u",number_el);
	/*	
	za sada je pretpostavka da su zapisi sortirani - treba provjeriti s filesort.c
	ali on trenutno ima neke greške, pa se čeka na njegovo ispravljanje	
	*/		
	root_info *korijen = (root_info*) malloc(sizeof(root_info));

	//broj LISTOVA--------------------B=3---------------number_leaf
	float x = (float) number_el;
	float y = x/B;
	//merge TODO
	int merge = 0;
	if((y - (int)y) < 0.5){
		merge = 1;
		printf("\n Zadnji list ima premalo elemenata - MERGE");		
	}
	
	int number_leaf = 0;
	while(y > 0){
		y = y - 1;
		number_leaf = number_leaf + 1;
	}//zamjena za ceil funkciju
	printf("\n Broj potrebnih listova %u", number_leaf);
	korijen->level[0] = number_leaf;

	//broj ČVOROVA (level above leafs)--------(B+1)-----number_node
	float m = (float) number_leaf;
	float node = m/(B+1);
	int number_node = 0;
	while(node > 0){
		node = node - 1;
		number_node = number_node + 1;
	}//zamjena za ceil funkciju
	printf("\n Broj potrebnih čvorova iznad listova %u", number_node);	
	korijen->level[1] = number_node;

	//broj ČVOROVA (remaining nodes)---------(B+1)------number_node
	float temp_one = (float) number_node;
	int temp_two = 0;
	int lvl = 2;
	int go = 1;
	while(go == 1){
		node = temp_one/(B+1);
		temp_two = 0;
		while(node > 0){
			node = node - 1;
			temp_two = temp_two + 1;
		}
		korijen->level[lvl] = temp_two;
		lvl++;
		number_node = number_node + temp_two;
		temp_one = (float) temp_two;
		printf("\n Broj potrebnih čvorova level above %u",temp_two);
		if(temp_two == 1)
			go = 0;
	}
	printf("\n\n Ukupan broj čvorova %u",number_node);
	//zapis root_info
	korijen->root = number_leaf + number_node;
	memcpy(block->data, korijen, sizeof(root_info));
	block->tuple_dict[0].address = 0;
	block->tuple_dict[0].type = BLOCK_TYPE_NORMAL;
    	block->tuple_dict[0].size = sizeof (root_info);
	block->free_space += sizeof (root_info);
	block->last_tuple_dict_id = 0;
	AK_write_block(block);
	printf("\n\n ID_IDX gdje je zapisan ROOT_INFO: %u",(block->last_tuple_dict_id));
 	printf("\n\n ------------------------------------------------------------LEAFS\n\n");
	//zapis listova
	int id,lf,b,help=0,elements=0;
	int help_two[number_el];
	for(lf=1;lf<=(korijen->level[0]);lf++){
		btree_node *leaf = (btree_node*) malloc(sizeof(btree_node));
		//just temp - need to get data from more blocks !
		i=0;
		AK_block *temp = (AK_block*) AK_read_block(addresses->address_from[ i ]);

		for(b=0;b<B;b++){
			if(elements<number_el){
				leaf->pointers[b].addBlock = addresses->address_from[ i ];
				leaf->pointers[b].indexTd = help;
				void *tem = (void *) calloc(MAX_VARCHAR_LENGTH, sizeof (void));
	      		        memcpy(tem, &(temp->data[temp->tuple_dict[help].address]), sizeof (int));
				leaf->values[b] = *((int *) tem);
				help_two[elements] = leaf->values[b];
				printf("\n %u value: %i",b,(leaf->values[b]));
				printf("\n %u pointer ID_TBL: %u",b,help);
				help += num_attr;
				elements++;
			}else{
				leaf->values[b] = -1;
				printf("\n %u value: %i",b,(leaf->values[b]));
				printf("\n %u pointer ID_TBL: NO",b);
				elements++;
			}
		}
		if(lf<(korijen->level[0])){
			leaf->pointers[B].addBlock = addresses->address_from[ i ];
			leaf->pointers[B].indexTd = lf+1;
		}	
		memcpy(&block->data[block->free_space], leaf, sizeof(btree_node));    
		id = block->last_tuple_dict_id + 1;
		block->tuple_dict[id].address = block->free_space;
		block->tuple_dict[id].type = LEAF;
		block->free_space += sizeof(btree_node);
		block->last_tuple_dict_id = id;
		AK_write_block(block);

		printf("\n ------------------------------ID_IDX %u.LEAF: %u",(lf),(block->last_tuple_dict_id));
	}
 	printf("\n\n ------------------------------------------------------------NODES\n\n");
	//zapis cvorova
	int no=1,v=1,d,e=0,f,h,offset,diff,skip=1,check;
	while(no==1){
		check=0;
		offset=1;
		for(h=0;h<e;h++)//zamjena za pow((B+1),e)
			offset = offset*(B+1); 
		diff = offset * B;//razlika između vrijednosti unutar čvora
		offset = offset * B + 1;
		printf("\n -------------------------------------------LEVEL");
		printf("\n\nLEVEL INFO: offset: %u, difference: %u",offset,diff);
		for(d=0;d<(korijen->level[v]);d++){
			printf("\n------------------------NODE %d", (d+1));
			btree_node *nodes = (btree_node*) malloc(sizeof(btree_node));
			for(f=0;f<B;f++){
				if((offset-1)<number_el){
					nodes->values[f] = help_two[offset-1];
					printf("\n %u value: %i",f,nodes->values[f]);
				}else{
					nodes->values[f] = -1;
					printf("\n %u value: %i",f,nodes->values[f]);
				}
				offset += diff;//razlika između vrijednosti u čvoru
			}
			offset += diff;//razlika vrijednosti između čvorova je duplo veća
			for(f=0;f<=B;f++){
					if(check<(korijen->level[v-1])){
						nodes->pointers[f].indexTd = skip;
						nodes->pointers[f].addBlock = adr_to_write;
						skip++;
						check++;
						printf("\n Pointer %u",nodes->pointers[f].indexTd);
					}else
						printf("\n Pointer - NO");
			}
			memcpy(&block->data[block->free_space], nodes, sizeof(btree_node)); 
			id = block->last_tuple_dict_id + 1;
			block->tuple_dict[id].address = block->free_space;
			block->tuple_dict[id].type = NODE;
			block->free_space += sizeof(btree_node);
			block->last_tuple_dict_id = id;
			AK_write_block(block);
			printf("\n ID_IDX of NODE: %u",(block->last_tuple_dict_id));
		}		
		e++;
		if(korijen->level[v] == 1)
			no = 0;
		v++;
	}

 	printf("\n\n ------------------------------------------------------------");
	printf("\n ----------------------------------------------------READING!");
 	printf("\n ------------------------------------------------------------\n\n");
	printf("\n\n ---------------ROOT INFO----------------------");
	adr_to_write = (int) AK_find_free_space(AK_get_index_addresses(indexName));		
	AK_block *block_two = (AK_block*) AK_read_block(adr_to_write);
	root_info *root = (root_info*) malloc(sizeof (root_info));
	memset(root, 0, sizeof (root_info));
	memcpy(root,block_two->data,sizeof (root_info));
	int o;
	for(o=0;o<ORDER;o++){
		printf("\n Level: %u",root->level[o]);
		if(root->level[o] == 1)
			break;
	}
	printf("\n\n ----------------LEAFS-------------------------");
	for(lf=1;lf<=(root->level[0]);lf++){
		printf("\n LEAF ID_IDX: %u -> Adresa u INDEKSU: %u ",lf,block_two->tuple_dict[lf].address);
		btree_node *list = (btree_node*) malloc(sizeof(btree_node));
		memset(list,0,sizeof(btree_node));
		memcpy(list,&block_two->data[block_two->tuple_dict[lf].address],sizeof(btree_node));
		for(b=0;b<B;b++){
			printf("\n Podatci: %i",list->values[b]);
			printf("\n Points tu ID_TBL: %u", list->pointers[b].indexTd);
		}
		printf("\n It points to next leaf on ID_IDX: %u", list->pointers[B].indexTd);
		printf("\n--------------------------");
	}
	printf("\n\n ----------------NODES-------------------------");
	int startAdd = sizeof(root_info)+((root->level[0])*sizeof(btree_node));
	int dof=1,ig,st=1;
	while(dof==1){
		for(ig=0;ig<(root->level[st]);ig++){
			btree_node *cvor = (btree_node*) malloc(sizeof(btree_node));
			memset(cvor,0,sizeof(btree_node));
			memcpy(cvor,&block_two->data[startAdd],sizeof(btree_node));
			startAdd += sizeof(btree_node);
			for(b=0;b<B;b++)
				printf("\n Podatci: %i",cvor->values[b]);
			for(b=0;b<=B;b++)
				printf("\n Pokazuje na cvor ID_IDX: %i u bloku %i",cvor->pointers[b].indexTd,cvor->pointers[b].addBlock);
		printf("\n----------------------------------------------------");
		}
		if(root->level[st]==1){
			dof = 0;
		}
		st++;
	}
	return EXIT_SUCCESS;
}

int AK_btree_delete(char *indexName){
    AK_delete_segment(indexName, SEGMENT_TYPE_INDEX);
    printf("INDEX %s DELETED!\n", indexName);
}

/**
  * @author Anđelko Spevec
  * @brief Function for searching or deleting a value in btree index
  * @param indexName - name of the index
  * @param searchValue - value that we are searching in the index
  * @param toDo - if 0 we just search else we delete the element if we find it
  * NOTES: 1.ako zbog delete ispraznimo leaf ispod 50% moramo MERGE 
    s susjednim - treba dodati (prvo implementacija inserta)
    2.range search
 */
void AK_btree_search_delete(char *indexName,int *searchValue,int *toDo){
	int adr_to_read = (int) AK_find_free_space(AK_get_index_addresses(indexName));
	AK_block *block = (AK_block*) AK_read_block(adr_to_read);
	root_info *root = (root_info*) malloc(sizeof (root_info));
	memset(root, 0, sizeof (root_info));
	memcpy(root,block->data,sizeof (root_info));
	btree_node *temp = (btree_node*) malloc(sizeof(btree_node));

	int address= block->tuple_dict[root->root].address;
	int type = block->tuple_dict[root->root].type; //node == 1, leaf == 0
	while(type == 1){
		memset(temp,0,sizeof(btree_node));
		memcpy(temp,&block->data[address],sizeof(btree_node));
		int b,goTo = B,done=0;
		for(b=0;b<B;b++){
			if((searchValue<(temp->values[b])) && (done == 0)){
				goTo=b;
				done = 1;
			}
		}
		address = block->tuple_dict[temp->pointers[goTo].indexTd].address;
		type = block->tuple_dict[temp->pointers[goTo].indexTd].type;
	}
	//Sada smo dosli do lista u kojem se nalazi tražena vrijednost, AKO postoji!
	memset(temp,0,sizeof(btree_node));
	memcpy(temp,&block->data[address],sizeof(btree_node));
	int f,found = 0;
	for(f=0;f<B;f++){
		if(searchValue == (temp->values[f])){
			found = 1;
			printf("\n FOUND! Block %u - IDX_TBL = %u", temp->pointers[f].addBlock, temp->pointers[f].indexTd);
			if(toDo == 1){
				temp->values[f] = -1;
				temp->pointers[f].addBlock = 0;
				temp->pointers[f].indexTd = 0;
				memcpy(&block->data[address],temp,sizeof(btree_node));
				AK_write_block(block);
				printf("\n Element je obrisan!");
			}
		}
	}
	if(found == 0){
		printf("\n Traženi element nije pronađen");
	}
}

int AK_btree_insert(char *indexName,int *insertValue, int *insertTd, int *insertBlock){
	int adr_to_read = (int) AK_find_free_space(AK_get_index_addresses(indexName));
	AK_block *block = (AK_block*) AK_read_block(adr_to_read);
	root_info *root = (root_info*) malloc(sizeof (root_info));
	memset(root, 0, sizeof (root_info));
	memcpy(root,block->data,sizeof (root_info));
	btree_node *temp = (btree_node*) malloc(sizeof(btree_node));
	int insertPath[ORDER];
	int insertIndexPath[ORDER];
	int address = block->tuple_dict[root->root].address;
	insertPath[0] = root->root;
	int type = block->tuple_dict[root->root].type;
	int inc=1;
	while(type == 1){
		memset(temp,0,sizeof(btree_node));
		memcpy(temp,&block->data[address],sizeof(btree_node));
		int b,goTo = B,done=0;
		for(b=0;b<B;b++){
			if((insertValue<(temp->values[b])) && (done == 0)){
				printf("\nPointer: %i",b);
				goTo=b;
				done = 1;
			}
		}
		insertIndexPath[inc-1] = goTo;
		insertPath[inc] = temp->pointers[goTo].indexTd;
		address = block->tuple_dict[temp->pointers[goTo].indexTd].address;
		type = block->tuple_dict[temp->pointers[goTo].indexTd].type;
		if(type == 1){
			inc++;
		}
		printf("\n Idi na cvor na koji pokazuje ID_IDX %i",(temp->pointers[goTo].indexTd));
	}
	int j;
	int destinationAddress = block->tuple_dict[insertPath[inc]].address;
	memset(temp,0,sizeof(btree_node));
	memcpy(temp,&block->data[destinationAddress],sizeof(btree_node));
	int freeSpace = 0;
	for(j=0;j<B;j++){
		if(temp->values[j] == -1){
			freeSpace++;
		}
	}
	int inserted = 0,increase=0;
	btree_node *temp_help = (btree_node*) malloc(sizeof(btree_node));
	memset(temp_help,0,sizeof(btree_node));
	if(freeSpace != 0){ //we have FREE space in LEAF
		for(j=0;j<B;j++){
			printf("\n vrijednost: %i", temp_help->values[j]);
		}
		for(j=0;j<B;j++){
			if(inserted == 0){
				if(insertValue < temp->values[increase]){
					temp_help->values[j] = insertValue;
					temp_help->pointers[j].indexTd = insertTd;
					temp_help->pointers[j].addBlock = insertBlock;
					inserted = 1;	
				}else{
					if(temp->values[increase] == -1){
						temp_help->values[j] = insertValue;
						temp_help->pointers[j].indexTd = insertTd;
						temp_help->pointers[j].addBlock = insertBlock;
						inserted = 1;
					}else{
						temp_help->values[j] = temp->values[increase];
						temp_help->pointers[j].indexTd = temp->pointers[increase].indexTd;
						temp_help->pointers[j].addBlock = temp->pointers[increase].addBlock;
						increase++;
					}
				}
			}else{
				temp_help->values[j] = temp->values[increase];
				temp_help->pointers[j] = temp->pointers[increase];
				increase++;
			}	
		}
		for(j=0;j<B;j++){
			printf("\n vrijednost: %i", temp_help->values[j]);
		}
		memset(&block->data[destinationAddress],0,sizeof(btree_node));
		memcpy(&block->data[destinationAddress],temp_help,sizeof(btree_node));
		AK_write_block(block);	
	}else{//we have to SPLIT the LEAF
		btree_node *temp_help_two = (btree_node*) malloc(sizeof(btree_node));
		memset(temp_help_two,0,sizeof(btree_node));
		int stop=0;
		int numberElementsFirst,numberElementsSecond;
		int amount = B % 2;
		if(amount == 0){
			//we need to insert B+1 elements in two leafs -> 4+1=3+2
			numberElementsFirst = (B/2) + 1; //B=4,(4/2)+1=3 elements in first leaf
			numberElementsSecond = B/2; //B=4,(4/2)=2 elements in second leaf
		}else{
			//we need to insert B+1 elements in two leafs -> 3+1=2+2
			numberElementsFirst = (B + 1)/2; //B=3,(3+1)/2=2 elements in first leaf
			numberElementsSecond = (B + 1)/2; //B=3,(3+1)/2=2 elements in second leaf 			
		}
		for(j=0;j<B;j++){
			if(stop == 0){
				if(inserted == 0){
					if(insertValue < temp->values[increase]){
						temp_help->values[j] = insertValue;
						temp_help->pointers[j].indexTd = insertTd;
						temp_help->pointers[j].addBlock = insertBlock;
						inserted = 1;
					}else{
						temp_help->values[j] = temp->values[increase];
						temp_help->pointers[j].indexTd = temp->pointers[increase].indexTd;
						temp_help->pointers[j].addBlock = temp->pointers[increase].addBlock;
						increase++;
					}
				}else{
					temp_help->values[j] = temp->values[increase];
					temp_help->pointers[j].indexTd = temp->pointers[increase].indexTd;
					temp_help->pointers[j].addBlock = temp->pointers[increase].addBlock;
					increase++;	
				}
				if((j+1)==numberElementsFirst)
					stop = 1;
			}else{
				temp_help->values[j] = -1;
				temp_help->pointers[j].indexTd = 0;
				temp_help->pointers[j].addBlock = 0;
			}
		}
		temp_help->pointers[B].indexTd = block->last_tuple_dict_id + 1; //points to new leaf
		temp_help->pointers[B].addBlock = adr_to_read; //update when fix multiple blocks
		memset(&block->data[destinationAddress],0,sizeof(btree_node));
		memcpy(&block->data[destinationAddress],temp_help,sizeof(btree_node)); 
		stop = 0;
		for(j=0;j<B;j++){
			if(stop == 0){
				if(inserted == 0){
					if(insertValue < temp->values[increase]){
						temp_help_two->values[j] = insertValue;
						temp_help_two->pointers[j].indexTd = insertTd;
						temp_help_two->pointers[j].addBlock = insertBlock;
						inserted = 1;
					}else{
						temp_help_two->values[j] = temp->values[increase];
						temp_help_two->pointers[j].indexTd = temp->pointers[increase].indexTd;
						temp_help_two->pointers[j].addBlock = temp->pointers[increase].addBlock;
						increase++;
					}
				}else{
					temp_help_two->values[j] = temp->values[increase];
					temp_help_two->pointers[j].indexTd = temp->pointers[increase].indexTd;
					temp_help_two->pointers[j].addBlock = temp->pointers[increase].addBlock;
					increase++;	
				}
				if((j+1)==numberElementsSecond)
					stop = 1;
			}else{
				temp_help_two->values[j] = -1;
				temp_help_two->pointers[j].indexTd = 0;
				temp_help_two->pointers[j].addBlock = 0;
			}		
		}
		temp_help_two->pointers[B].indexTd = temp->pointers[B].indexTd; //points to next leaf
		temp_help_two->pointers[B].addBlock = temp->pointers[B].addBlock;
		memcpy(&block->data[block->free_space], temp_help_two, sizeof(btree_node)); 
		int id = block->last_tuple_dict_id + 1;
		block->tuple_dict[id].address = block->free_space;
		block->tuple_dict[id].type = LEAF;
		block->free_space += sizeof(btree_node);
		block->last_tuple_dict_id = id;
		AK_write_block(block);

		int pathIndex = inc - 1;
		int nodeAddress = block->tuple_dict[insertPath[pathIndex]].address;
		int pointerIndex = insertIndexPath[pathIndex];
		memset(temp,0,sizeof(btree_node));
		memcpy(temp,&block->data[nodeAddress],sizeof(btree_node));
		for(j=0;j<B;j++){
			printf("\nStara vrinednost: %i", temp->values[j]);
		}
		for(j=0;j<=B;j++){
			printf("\nStari pointer: %i", temp->pointers[j].indexTd);
		}
		int firstValue = temp_help->values[0];
		int secondValue = temp_help_two->values[0];//secondValue > firstValue
		int firstPointer = insertPath[inc]; //pointer to original leaf
		int secondPointer = id; //pointer to new leaf
		memset(temp_help,0,sizeof(btree_node));

		int freePointer = 0;
		for(j=0;j<B;j++){
			if(temp->values[j] == -1)
				freePointer = 1;
		}
		int startSecond = 0, copyNode = 1,skip = 1;
		increase = 0;
		if(freePointer == 1){//in node above we have FREE POINTER to insert NEW LEAF
			temp_help->pointers[0].indexTd = temp->pointers[0].indexTd;
			if(pointerIndex == 0){
				startSecond = 1;
				copyNode = 0;
				skip = 0;
			}
			for(j=1;j<=B;j++){
				if((copyNode == 1) && (j != pointerIndex)){
					temp_help->values[j-1] = temp->values[increase];
					temp_help->pointers[j].indexTd = temp->pointers[increase+1].indexTd;
					increase++;
				}
				if(startSecond == 1){
					temp_help->values[j-1] = secondValue;
					temp_help->pointers[j].indexTd = secondPointer;
					if(skip == 1){
						increase++;
					}
					startSecond = 0;
					copyNode = 1;
				}
				if(j == pointerIndex){
					temp_help->values[j-1] = firstValue;
					temp_help->pointers[j].indexTd = firstPointer;
					startSecond = 1;
					copyNode = 0;
				}
			}
			for(j=0;j<B;j++){
				printf("\nNova vrijednost: %i", temp_help->values[j]);
			}
			for(j=0;j<=B;j++){
				printf("\nNovi pointer: %i", temp_help->pointers[j].indexTd);
			}
			memset(&block->data[nodeAddress],0,sizeof(btree_node));
			memcpy(&block->data[nodeAddress],temp_help,sizeof(btree_node)); 
			AK_write_block(block);
		}else{//FULL NODE -> we need to split NODE and add new NODE to pointer above
			//startSecond = 0, copyNode = 1,skip = 1;
/* NETESTIRANO !
			//insert numberElementsFirst - represents number of full POINTERS in NODE
			temp_help->pointers[0].indexTd = temp->pointers[0].indexTd;
			if(pointerIndex == 0){
				startSecond = 1;
				copyNode = 0;
				skip = 0;
			}
			stop = 0;
			increase = 0;
			for(j=1;j<=B;j++){
				if(stop == 0){
					if((copyNode == 1) && (j != pointerIndex)){
						temp_help->values[j-1] = temp->values[increase];
						temp_help->pointers[j].indexTd = temp->pointers[increase+1].indexTd;
						increase++;
					}
					if(startSecond == 1){
						temp_help->values[j-1] = secondValue;
						temp_help->pointers[j].indexTd = secondPointer;
						if(skip == 1){
							increase++;
						}
						startSecond = 0;
						copyNode = 1;
					}
					if(j == pointerIndex){
						temp_help->values[j-1] = firstValue;
						temp_help->pointers[j].indexTd = firstPointer;
						startSecond = 1;
						copyNode = 0;
					}	
					if(numberElementsFirst == j)
						stop = 1;
				}else{
					temp_help->values[j-1] = -1;
					temp_help->pointers[j].indexTd = 0; 
				}
			}
			stop = 0;
			memset(temp_help_two,0,sizeof(btree_node));
			//insert numberElementsSecond - represent number of full pointers in NODE
			temp_help_two->pointers[0].indexTd = temp->pointers[increase+1].indexTd;
			for(j=1;j<=B;j++){
				if(stop == 0){
					if((copyNode == 1) && (j != pointerIndex)){
						temp_help_two->values[j-1] = temp->values[increase];
						temp_help_two->pointers[j].indexTd = temp->pointers[increase+1].indexTd;
						increase++;
					}
					if(startSecond == 1){
						temp_help_two->values[j-1] = secondValue;
						temp_help_two->pointers[j].indexTd = secondPointer;
						if(skip == 1){
							increase++;
						}
						startSecond = 0;
						copyNode = 1;
					}
					if(j == pointerIndex){
						temp_help_two->values[j-1] = firstValue;
						temp_help_two->pointers[j].indexTd = firstPointer;
						startSecond = 1;
						copyNode = 0;
					}	
					if(numberElementsSecond == j)
						stop = 1;
				}else{
					temp_help_two->values[j-1] = -1;
					temp_help_two->pointers[j].indexTd = 0; 
				}
			}
*/
			/*NODE ABOVE INFO
			int pathIndexTwo = pathIndex-1;	
			int nodeAboveAddress = block->tuple_dict[insertPath[pathIndexTwo]].address;
			int pointerAboveIndex = insertIndexPath[pathIndexTwo];
			*/
		}

	}
	return EXIT_SUCCESS;
}

//just help for printing nodes !
void AK_DEL_TEST(){
	AK_block *block = (AK_block*) AK_read_block(421);
	btree_node *temp = (btree_node*) malloc(sizeof(btree_node));
	int adresa = block->tuple_dict[15].address;
	memset(temp,0,sizeof(btree_node));
	memcpy(temp,&block->data[adresa],sizeof(btree_node));
	int f;
	for(f=0;f<B;f++)
		printf("\n VRIJEDNOSTI: %i",temp->values[f]);
}

void Ak_btree_test() {
	char *tblName = "student";
	char *indexName = "student_btree_index";

	AK_list *att_list = (AK_list *) malloc(sizeof (AK_list));
	Ak_Init_L(att_list);
	Ak_InsertAtEnd_L(TYPE_ATTRIBS, "mbr\0", 4, att_list);

	AK_btree_create(tblName, att_list, indexName);

	int *searchValue = 35909;
	int *toDo = 0;
	AK_btree_search_delete(indexName, searchValue, toDo);

	//int *insertValue = 39123; //insert in leaf with available space
	int *insertValue = 35920; //insert in full leaf -> new leaf !
	int *insertTd = 101;
	int *insertBlock = 301;
	AK_btree_insert(indexName,insertValue,insertTd,insertBlock);
	//AK_DEL_TEST();
}

