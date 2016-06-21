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
 * @param name name of the view
 * @return View's id or EXIT_ERROR
 */
int AK_get_view_obj_id(char *name) {
    int id, i = 0;
    
    struct list_node *row;
    AK_PRO;
    //while ((row = (AK_list*)AK_get_row(i++, "AK_view"))) {
    while ((row = (struct list_node*)AK_get_row(i++, "AK_view"))) {
        struct list_node *name_elem = Ak_GetNth_L2(2,row);
        if (!strcmp(name_elem->data, name)) {
            memcpy(&id, row->next->data, sizeof(int));
	    AK_EPI;
            return id;
        }
    }
    AK_EPI;
    return EXIT_ERROR;
}

/**
 * @author Danko Sačer
 * @brief Returnes a query by its name
 * @param name name of the view
 * @return query string or EXIT_ERROR
 */
char* AK_get_view_query(char *name){
   int i = 0;
   char *query;
    
    struct list_node *row;
    AK_PRO;

    while ((row = (struct list_node*)AK_get_row(i++, "AK_view"))) {
        struct list_node *name_elem = Ak_GetNth_L2(2,row);
        if (!strcmp(name_elem->data, name)) {
            struct list_node *query_elem = Ak_GetNth_L2(3,row);
            query = query_elem->data;
	    AK_EPI;
	    return query;
        }
    }
    AK_EPI;
    return (char*)(EXIT_ERROR);
}

/**
 * @author Danko Sačer
 * @brief Returnes a relation expression by its name
 * param name name of the view
 * @return rel_exp string or EXIT_ERROR
 */
char* AK_get_rel_exp(char *name){
   int i = 0;
   char *rel_exp;
   
    struct list_node *row;
    AK_PRO;

    while ((row = (struct list_node*)AK_get_row(i++, "AK_view"))) {
        struct list_node *name_elem = Ak_GetNth_L2(2,row);
        if (!strcmp(name_elem->data, name)) {
            struct list_node *rel_exp_elem = Ak_GetNth_L2(3,row);
            rel_exp = rel_exp_elem->data;
	    AK_EPI;
	    return rel_exp;
        }
    }
    AK_EPI;
    return (char*)(EXIT_ERROR);
}

/**
 * @author Kresimir Ivkovic
 * @brief Adds a new view to the view table with the corresponding name and value (view query); set_id is optional, if it's not set, the system will determine the new id automatically
 * @param name name og the view
 * @param query query of the view
 * @param rel_exp relation expression of the view
 * @param set_id id of view
 * @return Id of the newly inserted view
 */
int AK_view_add(char *name, char *query, char *rel_exp, int set_id){
    char *tblName = "AK_view";
    AK_PRO;

    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);
    
    int view_id = AK_get_id();
    if(set_id!=0) view_id = set_id;
    Ak_Insert_New_Element(TYPE_INT, &view_id, tblName, "obj_id", row_root);

    //adding prefics '_view' to view name for integration of view relation expression (used in query_optimization.c for now) by Danko Sacer NOT SET
    //char set_name[strlen(name)];
    //strcpy(set_name, name);
    //strcat(set_name, "_view");
    Ak_Insert_New_Element(TYPE_VARCHAR, name, tblName, "name", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, query, tblName, "query", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, rel_exp, tblName, "rel_exp", row_root);
    Ak_insert_row(row_root);
    AK_EPI;
    return view_id;
}

/**
 * @author Kresimir Ivkovic
 * @brief Removes the view by its object id
 * @param obj_id object id of the view
 * @return Result of AK_delete_row for the view (success or error)
 */
int AK_view_remove_by_obj_id(int obj_id) {
    AK_PRO;
    obj_id+=1;
      
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);
    
    Ak_Insert_New_Element_For_Update(TYPE_INT, &obj_id, "AK_view", "obj_id", row_root, 1);
    int result = Ak_delete_row((AK_list_elem)row_root);
    
    Ak_DeleteAll_L3(&row_root);
    AK_EPI;
    return result;
}

/**
 * @author Kresimir Ivkovic
 * @brief Removes the view by its name by identifying the view's id and passing id to AK_view_remove_by_obj_id
 * @param name name of the view
 * @return Result of AK_view_remove_by_obj_id or EXIT_ERROR if no id is found
 */
int AK_view_remove_by_name(char *name) {
   AK_PRO;
    
    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);
    
   Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, name, "AK_view", "name", row_root, 1);
   int result = Ak_delete_row(row_root);
   AK_EPI;
   return result;
}

