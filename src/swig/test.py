#!/usr/bin/env python

import kalashnikovDB as ak47

class Functions:
    '''
    def create_table(self, table, attributes, types):
        parameters = []
        t_header = []
        if len(attributes) != len(types):
            print "error"
            return
        for i in range(len(attributes)):
            if types[i] == ak47.TYPE_INT:
                parametar = ak47.AK_create_create_table_parameter(ak47.TYPE_INT, attributes[i])
                parameters.append(parametar)
            elif types[i] == ak47.TYPE_VARCHAR:
                parametar = ak47.AK_create_create_table_parameter(ak47.TYPE_VARCHAR, attributes[i])
                parameters.append(parametar)
            elif types[i] == ak47.TYPE_FLOAT:
                parametar = ak47.AK_create_create_table_parameter(ak47.TYPE_FLOAT, attributes[i])
                parameters.append(parametar)
        parametar = ak47.AK_create_create_table_parameter(ak47.TYPE_VARCHAR, "abcd")
        parameters.append(parametar)
        ak47.AK_create_table(table, parameters, len(attributes))
        print "table created"	
    '''
    # @author: Luka Rajcevic
    # @brief: Creates header of attributes for a table
    # @param table - table name
    # @param attr_name - list of attribute names
    # @param attr_type - list of attribute types
    def create_table_header(self, table, attr_name, attr_type):
        return ak47.create_header_test(table, attr_name, attr_type)

    
    # @Brief: inserts data into specific table
    # @param table - table name
    # @param attr_name - list of attribute names
    # @param attr_value - list of attribute values
    # @param attr_type - list of attribute types
    def insert_data(self, table, attr_name, attr_value, attr_type):
        return ak47.insert_data_test(table, attr_name, attr_value, attr_type)
    

    def update_Row(self, table, column1, column2, key, new_value):
        element = ak47.list_elem()
        ak47.Ak_Init_L(element)
        ak47.Ak_DeleteAll_L(element)
        
        if type(key) == int:
            ak47.Ak_Insert_New_Element_For_Update(ak47.TYPE_INT, key, table, column1, element, 1)
        elif type(key) == float:
            ak47.Ak_Insert_New_Element_For_Update(ak47.TYPE_FLOAT, key, table, column1, element, 1)
        elif type(key) == str:
            ak47.Ak_Insert_New_Element_For_Update(ak47.TYPE_VARCHAR, key, table, column1, element, 1)
            
        if type(new_value) == int:
            ak47.Ak_Insert_New_Element_For_Update(ak47.TYPE_INT, new_value, table, column2, element, 0)
        elif type(new_value) == float:
            ak47.Ak_Insert_New_Element_For_Update(ak47.TYPE_FLOAT, new_value, table, column2, element, 0)
        elif type(new_value) == str:
            ak47.Ak_Insert_New_Element_For_Update(ak47.TYPE_VARCHAR, new_value, table, column2, element, 0)
        
        return ak47.Ak_update_row(element)
        
        
    def delete_Row(self, table, column1, key):
        element = ak47.list_elem()
        ak47.Ak_Init_L(element)
        ak47.Ak_DeleteAll_L(element)
        
        if type(key) == int:
            ak47.Ak_Insert_New_Element_For_Update(ak47.TYPE_INT, key, table, column1, element, 1)
        elif type(key) == float:
            ak47.Ak_Insert_New_Element_For_Update(ak47.TYPE_FLOAT, key, table, column1, element, 1)
        elif type(key) == str:
            ak47.Ak_Insert_New_Element_For_Update(ak47.TYPE_VARCHAR, key, table, column1, element, 1)
            
        return ak47.Ak_delete_row(element)

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
        element = ak47.list_elem()
        ak47.Ak_Init_L(element)
        ak47.Ak_DeleteAll_L(element)

        element = ak47.AK_get_tuple(row, col, table)
        return ak47.AK_tuple_to_string(element)
    '''
    def selection(self, table, table_res, expr):
        element = ak47.list_elem()
        ak47.Ak_Init_L(element)
        ak47.Ak_DeleteAll_L(element)
        
        operatori = ["<", ">", "=", "AND", "OR"]

        c = 0        
        for el in expr:
            if operatori.count(el) == 1:
                ak47.Ak_InsertAtEnd_L(ak47.TYPE_OPERATOR, el, len(el), element)
                c = 0
            elif c == 0:
                ak47.Ak_InsertAtEnd_L(ak47.TYPE_ATTRIBS, el, len(el), element)
                c = 1
            else:
                if type(el) == int:
                    ak47.Ak_InsertAtEnd_L(ak47.TYPE_INT, el, 4, element)
                elif type(el) == float:
                    ak47.Ak_InsertAtEnd_L(ak47.TYPE_FLOAT, el, 4, element)
                elif type(el) == str:
                    ak47.Ak_InsertAtEnd_L(ak47.TYPE_VARCHAR, el, len(el), element)
                c = 0
        
        return ak47.AK_selection(table, table_res, element)        
    '''
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
                
        element = ak47.list_elem()
        ak47.Ak_Init_L(element)
        ak47.Ak_DeleteAll_L(element)
        
        operatori = ["<", ">", "=", "AND", "OR", "+", "-", "*"]

        c = 0        
        for el in expr:
            if operatori.count(el) == 1:
                ak47.Ak_InsertAtEnd_L(ak47.TYPE_OPERATOR, el, len(el), element)
                c = 0
            elif table1_attributes.count(el) == 1 or table2_attributes.count(el) == 1:
                ak47.Ak_InsertAtEnd_L(ak47.TYPE_ATTRIBS, el, len(el), element)
                c = 1
            else:
                if type(el) == int:
                    ak47.Ak_InsertAtEnd_L(ak47.TYPE_INT, el, 4, element)
                elif type(el) == float:
                    ak47.Ak_InsertAtEnd_L(ak47.TYPE_FLOAT, el, 4, element)
                elif type(el) == str:
                    ak47.Ak_InsertAtEnd_L(ak47.TYPE_VARCHAR, el, len(el), element)
                c = 0
       
        ak47.AK_theta_join(table1, table2, table_res, element)

    def nat_Join(self, table1, table2, table_res, attributes):
        att = ak47.list_elem()
        ak47.Ak_Init_L(att)
        ak47.Ak_DeleteAll_L(att)
        for attribute in attributes:
            ak47.Ak_InsertAtEnd_L(ak47.TYPE_ATTRIBS, attribute, len(attribute), att)
        return ak47.AK_join(table1, table2, table_res, att)
        #ak47.Ak_DeleteAll_L(att)
        
    def union(self, table1, table2, table_res):
        return ak47.AK_union(table1, table2, table_res)
        
    def difference(self, table1, table2, table_res):
        return ak47.AK_difference(table1, table2, table_res)
        
    def projection(self, table1, table_res, attributes):
        att = ak47.list_elem()
        ak47.Ak_Init_L(att)
        ak47.Ak_DeleteAll_L(att)
        for attribute in attributes:
            ak47.Ak_InsertAtEnd_L(ak47.TYPE_ATTRIBS, attribute, len(attribute), att)
        return ak47.AK_projection(table1, table_res, att)
        #ak47.Ak_DeleteAll_L(att)
        
    def product(self, table1, table2, table_res):
        return ak47.AK_product(table1, table2, table_res)
        
    def rename_Table(self,table1, table2):
        return ak47.AK_rename(table1, "", table2, "")
        
    def rename_Attribute(self,table, att_old, att_new):
        return ak47.AK_rename(table, att_old, table, att_new)
        
    def intersect(self, table1, table2, table_res):
        return ak47.AK_intersect(table1, table2, table_res)
    
    '''
    #deprecated    
    def insert_Row(self, table, row):
        element = ak47.list_elem()
        ak47.Ak_Init_L(element)
        ak47.Ak_DeleteAll_L(element)
        brojac = 0
        for i in range(len(row)):
            if type(row[i]) == int:
                ak47.Ak_Insert_New_Element(ak47.TYPE_INT, row[i], table, ak47.AK_get_attr_name(table, brojac), element)
            elif type(row[i]) == float:
                ak47.Ak_Insert_New_Element(ak47.TYPE_FLOAT, row[i], table, ak47.AK_get_attr_name(table, brojac), element)
            elif type(row[i]) == str:
                ak47.Ak_Insert_New_Element(ak47.TYPE_VARCHAR, row[i], table, ak47.AK_get_attr_name(table, brojac), element)
            brojac += 1
        ak47.Ak_insert_row(element)
    '''
