/**
@file unique.c Provides functions for unique constraint
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

#include "unique.h"

/**
 * @author Domagoj Tuličić, updated by Nenad Makar 
 * @brief Function sets unique constraint on attribute(s)
 * @param char* tableName name of table
 * @param char attName[] name(s) of attribute(s), if you want to set UNIQUE constraint on combination of attributes seperate their names with constant SEPARATOR (see test)
 * @param char constraintName[] name of constraint
 * @return EXIT_ERROR or EXIT_SUCCESS
 **/

int Ak_set_constraint_unique(char* tableName, char attName[], char constraintName[]){
	int i;
	int j;
	int numRows;
	int newConstraint;
	int uniqueConstraintName;
	char attributeName[MAX_VARCHAR_LENGTH]="";
	char values[MAX_VARCHAR_LENGTH]="";
	struct list_node *row;
	struct list_node *row2;
	struct list_node *attribute;
	struct list_node *attribute2;
	dictionary* dict;
	AK_PRO;

	strcat(attributeName, "tableName");
	strcat(attributeName, SEPARATOR);
	strcat(attributeName, "attributeName");

	strcat(values, tableName);
	strcat(values, SEPARATOR);
	strcat(values, attName);

	newConstraint = AK_read_constraint_unique("AK_constraints_unique", attributeName, values);

	if(newConstraint == EXIT_ERROR)
	{
		printf("\nFAILURE!\nUNIQUE constraint already exists on (combination of) attribute(s): %s\nof table: %s\n\n", attName, tableName);
		AK_EPI;
		return EXIT_ERROR;
	}
	
	numRows = AK_get_num_records(tableName);
	
	if(numRows > 0)
	{
		row = AK_get_row(0, tableName);
		int numOfAttsInTable = Ak_Size_L2(row);
		int positionsOfAtts[numOfAttsInTable];
		int numOfImpAttPos = 0;
		char attNameCopy[MAX_VARCHAR_LENGTH];
		char *nameOfOneAtt;
		char namesOfAtts[numOfAttsInTable][MAX_VARCHAR_LENGTH];
		
		strncpy(attNameCopy, attName, sizeof(attNameCopy));

		nameOfOneAtt = strtok(attNameCopy, SEPARATOR);
		while(nameOfOneAtt != NULL)
		{
			positionsOfAtts[numOfImpAttPos] = AK_get_attr_index(tableName, nameOfOneAtt) + 1;
			strncpy(namesOfAtts[numOfImpAttPos], nameOfOneAtt, sizeof(namesOfAtts[numOfImpAttPos]));

			numOfImpAttPos++;

			nameOfOneAtt = strtok(NULL, SEPARATOR);
		}
		
		int match;
		int impoIndexInArray;
		

		dict = dictionary_new(0);
		if(!dict) {
			printf("ERROR: Dictionary initialization failed.");
			AK_EPI;
			return EXIT_ERROR;
		}
		for(i=0; i<numRows-1; i++)
		{
			row = AK_get_row(i, tableName);
				
			match = 1;
			for(impoIndexInArray=0; (impoIndexInArray<numOfImpAttPos)&&(match==1); impoIndexInArray++)
			{
				attribute = Ak_GetNth_L2(positionsOfAtts[impoIndexInArray], row);
				if(AK_tuple_to_string(attribute)==NULL)
				{
					match = 0;
				}
				else if(dictionary_get(dict, AK_tuple_to_string(attribute), NULL) == NULL)
				{
					dictionary_set(dict, AK_tuple_to_string(attribute), AK_tuple_to_string(attribute));
					match = 0;
				}
			}
				
			if(match == 1)
			{
				printf("\nFAILURE!\nExisting values in table: %s\nwould violate UNIQUE constraint which You would like to set on (combination of) attribute(s): %s\n\n", tableName, attName);
				AK_EPI;
				return EXIT_ERROR;
			}
		}
	}

	dictionary_del(dict);

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
	Ak_Insert_New_Element(TYPE_INT, &obj_id, "AK_constraints_unique", "obj_id", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, tableName, "AK_constraints_unique", "tableName", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, constraintName, "AK_constraints_unique", "constraintName", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, attName, "AK_constraints_unique", "attributeName", row_root);
	Ak_insert_row(row_root);
	Ak_DeleteAll_L3(&row_root);
	AK_free(row_root);
	printf("\nUNIQUE constraint is set on (combination of) attribute(s): %s\nof table: %s\n\n", attName, tableName);
	AK_EPI;
	return EXIT_SUCCESS;
}

