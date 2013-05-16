/**
@file test.c Provides functions for testing purposes
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
 17 */

#include <pthread.h>
#include <stdio.h>
#include "test.h"
#include "../trans/transaction.h"
#include "../file/table.h"
#include "../auxi/auxiliary.h"
#include "../opti/rel_eq_comut.h"


/**
 * @author Goran Štrok
 * @brief returns a string containing attribute types for supplied table name, seperated by ATTR_DELIMITER
 * @param tblName name of the table for which the attribute types will be returned
 */

char* AK_get_table_atribute_types(char* tblName){
    int len_attr, num_attr, next_attr, attr_type;
    int next_address = 0;
    char attr_buffer[4];

    num_attr = AK_num_attr(tblName);

    if (num_attr == -1) {
        return NULL;
    }

    char *attr = (char *) calloc(1, sizeof (char));
    AK_header *table_header = (AK_header *) AK_get_header(tblName);

    for (next_attr = 0; next_attr < num_attr; next_attr++) {
        
        attr_type = (table_header + next_attr)->type;
        
        len_attr = sprintf(attr_buffer,"%d",attr_type);

        attr = (char *) realloc(attr, len_attr + next_address + 1);
        memcpy(attr + next_address, attr_buffer, len_attr);
        next_address += len_attr;

        if (next_attr < num_attr - 1) {
            memcpy(attr + next_address, ATTR_DELIMITER, 1);
            next_address++;
        } else {
            memcpy(attr + next_address, "\0", 1);
        }
       
    }

    free(table_header);
    
    if (next_address > 0) {
        return attr;
    } else {
        free(attr);
        return NULL;
    }
}

/**
 * @author Luka Rajcevic
 * @brief Function for creating test table header
 * @param tbl_name - name of the table for which the header will be created
 * @param attr_name - array of attribute names
 * @param _num - number of attributes
 * @param _type - array of attribute types (eg. TYPE_INT, TYPE_VARCHAR, etc.)
 * @return 1 if ok, 0 otherwise
 */
int create_header_test(char* tbl_name, char** attr_name, int _num, int* _type){
    
    int i;
    
    AK_header t_header[ MAX_ATTRIBUTES ];
    AK_header* temp;
    for (i = 0; i < _num; i++){
        temp = (AK_header*) AK_create_header(attr_name[i], _type[i], FREE_INT, FREE_CHAR, FREE_CHAR);
        memcpy(t_header + i, temp, sizeof ( AK_header));
    }

    memset(t_header + _num, 0, MAX_ATTRIBUTES - _num);

    int startAddress = AK_initialize_new_segment(tbl_name, SEGMENT_TYPE_TABLE, t_header);

    if (startAddress != EXIT_ERROR)
        return 1;
    else
        return 0;
}

/**
 * @author Luka Rajcevic
 * @brief Function for inserting test data into table (needed for python testing)
 * @param tbl_name - name of the table for which the header will be created
 * @param attr_name - array of attribute names
 * @param attr_value - values of attributes to be inserted
 * @param _num - number of attributes
 * @param _type - array of attribute types (eg. TYPE_INT, TYPE_VARCHAR, etc.)
 * @return EXIT_SUCCESS if ok, EXIT_ERROR otherwise
 */
int insert_data_test(char* tbl_name, char** attr_name, char** attr_value, int _num, int* _type){

    int i;

    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);

    Ak_DeleteAll_L(row_root);
    for (i = 0; i < _num; i++){
        if (_type[i] == TYPE_VARCHAR)
            Ak_Insert_New_Element(_type[i], attr_value[i], tbl_name, attr_name[i], row_root);
        if (_type[i] == TYPE_INT){
            int val = atoi(attr_value[i]);
            Ak_Insert_New_Element(_type[i], &val, tbl_name, attr_name[i], row_root); 
        }
        if (_type[i] == TYPE_FLOAT){
            float val = atof(attr_value[i]);
            Ak_Insert_New_Element(_type[i], &val , tbl_name, attr_name[i], row_root);  
        }
    }

    return Ak_insert_row(row_root);
}

