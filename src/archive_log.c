#include "archive_log.h"


AK_archive_log(char* A, char* B, char* C, char *D, char *E, char *F, char *G)
{
     struct timeval tv;
     struct timezone tz;
     struct tm *tm;
     gettimeofday(&tv, &tz);
     tm=localtime(&tv.tv_sec);
     //printf("******TESTIRANJE LOGOVA******\n");

     FILE *fp;
     fp = fopen("log.log", "a+");
     fprintf(fp, "\n[%d:%d:%d-%d:%02d:%02d.%d]", tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec);
     fprintf(fp,  " Function: %s", A);

     if(!strcmp(A, "AK_join")){
    	 fprintf(fp,  " | Tables: %s, %s", B, C);
    	 fprintf(fp,  " | Destination table: %s", D);
     }

     /*else if(!strcmp(A, "Ak_dbg_messg")){
    	 fprintf(fp,  " | Level: %d,", B);
    	 fprintf(fp,  " | Type: %d", C);
    	 fprintf(fp,  " | Format: %s", D);
     }
*/
     else if(!strcmp(A, "AK_theta_join")){
    	 fprintf(fp,  " | Tables: %s, %s", B, C);
    	 fprintf(fp,  " | Destination table: %s", D);
     }

     else if(!strcmp(A, "AK_union")){
    	 fprintf(fp,  " | Tables: %s, %s", B, C);
    	 fprintf(fp,  " | Destination table: %s", D);
     }

     else if(!strcmp(A, "AK_initialize_new_segment")){
    	 fprintf(fp,  " | Table Name: %s", B);
    	 fprintf(fp,  " | Type: %d", C);
     }

     else if(!strcmp(A, "AK_create_header")){
    	 fprintf(fp,  " | Header Name: %s", B);
    	 fprintf(fp,  " | Type: %d", C);
    	 fprintf(fp,  " | Integrity: %d", D);
    	 fprintf(fp,  " | Constraint Name: %s", E);
    	 fprintf(fp,  " | Control Code: %s", F);
     }

     else if(!strcmp(A, "Ak_Insert_New_Element")){
    	 fprintf(fp,  " | NewType: %d", B);
    	 fprintf(fp,  " | Data: %s", C);
    	 fprintf(fp,  " | Table: %s", D);
    	 fprintf(fp,  " | Attribute Name: %s", E);
     }

     else if(!strcmp(A, "AK_trigger_add")){
    	 fprintf(fp,  " | Name: %s", B);
    	 fprintf(fp,  " | Event: %s", C);
    	 fprintf(fp,  " | Condition: %s", D);
    	 fprintf(fp,  " | Table: %s", E);
    	 fprintf(fp,  " | Function: %s", F);
     }

     else if(!strcmp(A, "AK_trigger_remove_by_name")){
    	 fprintf(fp,  " | Name: %s", B);
    	 fprintf(fp,  " | Table: %s", C);
     }

     else if(!strcmp(A, "AK_rename")){
    	 fprintf(fp,  " | Old table name: %s", B);
    	 fprintf(fp,  " | Old attr: %s", C);
    	 fprintf(fp,  " | New table name: %s", D);
    	 fprintf(fp,  " | New attr: %s", E);
     }

     else if(!strcmp(A, "AK_product")){
    	 fprintf(fp,  " | Tables: %s, %s", B, C);
    	 fprintf(fp,  " | Destination table: %s", D);
     }

     else if(!strcmp(A, "AK_projection")){
    	 fprintf(fp,  " | Source Table: %s", B);
    	 fprintf(fp,  " | Destination table: %s", C);
     }

     else if(!strcmp(A, "AK_intersect")){
    	 fprintf(fp,  " | Tables: %s, %s", B, C);
    	 fprintf(fp,  " | Destination table: %s", D);
     }
     else{
    	 fprintf(fp,  " | ERROR");
     }
     fclose(fp);
}
