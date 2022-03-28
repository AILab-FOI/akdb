/**
@file select.c Provides functions for SELECT relational operator
 *
 *
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
#include "select.h"
#include "../mm/memoman.h"

/**
 * @author Filip Žmuk
 * @brief Function that implements SELECT relational operator
 * @param srcTable - original table that is used for selection
 * @param destTable - table that contains the result
 * @param condition - condition for selection
 * @param attributes - atributes to be selected
 * @param ordering - atributes for result sorting
 * @return EXIT_SUCCESS if cache result in memory and print table else break 
 */
int AK_select(char *srcTable, char *destTable, struct list_node *attributes, struct list_node *condition, struct list_node *ordering)
{
    struct list_node *attribute;
    AK_PRO;

    //create help table name for selection
    char selection_table[MAX_ATT_NAME] = "";
    strcat(selection_table, srcTable);

    if(condition != NULL)
    {
        strcat(selection_table, "__selection");

        //select required rows
        if (AK_selection(srcTable, selection_table, condition) != EXIT_SUCCESS)
        {
            AK_EPI;
            return EXIT_ERROR;
        }
    }

    // create copy of attributes
    struct list_node *projectionAttributes = (struct list_node *)AK_malloc(sizeof(struct list_node));
    AK_Init_L3(&projectionAttributes);
    for (attribute = AK_First_L2(attributes); attribute; attribute = AK_Next_L2(attribute))
    {
        AK_InsertAtEnd_L3(TYPE_ATTRIBS, attribute->data, strlen(attribute->data), projectionAttributes);
    }


    //create help table name for sorting
    char sorted_table[ MAX_ATT_NAME ] = "";
    strcat(sorted_table, selection_table);
    //sort required rows
    if (ordering != NULL)
    {
        strcat(sorted_table, "__sorted");
        if (AK_sort_segment(selection_table, sorted_table, ordering) != EXIT_SUCCESS)
        {
            AK_DeleteAll_L3(&projectionAttributes);
            AK_free(projectionAttributes);
            AK_EPI;
            return EXIT_ERROR;
        }
    }

    //project required rows
    if (AK_projection(sorted_table, destTable, projectionAttributes, NULL) != EXIT_SUCCESS)
    {
        AK_DeleteAll_L3(&projectionAttributes);
        AK_free(projectionAttributes);
        AK_EPI;
        return EXIT_ERROR;
    }

    // free temp tables
    if(strcmp(srcTable, selection_table) != 0)
    {
        AK_delete_segment(selection_table, SEGMENT_TYPE_TABLE);
        if(strcmp(selection_table, sorted_table) != 0)
        {
            AK_delete_segment(sorted_table, SEGMENT_TYPE_TABLE);
        }
    }
    else
    {
        if(strcmp(srcTable, sorted_table) != 0)
        {
            AK_delete_segment(sorted_table, SEGMENT_TYPE_TABLE);
        }
    }

    AK_DeleteAll_L3(&projectionAttributes);
    AK_free(projectionAttributes);
    AK_EPI;
    return EXIT_SUCCESS;
}

/**
 * @author Renata Mesaros, updated by Filip Žmuk and Josip Susnjara
 * @brief Function for testing the implementation
 */
TestResult AK_select_test(){
	
    int succesfulTests = 0;
    int failedTests = 0;
    AK_PRO;
	
	// list of attributes which will be in the result of selection
	struct list_node *attributes = (struct list_node *) AK_malloc(sizeof (struct list_node));

	// list of elements which represent the condition for selection
	struct list_node *condition = (struct list_node *) AK_malloc(sizeof (struct list_node));

    // list of elements which represent the condition for ordering
	struct list_node *ordering = (struct list_node *) AK_malloc(sizeof (struct list_node));


	char *srcTable="student";
	char *destTable1="select_result1";
    char *destTable2="select_result2";

	AK_Init_L3(&attributes);
	AK_InsertAtEnd_L3(TYPE_ATTRIBS, "firstname", sizeof("firstname"), attributes);
	AK_InsertAtEnd_L3(TYPE_ATTRIBS, "year", sizeof("year"), attributes);
	AK_InsertAtEnd_L3(TYPE_ATTRIBS, "weight", sizeof("weight"), attributes);
    AK_InsertAtEnd_L3(TYPE_ATTRIBS, "weight+year", sizeof("weight+year"), attributes);

	AK_Init_L3(&condition);
	AK_InsertAtEnd_L3(TYPE_ATTRIBS, "year", sizeof("year"), condition);
    int year = 2008;
	AK_InsertAtEnd_L3(TYPE_INT, (char *)&year, sizeof(int), condition);
	AK_InsertAtEnd_L3(TYPE_OPERATOR, "<", sizeof("<"), condition);

    AK_Init_L3(&ordering);
	AK_InsertAtBegin_L3(TYPE_ATTRIBS, "firstname", sizeof("firstname"), ordering);


    if (AK_select(srcTable, destTable1, attributes, condition, ordering) == EXIT_SUCCESS)
    {
        succesfulTests++;
    }
    else
    {
        failedTests++;
    }
    
	AK_DeleteAll_L3(&attributes);
    AK_DeleteAll_L3(&ordering);
	AK_DeleteAll_L3(&condition);
    AK_Init_L3(&attributes);
	AK_InsertAtEnd_L3(TYPE_ATTRIBS, "firstname", sizeof("firstname"), attributes);
	AK_InsertAtEnd_L3(TYPE_ATTRIBS, "year", sizeof("year"), attributes);

    if (AK_select(destTable1, destTable2, attributes, NULL, NULL) == EXIT_SUCCESS)
    {
        succesfulTests++;
    }
    else
    {
        failedTests++;
    }
    
    AK_DeleteAll_L3(&attributes);
	
    AK_print_table(srcTable);
	printf("\n SELECT firstname, year, weight, weight+year FROM student WHERE year < 2008 ORDER BY firstname;\n\n");
    AK_print_table(destTable1);
    printf("\n SELECT firstname, year FROM select_result1;\n\n");
    AK_print_table(destTable2);
	
	AK_free(attributes);
	AK_free(condition);
	AK_free(ordering);
	AK_EPI;

	return TEST_result(succesfulTests, failedTests);
}
