#include "check_constraint.h"

//int AK_check_constraint(char *srcTable, AK_list *expr) {
int AK_check_constraint(char *srcTable, struct list_node *expr) {
    AK_PRO;
    AK_header *t_header = (AK_header *) AK_get_header(srcTable);
    int num_attr = AK_num_attr(srcTable);

    table_addresses *src_addr = (table_addresses*) AK_get_table_addresses(srcTable);
    /*
    AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L3(&row_root);
    */
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);
    
    int i, j, k, l, type, size, address;
    char data[MAX_VARCHAR_LENGTH];

    for (i = 0; src_addr->address_from[i] != 0; i++) {

        for (j = src_addr->address_from[i]; j < src_addr->address_to[i]; j++) {

            AK_mem_block *temp = (AK_mem_block *) AK_get_block(j);

            if (temp->block->last_tuple_dict_id == 0)
                break;
            for (k = 0; k < DATA_BLOCK_SIZE; k += num_attr) {

                if (temp->block->tuple_dict[k].type == FREE_INT)
                    break;

				for (l = 0; l < num_attr; l++) {
					type = temp->block->tuple_dict[k + l].type;
					size = temp->block->tuple_dict[k + l].size;
					address = temp->block->tuple_dict[k + l].address;
					memcpy(data, &(temp->block->data[address]), size);
					data[size] = '\0';
					Ak_Insert_New_Element(type, data, srcTable, t_header[l].att_name, row_root);
				}

				if (!AK_check_if_row_satisfies_expression(row_root, expr)){
					AK_EPI;
					return 0;
				}

				
				Ak_DeleteAll_L3(&row_root);
            }
        }
    }

    AK_free(src_addr);
    AK_free(t_header);
    AK_free(row_root);
    AK_EPI;
    return 1;
}


void AK_check_constraint_test() {
    AK_PRO;
    printf("\n********** CHECK CONSTRAINT TEST **********\n");
    
    struct list_node *expr = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&expr);


    char *num = "0";
	
    Ak_InsertAtEnd_L3(TYPE_ATTRIBS, "year", sizeof ("year"), expr);
    Ak_InsertAtEnd_L3(TYPE_INT, num, sizeof (int), expr);
    Ak_InsertAtEnd_L3(TYPE_OPERATOR, ">", sizeof ("<"), expr);
   
    char *tblName = "student";

    int a;
    a = AK_check_constraint(tblName, expr);
    printf("CHECK: %d\n", a);
   //AK_print_table("selection_test");


    Ak_DeleteAll_L3(&expr);
    AK_free(expr);
    AK_EPI;
}
