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

/**
 * @author Matija Novak, updated by Dino Laktašić 
 * @brief  Temporaly function to create table, and insert entry to the system_relation catalog
 * @param table table name
 * @param header AK_header of the new table
 * @param type_segment type of the new segment
 * @return No return value
 */
void AK_temp_create_table(char *table, AK_header *header, int type_segment) {
    AK_PRO;
    AK_block *sys_block = (AK_block *) AK_malloc(sizeof (AK_block));
    sys_block = (AK_block *) AK_read_block(1);

    int startAddress = AK_initialize_new_segment(table, type_segment, header);

    int broj = 8;
    //insert object_id
    AK_insert_entry(sys_block, TYPE_INT, &broj, 8);
    //insert table name
    AK_insert_entry(sys_block, TYPE_VARCHAR, table, 9);
    //insert start address
    broj = startAddress;
    AK_insert_entry(sys_block, TYPE_INT, &broj, 10);
    //insert end address
    broj = startAddress + 19;
    AK_insert_entry(sys_block, TYPE_INT, &broj, 11);

    AK_write_block(sys_block);
    AK_free(sys_block);
    AK_EPI;
}

/**
 * @author Matija Novak, rewrited and optimized by Dino Laktašić to support AK_list  
 * @brief  Function to create a new header for the projection table
 * @param old_block_add address of the block from which we copy headers we need
 * @param dstTable name of the new table
 * @param att list of the attributes which should the projeciton table contain
 * @return No return value
 */
void AK_create_block_header(int old_block, char *dstTable, struct list_node *att, struct list_node *att_operation) {
//void AK_create_block_header(int old_block, char *dstTable, AK_list *att) {
    AK_PRO;
    AK_block *temp_block = (AK_block *) AK_malloc(sizeof (AK_block));
    temp_block = (AK_block *) AK_read_block(old_block);
    AK_header *temp;

    //AK_list_elem list_elem;
    struct list_node *list_elem,*list_elem2;
    struct list_node *att_operation_elem,*a,*b,*c,*d,*operator,*cached_elem;

    AK_header header[MAX_ATTRIBUTES];
    
    int head = 0; //counter of the headers
    int new_head = 0; //counter of the new headers created for the projection table
    int a_type;
    int b_type;
    int header_type = NULL;

   
 
    while (strcmp(temp_block->header[head].att_name, "") != 0) {
   
	list_elem = Ak_First_L2(att);	
        while (list_elem != NULL ) {
            
            //if header is found than copy header
            
            if (strcmp(list_elem->data, temp_block->header[head].att_name) == 0) {
                
                
                memcpy(&header[new_head], &temp_block->header[head], sizeof (temp_block->header[head]));
             
                
                Ak_dbg_messg(HIGH, REL_OP, "Copy attribute header: %s\n", header[new_head].att_name);

                new_head++;
            }else if(strstr(list_elem->data,temp_block->header[head].att_name)!=NULL){
                           
                char * exp = (char *) malloc(1 + strlen(list_elem->data));
                strcpy(exp,list_elem->data);
                removeSubstring(exp,temp_block->header[head].att_name);


                int cached_head = head;
                while(strcmp(temp_block->header[cached_head].att_name,"")!=0){
                    
                    if(strstr(exp,temp_block->header[cached_head].att_name)!=NULL){
                    
                        int type = AK_determine_header_type(temp_block->header[head].type,temp_block->header[cached_head].type);
                        
                        temp = (AK_header*) AK_create_header(list_elem->data, type, FREE_INT, FREE_CHAR, FREE_CHAR);
                        memcpy(&header[new_head], temp, sizeof (AK_header));

                        new_head++;
                        break;
                    }
                    cached_head++;
                }
            }
            
            list_elem = list_elem->next;
            
        }
       
        head++;     
    }
    
    memset(header + new_head, '\0', MAX_ATTRIBUTES - new_head);

    AK_free(temp_block);
    AK_temp_create_table(dstTable, header, SEGMENT_TYPE_TABLE);
    AK_EPI;
}
char *AK_get_operator(char *exp){
    if(strstr(exp,"+"))
        return "+";
    else if(strstr(exp,"-"))
        return "-";
    else if(strstr(exp,"/"))
        return "/";
    else if(strstr(exp,"%"))
        return "%";
    else if(strstr(exp,"*"))
        return "*";

}
void removeSubstring(char *s,const char *toremove)
{
  while( s=strstr(s,toremove) ){
    memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
    break;
    }
}