/**
 * @author Luka Rajcevic
 * @brief Function for selection operator on one table
 + @param src_table - name of the source table
 + @param dest_table - table in which selection will be stored
 * @param sel_query - array of operators, operands and attributes (postfix query)
 * @param _num - number of attributes
 * @param _type - array of attribute types (eg. TYPE_INT, TYPE_VARCHAR, etc.)
 * @return EXIT_SUCCESS if ok, EXIT_ERROR otherwise
 *
 */
int selection_test(char* src_table, char* dest_table, char** sel_query, int _num, int* _type){
	printf("==================== SELECTION_TEST =====================\n");

    AK_list *expr = (AK_list *) malloc(sizeof (AK_list));
    Ak_Init_L(expr);

    // TYPE_OPERAND 10
    // TYPE_OPERATOR 11
    // TYPE_ATTRIBS 12

    strcpy(expr->table,dest_table);
    int i;
    for (i = 0; i < _num; i++){
        if (_type[i] == TYPE_INT){
            int val = atoi(sel_query[i]);
            Ak_InsertAtEnd_L(_type[i], (char*) &val, sizeof(int), expr);    
        }
        if (_type[i] == TYPE_FLOAT){
            float val = atof(sel_query[i]);
            Ak_InsertAtEnd_L(_type[i], (char *) &val, sizeof(float), expr);
        }
        if (_type[i] == TYPE_OPERATOR || _type[i] == TYPE_ATTRIBS || _type[i] == TYPE_VARCHAR){
            Ak_InsertAtEnd_L(_type[i], sel_query[i], strlen(sel_query[i]), expr);
        }
    }

    if (AK_selection(src_table, dest_table, expr) == EXIT_SUCCESS){
        Ak_DeleteAll_L(expr);
        free(expr);    
        return 1;
    }
    return 0;

}

/**
 * @author Luka Rajcevic
 * @brief prints requested column
 * @return 1 if column is found, 0 otherwise
 * @param num - 0 based index of column
 * @param tbl - name of the table
 */

char* FILEP = "table_test.txt";
FILE *fp;

 int get_column_test(int num, char* tbl){
    fp = fopen(FILEP,"a");

    AK_list *row = AK_get_column(num, tbl);
    if (!Ak_IsEmpty_L(row)){
        while (row->next != NULL){
            row = row->next;
            if (row->type == TYPE_INT){
                fprintf(fp, "| %d ", *((int *) (row)->data) );
            }
            if (row->type == TYPE_FLOAT){
                fprintf(fp, "| %f ", *((float *) (row)->data));
            }
            if (row->type == TYPE_VARCHAR){
                fprintf(fp, "| %s ", row->data);
            }
        }
        fclose(fp);
        return 1;
    }
    fclose(fp);
    return 0;
 }

/**
 * @author Luka Rajcevic
 * @brief prints requested row
 * @return 1 if row is found, 0 otherwise
 * @param num - 0 based index of row
 * @param tbl - name of the table
 */

 int get_row_test(int num, char* tbl){
    fp = fopen(FILEP,"a");
    
    AK_list *row = AK_get_row(num, tbl);
    if (!Ak_IsEmpty_L(row)){
        while (row->next != NULL){
            row = row->next;
            if (row->type == TYPE_INT){
                fprintf(fp, "| %d ", *((int *) (row)->data) );
            }
            if (row->type == TYPE_FLOAT){
                fprintf(fp, "| %f ", *((float *) (row)->data));
            }
            if (row->type == TYPE_VARCHAR){
                fprintf(fp, "| %s ", row->data);
            }
        }
        fclose(fp);
        return 1;
    }
    fclose(fp);
    return 0;
 }


/**
 * @author Dino Laktašić
 * @brief Function for creating test tables
 * @return No return value
 */
