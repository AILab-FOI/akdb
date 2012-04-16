/**
 * @brief  Function command.
 * @author Mario Kolmacic

 */
#include "command.h"

struct lista {
   //void *novi;
   int br;
   char tblName;
   int mbr;
   char ime;
   char prezime;
   int year;
   float weight;
   struct lista * next;
};

typedef struct lista item;


void AK_command(char *table, int *mb, char *a, char *b, int *c, float d) {
   int i;
   item * curr, * head;

   AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
   Ak_Init_L(row_root);
   head = NULL;

   for(i=1;i<=4;i++) {
      curr = (item *)malloc(sizeof(item));
      curr->br = i;
      curr->tblName = table;
      curr->mbr = mb;
      curr->ime = a;
      curr->prezime = b;
      curr->year = c;
      curr->weight = d;
      curr->next = head;
      head = curr;
   switch(i){
         case 1:
        	 printf("INSERT INTO student VALUES(35917, 'Lee','Harvey', 2027, 100.250);\n");
        	 Ak_DeleteAll_L(row_root);
        	 Ak_Insert_New_Element(TYPE_INT, &mb, "student", "mbr", row_root);
        	 Ak_Insert_New_Element(TYPE_VARCHAR, a, "student", "firstname", row_root);
        	 Ak_Insert_New_Element(TYPE_VARCHAR, b, "student", "lastname", row_root);
        	 Ak_Insert_New_Element(TYPE_INT, &c, "student", "year", row_root);
        	 Ak_Insert_New_Element(TYPE_FLOAT, &d, "student", "weight", row_root);
        	 Ak_insert_row(row_root);

        	 AK_print_table(table);
             break;
         case 2:
        	 printf("\nUPDATE student SET firstname='Ivo', lastname='Ivic' where mbr=35916;\n");
        	 Ak_DeleteAll_L(row_root);
        	 Ak_Insert_New_Element_For_Update(TYPE_INT, &mb, "student", "mbr", row_root, 1);
        	 Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "Ivo", "student", "firstname", row_root, 0);
        	 Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "Ivic", "student", "lastname", row_root, 0);
        	 Ak_update_row(row_root);
        	 //Ak_delete_row(row_root);
        	 AK_print_table(table);
        	 break;
         case 3:
        	 printf("\nDELETE FROM student WHERE mbr=35916;\n");
        	 Ak_delete_row(row_root);
             break;
         case 4:
        	 printf("\nSELECT * FROM student;\n");
        	 AK_print_table(table);
        	 break;
         default:
             break;
             }
   }

   curr = head;

   /*while(curr) {
      printf("%d\n", curr->novi);
      curr = curr->next ;
   }*/
   system("pause");
}



void AK_test_command(){
    printf("***Test Command***\n");
	AK_command("student", 35917, "Lee", "Harvey", 2027, 100.250);

    }