int AK_determine_header_type(int a,int b){

    if(a == TYPE_VARCHAR || b == TYPE_VARCHAR){
        return TYPE_VARCHAR;
    }
    else if(a == TYPE_NUMBER || b == TYPE_NUMBER){
        return TYPE_NUMBER;
    }
    else if(a == TYPE_FLOAT || b == TYPE_FLOAT){
        return TYPE_FLOAT;
    }
    else if(a == TYPE_INT || b == TYPE_INT){
        return TYPE_INT;
    }

}

char *AK_create_header_name(char * first,char *second, char * operator){
    
    char *name= (char *) malloc(1 + strlen(first)+strlen(operator));
    strcpy(name,first);
    strcat(name,operator);
    char *finalName = (char *) malloc(1 + strlen(name)+strlen(second));
    strcpy(finalName,name);
    strcat(finalName,second);
    return finalName;
}
/** 
  * @author Matija Novak, rewrited and optimized by Dino Laktašić to support AK_list
  * @brief  Function for copying the data from old table block to the new projection table
  * @param old_block block from which we copy data
  * @param dstTable name of the new table
  * @param att list of the attributes which should the projeciton table contain
  * @retrun No return value
 */
void AK_copy_block_projection(AK_block *old_block, struct list_node *att, struct list_node * att_operation, char *dstTable,struct list_node *expr) {
    AK_PRO;
    /*
    AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L3(&row_root);
    */
    struct list_node * row_root = (struct list_node *) AK_malloc(sizeof(struct list_node));
    Ak_Init_L3(&row_root);

    //AK_list_elem list_elem;
    struct list_node * list_elem,*a,*c,*operator;
    struct list_node * end_list;
    int i; //tuple_dict counter
    int head; //head counter
    int something_to_copy; //boolean variable to indicate if there is data to copy in these set of tuple_dicts
    int size; //current tuple_dict data size
    int perform = 0;

    char data[MAX_VARCHAR_LENGTH]; //data to copy

    // data for operation
    struct AK_operand *first=(struct AK_operand *) AK_malloc(sizeof(struct AK_operand));
    struct AK_operand *second=(struct AK_operand *) AK_malloc(sizeof(struct AK_operand));

    
    
    //iterate through all tuple_dicts in block
    for (i = 0; i < DATA_BLOCK_SIZE;) {
        head = something_to_copy = 0;

        while (strcmp(old_block->header[head].att_name, "") != 0) {
        
            //list_elem = (AK_list_elem) Ak_First_L2(att);
	    list_elem = (struct list_node *) Ak_First_L2(att);

            while (list_elem != NULL) {
                size = old_block->tuple_dict[i].size;
                //used to check if the data is correct
                int overflow = old_block->tuple_dict[i].size + old_block->tuple_dict[i].address;
                
                //if the data is what we need, if the size is not null, and data is correct
                if ((strcmp(list_elem->data, old_block->header[head].att_name) == 0) && (size != 0)
                        && (overflow < old_block->AK_free_space + 1) && (overflow > -1)) {
                    
                    memset(data, 0, MAX_VARCHAR_LENGTH);
                    memcpy(data, old_block->data + old_block->tuple_dict[i].address, old_block->tuple_dict[i].size);
                   
                    //insert element to list to be inserted into new table
                    Ak_Insert_New_Element(old_block->tuple_dict[i].type, data, dstTable, list_elem->data, row_root); //ForUpdate 0
                    something_to_copy = 1;
                }else if((strstr(list_elem->data,old_block->header[head].att_name)!=NULL)&& (size != 0)
                        && (overflow < old_block->AK_free_space + 1) && (overflow > -1)){

                            
                            

                            memset(first->value, 0, MAX_VARCHAR_LENGTH);
                            memcpy(first->value, old_block->data + old_block->tuple_dict[i].address, old_block->tuple_dict[i].size);

                            char * exp = (char *) malloc(1 + strlen(list_elem->data));
                            strcpy(exp,list_elem->data);

                            removeSubstring(exp,old_block->header[head].att_name);

                            first->type = old_block->header[head].type;
                            int cached_head = head;
                            int j=i;
                            int size2 = old_block->tuple_dict[j].size;
                            //used to check if the data is correct
                            int overflow2 = old_block->tuple_dict[j].size + old_block->tuple_dict[j].address;
                            while(strcmp(old_block->header[cached_head].att_name,"")!=0){
                                if((strstr(exp,old_block->header[cached_head].att_name)!=NULL)&& (size2 != 0)
                                && (overflow2 < old_block->AK_free_space + 1) && (overflow2 > -1)){
                        
                                memset(second->value, 0, MAX_VARCHAR_LENGTH);
                                memcpy(second->value, old_block->data + old_block->tuple_dict[j].address, old_block->tuple_dict[j].size);
                                second->type = old_block->header[cached_head].type;
                                
                                int type = AK_determine_header_type(first->type,second->type);
                                Ak_Insert_New_Element(type, AK_perform_operatrion(AK_get_operator(list_elem->data),first,second,type), dstTable, list_elem->data, row_root);

                                break;
                        }
                        cached_head++;
                        j++;
                    }
                }

                
                list_elem = list_elem->next;
            }
            

            head++;
            i++;
        }

        //write row to the porojection table
        if (something_to_copy) {
            Ak_dbg_messg(HIGH, REL_OP, "\nInsert row to projection table.\n");

            if(expr != NULL){
                if(AK_check_if_row_satisfies_expression(row_root,expr)){
                    Ak_insert_row(row_root);
                }
            }else{
                Ak_insert_row(row_root);
            }

            
            	    
            Ak_DeleteAll_L3(&row_root);
        }
    }
    AK_EPI;
    AK_free(first);
    AK_free(second);
    AK_free(row_root);
}