/**
 * @author Kresimir Ivkovic
 * @brief Renames a view (based on it's name) from "name" to "new_name"
 * @param name name of the view
 * @param new_name new name of the view
 * @return error or success
 */
int AK_view_rename(char *name, char *new_name){
   int result = 0, i = 0;
   int view_id;
   char *query;
   char *rel_exp;
   
   struct list_node *row;
   AK_PRO;
   
   while ((row = (struct list_node *)AK_get_row(i++, "AK_view"))) {
       struct list_node *name_elem = Ak_GetNth_L2(2,row);
        if (!strcmp(name_elem->data, name)) {
            struct list_node *view_elem = Ak_GetNth_L2(1,row);
            struct list_node *query_rel_exp_elem = Ak_GetNth_L2(3,row);
            memcpy(&view_id, view_elem->data, sizeof(int));
            query = query_rel_exp_elem->data;
	    rel_exp = query_rel_exp_elem->data;
        }
    }

   result = AK_view_remove_by_name(name);
   result = AK_view_remove_by_name(name);
   result = AK_view_add(new_name, query, rel_exp, view_id);
   AK_EPI;
   return result;
}

/**
 * @author Kresimir Ivkovic
 * @brief Changes the query for a view (determined by it's name) to "query"
 * @param name of the query
 * @param query new query of the view
 * @param rel_exp relation expression of the view
 * @return error or success
 */
int AK_view_change_query(char *name, char *query, char *rel_exp){
   AK_PRO;
   int view_id = AK_get_view_obj_id(name);

   int result = AK_view_remove_by_name(name);
   result = AK_view_add(name, query, rel_exp, view_id);
   AK_EPI;
   return result;
}

/**
 * @author Kresimir Ivkovic, updated by Lidija Lastavec
 * @brief A testing function for view.c functions.
 */
void AK_view_test() {
   AK_PRO;
   printf("\n*******View test**********\n\n");
   AK_view_add("view1", "SELECT lastname FROM profesor","profesor;lastname;", 0);
   AK_view_add("view2", "query2","firstname;", 0);
   AK_view_add("view3", "query3","*;profesor;", 0);
   AK_view_add("view4", "query4","2 >;", 0);
   AK_view_add("view5", "query5","id_object;", 0);
   AK_view_add("view6", "query6","lastname", 0);
   AK_view_add("view7", "query7","student;firstname", 0);
   AK_print_table("AK_view");

   printf("Obj_id za view1: %d\n", AK_get_view_obj_id("view1"));
   printf("Query za view1: %s\n\n", AK_get_view_query("view1"));

   printf("Obj_id za view2: %d\n", AK_get_view_obj_id("view2"));
   printf("Query za view2: %s\n\n", AK_get_view_query("view2"));

   printf("Obj_id za view3: %d\n", AK_get_view_obj_id("view3"));
   printf("Query za view3: %s\n\n", AK_get_view_query("view3"));

   printf("Obj_id za view4: %d\n", AK_get_view_obj_id("view4"));
   printf("Query za view4: %s\n\n", AK_get_view_query("view4"));

   printf("Obj_id za view5: %d\n", AK_get_view_obj_id("view5"));
   printf("Query za view5: %s\n\n", AK_get_view_query("view5"));

   printf("\nRemoving view 'view1' and 'view2'...\n");
   AK_view_remove_by_name("view1");
   AK_view_remove_by_name("view2");
   AK_print_table("AK_view");
   
   printf("\nRemoving view 'view6' and 'view7' by Obj_id...\n");
   printf("Obj_id for view6: %d\n", AK_get_view_obj_id("view6"));
   printf("Obj_id for view7: %d\n", AK_get_view_obj_id("view7"));
    printf("\nTEST FALED!!!\n\n");   
   AK_view_remove_by_obj_id(AK_malloc(sizeof(AK_get_view_obj_id("view6"))));
   AK_view_remove_by_obj_id(AK_malloc(sizeof(AK_get_view_obj_id("view7"))));
   // Function AK_view_remove_by_obj_id does not work!_tested by Lidija Lastavec   
   AK_print_table("AK_view");
  

   printf("\nRenaming 'view3' to 'view300'...\n");
   AK_view_rename("view3","view300");
   AK_print_table("AK_view");

   printf("\nChanging 'view4' query to '44query44'...\n");
   AK_view_change_query("view4","44query44","student;id");
   AK_print_table("AK_view");
   AK_EPI;
}
