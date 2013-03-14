#!/usr/bin/env python

import kalashnikovDB as ak47

class Funkcije:
    def create_table(self, table, attributes, types):
        parameters = []
        t_header = []
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
            
        ak47.Ak_update_row(element)
        
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
            
        ak47.Ak_delete_row(element)
       
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
        
        ak47.AK_selection(table, table_res, element)        
        
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
        ak47.AK_join(table1, table2, table_res, att)
        ak47.Ak_DeleteAll_L(att)
        
    def union(self, table1, table2, table_res):
        ak47.AK_union(table1, table2, table_res)
        
    def difference(self, table1, table2, table_res):
        ak47.AK_difference(table1, table2, table_res)
        
    def projection(self, table1, table2, table_res, attributes):
        att = ak47.list_elem()
        ak47.Ak_Init_L(att)
        ak47.Ak_DeleteAll_L(att)
        for attribute in attributes:
            ak47.Ak_InsertAtEnd_L(ak47.TYPE_ATTRIBS, attribute, len(attribute), att)
        ak47.AK_projection(table1, table2, table_res, att)
        ak47.Ak_DeleteAll_L(att)
        
    def product(self, table1, table2, table_res):
        ak47.AK_product(table1, table2, table_res)
        
    def rename_Table(self,table1, table2):
        ak47.AK_rename(table1, "", table2, "")
        
    def rename_Attribute(self,table, att_old, att_new):
        ak47.AK_rename(table, att_old, table, att_new)
        
    def intersect(self, table1, table2, table_res):
        ak47.AK_intersect(table1, table2, table_res)
        
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

  

ak47.main()

f = Funkcije()

#ak47.AK_inflate_config()
#ak47.AK_init_disk_manager()
#ak47.AK_memoman_init()

print "Python insert test:"
f.insert_Row("student", [689, "A", "B", 2012, 130.53])
f.insert_Row("student", [777, "A", "B", 2013, 90.53])
ak47.AK_print_table("student")

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