char *AK_perform_operatrion(char *op,struct AK_operand *ab, struct AK_operand *bb,int type){

    char *entry_data_f[MAX_VARCHAR_LENGTH];
    char *entry_data_i[MAX_VARCHAR_LENGTH];
    char *entry_data_d[MAX_VARCHAR_LENGTH];
    void * test;

    if(type == TYPE_NUMBER){
        double a,b,result;
        if(ab->type == TYPE_NUMBER)
            a = (*(double*)ab->value);
        if(bb->type == TYPE_NUMBER)
            b = (*(double*)bb->value);
        if(ab->type == TYPE_FLOAT)
            a = (*(float*)ab->value);
        if(bb->type == TYPE_FLOAT)
            b = (*(float*)bb->value);
        if(ab->type == TYPE_INT)
            a = (*(int*)ab->value);
        if(bb->type == TYPE_INT)
            b = (*(int*)bb->value);

        if(strcmp(op, "+")==0)
            result = a + b;
        else if(strcmp(op, "-")==0)
            result = a - b;
        else if(strcmp(op, "*")==0)
            result = a * b;
        else if(strcmp(op, "/")==0)
            result = a / b;
        

        sprintf(entry_data_d,"%d",result);
        test = &result;
    }
    else if(type == TYPE_FLOAT){
        float a1,b1,result1;
        if(ab->type == TYPE_NUMBER)
            a1 = (*(double*)ab->value);
        if(bb->type == TYPE_NUMBER)
            b1 = (*(double*)bb->value);
        if(ab->type == TYPE_FLOAT)
            a1 = (*(float*)ab->value);
        if(bb->type == TYPE_FLOAT)
            b1 = (*(float*)bb->value);
        if(ab->type == TYPE_INT)
            a1 = (*(int*)ab->value);
        if(bb->type == TYPE_INT)
            b1 = (*(int*)bb->value);
        
        if(strcmp(op, "+")==0)
            result1 = a1 + b1;
        else if(strcmp(op, "-")==0)
            result1 = a1 - b1;
        else if(strcmp(op, "*")==0)
            result1 = a1 * b1;
        else if(strcmp(op, "/")==0)
            result1 = a1 / b1;
        

        sprintf(entry_data_f,"%f",result1);
        test = &result1;
    }
    else if(type == TYPE_INT){
        int a2,b2,result2;
        if(ab->type == TYPE_NUMBER)
            a2 = (*(double*)ab->value);
        if(bb->type == TYPE_NUMBER)
            b2 = (*(double*)bb->value);
        if(ab->type == TYPE_FLOAT)
            a2 = (*(float*)ab->value);
        if(bb->type == TYPE_FLOAT)
            b2 = (*(float*)bb->value);
        if(ab->type == TYPE_INT)
            a2 = (*(int*)ab->value);
        if(bb->type == TYPE_INT)
            b2 = (*(int*)bb->value);
        
        if(strcmp(op, "+")==0)
            result2 = a2 + b2;
        else if(strcmp(op, "-")==0)
            result2 = a2 - b2;
        else if(strcmp(op, "*")==0)
            result2 = a2 * b2;
        else if(strcmp(op, "/")==0)
            result2 = a2 / b2;
        else if(strcmp(op, "%")==0)
            result2 = a2 % b2;

        sprintf(entry_data_i,"%i",result2);
        test = &result2;

    }
   
    return test;
    
}
/**
 * @author Matija Novak, rewrited and optimized by Dino Laktašić, now support cacheing
 * @brief  Function makes a projection of some table
 * @param att - list of atributes on which we make projection
 * @param dstTable table name for projection table
 * @return EXIT_SUCCESS if continues succesfuly, when not EXIT_ERROR
 */