void AK_create_test_tables() {
    int mbr, year, id_prof, id_department;
    float weight;

    //---------------------------------------> CREATE TABLE 'STUDENT' <---------------------------------------
    //create header
    AK_header t_header[ MAX_ATTRIBUTES ];
    AK_header* temp;

    temp = (AK_header*) AK_create_header("mbr", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 1, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("lastname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 2, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("year", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 3, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("weight", TYPE_FLOAT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 4, temp, sizeof ( AK_header));
    memset(t_header + 5, 0, MAX_ATTRIBUTES - 5);

    //create table
    char *tblName = "student";
    int startAddress = AK_initialize_new_segment(tblName, SEGMENT_TYPE_TABLE, t_header);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName);

    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);

    mbr = 35890;
    year = 1999;
    weight = 80.00;

    //insert rows in table student
    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Dino", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Laktasic", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Netko", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Netkic", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Mislav", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Cakaric", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Ivan", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Horvat", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Ivo", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Ivic", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Marko", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Markovic", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Ivan", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Ivanovic", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Josip", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Josipovic", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Ivan", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Ankovic", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Marina", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Marovic", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Mario", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Maric", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Matija", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Matkovic", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Ivana", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Ivic", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "John", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Smith", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "William", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Brown", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "David", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Jones", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Robert", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "White", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "James", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Jones", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Jack", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Moore", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Joseph", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Harris", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Richard", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Thomas", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Daniel", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Jackson", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Martin", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Clark", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Joe", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Davis", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Paul", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Lee", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Steve", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Parker", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    AK_print_table(tblName);
    //-------------------------------------------------------------------------------------------------------


    //--------------------------------------> CREATE TABLE 'PROFESSOR' <-------------------------------------
    //create header
    AK_header t_header2[ MAX_ATTRIBUTES ];

    temp = (AK_header*) AK_create_header("id_prof", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 1, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("lastname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 2, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("tel", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 3, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("email", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 4, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("web_page", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 5, temp, sizeof ( AK_header));
    memset(t_header2 + 6, 0, MAX_ATTRIBUTES - 6);

    //create table
    tblName = "professor";
    startAddress = AK_initialize_new_segment(tblName, SEGMENT_TYPE_TABLE, t_header2);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName);

    //row_root = (element) malloc(sizeof (list));
    Ak_Init_L(row_root);

    id_prof = 35890;
    id_prof++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Miroslav", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Baca", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, "042390873", tblName, "tel", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "miroslav.baca@foi.hr", tblName, "email", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "www.foi.hr/nastavnici/baca.miroslav/", tblName, "web_page", row_root);
    Ak_insert_row(row_root);

    id_prof++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Igor", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Balaban", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, "000000000", tblName, "tel", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "igor.balaban@foi.hr", tblName, "email", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "www.foi.hr/nastavnici/balaban.igor/", tblName, "web_page", row_root);
    Ak_insert_row(row_root);

    id_prof++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Antun", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Brumnic", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, "042213777", tblName, "tel", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "antun.brumnic@foi.hr", tblName, "email", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "www.foi.hr/nastavnici/brumnic.antun/", tblName, "web_page", row_root);
    Ak_insert_row(row_root);

    id_prof++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Mirko", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Cubrilo", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, "042213777", tblName, "tel", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "mirko.cubrilo@foi.hr", tblName, "email", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "www.foi.hr/nastavnici/cubrilo.mirko/", tblName, "web_page", row_root);
    Ak_insert_row(row_root);

    id_prof++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Dragutin", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Kermek", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, "042213777", tblName, "tel", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "dragutin.kermek@foi.hr", tblName, "email", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "www.foi.hr/nastavnici/kermek.dragutin/", tblName, "web_page", row_root);
    Ak_insert_row(row_root);

    id_prof++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Tonimir", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Kisasondi", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, "042213777", tblName, "tel", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "tonimir.kisasondi@foi.hr", tblName, "email", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "www.foi.hr/nastavnici/kisasondi.tonimir/", tblName, "web_page", row_root);
    Ak_insert_row(row_root);

    id_prof++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Alen", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Lovrencic", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, "042390866", tblName, "tel", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "alovrenc@foi.hr", tblName, "email", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "www.foi.hr/nastavnici/lovrencic.alen/", tblName, "web_page", row_root);
    Ak_insert_row(row_root);

    id_prof++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Markus", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Schatten", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, "042390892", tblName, "tel", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "markus.schatten@foi.hr", tblName, "email", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "www.foi.hr/nastavnici/schatten.markus/", tblName, "web_page", row_root);
    Ak_insert_row(row_root);

    id_prof++;
	Ak_DeleteAll_L(row_root);
	Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Neven", tblName, "firstname", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Vrcek", tblName, "lastname", row_root);
	Ak_Insert_New_Element(TYPE_INT, "042390892", tblName, "tel", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "neven.vrcek@foi.hr", tblName, "email", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "www.foi.hr/nastavnici/vrcek.neven/", tblName, "web_page", row_root);
	Ak_insert_row(row_root);

    AK_print_table(tblName);
    //-------------------------------------------------------------------------------------------------------


	//--------------------------------------> CREATE TABLE 'PROFESSOR2' <-------------------------------------
	//create header
	
	temp = (AK_header*) AK_create_header("id_prof", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
	memcpy(t_header2, temp, sizeof ( AK_header));
	temp = (AK_header*) AK_create_header("firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
	memcpy(t_header2 + 1, temp, sizeof ( AK_header));
	temp = (AK_header*) AK_create_header("lastname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
	memcpy(t_header2 + 2, temp, sizeof ( AK_header));
	temp = (AK_header*) AK_create_header("tel", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
	memcpy(t_header2 + 3, temp, sizeof ( AK_header));
	memset(t_header2 + 4, 0, MAX_ATTRIBUTES - 4);

	//create table
	tblName = "professor2";
	startAddress = AK_initialize_new_segment(tblName, SEGMENT_TYPE_TABLE, t_header2);

	if (startAddress != EXIT_ERROR)
		printf("\nTABLE %s CREATED!\n", tblName);

	//row_root = (element) malloc(sizeof (list));
	Ak_Init_L(row_root);

	id_prof = 35890;
	id_prof++;
	Ak_DeleteAll_L(row_root);
	Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Miroslav", tblName, "firstname", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Baca", tblName, "lastname", row_root);
	Ak_Insert_New_Element(TYPE_INT, "042390873", tblName, "tel", row_root);
	Ak_insert_row(row_root);

	id_prof++;
	Ak_DeleteAll_L(row_root);
	Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Igor", tblName, "firstname", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Balaban", tblName, "lastname", row_root);
	Ak_Insert_New_Element(TYPE_INT, "000000000", tblName, "tel", row_root);
	Ak_insert_row(row_root);

	id_prof++;
	Ak_DeleteAll_L(row_root);
	Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Antun", tblName, "firstname", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Brumnic", tblName, "lastname", row_root);
	Ak_Insert_New_Element(TYPE_INT, "042213777", tblName, "tel", row_root);
	Ak_insert_row(row_root);

	id_prof++;
	Ak_DeleteAll_L(row_root);
	Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Mirko", tblName, "firstname", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Cubrilo", tblName, "lastname", row_root);
	Ak_Insert_New_Element(TYPE_INT, "042213777", tblName, "tel", row_root);
	Ak_insert_row(row_root);

	id_prof++;
	Ak_DeleteAll_L(row_root);
	Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Dragutin", tblName, "firstname", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Kermek", tblName, "lastname", row_root);
	Ak_Insert_New_Element(TYPE_INT, "042213777", tblName, "tel", row_root);
	Ak_insert_row(row_root);

	id_prof++;
	Ak_DeleteAll_L(row_root);
	Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Tonimir", tblName, "firstname", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Kisasondi", tblName, "lastname", row_root);
	Ak_Insert_New_Element(TYPE_INT, "042213777", tblName, "tel", row_root);
	Ak_insert_row(row_root);

	id_prof++;
	Ak_DeleteAll_L(row_root);
	Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Alen", tblName, "firstname", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Lovrencic", tblName, "lastname", row_root);
	Ak_Insert_New_Element(TYPE_INT, "042390866", tblName, "tel", row_root);
	Ak_insert_row(row_root);

	id_prof++;
	Ak_DeleteAll_L(row_root);
	Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Markus", tblName, "firstname", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Schatten", tblName, "lastname", row_root);
	Ak_Insert_New_Element(TYPE_INT, "042390892", tblName, "tel", row_root);
	Ak_insert_row(row_root);

	id_prof++;
	Ak_DeleteAll_L(row_root);
	Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Neven", tblName, "firstname", row_root);
	Ak_Insert_New_Element(TYPE_VARCHAR, "Vrcek", tblName, "lastname", row_root);
	Ak_Insert_New_Element(TYPE_INT, "042390892", tblName, "tel", row_root);
	Ak_insert_row(row_root);

	AK_print_table(tblName);
    //-------------------------------------------------------------------------------------------------------


    //--------------------------------------> CREATE TABLE 'ASSISTANT' <-------------------------------------
    //create table, same header as professor for intersect test
    AK_header t_header3[ MAX_ATTRIBUTES ];

    temp = (AK_header*) AK_create_header("id_prof", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3 + 1, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("lastname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3 + 2, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("tel", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3 + 3, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("email", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3 + 4, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("web_page", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3 + 5, temp, sizeof ( AK_header));
    memset(t_header3 + 6, 0, MAX_ATTRIBUTES - 6);

    tblName = "assistant";
    startAddress = AK_initialize_new_segment(tblName, SEGMENT_TYPE_TABLE, t_header3);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName);

    //row_root = (element) malloc(sizeof (list));
    Ak_Init_L(row_root);

    id_prof = 35892;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Igor", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Balaban", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, "000000000", tblName, "tel", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "igor.balaban@foi.hr", tblName, "email", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "www.foi.hr/nastavnici/balaban.igor/", tblName, "web_page", row_root);
    Ak_insert_row(row_root);

    id_prof = 35896;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Tonimir", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Kisasondi", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, "042213777", tblName, "tel", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "tonimir.kisasondi@foi.hr", tblName, "email", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "www.foi.hr/nastavnici/kisasondi.tonimir/", tblName, "web_page", row_root);
    Ak_insert_row(row_root);

    id_prof = 35898;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Markus", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Schatten", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, "042390892", tblName, "tel", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "markus.schatten@foi.hr", tblName, "email", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "www.foi.hr/nastavnici/schatten.markus/", tblName, "web_page", row_root);
    Ak_insert_row(row_root);
	
	id_prof = 35899;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Miran", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Zlatović", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, "042390858", tblName, "tel", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "miran.zlatovic@foi.hr", tblName, "email", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "www.foi.hr/nastavnici/zlatovic.miran/index.html", tblName, "web_page", row_root);
    Ak_insert_row(row_root);

    AK_print_table(tblName);
    //-------------------------------------------------------------------------------------------------------


    //--------------------------------------> CREATE TABLE 'EMPLOYEE' <--------------------------------------
    //create header
    AK_header t_header4[ MAX_ATTRIBUTES ];

    temp = (AK_header*) AK_create_header("id_prof", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header4, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("id_department", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header4 + 1, temp, sizeof ( AK_header));
    memset(t_header4 + 2, 0, MAX_ATTRIBUTES - 2);

    //create table
    tblName = "employee";
    startAddress = AK_initialize_new_segment(tblName, SEGMENT_TYPE_TABLE, t_header4);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName);

    //row_root = (element) malloc(sizeof (list));
    Ak_Init_L(row_root);

    id_prof = 35890;
    id_department = 1;
    id_prof++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_department, tblName, "id_department", row_root);
    Ak_insert_row(row_root);

    id_prof++;
    id_department++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_department, tblName, "id_department", row_root);
    Ak_insert_row(row_root);

    id_prof++;
    id_department++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_department, tblName, "id_department", row_root);
    Ak_insert_row(row_root);

    id_prof++;
    id_department++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_department, tblName, "id_department", row_root);
    Ak_insert_row(row_root);

    id_prof++;
    id_department++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_department, tblName, "id_department", row_root);
    Ak_insert_row(row_root);

    id_prof++;
    id_department++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_department, tblName, "id_department", row_root);
    Ak_insert_row(row_root);

    id_prof++;
    id_department++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_department, tblName, "id_department", row_root);
    Ak_insert_row(row_root);

    //don't need id_prof++ here
    id_prof++;
    //id_department++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_department, tblName, "id_department", row_root);
    Ak_insert_row(row_root);

    AK_print_table(tblName);
    //-------------------------------------------------------------------------------------------------------


    //-------------------------------------> CREATE TABLE 'DEPARTMENT' <-------------------------------------
    //create header
    AK_header t_header5[ MAX_ATTRIBUTES ];

    temp = (AK_header*) AK_create_header("id_department", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header5, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("dep_name", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header5 + 1, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("manager", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header5 + 2, temp, sizeof ( AK_header));
    memset(t_header5 + 3, 0, MAX_ATTRIBUTES - 3);

    //create table
    tblName = "department";
    startAddress = AK_initialize_new_segment(tblName, SEGMENT_TYPE_TABLE, t_header5);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName);

    row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);

    id_department = 1;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_department, tblName, "id_department", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Department of Economics", tblName, "dep_name", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Redep", tblName, "manager", row_root);
    Ak_insert_row(row_root);

    //id_department++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_department, tblName, "id_department", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Department of Organization", tblName, "dep_name", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Zugaj", tblName, "manager", row_root);
    Ak_insert_row(row_root);

    id_department++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_department, tblName, "id_department", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Department of Quantitative Methods", tblName, "dep_name", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Kero", tblName, "manager", row_root);
    Ak_insert_row(row_root);

    /* too long
    id_department++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    InsertNewElement(TYPE_VARCHAR, "Department of theoretical and applied foundations of Information Science", tblName, "dep_name", row_root);
    InsertNewElement(TYPE_VARCHAR, "", tblName, "manager", row_root);
    insert_row(row_root);*/

    //id_department++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_department, tblName, "id_department", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Department of Information Technology and Computing", tblName, "dep_name", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Hutinski", tblName, "manager", row_root);
    Ak_insert_row(row_root);

    id_department++;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id_department, tblName, "id_department", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Department of Information Systems Development", tblName, "dep_name", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Vrcek", tblName, "manager", row_root);
    Ak_insert_row(row_root);

    /* too long
    id_department++;
    DeleteAllElements(row_root);
    InsertNewElement(TYPE_INT, &id_department, tblName, "id_department", row_root);
    InsertNewElement(TYPE_VARCHAR, "Department of foreign languages and general education discipline", tblName, "dep_name", row_root);
    InsertNewElement(TYPE_VARCHAR, "", tblName, "manager", row_root);
    insert_row(row_root);*/

    AK_print_table(tblName);
    //-------------------------------------------------------------------------------------------------------


    
    
    
    
    //---------------------------------------> CREATE TABLE 'COURSE' <---------------------------------------
    //create header
    AK_header t_header6[MAX_ATTRIBUTES];

    temp = (AK_header*) AK_create_header("id_course", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header6, temp, sizeof (AK_header));
    temp = (AK_header*) AK_create_header("name", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header6 + 1, temp, sizeof (AK_header));
    temp = (AK_header*) AK_create_header("category", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header6 + 2, temp, sizeof (AK_header));
    temp = (AK_header*) AK_create_header("lecturer", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header6 + 3, temp, sizeof (AK_header));
    temp = (AK_header*) AK_create_header("active", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header6 + 4, temp, sizeof (AK_header));
    memset(t_header6 + 5, '\0', MAX_ATTRIBUTES - 5);

    //create table
    tblName = "course";
    startAddress = AK_initialize_new_segment(tblName, SEGMENT_TYPE_TABLE, t_header6);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName);
    //AK_rel_eq_comut_test();	
    Ak_DeleteAll_L(row_root);
    free(row_root);
    //------------------------------------------------------------------------------------------------------
    //AK_op_rename_test();
    
  //  AK_sequence_test();

//AK_drop_test();

/*******************************/
//printf("\n\n----------------View test----------------\n\n");
//	AK_view_test();
/*******************************/
}


