/**
 * @file constraint_names.c Provides functions for checking if constraint name is unique in database
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
 * @author Nenad Makar, updated by Mislav Jurinić
 * @brief Function checks if constraint name would be unique in database 
 * @param char constraintName name which you want to give to constraint which you are trying to create
 * @return EXIT_ERROR or EXIT_SUCCESS
 **/
int Ak_check_constraint_name(char *constraintName) {
	int i, j;
	int num_rows;

	/**
	 * TODO add other constraint names from the catalog
	 * Also add them to "constants.h"
	 */
	char constraint_table_names[][50] = {
		AK_CONSTRAINTS_BEWTEEN,
		AK_CONSTRAINTS_UNIQUE,
		AK_CONSTRAINTS_CHECK_CONSTRAINT,
		AK_CONSTRAINTS_NOT_NULL,
		AK_REFERENCE
	};

	size_t constraint_table_names_size = sizeof(constraint_table_names) / sizeof(constraint_table_names[0]);

	struct list_node *row;
	struct list_node *attribute;

	AK_PRO;

	for (i = 0; i < constraint_table_names_size; ++i) {
		num_rows = AK_get_num_records(constraint_table_names[i]);

		for (j = 0; j < num_rows; ++j) {
			row = AK_get_row(j, constraint_table_names[i]);
			attribute = Ak_GetNth_L2(3, row);
			
			if (strcmp(attribute->data, constraintName) == 0) {
				AK_EPI;
				return EXIT_ERROR;
			}
		}
	}
		
	//OTHER CONSTRAINTS ARE NOT YET IMPLEMENTED, COMPLETE THIS FUNCTION WHEN THAT HAPPENS!!!
	//WRITE SIMILIAR CODE TO THE CODE ABOVE, CHECK TABLES AND ATTRIBUTES NAMES IN dbman.c
	//IF THERE ARE PROBLEMS CHECK, ACCORDING TO SYSTEM CATALOG, IF 1. PARAMETER of Ak_GetNth_L2 IS CORRECT (INDEXES START FROM 1!)
	
	AK_EPI;

	return EXIT_SUCCESS;
}
 
/**
  * @author Nenad Makar 
  * @brief Function tests if constraint name would be unique in database
  * @return No return value
  */
void AK_constraint_names_test() {
	char *constraintName1 = "nameUnique";
	char *constraintName2 = "nameUNIQUE";
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
