/**
@file view.c Provides functions for views
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
 * 
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include "view.h"

/**
 * @author Kresimir Ivkovic
 * @brief Finds an object's id by its name
 * @return View's id or EXIT_ERROR
 */
int AK_get_view_obj_id(char *name) {
    int id, i = 0;
    AK_list *row;
    while ((row = (AK_list*)AK_get_row(i++, "AK_view"))) {
        if (!strcmp(row->next->next->data, name)) {
            memcpy(&id, row->next->data, sizeof(int));
            return id;
        }
    }
    return EXIT_ERROR;
}

/**
 * @author Kresimir Ivkovic
 * @brief Adds a new view to the view table with the corresponding name and value (view query); set_id is optional, if it's not set, the system will determine the new id automatically
 * @return Id of the newly inserted view
 */
int AK_view_add(char *name, char *query, int set_id){
    char *tblName = "AK_view";
    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);
    int view_id = AK_get_id();
    if(set_id!=0) view_id = set_id;
    Ak_Insert_New_Element(TYPE_INT, &view_id, tblName, "obj_id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, name, tblName, "name", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, query, tblName, "query", row_root);
    Ak_insert_row(row_root);
    return view_id;
}

/**
 * @author Kresimir Ivkovic
 * @brief Removes the view by its object id
 * @return Result of AK_delete_row for the view (success or error)
 */
int AK_view_remove_by_obj_id(int obj_id) {
    obj_id+=1;
    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L((AK_list_elem)row_root);
    Ak_Insert_New_Element_For_Update(TYPE_INT, &obj_id, "AK_view", "obj_id", row_root, 1);
    int result = Ak_delete_row((AK_list_elem)row_root);  
    Ak_DeleteAll_L((AK_list_elem)row_root);
    return result;
}

/**
 * @author Kresimir Ivkovic
 * @brief Removes the view by its name by identifying the view's id and passing id to AK_view_remove_by_obj_id
 * @return Result of AK_view_remove_by_obj_id or EXIT_ERROR if no id is found
 */
int AK_view_remove_by_name(char *name) {
   AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
   Ak_Init_L((AK_list_elem)row_root);
   Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, name, "AK_view", "name", row_root, 1);
   int result = Ak_delete_row(row_root);
   return result;
}

/**
 * @author Kresimir Ivkovic
 * @brief Renames a view (based on it's name) from "name" to "new_name"
 * @return error or success 
 */
int AK_view_rename(char *name, char *new_name){
   int result = 0, i = 0;
   int view_id;
   char *query;
   AK_list *row;
   while ((row = (AK_list*)AK_get_row(i++, "AK_view"))) {
        if (!strcmp(row->next->next->data, name)) {
            memcpy(&view_id, row->next->data, sizeof(int));
            query = row->next->next->next->data;
        }
    }
   result = AK_view_remove_by_name(name);
   result = AK_view_remove_by_name(name);
   result = AK_view_add(new_name, query, view_id);
   return result;
}

/**
 * @author Kresimir Ivkovic
 * @brief Changes the query for a view (determined by it's name) to "query"
 * @return error or success 
 */
int AK_view_change_query(char *name, char *query){
   int view_id = AK_get_view_obj_id(name);
   int result = AK_view_remove_by_name(name);
   result = AK_view_add(name, query, view_id);
   return result;
}

/**
 * @author Kresimir Ivkovic
 * @brief A testing function for view.c functions.
 * @return 
 */
void AK_view_test() {
   AK_view_add("view1", "query1", 0);
   AK_view_add("view2", "query2", 0);
   AK_view_add("view3", "query3", 0);
   AK_view_add("view4", "query4", 0);
   AK_view_add("view5", "query5", 0);
   AK_print_table("AK_view");

   printf("Obj_id za view1: %d\n", AK_get_view_obj_id("view1"));
   printf("Obj_id za view2: %d\n", AK_get_view_obj_id("view2"));
   printf("Obj_id za view3: %d\n", AK_get_view_obj_id("view3"));
   printf("Obj_id za view4: %d\n", AK_get_view_obj_id("view4"));
   printf("Obj_id za view5: %d\n", AK_get_view_obj_id("view5"));

   printf("\nRemoving view 'view1' and 'view2'...\n");
   AK_view_remove_by_name("view1");
   AK_view_remove_by_name("view2");
   AK_print_table("AK_view");

   printf("\nRenaming view 'view3' to 'view300'...\n");
   AK_view_rename("view3","view300");
   AK_print_table("AK_view");

   printf("\nChanging 'view4' query to '44query44'...\n");
   AK_view_change_query("view4","44query44");
   AK_print_table("AK_view");
  
}

