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
		return EXIT_ERROR;
		AK_EPI;
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
		
		strcpy(attNameCopy, attName);

		nameOfOneAtt = strtok(attNameCopy, SEPARATOR);
		while(nameOfOneAtt != NULL)
		{
			positionsOfAtts[numOfImpAttPos] = AK_get_attr_index(tableName, nameOfOneAtt) + 1;
			strcpy(namesOfAtts[numOfImpAttPos], nameOfOneAtt);
			numOfImpAttPos++;

			nameOfOneAtt = strtok(NULL, SEPARATOR);
		}
		
		int match;
		int impoIndexInArray;
		
		for(i=0; i<numRows-1; i++)
		{
			row = AK_get_row(i, tableName);
			
			for(j=i+1; j<numRows; j++)
			{
				row2 = AK_get_row(j, tableName);
				match = 1;
				
				for(impoIndexInArray=0; (impoIndexInArray<numOfImpAttPos)&&(match==1); impoIndexInArray++)
				{
					attribute = Ak_GetNth_L2(positionsOfAtts[impoIndexInArray], row);
					attribute2 = Ak_GetNth_L2(positionsOfAtts[impoIndexInArray], row2);
					if(AK_tuple_to_string(attribute)==NULL || AK_tuple_to_string(attribute2)==NULL)
					{
						match = 0;
					}
					else if(strcmp(AK_tuple_to_string(attribute), AK_tuple_to_string(attribute2)) != 0)
					{
						match = 0 ;
					}

				}
				
				if(match == 1)
				{
					printf("\nFAILURE!\nExisting values in table: %s\nwould violate UNIQUE constraint which You would like to set on (combination of) attribute(s): %s\n\n", tableName, attName);
					return EXIT_ERROR;
					AK_EPI;
				}
			}
		}
	}

	uniqueConstraintName = Ak_check_constraint_name(constraintName);

	if(uniqueConstraintName == EXIT_ERROR)
	{
		printf("\nFAILURE!\nConstraint name: %s\nalready exists in database\n\n", constraintName);
		return EXIT_ERROR;
		AK_EPI;
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
	return EXIT_SUCCESS;
	AK_EPI;
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
		strcpy(newValueCopy, newValue);

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
						return EXIT_SUCCESS;
						AK_EPI;	
					}
					
					struct list_node *row2 = AK_get_row(0, table->data);
					int numOfAttsInTable = Ak_Size_L2(row2);
					int positionsOfAtts[numOfAttsInTable];
					int numOfImpAttPos = 0;
					char attNameCopy[MAX_VARCHAR_LENGTH];
					char *nameOfOneAtt;
					char namesOfAtts[numOfAttsInTable][MAX_VARCHAR_LENGTH];
					struct list_node *attribute2;
					
					strcpy(attNameCopy, attName);

					nameOfOneAtt = strtok(attNameCopy, SEPARATOR);
					while(nameOfOneAtt != NULL)
					{
						positionsOfAtts[numOfImpAttPos] = AK_get_attr_index(table->data, nameOfOneAtt) + 1;
						strcpy(namesOfAtts[numOfImpAttPos], nameOfOneAtt);
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

					strcpy(newValueCopy2, newValue);

					value2 = strtok(newValueCopy2, SEPARATOR);
					while(value2 != NULL)
					{
						strcpy(values[index], value2);
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
							return EXIT_ERROR;
							AK_EPI;
						}
					}
					
					return EXIT_SUCCESS;
					AK_EPI;
				}
			}
		}
		
		return EXIT_SUCCESS;
		AK_EPI;
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
		
		strcpy(attNameCopy, attName);

		nameOfOneAtt = strtok(attNameCopy, SEPARATOR);
		while(nameOfOneAtt != NULL)
		{
			positionsOfAtts[numOfImpAttPos] = AK_get_attr_index(tableName, nameOfOneAtt) + 1;
			strcpy(namesOfAtts[numOfImpAttPos], nameOfOneAtt);
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

		strcpy(newValueCopy2, newValue);

		value2 = strtok(newValueCopy2, SEPARATOR);
		strcpy(values[index], value2);
		index++;
		value2 = strtok(NULL, "");
		strcpy(values[index], value2+strlen(SEPARATOR)-1);

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
				return EXIT_ERROR;
				AK_EPI;
			}
		}
		
		return EXIT_SUCCESS;
		AK_EPI;
	}
	else
	{
		return EXIT_SUCCESS;
		AK_EPI;			
	}
}
 
/**
  * @author Domagoj Tuličić, updated by Nenad Makar 
  * @brief Function for testing UNIQUE constraint
  * @return No return value
  */

