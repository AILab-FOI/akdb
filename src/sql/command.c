#include "command.h"

/** 
 * @author Mario Kolmacic updated by Ivan Pusic
 * @brief Function for executing given commands (SELECT, UPDATE, DELETE AND INSERT)
 * @param komande Commands array to execute
 * @param brojkomandi Number of commands in array
 */
int AK_command(command * komande, int brojkomandi) {
    int i;
    AK_PRO;
    for (i = 0; i < brojkomandi; ++i) {
        switch(komande[i].id_command){
        case SELECT:
            printf("***SELECT***\n");
            char *ext = "_selection_tmp_table";
            char *dest_table = AK_malloc(strlen(ext) + strlen(komande[i].tblName) + 1);
            strcat(dest_table, komande[i].tblName);
            strcat(dest_table, ext);
            if(AK_selection(komande[i].tblName, dest_table, (struct list_node*)komande[i].parameters) == EXIT_ERROR){
		AK_EPI;
                return EXIT_ERROR;
	    }
            break;
            
        case UPDATE:
            printf("***UPDATE***\n");
            if(Ak_update_row( ((struct list_node *) (komande[i].parameters))) == EXIT_ERROR){
                AK_EPI;
                return EXIT_ERROR;
            }
            AK_print_table(komande[i].tblName);

            break;
        case DELETE:
            printf("***DELETE***\n");        	 
            if(Ak_update_row( ((struct list_node *) (komande[i].parameters))) == EXIT_ERROR){
		AK_EPI;
                return EXIT_ERROR;
	    }
            if(Ak_delete_row( ((struct list_node *) (komande[i].parameters))) == EXIT_ERROR){
		AK_EPI;
                return EXIT_ERROR;
	    }
            AK_print_table(komande[i].tblName);
            break;
           
        case INSERT:
            printf("***INSERT***\n");
            if(Ak_insert_row( ((struct list_node *) (komande[i].parameters))) == EXIT_ERROR){
		AK_EPI;
                return EXIT_ERROR;
	    }
            AK_print_table(komande[i].tblName);
            break;
        default:
            break;
        }
    }
    AK_EPI;
    return EXIT_SUCCESS;
}

void AK_test_command(){
    AK_PRO;
    printf("***Test Command***\n");
    int brojkomandi;
/*
    AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L(row_root);
    */

    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);
    
    char *tblName = "student";
   
    int mbr, year;
    float weight;
    mbr = 35917;
    year = 2012;
    weight = 82.00;
    //Ak_DeleteAll_L(row_root);
    Ak_DeleteAll_L3(&row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Mario", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Kolmacic", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    command *komande = AK_malloc(sizeof(command)*4);
    komande[0].id_command = 2;
    komande[0].tblName = "student";
    komande[0].parameters = row_root;

    mbr = 35900;
    /*row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L(row_root);
    Ak_DeleteAll_L(row_root);*/
    
    row_root = (struct list_node *) AK_malloc(sizeof (struct list_node ));
    Ak_Init_L3(&row_root);
    Ak_DeleteAll_L(&row_root);

    Ak_Insert_New_Element_For_Update(TYPE_INT, &mbr, tblName, "mbr", row_root, 1);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "FOI", tblName, "firstname", row_root, 0);

    komande[1].id_command = 0;
    komande[1].tblName = "student";
    komande[1].parameters = row_root;

    int id_prof;
    id_prof = 35893;
    /*
    row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L(row_root);
    Ak_DeleteAll_L(row_root);
    */
    
    row_root = (struct list_node *) AK_malloc(sizeof (struct list_node ));
    Ak_Init_L3(&row_root);
    Ak_DeleteAll_L(&row_root);

    Ak_Insert_New_Element_For_Update(TYPE_INT, &id_prof, tblName, "id_prof", row_root, 1);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "FOI", tblName, "firstname", row_root, 0);

    komande[2].id_command = 1;
    komande[2].tblName = "professor";
    komande[2].parameters = row_root;

    brojkomandi = 3;

    AK_command(komande, brojkomandi);
    AK_EPI;
}

/*
  int id_prof;
  id_prof = 35888;
  Ak_Insert_New_Element(TYPE_INT, &id_prof, tblName, "id_prof", row_root);
  Ak_Insert_New_Element(TYPE_VARCHAR, "Lee", tblName, "firstname", row_root);
  Ak_Insert_New_Element(TYPE_VARCHAR, "Harvey", tblName, "lastname", row_root);
  Ak_Insert_New_Element(TYPE_INT, "000000000", tblName, "tel", row_root);
  Ak_Insert_New_Element(TYPE_VARCHAR, "email@foi.hr", tblName, "email", row_root);
  Ak_Insert_New_Element(TYPE_VARCHAR, "www.foi.hr/nastavnici", tblName, "web_page", row_root);


  Ak_Insert_New_Element_For_Update(TYPE_INT, &id_prof, tblName, "id_prof", row_root, 1);
  Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "FOI", tblName, "firstname", row_root, 0);
*/

/*

  mbr = 35915;
  row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
  Ak_Init_L(row_root);
  Ak_DeleteAll_L(row_root);

  Ak_Insert_New_Element_For_Update(TYPE_INT, &mbr, tblName, "mbr", row_root, 1);
  Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "DELETE", tblName, "firstname", row_root, 0);

  komande[2].id_command = 3;
  komande[2].tblName = "student";
  komande[2].parameters = row_root;

*/
