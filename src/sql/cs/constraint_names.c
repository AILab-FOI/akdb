/**
@file constraint_names.c Provides functions for checking if constraint name is unique in database
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

#include "constraint_names.h"

/**
 * @author Nenad Makar 
 * @brief Function checks if constraint name would be unique in database 
 * @param char constraintName name which you want to give to constraint which you are trying to create
 * @return EXIT_ERROR or EXIT_SUCCESS
 **/

int Ak_check_constraint_name(char* constraintName)
{
	int i;
	int numRows;
	struct list_node *row;
	struct list_node *attribute;
	AK_PRO;
	
	numRows = AK_get_num_records("AK_constraints_not_null");
	for(i=0; i<numRows; i++)
	{
		row = AK_get_row(i, "AK_constraints_not_null");
		attribute = Ak_GetNth_L2(3, row);
		
		if(strcmp(attribute->data, constraintName) == 0)
		{
			return EXIT_ERROR;
			AK_EPI;
		}
	}
	
	numRows = AK_get_num_records("AK_constraints_unique");
	for(i=0; i<numRows; i++)
	{
		row = AK_get_row(i, "AK_constraints_unique");
		attribute = Ak_GetNth_L2(3, row);
		
		if(strcmp(attribute->data, constraintName) == 0)
		{
			return EXIT_ERROR;
			AK_EPI;
		}
	}

	numRows = AK_get_num_records("AK_reference");
	for(i=0; i<numRows; i++)
	{
		row = AK_get_row(i, "AK_reference");
		attribute = Ak_GetNth_L2(2, row);
		
		if(strcmp(attribute->data, constraintName) == 0)
		{
			return EXIT_ERROR;
			AK_EPI;
		}
	}
	
	numRows = AK_get_num_records("AK_constraints_between");
	for(i=0; i<numRows; i++)
	{
		row = AK_get_row(i, "AK_constraints_between");
		attribute = Ak_GetNth_L2(3, row);
		
		if(strcmp(attribute->data, constraintName) == 0)
		{
			return EXIT_ERROR;
			AK_EPI;
		}
	}
		
	//OTHER CONSTRAINTS ARE NOT YET IMPLEMENTED, COMPLETE THIS FUNCTION WHEN THAT HAPPENS!!!
	//WRITE SIMILIAR CODE TO THE CODE ABOVE, CHECK TABLES AND ATTRIBUTES NAMES IN dbman.c
	//IF THERE ARE PROBLEMS CHECK, ACCORDING TO SYSTEM CATALOG, IF 1. PARAMETER of Ak_GetNth_L2 IS CORRECT (INDEXES START FROM 1!)
	
	return EXIT_SUCCESS;
	AK_EPI;
}
 
/**
  * @author Nenad Makar 
  * @brief Function tests if constraint name would be unique in database
  * @return No return value
  */

void AK_constraint_names_test()
{
	char* constraintName1 = "nameUnique";
	char* constraintName2 = "nameUNIQUE";
	int result;
	AK_PRO;
	
	printf("\nExisting constraints:\n\n");
	AK_print_table("AK_constraints_not_null");
	AK_print_table("AK_constraints_unique");
	AK_print_table("AK_reference");
	AK_print_table("AK_constraints_between");
	
	printf("\nChecking if constraint name %s would be unique in database...\n", constraintName1);
	result = Ak_check_constraint_name(constraintName1);
	printf("Yes (0) No (-1): %d\n\n", result);
	
	printf("\nChecking if constraint name %s would be unique in database...\n", constraintName2);
	result = Ak_check_constraint_name(constraintName2);
	printf("Yes (0) No (-1): %d\n\n", result);
	
	printf("\nTest succeeded.");
	AK_EPI;
}