/**
 * @author Domagoj Tuličić, updated by Nenad Makar 
 * @brief Function checks if insertion of some value(s) would violate UNIQUE constraint
 * @param char* tableName name of table
 * @param char attName[] name(s) of attribute(s), if you want to check combination of values of more attributes seperate names of attributes with constant SEPARATOR (see test)
 * @param char newValue[] new value(s), if you want to check combination of values of more attributes seperate their values with constant SEPARATOR (see test),
	if some value(s) which you want to check isn't stored as char (string) convert it to char (string) using AK_tuple_to_string(struct list_node *tuple) or with sprintf 
	in a similiar way it's used in that function (if value isn't part of a *tuple),
	to concatenate more values in newValue[] use strcat(destination, source) and put constant SEPARATOR between them (see test)
	if newValue[] should contain NULL sign pass it as " " (space)
 * @return EXIT_ERROR or EXIT_SUCCESS
 **/

int AK_read_constraint_unique(char* tableName, char attName[], char newValue[]){
	char *value;
	//Because strtok could create problems for newValue...
	char newValueCopy[MAX_VARCHAR_LENGTH];
	int numOfValues = 0;
	AK_PRO;

	int strcmpTableName = strcmp(tableName, "AK_constraints_unique");
	int strcmpAttName = strcmp(attName, "obj_id");
	
	//If there's check if combination of values of attributes tableName and attributeName of table AK_constraints_unique are UNIQUE numOfValues is 2
	//First value is value of attribute tableName and second value is value of attribute attributeName which could be made of more SEPARATORS and then it would seem like
	//there are more than two values in newValue[]
	//That combination is used to check if there's some UNIQUE constraint set on table AK_constraints_unique
	//Combination of values of attributes tableName and attributeName is only needed UNIQUE combination in table AK_constraints_unique
	//Values of attribute obj_id (alone) will be UNIQUE when PRIMARY KEY will be created on table AK_constraints_unique
	//UNIQUENESS of value of attribute constraintName is checked with Ak_check_constraint_name among all constraint names in database so there's
	//no need to set UNIQUE constraint on that attribute
	if(strcmpTableName==0 && strcmpAttName!=0)
	{
		numOfValues = 2;
	}
	else
	{
		strncpy(newValueCopy, newValue, sizeof(newValueCopy));

		value = strtok(newValueCopy, SEPARATOR);
		while(value != NULL)
		{
			//Every combination which contains NULL sign ( ) is UNIQUE
			//http://www.postgresql.org/docs/current/interactive/ddl-constraints.html#DDL-CONSTRAINTS-UNIQUE-CONSTRAINTS
			if(strcmp(value, " ") == 0)
			{
				return EXIT_SUCCESS;
				AK_EPI;	
			}
			numOfValues++;
			value = strtok(NULL, SEPARATOR);
		}
	}

	int numRecords = AK_get_num_records("AK_constraints_unique");
	
	if(numRecords!=0 && (strcmpTableName!=0 || (strcmpTableName==0 && strcmpAttName==0)))
	{
		struct list_node *row;
		struct list_node *attribute;
		struct list_node *table;
		int i;
		
		for(i=0; i<numRecords; i++)
		{
			row = AK_get_row(i, "AK_constraints_unique");
			attribute = Ak_GetNth_L2(4, row);
			
			if(strcmp(attribute->data, attName) == 0)
			{
				table = Ak_GetNth_L2(2, row);
				
				if(strcmp(table->data, tableName) == 0)
				{
					int numRows = AK_get_num_records(table->data);
					
					if(numRows == 0)
					{
						AK_EPI;
						return EXIT_SUCCESS;
					}
					
					struct list_node *row2 = AK_get_row(0, table->data);
					int numOfAttsInTable = Ak_Size_L2(row2);
					int positionsOfAtts[numOfAttsInTable];
					int numOfImpAttPos = 0;
					char attNameCopy[MAX_VARCHAR_LENGTH];
					char *nameOfOneAtt;
					char namesOfAtts[numOfAttsInTable][MAX_VARCHAR_LENGTH];
					struct list_node *attribute2;
					
					strncpy(attNameCopy, attName, sizeof(attNameCopy));

					nameOfOneAtt = strtok(attNameCopy, SEPARATOR);
					while(nameOfOneAtt != NULL)
					{
						positionsOfAtts[numOfImpAttPos] = AK_get_attr_index(table->data, nameOfOneAtt) + 1;
						strncpy(namesOfAtts[numOfImpAttPos], nameOfOneAtt, sizeof(namesOfAtts[numOfImpAttPos]));
						numOfImpAttPos++;

						nameOfOneAtt = strtok(NULL, SEPARATOR);
					}
					
					int h;
					int impoIndexInArray;
					int match;
					int index = 0;
					char *value2;
					char newValueCopy2[MAX_VARCHAR_LENGTH];
					char values[numOfValues][MAX_VARCHAR_LENGTH];

					strncpy(newValueCopy2, newValue, sizeof(newValueCopy2));

					value2 = strtok(newValueCopy2, SEPARATOR);
					while(value2 != NULL)
					{
						strncpy(values[index], value2, sizeof(values[index]));
						index++;
						value2 = strtok(NULL, SEPARATOR);
					}

					
					for(h=0; h<numRows; h++)
					{
						row2 = AK_get_row(h, table->data);

						match = 1;
						
						for(impoIndexInArray=0; (impoIndexInArray<numOfImpAttPos)&&(match==1); impoIndexInArray++)
						{
							attribute2 = Ak_GetNth_L2(positionsOfAtts[impoIndexInArray], row2);
							if(AK_tuple_to_string(attribute2) == NULL)
							{
								match = 0;
							}
							else if(strcmp(values[impoIndexInArray], AK_tuple_to_string(attribute2)) != 0)
							{
								match = 0 ;
							}

						}
						
						if(match == 1)
						{
							AK_EPI;
							return EXIT_ERROR;
						}
					}
					
					AK_EPI;
					return EXIT_SUCCESS;
				}
			}
		}
		
		AK_EPI;
		return EXIT_SUCCESS;
	}
	else if(numRecords !=0 && strcmpTableName==0 && strcmpAttName!=0)
	{
		struct list_node *row = AK_get_row(0, tableName);
		int numOfAttsInTable = Ak_Size_L2(row);
		int positionsOfAtts[numOfAttsInTable];
		int numOfImpAttPos = 0;
		char attNameCopy[MAX_VARCHAR_LENGTH];
		char *nameOfOneAtt;
		char namesOfAtts[numOfAttsInTable][MAX_VARCHAR_LENGTH];
		struct list_node *attribute2;
		
		strncpy(attNameCopy, attName, sizeof(attNameCopy));

		nameOfOneAtt = strtok(attNameCopy, SEPARATOR);
		while(nameOfOneAtt != NULL)
		{
			positionsOfAtts[numOfImpAttPos] = AK_get_attr_index(tableName, nameOfOneAtt) + 1;
			strncpy(namesOfAtts[numOfImpAttPos], nameOfOneAtt, sizeof(namesOfAtts[numOfImpAttPos]));
			numOfImpAttPos++;

			nameOfOneAtt = strtok(NULL, SEPARATOR);
		}
		
		int h;
		int impoIndexInArray;
		int match;
		int index = 0;
		char *value2;
		char newValueCopy2[MAX_VARCHAR_LENGTH];
		char values[numOfValues][MAX_VARCHAR_LENGTH];

		strncpy(newValueCopy2, newValue, sizeof(newValueCopy2));

		value2 = strtok(newValueCopy2, SEPARATOR);
		strncpy(values[index], value2, sizeof(values));
		index++;
		value2 = strtok(NULL, "");
		strncpy(values[index], value2+strlen(SEPARATOR)-1, sizeof(values[index]));

		int numRows = AK_get_num_records(tableName);

		for(h=0; h<numRows; h++)
		{
			row = AK_get_row(h, tableName);

			match = 1;
			
			for(impoIndexInArray=0; (impoIndexInArray<numOfImpAttPos)&&(match==1); impoIndexInArray++)
			{
				attribute2 = Ak_GetNth_L2(positionsOfAtts[impoIndexInArray], row);
				if(AK_tuple_to_string(attribute2) == NULL)
				{
					match = 0;
				}
				else if(strcmp(values[impoIndexInArray], AK_tuple_to_string(attribute2)) != 0)
				{
					match = 0 ;
				}

			}
			
			if(match == 1)
			{
				AK_EPI;
				return EXIT_ERROR;
			}
		}
		
		AK_EPI;
		return EXIT_SUCCESS;
	}
	else
	{
		AK_EPI;
		return EXIT_SUCCESS;
	}
}
 