# call to main.c from python (invoking AK_create_test_tables) 
#ak47.main()


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

f = Functions()

# author: Luka Rajcevic
# this function tests methods from Functions class, more specifically its
# create_table_header and insert_data methods.
def create_test():
    '''
    >>> ak47.AK_inflate_config()

    >>> ak47.AK_init_disk_manager()
    0
    >>> ak47.AK_memoman_init()
    0
    >>> f.create_table_header("student", student_attr_name, student_attr_type)
    1
    >>> f.insert_data("student", student_attr_name, student_attr_value_1, student_attr_type)
    0
    >>> f.insert_data("student", student_attr_name, student_attr_value_2, student_attr_type)
    0
    >>> f.insert_data("student", student_attr_name, student_attr_value_3, student_attr_type)
    0
    >>> f.insert_data("student", student_attr_name, student_attr_value_4, student_attr_type)
    0
    >>> f.insert_data("student", student_attr_name, student_attr_value_5, student_attr_type)
    0
    >>> f.insert_data("student", student_attr_name, student_attr_value_6, student_attr_type)
    0
    >>> f.insert_data("student", student_attr_name, student_attr_value_7, student_attr_type)
    0
    >>> f.insert_data("student", student_attr_name, student_attr_value_8, student_attr_type)
    0
    >>> ak47.AK_print_table("student")
    >>> f.delete_Row("student", "id_student", 1)
    0
    >>> ak47.AK_print_table("student")
    '''

