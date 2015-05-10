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
 * @brief Function that sets unique constraint on attribute
 * @param char* tableName name of table
 * @param char attName[] name(s) of attribute(s), if you want to set UNIQUE constraint on combination of attributes seperate their names with Alt Gr+M+Alt Gr +Ž (see test)
 * @param char* constraintName name of constraint
 * @return No return value
 **/

void Ak_set_constraint_unique(char* tableName, char attName[], char* constraintName){
	AK_PRO;

	/*Check if constraintName already exists in database or if UNIQUE constraint is already set on attribute/s attName of table tableName
	It should be done by using a function from new file or in system catalog
	Put the rest of the code in if() and execute it if constraintName doesn't already exist in database and if UNIQUE constraint
	isn't already set on attribute(s) attName of table tableName (if pair of tableName and attName(s) doesn't already exist in some row of
	table AK_constraints_unique)*/

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
	AK_EPI;
}

/**
 * @author Domagoj Tuličić, updated by Nenad Makar 
 * @brief Function reads if unique constraint exist
 * @param char* tableName name of table
 * @param char attName[] name(s) of attribute(s), if you want to check combination of values of more attributes seperate names of attributes with Alt Gr+M+Alt Gr +Ž (see test)
 * @param char newValue[] new value(s), if you want to check combination of values of more attributes seperate their values with Alt Gr+M+Alt Gr +Ž (see test)
	if some value should be float make sure that when you convert it to char it'll have 6 decimal places (because of AK_tuple_to_string(struct list_node *tuple)),
	add some 0 if needed
	if some attribute should contain NULL sign pass it as " " (space)
 * @return EXIT_ERROR or EXIT_SUCCESS
 **/

int AK_read_constraint_unique(char* tableName, char attName[], char newValue[]){
	char splitter[]="§¤";
	char *value;
	//Because strtok could create problems for newValue...
	char newValueCopy[MAX_VARCHAR_LENGTH];
	int numOfValues = 0;
	AK_PRO;
	
	strcpy(newValueCopy, newValue);

	value = strtok(newValueCopy, splitter);
	while(value != NULL)
	{
		//Every combination which contains NULl sign ( ) is UNIQUE
		//http://www.postgresql.org/docs/current/interactive/ddl-constraints.html#DDL-CONSTRAINTS-UNIQUE-CONSTRAINTS
		if(strcmp(value, " ") == 0)
		{
			return EXIT_SUCCESS;
			AK_EPI;	
		}
		numOfValues++;
		value = strtok(NULL, splitter);
	}

	int numRecords = AK_get_num_records("AK_constraints_unique");
	
	if(numRecords != 0)
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

					nameOfOneAtt = strtok(attNameCopy, splitter);
					while(nameOfOneAtt != NULL)
					{
						positionsOfAtts[numOfImpAttPos] = AK_get_attr_index(table->data, nameOfOneAtt) + 1;
						strcpy(namesOfAtts[numOfImpAttPos], nameOfOneAtt);
						numOfImpAttPos++;

						nameOfOneAtt = strtok(NULL, splitter);
					}
					
					int h;
					int impoIndexInArray;
					int match;
					int index = 0;
					char *value2;
					char newValueCopy2[MAX_VARCHAR_LENGTH];
					char values[numOfValues][MAX_VARCHAR_LENGTH];

					strcpy(newValueCopy2, newValue);

					value2 = strtok(newValueCopy2, splitter);
					while(value2 != NULL)
					{
						strcpy(values[index], value2);
						index++;
						value2 = strtok(NULL, splitter);
					}
					
					for(h=0; h<numRows; h++)
					{
						row2 = AK_get_row(h, table->data);

						match = 1;
						
						for(impoIndexInArray=0; (impoIndexInArray<numOfImpAttPos) && (match==1); impoIndexInArray++)
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
	else
	{
		return EXIT_SUCCESS;
		AK_EPI;			
	}
}
 
/**
  * @author Domagoj Tuličić, updated by Nenad Makar 
  * @brief Function for testing unique constraint
  * @return No return value
  */

void AK_unique_test() {
	char* tableName = "student";
	char attName[] = "firstname";
	char attName3[] = "mbr§¤lastname";
	char attName6[] = "mbr§¤lastname§¤weight";
	char* constraintName = "studentUnique";
	char* constraintName3 = "mbr§¤lastnameUnique";
	char* constraintName6 = "mbr§¤lastname§¤weightUnique";
	char newValue[] = "Ivan";
	char newValue2[] = "Testinjo";
	char newValue3[] = "35895§¤Ivic";
	char newValue4[] = "50000§¤Ivic";
	char newValue5[] = " §¤ ";
	char newValue6[] = "35916§¤Parker§¤99.500000";
	char newValue7[] = "35916§¤Parker§¤115.567000";
	AK_PRO;
	printf("\nExisting UNIQUE constraints:\n\n");
	AK_print_table("AK_constraints_unique");
	printf("\nTrying to set UNIQUE constraint on attribute %s of table %s...\n\n", attName, tableName);
	Ak_set_constraint_unique(tableName, attName, constraintName);
	AK_print_table("AK_constraints_unique");
	printf("\nTest table:\n\n");	
	AK_print_table(tableName);
	printf("\nChecking if value %s would be UNIQUE in attribute %s of table %s...\nYes (0) No (-1): %d\n\n", newValue, attName, tableName, AK_read_constraint_unique(tableName, attName, newValue));
	printf("\nChecking if value %s would be UNIQUE in attribute %s of table %s...\nYes (0) No (-1): %d\n\n", newValue2, attName, tableName, AK_read_constraint_unique(tableName, attName, newValue2));
	printf("\nTrying to set UNIQUE constraint on attributes %s of table %s...\n\n", attName3, tableName);
	Ak_set_constraint_unique(tableName, attName3, constraintName3);
	AK_print_table("AK_constraints_unique");
	printf("\nChecking if combination of values %s would be UNIQUE in attribute %s of table %s...\nYes (0) No (-1): %d\n\n", newValue3, attName3, tableName, AK_read_constraint_unique(tableName, attName3, newValue3));
	printf("\nChecking if combination of values %s would be UNIQUE in attribute %s of table %s...\nYes (0) No (-1): %d\n\n", newValue4, attName3, tableName, AK_read_constraint_unique(tableName, attName3, newValue4));
	printf("\nChecking if combination of values %s would be UNIQUE in attribute %s of table %s...\nYes (0) No (-1): %d\n\n", newValue5, attName3, tableName, AK_read_constraint_unique(tableName, attName3, newValue5));
	printf("\nTrying to set UNIQUE constraint on attributes %s of table %s...\n\n", attName6, tableName);
	Ak_set_constraint_unique(tableName, attName6, constraintName6);
	AK_print_table("AK_constraints_unique");
	printf("\nChecking if combination of values %s would be UNIQUE in attribute %s of table %s...\nYes (0) No (-1): %d\n\n", newValue6, attName6, tableName, AK_read_constraint_unique(tableName, attName6, newValue6));
	printf("\nChecking if combination of values %s would be UNIQUE in attribute %s of table %s...\nYes (0) No (-1): %d\n\n", newValue7, attName6, tableName, AK_read_constraint_unique(tableName, attName6, newValue7));
	printf("\nTest succeeded.\nUNIQUE constraints are set.");
	AK_EPI;
}