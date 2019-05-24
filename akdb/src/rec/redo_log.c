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
 * @author @author Krunoslav Bilić updated by Dražen Bandić, second update by Tomislav Turek
 * @brief Function adds new element to redolog
 * @return EXIT_FAILURE if not allocated memory for ispis, otherwise EXIT_SUCCESS
 */
int AK_add_to_redolog(int command, struct list_node *row_root){
    AK_PRO;

    if (redo_log == NULL)
      return EXIT_FAILURE;
    int n = redo_log->number;
    printf("AK_add_to_redolog: Recovery checkpoint %d\n", n);

    if(n == MAX_REDO_LOG_ENTRIES){
        AK_archive_log(-10);
        n = 0;
    }

    struct list_node * el = (struct list_node *) AK_First_L2(row_root);
    
    char* record;
    if((record = (char*) AK_calloc(MAX_VARCHAR_LENGTH, sizeof(char))) == NULL){
        AK_EPI;
    	return EXIT_FAILURE;
    }

    char table[MAX_ATT_NAME];
    memset(table, '\0', MAX_ATT_NAME);
    memcpy(&table, el->table, strlen(el->table));

    int max = AK_num_attr(table);
    int numAttr = 1;

    char** attrs = AK_calloc(MAX_ATTRIBUTES, sizeof(char*));
    int i = 0;
    while (el != NULL) {
        switch (el->type) {

            case FREE_CHAR:
                strncat(record, "null", 4);
                attrs[i] = "null";
                break;
            case TYPE_INT:
				attrs[i] = (char*) AK_malloc(MAX_VARCHAR_LENGTH * sizeof(char));
                sprintf (attrs[i], "%i", *((int *) el->data));
                strncat(record, attrs[i], strlen(attrs[i]));
                break;
            case TYPE_FLOAT:
				attrs[i] = (char*) AK_malloc(MAX_VARCHAR_LENGTH * sizeof(char));
               	sprintf (attrs[i], "%.3f", *((float *) el->data));
               	strncat(record, attrs[i], strlen(attrs[i]));
                break;
            case TYPE_VARCHAR:
            default:
                attrs[i] = AK_check_attributes(el->data);
                strncat(record, attrs[i], strlen(el->data));
                break;
        }
        if(numAttr < max){
            strncat(record, "|", 1);
        }
        numAttr++;
		i++;
        el = el->next;
    }
    
    printf("AK_add_to_redolog: redolog new entry -- %s, %s\n", table, record);
    memcpy(redo_log->command_recovery[n].table_name, table, strlen(table));
	for(i=0; i<numAttr-1; i++)
		strcpy(redo_log->command_recovery[n].arguments[i], attrs[i]);
    redo_log->command_recovery[n].operation = command;
    redo_log->command_recovery[n].finished = 0;
    redo_log->number = n+1;
    AK_free(record);
	for(i=0; i < numAttr-1; i++)
		AK_free(attrs[i]);
	AK_free(attrs);
    printf("AK_add_to_redolog: Recovery checkpoint saved\n");
    AK_EPI;
    return EXIT_SUCCESS;
}

void AK_redolog_commit() {
    int i;
    for(i = 0; i < redo_log->number; i++) {
        redo_log->command_recovery[i].finished = 1;
    }
}


/**
 * @author Krunoslav Bilić updated by Dražen Bandić, second update by Tomislav Turek
 * @brief Function prints out the content of redolog memory
 * @return No return value.
 */
void AK_printout_redolog(){
    AK_PRO;
    int x = redo_log->number;
    int i = 0;
    for (i = 0; i < x; i++){
        printf("%d. %s %s %s\n", i, redo_log->command_recovery[i].table_name,
                redo_log->command_recovery[i].operation, redo_log->command_recovery[i].arguments);
    }
    AK_EPI;
}

/**
 * @author Dražen Bandić
 * @brief Checks if the attribute contains '|', and if it does it replaces it with "\|"
 * @return new attribute
 */
char* AK_check_attributes(char *attributes){
    AK_PRO;
    int n = strlen(attributes);

    if( attributes[n-1] == '\\' ){
        if( n == MAX_VARCHAR_LENGTH )
            attributes[n-1] = ' ';
        else
            attributes[n] = ' ';
    }

    char* result = AK_malloc(MAX_VARCHAR_LENGTH * sizeof(char));
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
    AK_EPI;
    return result;
}