void AK_unique_test() {
	char* tableName = "student";
	char attName0[] = "year";
	char attName[] = "firstname";
	char attName3[MAX_VARCHAR_LENGTH]="";
	char attName6[MAX_VARCHAR_LENGTH]="";
	char attName7[MAX_VARCHAR_LENGTH]="mbr";
	char constraintName0[] = "yearUnique";
	char constraintName[] = "firstnameUnique";
	char constraintName3[MAX_VARCHAR_LENGTH]="";
	char constraintName6[MAX_VARCHAR_LENGTH]="";
	char newValue0[] = "2022";
	char newValue00[] = "2050";
	char newValue[] = "Ivan";
	char newValue2[] = "Testinjo";
	char newValue3[MAX_VARCHAR_LENGTH]="";
	char newValue4[MAX_VARCHAR_LENGTH]="";
	char newValue5[MAX_VARCHAR_LENGTH]="";
	char newValue6[MAX_VARCHAR_LENGTH]="";
	char newValue7[MAX_VARCHAR_LENGTH]="";
	int result;
	AK_PRO;

	strcat(attName3, "mbr");
	strcat(attName3, SEPARATOR);
	strcat(attName3, "lastname");
	
	strcat(attName6, "mbr");
	strcat(attName6, SEPARATOR);
	strcat(attName6, "lastname");
	strcat(attName6, SEPARATOR);
	strcat(attName6, "weight");
	
	strcat(constraintName3, "mbr");
	strcat(constraintName3, SEPARATOR);
	strcat(constraintName3, "lastnameUnique");
	
	strcat(constraintName6, "mbr");
	strcat(constraintName6, SEPARATOR);
	strcat(constraintName6, "lastname");
	strcat(constraintName6, SEPARATOR);
	strcat(constraintName6, "weightUnique");
	
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
	printf("\nTrying to set UNIQUE constraint on attribute %s of table %s...\n\n", attName0, tableName);
	result = Ak_set_constraint_unique(tableName, attName0, constraintName0);
	AK_print_table("AK_constraints_unique");
	if(result == EXIT_SUCCESS)
	{
		printf("\nChecking if value %s would be UNIQUE in attribute %s of table %s...\nYes (0) No (-1): %d\n\n", newValue0, attName0, tableName, AK_read_constraint_unique(tableName, attName0, newValue0));
		printf("\nChecking if value %s would be UNIQUE in attribute %s of table %s...\nYes (0) No (-1): %d\n\n", newValue00, attName0, tableName, AK_read_constraint_unique(tableName, attName0, newValue00));
	}
	printf("\nTrying to set UNIQUE constraint on attribute %s of table %s AGAIN...\n\n", attName0, tableName);
	result = Ak_set_constraint_unique(tableName, attName0, constraintName0);
	AK_print_table("AK_constraints_unique");
	printf("\nTrying to set UNIQUE constraint on attribute %s of table %s...\n\n", attName, tableName);
	result = Ak_set_constraint_unique(tableName, attName, constraintName);
	AK_print_table("AK_constraints_unique");
	if(result == EXIT_SUCCESS)
	{
		printf("\nChecking if value %s would be UNIQUE in attribute %s of table %s...\nYes (0) No (-1): %d\n\n", newValue, attName, tableName, AK_read_constraint_unique(tableName, attName, newValue));
		printf("\nChecking if value %s would be UNIQUE in attribute %s of table %s...\nYes (0) No (-1): %d\n\n", newValue2, attName, tableName, AK_read_constraint_unique(tableName, attName, newValue2));
	}
	printf("\nTrying to set UNIQUE constraint on combination of attributes %s of table %s...\n\n", attName3, tableName);
	result = Ak_set_constraint_unique(tableName, attName3, constraintName3);
	AK_print_table("AK_constraints_unique");
	if(result == EXIT_SUCCESS)
	{
		printf("\nChecking if combination of values %s would be UNIQUE in attributes %s of table %s...\nYes (0) No (-1): %d\n\n", newValue3, attName3, tableName, AK_read_constraint_unique(tableName, attName3, newValue3));
		printf("\nChecking if combination of values %s would be UNIQUE in attributes %s of table %s...\nYes (0) No (-1): %d\n\n", newValue4, attName3, tableName, AK_read_constraint_unique(tableName, attName3, newValue4));
		printf("\nChecking if combination of values %s would be UNIQUE in attributes %s of table %s...\nYes (0) No (-1): %d\n\n", newValue5, attName3, tableName, AK_read_constraint_unique(tableName, attName3, newValue5));
	}
	printf("\nTrying to set UNIQUE constraint on combination of attributes %s of table %s...\n\n", attName6, tableName);
	result = Ak_set_constraint_unique(tableName, attName6, constraintName6);
	AK_print_table("AK_constraints_unique");
	if(result == EXIT_SUCCESS)
	{
		printf("\nChecking if combination of values %s would be UNIQUE in attributes %s of table %s...\nYes (0) No (-1): %d\n\n", newValue6, attName6, tableName, AK_read_constraint_unique(tableName, attName6, newValue6));
		printf("\nChecking if combination of values %s would be UNIQUE in attributes %s of table %s...\nYes (0) No (-1): %d\n\n", newValue7, attName6, tableName, AK_read_constraint_unique(tableName, attName6, newValue7));
	}
	printf("\nTrying to set UNIQUE constraint on combination of attributes %s of table %s AGAIN...\n\n", attName6, tableName);
	result = Ak_set_constraint_unique(tableName, attName6, constraintName6);
	AK_print_table("AK_constraints_unique");
	printf("\nTrying to set UNIQUE constraint with name %s AGAIN...\n\n", constraintName6);
	result = Ak_set_constraint_unique(tableName, attName7, constraintName6);
	AK_print_table("AK_constraints_unique");
	printf("\nTest succeeded.");
	AK_EPI;
}