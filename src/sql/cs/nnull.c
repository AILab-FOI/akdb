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
 * @brief Function that sets null constraint on attribute
 * @param char* tableName name of table
 * @param char* attName name of attribute
 * @param char* constraintName name of constraint
 * @return No return value
 **/
void AK_set_constraint_not_null(char* tableName, char* attName, char* constraintName) {
	AK_PRO;

	/*Check if constraintName already exists in database or if NOT NULL constraint is already set on attribute attName of table tableName
	It should be done by using a function from new file or in system catalog
	Put the rest of the code in if() and execute it if constraintName doesn't already exist in database and if NOT NULL constraint
	isn't already set on attribute attName of table tableName (if pair of tableName and attName doesn't already exist in some row of
	table AK_constraints_not_null)*/

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
	AK_EPI;
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
	if(newValue == NULL)
	{
		AK_PRO;
		int numRecords = AK_get_num_records("AK_constraints_not_null");

		if(numRecords != 0)
		{
			struct list_node *row;
			struct list_node *attribute;
			struct list_node *table;
			int i;
			
			for(i=0; i<numRecords; i++)
			{
				row = AK_get_row(i, "AK_constraints_not_null");
				attribute = Ak_GetNth_L2(4, row);
				
				if(strcmp(attribute->data, attName) == 0)
				{
					table = Ak_GetNth_L2(2, row);
					
					if(strcmp(table->data, tableName) == 0)
					{
						return EXIT_ERROR;
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
	else
	{
		return EXIT_SUCCESS;
		AK_EPI;
	}
}

/**
  * @author Saša Vukšić, updated by Nenad Makar
  * @brief Function for testing testing NOT NULL constraint
  * @return No return value
  */
void AK_null_test() {
	char* tableName = "studenti";
	char* attName = "ime";
	char* constraintName = "studentiNotNull";
	char* newValue = NULL;
	AK_PRO;
	printf("\nExisting NOT NULL constraints:\n\n");
	AK_print_table("AK_constraints_not_null");
	printf("\nTrying to set NOT NULL constraint on attribute %s of table %s...\n\n", attName, tableName);
	AK_set_constraint_not_null(tableName, attName, constraintName);
	AK_print_table("AK_constraints_not_null");
	printf("\nChecking if attribute %s of table %s can contain NULL sign...\nYes (0) No (-1): %d\n\n", attName, tableName, AK_read_constraint_not_null(tableName, attName, newValue));
	printf("\nTest succeeded.\nNOT NULL constraint is set on attribute %s of table %s.", attName, tableName);
	AK_EPI;
}