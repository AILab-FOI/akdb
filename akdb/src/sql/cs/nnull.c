/**
@file nnull.c Provides functions for not null constraint
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

#include "nnull.h"

/**
 * @author Saša Vukšić, updated by Nenad Makar
 * @brief Function that sets NOT NULL constraint on attribute
 * @param char* tableName name of table
 * @param char* attName name of attribute
 * @param char* constraintName name of constraint
 * @return EXIT_ERROR or EXIT_SUCCESS
 **/
int AK_set_constraint_not_null(char* tableName, char* attName, char* constraintName) {
	int i;
	int numRows;
	int newConstraint;
	int uniqueConstraintName;
	struct list_node *row;
	struct list_node *attribute;

	AK_PRO;

	newConstraint = AK_read_constraint_not_null(tableName, attName, NULL);

	if(newConstraint == EXIT_ERROR)
	{
		printf("\nFAILURE!\nNOT NULL constraint already exists on attribute: %s\nof table: %s\n\n", attName, tableName);
		AK_EPI;
		return EXIT_ERROR;
	}
	
	numRows = AK_get_num_records(tableName);
	
	if(numRows > 0)
	{
		int positionOfAtt = AK_get_attr_index(tableName, attName) + 1;
		
		for(i=0; i<numRows; i++)
		{
			row = AK_get_row(i, tableName);
			attribute = Ak_GetNth_L2(positionOfAtt, row);
			
			if(AK_tuple_to_string(attribute) == NULL)
			{
				printf("\nFAILURE!\nTable: %s\ncontains NULL sign and that would violate NOT NULL constraint which You would like to set on attribute: %s\n\n", tableName, attName);
				AK_EPI;
				return EXIT_ERROR;
			}
		}
	}

	uniqueConstraintName = Ak_check_constraint_name(constraintName);

	if(uniqueConstraintName == EXIT_ERROR)
	{
		printf("\nFAILURE!\nConstraint name: %s\nalready exists in database\n\n", constraintName);
		AK_EPI;
		return EXIT_ERROR;
	}
	
	struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
	Ak_Init_L3(&row_root);

	int obj_id = AK_get_id();
	Ak_Insert_New_Element(TYPE_INT, &obj_id, "AK_constraints_not_null", "obj_id", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, tableName, "AK_constraints_not_null", "tableName", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, constraintName, "AK_constraints_not_null", "constraintName", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, attName, "AK_constraints_not_null", "attributeName", row_root);
	Ak_insert_row(row_root);
	Ak_DeleteAll_L3(&row_root);
	AK_free(row_root);
	printf("\nNOT NULL constraint is set on attribute: %s\nof table: %s\n\n", attName, tableName);
	
	AK_EPI;

	return EXIT_SUCCESS;
}

/**
 * @author Saša Vukšić, updated by Nenad Makar
 * @brief Function checks if there's violation of NOT NULL constraint
 * @param char* tableName name of table
 * @param char* attName name of attribute
 * @param char* newValue new value
 * @return EXIT_ERROR or EXIT_SUCCESS
 **/

int AK_read_constraint_not_null(char* tableName, char* attName, char* newValue) {
	int numRecords = AK_get_num_records("AK_constraints_not_null");

	struct list_node *row;
	struct list_node *attribute;
	struct list_node *table;
	int i;
	
	AK_PRO;

	if(newValue == NULL) {
		if(numRecords != 0) {
			for (i = 0; i < numRecords; i++) {
				row = AK_get_row(i, "AK_constraints_not_null");
				attribute = Ak_GetNth_L2(4, row);
				
				if(strcmp(attribute->data, attName) == 0) {
					table = Ak_GetNth_L2(2, row);
					
					if(strcmp(table->data, tableName) == 0) {
						AK_EPI;
						return EXIT_ERROR;
					}
				}
			}			
		}
	}

	AK_EPI;
	return EXIT_SUCCESS;		
}

/**
 * @author Maja Vračan
 * @brief Function for deleting specific not null constraint
 * @param tableName name of table on which constraint refers
 * @param attName name of attribute on which constraint is declared
 * @param constraintName name of constraint 
 * @return EXIT_SUCCESS when constraint is deleted, else EXIT_ERROR
 */
int AK_delete_constraint_not_null(char* tableName, char attName[], char constraintName[]){
    
    AK_PRO;
    int address, i, j, k, l, size;
    int num_attr = AK_num_attr("AK_constraints_not_null");
    AK_header *t_header = (AK_header *) AK_get_header("AK_constraints_not_null");
    table_addresses *src_addr = (table_addresses*) AK_get_table_addresses("AK_constraints_not_null");
    for (i = 0; src_addr->address_from[i] != 0; i++) {
        for (j = src_addr->address_from[i]; j < src_addr->address_to[i]; j++) {
            AK_mem_block *temp = (AK_mem_block *) AK_get_block(j);
            if (temp->block->last_tuple_dict_id == 0)
                break;
            for (k = 0; k < DATA_BLOCK_SIZE; k += num_attr) {
                if (temp->block->tuple_dict[k].type == FREE_INT)
                    break;

                for (l = 0; l < num_attr; l++) {
                    if(strcmp(t_header[l].att_name, "constraintName") == 0) {
                        size = temp->block->tuple_dict[k + l].size;
                        address = temp->block->tuple_dict[k + l].address;
                        char data[size];
                        memcpy(data, &(temp->block->data[address]), size);
                        data[size] = '\0';
                        if(strcmp(data, constraintName) == 0) { 
                            temp->block->tuple_dict[k].size = 0;
                            AK_EPI;
                            return EXIT_SUCCESS;
                        }
                    }
                }
            }
        }
    }
    AK_EPI;
    return EXIT_ERROR;
}

/**
  * @author Saša Vukšić, updated by Nenad Makar
  * @brief Function for testing testing NOT NULL constraint
  * @return No return value
  */
void AK_null_test() {
	char* tableName = "student";
	char* attName = "firstname";
	char* attName2 = "lastname";
	char* constraintName = "firstnameNotNull";
	char* newValue = NULL;
	int result;
	AK_PRO;
	printf("\nExisting NOT NULL constraints:\n\n");
	AK_print_table("AK_constraints_not_null");
	printf("\nTest table:\n\n");	
	AK_print_table(tableName);
	printf("\nTrying to set NOT NULL constraint on attribute %s of table %s...\n\n", attName, tableName);
	result = AK_set_constraint_not_null(tableName, attName, constraintName);
	AK_print_table("AK_constraints_not_null");
	if(result == EXIT_SUCCESS)
	{
		printf("\nChecking if attribute %s of table %s can contain NULL sign...\nYes (0) No (-1): %d\n\n", attName, tableName, AK_read_constraint_not_null(tableName, attName, newValue));
	}
        // delete test
        printf("\nDelete test");
        result = AK_delete_constraint_not_null(tableName, attName, constraintName);
        AK_print_table("AK_constraints_not_null");
        if(result == EXIT_SUCCESS) {
            printf("\nSUCCESS");
        }
        printf("\nDelete test finish");
	printf("\nTrying to set NOT NULL constraint on attribute %s of table %s AGAIN...\n\n", attName, tableName);
	result = AK_set_constraint_not_null(tableName, attName, constraintName);
	AK_print_table("AK_constraints_not_null");
	printf("\nTrying to set NOT NULL constraint with name %s AGAIN...\n\n", constraintName);
	result = AK_set_constraint_not_null(tableName, attName2, constraintName);
	AK_print_table("AK_constraints_not_null");
	printf("\nTest succeeded.");
	AK_EPI;
}