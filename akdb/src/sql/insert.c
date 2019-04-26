/**
 * @file insert.h TODO
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


#include "insert.h"

/**
 * @author Filip Žmuk
 * @brief Function that implements INSERT comand
 * @param tableName table in which rows will be inserted
 * @param columns list of columns
 * @param values values for columns to be inserted
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_insert(char* tableName, struct list_node *columns, struct list_node **values, int rows) 
{
    AK_PRO;

    if(columns == NULL) 
    {
        // TODO columns = get_columns_for_table(tableName)
    }


    for(int i = 0; i < rows; i++)
    {
        struct list_node *row = (struct list_node *) AK_malloc(sizeof (struct list_node));
        struct list_node *column = columns;
        Ak_Init_L3(&row);
        while(column)
        {
            // 
            //  Ak_Insert_New_Element(TYPE_VARCHAR, "Vacenovski", "testna", "Prezime", row_root);
            column = column->next; 
        }
        Ak_insert_row(row);
        Ak_DeleteAll_L3(&row);
        AK_free(row);
    }

    AK_EPI;
    return EXIT_ERROR;
}

TestResult Ak_insert_test() {
    AK_PRO;
    AK_EPI;
    return TEST_result(0, 0);
}