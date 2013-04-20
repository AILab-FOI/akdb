/**
 * @brief  Function command.
 * @author Mario Kolmacic

 */
#include "command.h"
#include <pthread.h>


void AK_command(command * komande, int brojkomandi) {
    int i;
    for (i = 0; i < brojkomandi; ++i) {
        switch(komande[i].id_command){
        case SELECT:
            printf("***SELECT***\n");
            char *ext = "_selection_tmp_table";
            char *dest_table = malloc(strlen(ext) + strlen(komande[i].tblName) + 1);
            strcat(dest_table, komande[i].tblName);
            strcat(dest_table, ext);
            AK_selection(komande[i].tblName, dest_table, (AK_list*)komande[i].parameters);
            break;
            
        case UPDATE:
            printf("***UPDATE***\n");

            Ak_update_row( ((AK_list_elem) (komande[i].parameters)));
            AK_print_table(komande[i].tblName);

            break;
        case DELETE:
            printf("***DELETE***\n");        	 
            Ak_update_row( ((AK_list_elem) (komande[i].parameters)));
            Ak_delete_row( ((AK_list_elem) (komande[i].parameters)));
            AK_print_table(komande[i].tblName);
            break;
           
        case INSERT:
            printf("***INSERT***\n");

            Ak_insert_row( ((AK_list_elem) (komande[i].parameters)));
            AK_print_table(komande[i].tblName);

            break;
        default:
            break;
        }
    }
}

void AK_test_command(){
    printf("***Test Command***\n");
    int brojkomandi;

    AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);
    char *tblName = "student";
   
    int mbr, year;
    float weight;
    mbr = 35917;
    year = 2012;
    weight = 82.00;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Mario", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Kolmacic", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    command *komande = malloc(sizeof(command)*4);
    komande[0].id_command = 2;
    komande[0].tblName = "student";
    komande[0].parameters = row_root;

    mbr = 35900;
    row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);
    Ak_DeleteAll_L(row_root);

    Ak_Insert_New_Element_For_Update(TYPE_INT, &mbr, tblName, "mbr", row_root, 1);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "FOI", tblName, "firstname", row_root, 0);

    komande[1].id_command = 0;
    komande[1].tblName = "student";
    komande[1].parameters = row_root;

    int id_prof;
    id_prof = 35893;
    row_root = (AK_list_elem) malloc(sizeof (AK_list));
    Ak_Init_L(row_root);
    Ak_DeleteAll_L(row_root);

    Ak_Insert_New_Element_For_Update(TYPE_INT, &id_prof, tblName, "id_prof", row_root, 1);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "FOI", tblName, "firstname", row_root, 0);

    komande[2].id_command = 1;
    komande[2].tblName = "professor";
    komande[2].parameters = row_root;

    brojkomandi = 3;

    AK_command(komande, brojkomandi);
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
  row_root = (AK_list_elem) malloc(sizeof (AK_list));
  Ak_Init_L(row_root);
  Ak_DeleteAll_L(row_root);

  Ak_Insert_New_Element_For_Update(TYPE_INT, &mbr, tblName, "mbr", row_root, 1);
  Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "DELETE", tblName, "firstname", row_root, 0);

  komande[2].id_command = 3;
  komande[2].tblName = "student";
  komande[2].parameters = row_root;

*/
