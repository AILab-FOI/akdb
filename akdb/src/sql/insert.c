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
 * @brief Checks if table has given attributes
 * @param tableName table in which rows will be inserted
 * @param columns list of columns
 * @return 0 if table doesn't have required columns, 1 othervise
 */
int AK_columns_are_valid(char *tblName, struct list_node *columns) {
    AK_PRO;
    struct list_node* column = Ak_First_L2(columns);
    while(column) {
        int index = AK_get_attr_index(tblName, column->attribute_name);
        if(index < 0) {

            AK_EPI;
            return 0;
        }
        column = Ak_Next_L2(column);
    }
    AK_EPI;
    return 1;
}

/**
 * @author Filip Žmuk
 * @brief Function that implements INSERT comand
 * @param tableName table in which rows will be inserted
 * @param columns list of columns
 * @param values values for columns to be inserted
 * @return EXIT_SUCCESS or EXIT_ERROR
 */
int AK_insert(char* tblName, struct list_node *columns, struct list_node *values) 
{
    AK_PRO;
    if(!AK_table_exist(tblName)) {
        AK_EPI;
        return EXIT_ERROR;
    }

    if(values && !AK_columns_are_valid(tblName, columns)) {
        AK_EPI;
        return EXIT_ERROR;
    }

    int num_attr = AK_num_attr(tblName);
    int num_values = Ak_Size_L2(values);
    int num_columns = num_attr;
    if(columns) 
    {
        num_columns = Ak_Size_L2(columns);
    }

    if(num_values % num_columns) {
        AK_EPI;
        return EXIT_ERROR;
    }

    AK_header *t_header = AK_get_header(tblName);

    struct list_node *row;
    struct list_node *column = Ak_First_L2(columns);
    int i = 0; // index of values
    struct list_node *value = Ak_First_L2(values);

    while(value)
    {
        if(i == 0) {
            row = (struct list_node *) AK_malloc(sizeof (struct list_node));
            Ak_Init_L3(&row);
        }

        char * attribute_name = value ? value->attribute_name : t_header[i].att_name;
        Ak_Insert_New_Element(value->type, value->data, tblName, attribute_name, row);
        column = Ak_Next_L2(column); 

        i++;

        if(i >= num_columns) {
            i = 0;
            if(Ak_insert_row(row) != EXIT_SUCCESS) {
                AK_free(t_header);
                AK_EPI;
                return EXIT_ERROR;
            }
            Ak_DeleteAll_L3(&row);
            AK_free(row);
            column = Ak_First_L2(columns);
        }

        value = Ak_Next_L2(value);
    }

    AK_free(t_header);
    AK_EPI;
    return EXIT_SUCCESS;
}

TestResult Ak_insert_test() {
    struct list_node *row_root;
    int id_field;
    double size_field;

    AK_PRO;
    char *testTable = "insertTestTable";

    if(AK_table_exist(testTable)) {
        //AK_drop(DROP_TABLE, testTable);
    }

    AK_header header[4] = {
    {TYPE_INT, "id", {0}, {{'\0'}}, {{'\0'}}},
    {TYPE_VARCHAR, "name", {0}, {{'\0'}}, {{'\0'}}},
    {TYPE_FLOAT, "size", {0}, {{'\0'}}, {{'\0'}}},
    {0, {'\0'}, {0}, {{'\0'}}, {{'\0'}}}
    };

    if(AK_initialize_new_segment(testTable, SEGMENT_TYPE_TABLE, header) == EXIT_ERROR) {
        printf("\nCouldn't create test table\n");
        AK_EPI;
        return TEST_result(0, 1);
    }

    AK_print_table(testTable);

    row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);


    printf("\nInsert without specifing table columns\n");

    id_field = 1;
    size_field = 3.5;
    Ak_DeleteAll_L3(&row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_field, testTable, "", row_root); // id
    Ak_Insert_New_Element(TYPE_VARCHAR, "Petra", testTable, "", row_root); // name
    Ak_Insert_New_Element(TYPE_FLOAT, &size_field, testTable, "", row_root); // size
    AK_insert(testTable, NULL, row_root);

    AK_print_table(testTable);

    //AK_drop(DROP_TABLE, testTable);


    Ak_DeleteAll_L3(&row_root);
    AK_free(row_root);
    AK_EPI;
    return TEST_result(0, 0);
}