# author: Luka Rajcevic
# this function tests methods from Functions class,
# it queries table for its properties (attribute count, record count etc.)
# bugs:
#   -> get_row_test() does not print number values
#   -> rename_Table() does not work (segfault)
def table_properties_test():
    '''
    >>> ak47.AK_inflate_config()

    >>> ak47.AK_init_disk_manager()
    0
    >>> ak47.AK_memoman_init()
    0
    >>> f.create_table_header("class", class_attr_name, class_attr_type)
    1
    >>> f.insert_data("class", class_attr_name, class_attr_value_1, class_attr_type)
    0
    >>> f.insert_data("class", class_attr_name, class_attr_value_2, class_attr_type)
    0
    >>> f.insert_data("class", class_attr_name, class_attr_value_3, class_attr_type)
    0
    >>> f.insert_data("class", class_attr_name, class_attr_value_4, class_attr_type)
    0
    >>> f.attribute_count("class")
    3
    >>> f.records_count("class")
    4
    >>> f.get_column_test(1, "class")
    1
    >>> f.get_column_test(0, "class")
    1
    >>> f.get_row_test(1, "class")
    1
    >>> f.get_attr_name("class", 1)
    'class_name'
    >>> f.get_attr_name("class", 0)
    'id_class'
    >>> f.get_attr_index("class", "year")
    2
    >>> f.get_value(1, 2, "class")
    '2011'
    >>> f.rename_Attribute("class", "year", "anno")
    0
    >>> ak47.AK_print_table("class")

    ''' 

# author: Luka Rajcevic
# creates test tables
# made for saving space (for every function we would have to populate tables to test
# methods, so this function is made to be called on start of every test (except insert test))

def create_tables():
    #initialize memory
    ak47.AK_inflate_config()
    ak47.AK_init_disk_manager()
    ak47.AK_memoman_init()
    
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


# author: Luka Rajcevic
# Test data for selection
# select * from student where year < 1990
student_attr_name = ["id_student", "firstname", "lastname", "year", "weight"]
selection_query_1 = ["year", "1991", ">"]
selection_query_2 = ["year", "1990", ">"]
selection_query_3 = ["firstname", "Manuel", "=="]
selection_query_1_types = [ak47.TYPE_ATTRIBS, ak47.TYPE_INT, ak47.TYPE_OPERATOR]


