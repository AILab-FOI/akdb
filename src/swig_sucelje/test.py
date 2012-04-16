import kalashnikovDB


print kalashnikovDB.MAX_NUM_OF_BLOCKS
kalashnikovDB.main()

kalashnikovDB.AK_op_selection_test()
kalashnikovDB.AK_op_projection_test()


a = kalashnikovDB.list_elem
a.type = 2
a.size = 3
a.data = "PODACI"
a.table = "NAZIV TABLICE"
a.attribute_name = "NAME"
a.constraint = 123

print a.type
print a.size
print a.data
print a.table
print a.attribute_name
print a.constraint