#include "command.h"

/** 
 * @author Mario Kolmacic updated by Ivan Pusic and Tomislav Ilisevic
 * @brief Function for executing given commands (SELECT, UPDATE, DELETE AND INSERT)
 * @param komande Commands array to execute
 * @param brojkomandi Number of commands in array
 * @return ERROR_EXIT only if command can't be executed returns EXIT_ERROR
 */
int AK_command(command * komande, int brojkomandi) {
    int i;
    AK_PRO;
    for (i = 0; i < brojkomandi; ++i) {
        switch(komande[i].id_command){
        case SELECT:
            printf("***SELECT***\n");
            char *ext = "_selection_tmp_table";
            char *dest_table = AK_malloc(strlen(ext) + strlen(komande[i].tblName)+1);
            strcpy(dest_table, komande[i].tblName);
            strcat(dest_table, ext);
            if(AK_selection(komande[i].tblName, dest_table, (struct list_node*)komande[i].parameters) == EXIT_ERROR){ // unutar funkcije je ispis privremene tablice
		AK_EPI;
                return EXIT_ERROR;
	    }

            break;
            
        case UPDATE:
            printf("***UPDATE***\n");
            if(Ak_update_row( (struct list_node *) (komande[i].parameters)) == EXIT_ERROR){
                AK_EPI;
                return EXIT_ERROR;
            }
            AK_print_table(komande[i].tblName);

            break;
        case DELETE:
            printf("***DELETE***\n");        	 
            if(Ak_delete_row( (struct list_node *) (komande[i].parameters)) == EXIT_ERROR){
		AK_EPI;
                return EXIT_ERROR;
	    }
            AK_print_table(komande[i].tblName);
            break;
           
        case INSERT:
            printf("***INSERT***\n");
            if(Ak_insert_row( (struct list_node *) (komande[i].parameters)) == EXIT_ERROR){
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

/**
 * @author Unknown, updated by Tomislav Ilisevic
 * @brief  Function for testing commands
 *
 */
void AK_test_command(){
    AK_PRO;
    printf("***Test Command***\n");
    int brojkomandi = 4;
    command *komande = AK_malloc(sizeof(command)*(brojkomandi+1));


	// Test za komandu INSERT

    struct list_node *row_root = (struct list_node *) AK_malloc(sizeof (struct list_node));
    Ak_Init_L3(&row_root);
    
    char *tblName = "student";
   
    int mbr, year;
    float weight;
    mbr = 35917;
    year = 2012;
    weight = 82.00;
    Ak_DeleteAll_L3(&row_root);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName, "mbr", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Mario", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Kolmacic", tblName, "lastname", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);

    komande[0].id_command = INSERT;
    komande[0].tblName = "student";
    komande[0].parameters = row_root;


	// Test za komandu UPDATE

    mbr = 35900;
    
    row_root = (struct list_node *) AK_malloc(sizeof (struct list_node ));
    Ak_Init_L3(&row_root);
    Ak_DeleteAll_L3(&row_root);

    Ak_Insert_New_Element_For_Update(TYPE_INT, &mbr, tblName, "mbr", row_root, 1);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "FOI", tblName, "firstname", row_root, 0);

    komande[1].id_command = UPDATE;
    komande[1].tblName = "student";
    komande[1].parameters = row_root;


	// Test za komandu DELETE

    int id_prof;
    id_prof = 35893;
    
    row_root = (struct list_node *) AK_malloc(sizeof (struct list_node ));
    Ak_Init_L3(&row_root);
    Ak_DeleteAll_L3(&row_root);

    Ak_Insert_New_Element_For_Update(TYPE_INT, &id_prof, tblName, "id_prof", row_root, 1);
    Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "FOI", tblName, "firstname", row_root, 0);

    komande[2].id_command = DELETE;
    komande[2].tblName = "professor";
    komande[2].parameters = row_root;


	// Test za komandu SELECT

	row_root = (struct list_node *) AK_malloc(sizeof (struct list_node ));
	Ak_Init_L3(&row_root);
	Ak_DeleteAll_L3(&row_root);

//	printf("\nQUERY: SELECT * FROM student WHERE firstname = 'Robert';\n\n");

	Ak_InsertAtEnd_L3(TYPE_ATTRIBS, "firstname", sizeof ("firstname"), row_root);
	Ak_InsertAtEnd_L3(TYPE_VARCHAR, "Robert", sizeof ("Robert"), row_root);
	Ak_InsertAtEnd_L3(TYPE_OPERATOR, "=", sizeof ("="), row_root);

	komande[3].id_command = SELECT;
	komande[3].tblName = "student";
	komande[3].parameters = row_root;


	// Izvrsi komande
	AK_command(komande, brojkomandi);
	Ak_DeleteAll_L3(&row_root);

	AK_EPI;
}