# author: Luka Rajcevic
# test functions for CRUD operations
# current bugs:
#   -> select works only on integers (floats and strings do not work)
#   -> update doesn't do anything (table stays the same)
#   -> delete is removing the wrong row (eg. 3 instead of 4, or 7 instead of 8)
def selection_test():
    '''
    >>> create_tables()
    1
    >>> f.sel("student", "s2", selection_query_1, selection_query_1_types)
    1
    >>> f.sel("student", "s3", selection_query_2, selection_query_1_types)
    1
    >>> f.update_Row("student", "id_student", "year", 1, 50)
    0
    >>> ak47.AK_print_table("student")

    >>> f.delete_Row("student", "id_student", 3)
    0
    >>> f.delete_Row("student", "id_student", 7)
    0
    >>> ak47.AK_print_table("student")

    '''

def rel_algebra_test():
    '''
    >>> create_tables() 
    1
    >>> f.product("student", "class", "product")
    0
    >>> ak47.AK_print_table("product")

    >>> f.intersect("class_2", "class", "intersect")
    0
    >>> ak47.AK_print_table("intersect")
    
    >>> f.difference("class_2", "class", "difference")
    0
    >>> ak47.AK_print_table("difference")
    
    >>> f.union("class_2", "class", "union")
    0
    >>> ak47.AK_print_table("union")
    
    >>> f.projection("student", "projection", ["id_student", "year"])
    0
    >>> ak47.AK_print_table("projection")

    >>> f.projection("student", "projection", ["year"])
    0
    >>> ak47.AK_print_table("projection")
    
    >>> f.projection("student", "projection", ["firstname", "year", "lastname"])
    0
    >>> ak47.AK_print_table("projection")
    
    >>> f.nat_Join("class_2", "class_3", "natural_join", ["id_class", "class_name"])
    0
    >>> ak47.AK_print_table("natural_join")
    '''
# author: Luka Rajcevic
# function for other tests implemented in project
# ===>  ADD YOUR OWN TEST CALLS HERE
def other_tests():
    '''
    >>ak47.AK_observable_test();

    ''' 




# old test method calls
# not working because method implementation is changed
'''
print "\n\n"
print "Python insert test:"
f.insert_Row("student", [689, "A", "B", 2012, 130.53])
f.insert_Row("student", [777, "A", "B", 2013, 90.53])
#ak47.AK_print_table("student")


print "\n\n"
print "Python update test:"
f.update_Row("student", "mbr", "lastname", 35916, "!!!!!!!!!!!!!")
f.update_Row("student", "mbr", "year", 777, 476)
ak47.AK_print_table("student")

print "\n\n"
print "Python delete test:"
f.delete_Row("student", "mbr", 689)
ak47.AK_print_table("student")

print "\n\n"
print "Python selection test:"
f.selection("student", "s2", ["year", 1000, "<", "firstname", "Robert","=", "OR"])
ak47.AK_print_table("s2")

print "\n\n"
print "Python theta join test:"
f.theta_join("department", "professor", "s3", ["manager", "lastname", "="])
ak47.AK_print_table("s3")

print "\n\n"
print "Python create table test:"
f.create_table("create_test", ["mbr", "firstname", "lastname", "year", "weight"], [ak47.TYPE_INT, ak47.TYPE_VARCHAR, ak47.TYPE_VARCHAR, ak47.TYPE_INT, ak47.TYPE_FLOAT])
ak47.AK_print_table("AK_relation")
f.insert_Row("create_test", [689, "A", "B", 2012, 130.53])
f.insert_Row("create_test", [777, "A", "B", 2013, 90.53])
f.insert_Row("create_test", [111, "A", "B", 2013, 90.53])
f.insert_Row("create_test", [222, "A", "B", 2013, 90.53])
f.insert_Row("create_test", [333, "A", "B", 2013, 90.53])
f.insert_Row("create_test", [444, "A", "B", 2013, 90.53])
ak47.AK_print_table("create_test")

#ak47.AK_flush_cache()
'''