//int AK_projection(char *srcTable, char *dstTable, AK_list *att) {
int AK_projection(char *srcTable, char *dstTable, struct list_node *att,struct list_node * att_operation, struct list_node *expr) {
    //geting the table addresses from table on which we make projection
    AK_PRO;
    table_addresses *src_addr = (table_addresses *) AK_get_table_addresses(srcTable);

    if (src_addr->address_from[0] != 0) {
        //create new segmenet for the projection table
        AK_create_block_header(src_addr->address_from[0], dstTable, att,att_operation);

        Ak_dbg_messg(LOW, REL_OP, "TABLE %s CREATED from %s!\n", dstTable, srcTable);
        Ak_dbg_messg(MIDDLE, REL_OP, "\nAK_projection: start copying data\n");

        int startAddress = 0, i = 0, j;

        //for each extent that contains blocks needed for projection
        for (i = 0; src_addr->address_from[i] != 0; i++) { //MAX_EXTENTS_IN_SEGMENT
            startAddress = src_addr->address_from[i];

            if (startAddress != 0) {
                Ak_dbg_messg(MIDDLE, REL_OP, "\nAK_projection: copy extent: %d\n", i);

                //for each block in extent
                for (j = startAddress; j <= src_addr->address_to[i]; j++) {
                    AK_mem_block *temp = (AK_mem_block *) AK_get_block(j);
                    //AK_block *temp = (AK_block *) AK_read_block(j);

                    if (temp->block->last_tuple_dict_id == 0) {
                        break;
                    }

                    Ak_dbg_messg(MIDDLE, REL_OP, "\nAK_projection: copy block: %d\n", j);

                    //get projection tuples from block
                    AK_copy_block_projection(temp->block, att,att_operation, dstTable,expr);
                }
            } else break;
        }
		
        AK_free(src_addr);
        Ak_dbg_messg(LOW, REL_OP, "PROJECTION_TEST_SUCCESS\n\n");
	AK_EPI;
        return EXIT_SUCCESS;
    } else {
		AK_free(src_addr);
        Ak_dbg_messg(LOW, REL_OP, "\n AK_projection: Table doesn't exist!");
        AK_EPI;
        return EXIT_ERROR;
    }
    AK_EPI;
}

/**
 *  @author Dino Laktašić
 * @brief  Function for projection operator testing
 * @return No return value
 */

void AK_op_projection_test() {
    AK_PRO;
    printf("\n********** PROJECTION TEST **********\n\n");

    struct list_node * att = (struct list_node *) AK_malloc(sizeof(struct list_node));
    struct list_node * expr = (struct list_node *) AK_malloc(sizeof(struct list_node));
    struct list_node * att_operation = (struct list_node*) AK_malloc(sizeof(struct list_node));
    Ak_Init_L3(&att);
    Ak_Init_L3(&att_operation);
    char *destTable2 = "projection_test2";
    printf("\nSelect firstname,lastname from student,weight,weight+year,weight-year\n\n");
    Ak_InsertAtEnd_L3(TYPE_ATTRIBS, "firstname", sizeof ("firstname"), att);
    Ak_InsertAtEnd_L3(TYPE_ATTRIBS, "lastname", sizeof ("lastname"), att);
    Ak_InsertAtEnd_L3(TYPE_ATTRIBS, "weight", sizeof ("weight"), att);
    Ak_InsertAtEnd_L3(TYPE_ATTRIBS, "year", sizeof ("year"), att);
    Ak_InsertAtEnd_L3(TYPE_ATTRIBS, "weight+year", sizeof("weight+year"),att);
    Ak_InsertAtEnd_L3(TYPE_ATTRIBS, "year*year", sizeof("year*year"),att);
    Ak_InsertAtEnd_L3(TYPE_ATTRIBS, "year-weight", sizeof("year-weight"),att);
    Ak_InsertAtEnd_L3(TYPE_ATTRIBS, "year/weight", sizeof("year/weight"),att);

    AK_projection("student", "projection_test", att, att_operation, NULL);
    AK_print_table("projection_test");

    strcpy(expr->table,destTable2);
    char expression []= "%in%";
    Ak_InsertAtEnd_L3(TYPE_ATTRIBS, "firstname", sizeof ("firstname"), expr);
    Ak_InsertAtEnd_L3(TYPE_VARCHAR, &expression, sizeof (char), expr);
    Ak_InsertAtEnd_L3(TYPE_OPERATOR, "LIKE", sizeof ("LIKE"), expr);

    printf("\nSelect firstname,lastname from student where firstname LIKE .*in.*\n\n");
    AK_projection("student", "projection_test2", att, NULL, expr);
    AK_print_table("projection_test2");

    Ak_DeleteAll_L3(&att);
    AK_EPI;
}
