#!/usr/bin/env python
#test.py

import kalashnikovDB as ak47
import test_strings as ts

## @package test
#  Documentation for this module.
#  This module is used for testing purposes
#

class Functions:
 
    def create_table_header(self, table, attr_name, attr_type):
        return ak47.create_header_test(table, attr_name, attr_type)

    def insert_data(self, table, attr_name, attr_value, attr_type):
        return ak47.insert_data_test(table, attr_name, attr_value, attr_type)

    def update_Row(self, table, column1, column2, key, new_value):
        element = ak47.list_node()
        
        if type(key) == int:
            ak47.AK_Update_Existing_Element(ak47.TYPE_INT, key, table, column1, element)
        elif type(key) == float:
            ak47.AK_Update_Existing_Element(ak47.TYPE_FLOAT, key, table, column1, element)
        elif type(key) == str:
            ak47.AK_Update_Existing_Element(ak47.TYPE_VARCHAR, key, table, column1, element)
            
        if type(new_value) == int:
            ak47.AK_Insert_New_Element(ak47.TYPE_INT, new_value, table, column2, element)
        elif type(new_value) == float:
            ak47.AK_Insert_New_Element(ak47.TYPE_FLOAT, new_value, table, column2, element)
        elif type(new_value) == str:
            ak47.AK_Insert_New_Element(ak47.TYPE_VARCHAR, new_value, table, column2, element)
        
        return ak47.AK_update_row(element)
        
        
    def delete_Row(self, table, column1, key):
        element = ak47.list_node()
        
        if type(key) == int:
            ak47.AK_Update_Existing_Element(ak47.TYPE_INT, key, table, column1, element)
        elif type(key) == float:
            ak47.AK_Update_Existing_Element(ak47.TYPE_FLOAT, key, table, column1, element)
        elif type(key) == str:
            ak47.AK_Update_Existing_Element(ak47.TYPE_VARCHAR, key, table, column1, element)
            
        return ak47.AK_delete_row(element)

    def sel(self, src_table, dest_table, query, query_types ):
        return ak47.selection_test(src_table, dest_table, query, query_types)

    def attribute_count(self, table):
        return ak47.AK_num_attr(table)

    def records_count(self, table):
        return ak47.AK_get_num_records(table)

    def get_column_test(self, col_num, table):
        return ak47.get_column_test(col_num, table)

    def get_row_test(self, row_num, table):
        return ak47.get_row_test(row_num, table)

    def get_attr_name(self, table, index):
        return ak47.AK_get_attr_name(table, index)

    def get_attr_index(self, table, attr_name):
        return ak47.AK_get_attr_index(table, attr_name)

    def get_value(self, row, col, table):
        element = ak47.AK_get_tuple(row, col, table)
        return ak47.AK_tuple_to_string(element)

    def theta_join(self, table1, table2, table_res, expr):
        table1_attributes = []
        table2_attributes = []
        
        c = 0
        while 1:
            attribute = ak47.AK_get_attr_name(table1, c)
            if attribute != None:
                table1_attributes.append(attribute)
                c += 1
            else:
                break
 
        c = 0
        while 1:
            attribute = ak47.AK_get_attr_name(table2, c)
            if attribute != None:
                table2_attributes.append(attribute)
                c += 1
            else:
                break
                
        element = ak47.list_node()
        ak47.AK_Init_L3(element)
        ak47.AK_DeleteAll_L3(element)
        
        operatori = ["<", ">", "=", "AND", "OR", "+", "-", "*"]

        c = 0        
        for el in expr:
            if operatori.count(el) == 1:
                ak47.AK_InsertAtEnd_L3(ak47.TYPE_OPERATOR, el, len(el), element)
                c = 0
            elif table1_attributes.count(el) == 1 or table2_attributes.count(el) == 1:
                ak47.AK_InsertAtEnd_L3(ak47.TYPE_ATTRIBS, el, len(el), element)
                c = 1
            else:
                if type(el) == int:
                    ak47.AK_InsertAtEnd_L3(ak47.TYPE_INT, el, 4, element)
                elif type(el) == float:
                    ak47.AK_InsertAtEnd_L3(ak47.TYPE_FLOAT, el, 4, element)
                elif type(el) == str:
                    ak47.AK_InsertAtEnd_L3(ak47.TYPE_VARCHAR, el, len(el), element)
                c = 0
       
        ak47.AK_theta_join(table1, table2, table_res, element)

    def nat_Join(self, table1, table2, table_res, attributes):
        att = ak47.list_node()
        ak47.AK_Init_L3(att)
        ak47.AK_DeleteAll_L3(att)
        for attribute in attributes:
            ak47.AK_InsertAtEnd_L3(ak47.TYPE_ATTRIBS, attribute, len(attribute), att)
        return ak47.AK_join(table1, table2, table_res, att)
        #ak47.AK_DeleteAll_L3(att)
        
    def union(self, table1, table2, table_res):
        return ak47.AK_union(table1, table2, table_res)
        
    def difference(self, table1, table2, table_res):
        return ak47.AK_difference(table1, table2, table_res)
        
    def projection(self, table1, table_res, attributes):
        att = ak47.list_node()

        for attribute in attributes:
            ak47.AK_InsertAtEnd_L3(ak47.TYPE_ATTRIBS, attribute, len(attribute), att)
        return ak47.AK_projection(table1, table_res,att, None)
        
        
    def product(self, table1, table2, table_res):
        return ak47.AK_product(table1, table2, table_res)
        
    def rename_Table(self,table1, table2):
        return ak47.AK_rename(table1, "", table2, "")
        
    def rename_Attribute(self,table, att_old, att_new):
        return ak47.AK_rename(table, att_old, table, att_new)
        
    def intersect(self, table1, table2, table_res):
        return ak47.AK_intersect(table1, table2, table_res)
    
    def clear_file(self, filename):
        open(filename, 'w').close()
        return 0;

    ## prints table to txt file, and then verifies if it's updated
    ## in the it should have been
    ## parameters are table to print and string to verify the table
    ## filename is table_test.txt
    def verify_table(self, table, verifier, filename):
        self.clear_file(filename)
        ak47.AK_print_table_to_file(table)
        f = open(filename, "r")
        s = ""
        for line in f:
            s += line
        #print s
        if (s == verifier):
            return 0
        else:
            return -1
    def verify_row_or_column(self, verifier, filename):
        f = open(filename, "r")
        s = ""
        for line in f:
            s += line
        #print s
        if (s == verifier):
            return 0
        else:
            return -1