/**
 * @author Maja Vračan
 * @brief Function for deleting specific unique constraint
 * @param tableName name of table on which constraint refers
 * @param attName name of attribute on which constraint is declared
 * @param constraintName name of constraint 
 * @return EXIT_SUCCESS when constraint is deleted, else EXIT_ERROR
 */
int AK_delete_constraint_unique(char* tableName, char attName[], char constraintName[]){
    int address, i, j, k, l, size;
	AK_PRO;
    int num_attr = AK_num_attr("AK_constraints_unique");
    AK_header *t_header = (AK_header *) AK_get_header("AK_constraints_unique");
    table_addresses *src_addr = (table_addresses*) AK_get_table_addresses("AK_constraints_unique");
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
  * @author Domagoj Tuličić, updated by Nenad Makar 
  * @brief Function for testing UNIQUE constraint
  * @return No return value
  */

void AK_unique_test() {
	char* tableName = "student";
	char attYear[] = "year";
	char attFirstname[] = "firstname";
	char attNames1[MAX_VARCHAR_LENGTH]="";
	char attNames2[MAX_VARCHAR_LENGTH]="";
	char attNames3[MAX_VARCHAR_LENGTH]="mbr";
	char constraintYear[] = "yearUnique";
    char constraintMbr[] = "mbrUnique";
	char constraintName[] = "firstnameUnique";
	char constraintName1[MAX_VARCHAR_LENGTH]="";
	char constraintName2[MAX_VARCHAR_LENGTH]="";
	char newValue0[] = "2022";
	char newValue1[] = "2050";
	char newValue[] = "Ivan";
	char newValue2[] = "Testinjo";
	char newValue3[MAX_VARCHAR_LENGTH]="";
	char newValue4[MAX_VARCHAR_LENGTH]="";
	char newValue5[MAX_VARCHAR_LENGTH]="";
	char newValue6[MAX_VARCHAR_LENGTH]="";
	char newValue7[MAX_VARCHAR_LENGTH]="";
	int result;
	AK_PRO;

	strcat(attNames1, "mbr");
	strcat(attNames1, SEPARATOR);
	strcat(attNames1, "lastname");
	
	strcat(attNames2, "mbr");
	strcat(attNames2, SEPARATOR);
	strcat(attNames2, "lastname");
	strcat(attNames2, SEPARATOR);
	strcat(attNames2, "weight");
	
	strcat(constraintName1, "mbr");
	strcat(constraintName1, SEPARATOR);
	strcat(constraintName1, "lastnameUnique");
	
	strcat(constraintName2, "mbr");
	strcat(constraintName2, SEPARATOR);
	strcat(constraintName2, "lastname");
	strcat(constraintName2, SEPARATOR);
	strcat(constraintName2, "weightUnique");
	
	strcat(newValue3, "35895");
	strcat(newValue3, SEPARATOR);
	strcat(newValue3, "Ivic");
	
	strcat(newValue4, "50000");
	strcat(newValue4, SEPARATOR);
	strcat(newValue4, "Ivic");
	
	strcat(newValue5, " ");
	strcat(newValue5, SEPARATOR);
	strcat(newValue5, " ");
	
	strcat(newValue6, "35916");
	strcat(newValue6, SEPARATOR);
	strcat(newValue6, "Parker");
	strcat(newValue6, SEPARATOR);
	strcat(newValue6, "99.500000");
	
	strcat(newValue7, "35916");
	strcat(newValue7, SEPARATOR);
	strcat(newValue7, "Parker");
	strcat(newValue7, SEPARATOR);
	strcat(newValue7, "115.567000");
	
	printf("\nExisting UNIQUE constraints:\n\n");
	AK_print_table("AK_constraints_unique");
	printf("\nTest table:\n\n");	
	AK_print_table(tableName);
        
    printf("\n============== Running Test #1 ==============\n");
	printf("\nTrying to set UNIQUE constraint on attribute %s of table %s...\n\n", attYear, tableName);
	result = Ak_set_constraint_unique(tableName, attYear, constraintYear);
	AK_print_table("AK_constraints_unique");
	if(result == EXIT_SUCCESS)
	{
		printf("\nChecking if value %s would be UNIQUE in attribute %s of table %s...\nYes (0) No (-1): %d\n\n", newValue0, attYear, tableName, AK_read_constraint_unique(tableName, attYear, newValue0));
		printf("\nChecking if value %s would be UNIQUE in attribute %s of table %s...\nYes (0) No (-1): %d\n\n", newValue1, attYear, tableName, AK_read_constraint_unique(tableName, attYear, newValue1));
	}
        
        printf("\n============== Running Test #2 ==============\n");
        printf("\nDelete test");
        result = AK_delete_constraint_unique(tableName, attYear, constraintYear);
        AK_print_table("AK_constraints_unique");
        if(result == EXIT_SUCCESS) {
            printf("\nSUCCESS");
        }
        printf("\nDelete test finish");
        
        printf("\n============== Running Test #3 ==============\n");
	printf("\nTrying to set UNIQUE constraint on attribute %s of table %s AGAIN...\n\n", attYear, tableName);
	result = Ak_set_constraint_unique(tableName, attYear, constraintYear);
	AK_print_table("AK_constraints_unique");
        
        printf("\n============== Running Test #4 ==============\n");
	printf("\nTrying to set UNIQUE constraint on attribute %s of table %s...\n\n", attFirstname, tableName);
	result = Ak_set_constraint_unique(tableName, attFirstname, constraintName);
	AK_print_table("AK_constraints_unique");
	if(result == EXIT_SUCCESS)
	{
		printf("\nChecking if value %s would be UNIQUE in attribute %s of table %s...\nYes (0) No (-1): %d\n\n", newValue, attFirstname, tableName, AK_read_constraint_unique(tableName, attFirstname, newValue));
		printf("\nChecking if value %s would be UNIQUE in attribute %s of table %s...\nYes (0) No (-1): %d\n\n", newValue2, attFirstname, tableName, AK_read_constraint_unique(tableName, attFirstname, newValue2));
	}
        
        printf("\n============== Running Test #5 ==============\n");
	printf("\nTrying to set UNIQUE constraint on combination of attributes %s of table %s...\n\n", attNames1, tableName);
	result = Ak_set_constraint_unique(tableName, attNames1, constraintName1);
	AK_print_table("AK_constraints_unique");
	if(result == EXIT_SUCCESS)
	{
		printf("\nChecking if combination of values %s would be UNIQUE in attributes %s of table %s...\nYes (0) No (-1): %d\n\n", newValue3, attNames1, tableName, AK_read_constraint_unique(tableName, attNames1, newValue3));
		printf("\nChecking if combination of values %s would be UNIQUE in attributes %s of table %s...\nYes (0) No (-1): %d\n\n", newValue4, attNames1, tableName, AK_read_constraint_unique(tableName, attNames1, newValue4));
		printf("\nChecking if combination of values %s would be UNIQUE in attributes %s of table %s...\nYes (0) No (-1): %d\n\n", newValue5, attNames1, tableName, AK_read_constraint_unique(tableName, attNames1, newValue5));
	}
        
        
        printf("\n============== Running Test #6 ==============\n");
	printf("\nTrying to set UNIQUE constraint on combination of attributes %s of table %s ...\n\n", attNames2, tableName);
	result = Ak_set_constraint_unique(tableName, attNames2, constraintName2);
	AK_print_table("AK_constraints_unique");

        
        printf("\n============== Running Test #7 ==============\n");
	printf("\nTrying to set UNIQUE constraint on combination of attributes %s of table %s AGAIN...\n\n", attNames2, tableName);
	result = Ak_set_constraint_unique(tableName, attNames2, constraintName2);
	AK_print_table("AK_constraints_unique");
	
        
        printf("\n============== Running Test #8 ==============\n");
	printf("\nTrying to set UNIQUE constraint with name %s ...\n\n", constraintMbr);
	result = Ak_set_constraint_unique(tableName, attNames3, constraintMbr);
	AK_print_table("AK_constraints_unique");
	printf("\nTest succeeded.");
	AK_EPI;
}
