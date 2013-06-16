/**
 @file redo_log.c Provides redolog functions.
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include "redo_log.h"

/**
 * @author @author Krunoslav Bilić updated by Dražen Bandić
 * @brief Function adds new element to redolog
 * @return EXIT_FAILURE if not allocated memory for ispis, otherwise EXIT_SUCCESS
 */
int AK_add_to_redolog(char command[6], AK_list *row_root){

    int n = redo_log->number;

    if(n == MAX_REDO_LOG_ENTRIES){
        AK_archive_log();
        n = 0;
    }

    AK_list_elem el = (AK_list_elem) Ak_First_L(row_root);
    
    char* record;
    if((record = (char*) calloc(MAX_VARCHAR_LENGTH, sizeof(char))) == NULL){
    	return EXIT_FAILURE;
    }

    char* int_char = (char*) malloc(MAX_VARCHAR_LENGTH * sizeof(char));
    char* float_char = (char*) malloc(MAX_VARCHAR_LENGTH * sizeof(char));
    char* default_char = (char*) malloc(MAX_VARCHAR_LENGTH * sizeof(char));

    char table[MAX_ATT_NAME];   
    memset(table, '\0', MAX_ATT_NAME);
    memcpy(&table, el->table, strlen(el->table));

    int max = AK_num_attr(table);
    int numAttr = 1;

    while (el != NULL) {
        switch (el->type) {

            case FREE_CHAR:
                strncat(record, "null", 4);
                break;
            case TYPE_INT:
                sprintf (int_char, "%i", *((int *) el->data));
                strncat(record, int_char, strlen(int_char));
                break;
            case TYPE_FLOAT:
               	sprintf (float_char, "%.3f", *((float *) el->data));
               	strncat(record, float_char, strlen(float_char));
                break;
            case TYPE_VARCHAR:
            default:
                default_char = AK_check_attributes(el->data);
                strncat(record, default_char, strlen(el->data));
                break;
        }
        if(numAttr < max){
            strncat(record, "|", 1);
        }
        numAttr++;
        el = el->next;
    }

    memcpy(redo_log->table_name[n], table, strlen(table));
    memcpy(redo_log->attributes[n], record, strlen(record));
    memcpy(redo_log->command[n], command, strlen(command));
    
    int x = n + 1;

    redo_log->number = x;

    free(record);
    free(int_char);
    free(float_char);
    free(default_char);

    return EXIT_SUCCESS;
}


/** 
 * @author Krunoslav Bilić updated by Dražen Bandić
 * @brief Function prints out the content of redolog memory
 * @return No return value.
 */
void AK_printout_redolog(){
    int x = redo_log->number;
    int i = 0;
    for (i = 0; i < x; i++){
        printf("%d. %s %s %s\n", i, redo_log->table_name[i], redo_log->command[i], redo_log->attributes[i]);
    }
}

/** 
 * @author Dražen Bandić
 * @brief Checks if the attribute contains '|', and if it does it replaces it with "\|"
 * @return new attribute
 */
char* AK_check_attributes(char *attributes){

    int n = strlen(attributes);

    if( attributes[n-1] == '\\' ){
        if( n == MAX_VARCHAR_LENGTH )
            attributes[n-1] = ' ';
        else
            attributes[n] = ' ';
    }

    char* result = malloc(MAX_VARCHAR_LENGTH * sizeof(char));
    int index = 0;
    int att_index = 0;

    do{
        if ( attributes[att_index] == '|' ){
            if ( index + 2 <= MAX_VARCHAR_LENGTH ){
                result[index++] = '\\';
                result[index++] = '|';
            } else {
                break;
            }
        } else {
            result[index++] = attributes[att_index];
        }
        att_index++;
    } while ( att_index < n || index < MAX_VARCHAR_LENGTH);

    return result;
}