# call to main.c from python (invoking AK_create_test_tables)
#ak47.main()
f = Functions()
ak47.AK_inflate_config()
ak47.AK_init_disk_manager()
ak47.AK_memoman_init()


# author: Luka Rajcevic
# Test data for "student" table
# attribute names, attribute types and attribute data
student_attr_name = ["id_student", "firstname", "lastname", "year", "weight"]
student_attr_type = [ak47.TYPE_INT, ak47.TYPE_VARCHAR, ak47.TYPE_VARCHAR, ak47.TYPE_INT, ak47.TYPE_FLOAT]
student_attr_value_1 = ["1", "Collin", "Channell", "1990", "100.1"]
student_attr_value_2 = ["2", "Manuel", "Mcguffey", "1991", "80"]
student_attr_value_3 = ["3", "Lawanda", "Leep", "1990", "75.6"]
student_attr_value_4 = ["4", "Johnie", "Janda", "1992", "61"]
student_attr_value_5 = ["5", "Daisey", "Dunagan", "1987", "63.2"]
student_attr_value_6 = ["6", "Orpha", "Ousley", "1989", "80.23"]
student_attr_value_7 = ["7", "Hedwig", "Hester", "1989", "66"]
student_attr_value_8 = ["8", "James", "Jaime", "1993", "75.3"]

# author: Luka Rajcevic
# Test data for "class" table
# attribute names, attribute types and attribute data
class_attr_name = ["id_class", "class_name", "year"]
class_attr_type = [ak47.TYPE_INT, ak47.TYPE_VARCHAR, ak47.TYPE_INT]
class_attr_value_1 = ["1", "Art History", "2009"]
class_attr_value_2 = ["2", "Biology", "2011"]
class_attr_value_3 = ["3", "Chemistry", "2011"]
class_attr_value_4 = ["4", "Computer Science", "2012"]

# author: Luka Rajcevic
# Test data for "class_2" table
# attribute names, attribute types and attribute data
class_2_attr_name = ["id_class", "class_name", "year"]
class_2_attr_type = [ak47.TYPE_INT, ak47.TYPE_VARCHAR, ak47.TYPE_INT]
class_2_attr_value_1 = ["1", "Art History", "2009"]
class_2_attr_value_2 = ["2", "Cognitive Studies", "2002"]
class_2_attr_value_3 = ["3", "Chemistry", "2011"]
class_2_attr_value_4 = ["4", "Urban Design", "2000"]

# author: Luka Rajcevic
# Test data for "class_3" table
# attribute names, attribute types and attribute data
class_3_attr_name = ["id_class", "class_name", "year", "language"]
class_3_attr_type = [ak47.TYPE_INT, ak47.TYPE_VARCHAR, ak47.TYPE_INT]
class_3_attr_value_1 = ["1", "Data Structures", "2009", "German"]
class_3_attr_value_2 = ["2", "Cognitive Studies", "2002", "English"]
class_3_attr_value_3 = ["3", "Chemistry", "2011", "Croatian"]
class_3_attr_value_4 = ["4", "Urban Design", "2000", "Spanish"]



# author: Luka Rajcevic
# creates test tables
# made for saving space (for every function we would have to populate tables to test
# methods, so this function is made to be called on start of every test (except insert test))

def create_tables():
    #create student table and insert data
    ak47.create_header_test("student", student_attr_name, student_attr_type)
    ak47.insert_data_test("student", student_attr_name, student_attr_value_1, student_attr_type)
    ak47.insert_data_test("student", student_attr_name, student_attr_value_2, student_attr_type)
    ak47.insert_data_test("student", student_attr_name, student_attr_value_3, student_attr_type)
    ak47.insert_data_test("student", student_attr_name, student_attr_value_4, student_attr_type)
    ak47.insert_data_test("student", student_attr_name, student_attr_value_5, student_attr_type)
    ak47.insert_data_test("student", student_attr_name, student_attr_value_6, student_attr_type)
    ak47.insert_data_test("student", student_attr_name, student_attr_value_7, student_attr_type)
    ak47.insert_data_test("student", student_attr_name, student_attr_value_8, student_attr_type)

    #create class table and insert data
    ak47.create_header_test("class", class_attr_name, class_attr_type)
    ak47.insert_data_test("class", class_attr_name, class_attr_value_1, class_attr_type)
    ak47.insert_data_test("class", class_attr_name, class_attr_value_2, class_attr_type)
    ak47.insert_data_test("class", class_attr_name, class_attr_value_3, class_attr_type)
    ak47.insert_data_test("class", class_attr_name, class_attr_value_4, class_attr_type)

    #create class_2 table and insert data
    ak47.create_header_test("class_2", class_2_attr_name, class_2_attr_type)
    ak47.insert_data_test("class_2", class_2_attr_name, class_2_attr_value_1, class_2_attr_type)
    ak47.insert_data_test("class_2", class_2_attr_name, class_2_attr_value_2, class_2_attr_type)
    ak47.insert_data_test("class_2", class_2_attr_name, class_2_attr_value_3, class_2_attr_type)
    ak47.insert_data_test("class_2", class_2_attr_name, class_2_attr_value_4, class_2_attr_type)
    #create class_3 table and insert data
    ak47.create_header_test("class_3", class_3_attr_name, class_3_attr_type)
    ak47.insert_data_test("class_3", class_3_attr_name, class_3_attr_value_1, class_3_attr_type)
    ak47.insert_data_test("class_3", class_3_attr_name, class_3_attr_value_2, class_3_attr_type)
    ak47.insert_data_test("class_3", class_3_attr_name, class_3_attr_value_3, class_3_attr_type)
    ak47.insert_data_test("class_3", class_3_attr_name, class_3_attr_value_4, class_3_attr_type)
    return 1

