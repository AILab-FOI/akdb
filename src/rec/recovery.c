/**
 @file recovery.c Provides recovery functions.
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

#include "recovery.h"


/** 
 * @author Dražen Bandić
 * @brief Gets line for line from the arhive log and calls functions to reverse the command
 * @param fileName - name of the archive log
 * @return no value
 */
 void AK_recover_archive_log(char* fileName){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("../src/rec/log.log", "r");

    if (fp == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        AK_recover_line(line);
    }
    
    fclose(fp);
}

/** 
 * @author Dražen Bandić
 * @brief Reverses the line which it gets
 * @param line - line/command from archive log
 * @return no value
 */
void AK_recover_line(char* line){
    char* table = AK_get_recovery_line_table(line);
    int n= AK_recovery_get_id(line);

    char c;
    do{
        if ( *line == ' '){
            line++;
            c = *line;
            break;
        }
        line++;
    } while ( *line != '\0' || index < MAX_VARCHAR_LENGTH);

    if(c == 'I'){
        Ak_delete_row_by_id(n, table);
    } else if(c == 'D'){
        AK_recovery_insert_row(table, line);
    }
}

/** 
 * @author Dražen Bandić
 * @brief Gets the table name from the command
 * @param command - command from archive log
 * @return table name
 */
char* AK_get_recovery_line_table(char* command){
    char* result = AK_malloc(MAX_VARCHAR_LENGTH * sizeof(char));
    int index = 0;

    do{
        if ( *command == ' '){
            break;
        } else {
            result[index++] = *command;
        }
        command++;
    } while ( *command != '\0' || index < MAX_VARCHAR_LENGTH);
    return result;
}

/** 
 * @author Dražen Bandić
 * @brief Gets the id of the row (from the command)
 * @param command - command from archive log
 * @return id of the row
 */
int AK_recovery_get_id(char *command){

    int length = strlen(command);

    int i = 0;
    int j= 0;
    char* result = (char*) AK_calloc(MAX_VARCHAR_LENGTH, sizeof(char));

    for(i = length-1, j = 0; i >= 0, j < length; i--, j++){
        if( command[i] == '|' ){
            break;
        }
        result[j] = command[i];
    }

    char* result2 = (char*) AK_calloc(MAX_VARCHAR_LENGTH, sizeof(char));

    length = strlen(result);

    for(i = length-1, j = 0; i >= 0, j < length; i--, j++){
        result2[j] = result[i];
    }

    int n = atoi(result2);

    return n;
}

/** 
 * @author Dražen Bandić
 * @brief Inserts a new row in table with attributes
 * @param table - table name to insert to
 * @param attributes - attribute to insert
 * @return no value
 */
void AK_recovery_insert_row(char* table, char* attributes){

    char* result = (char*) AK_calloc(MAX_VARCHAR_LENGTH, sizeof(char));
    int i = 0;
    int index = 0;
    do{
        if(*attributes == ' '){
            i = 1;
            attributes++;
        }
        if(i == 1){
            result[index++] = *attributes;
        }
        attributes++;
    } while ( *attributes != '\0' || index < MAX_VARCHAR_LENGTH);

    result[strlen(result)-1] = '\0';
    char** attr_value = AK_recovery_tokenize(result, "|", 1);

    char* table_attr_names = AK_rel_eq_get_atrributes_char(table);
    char** attr_name = AK_recovery_tokenize(table_attr_names, ";", 0);

    char* table_attr_types = AK_get_table_atribute_types(table);
    char** attr_types = AK_recovery_tokenize(table_attr_types, ";", 0);

    int type[MAX_ATTRIBUTES];
    for(i = 0; i < MAX_ATTRIBUTES; i++){
        if(attr_types[i] == NULL){
            break;
        }
        type[i] = atoi(attr_types[i]);
    }

    int n = i;

    insert_data_test(table, attr_name, attr_value, n, type);
}

/** 
 * @author Dražen Bandić
 * @brief Checks if the redolog_attribute contains "\|", and if it does it replaces it with '|'
 * @param attributes - attribute to check
 * @return new attribute
 */
char* AK_check_redolog_attributes(char* attributes){

    char* result = AK_malloc(MAX_VARCHAR_LENGTH * sizeof(char));
    int index = 0;

    do{
        if ( *attributes == '\\' && *(attributes++)  == '|' ){
            result[index++] = '|';
        } else {
            result[index++] = *attributes;
        }
        attributes++;
    } while ( *attributes != '\0' || index < MAX_VARCHAR_LENGTH);

    return result;
}

/** 
 * @author Dražen Bandić
 * @brief Tokenizes the input with the given delimiter and puts them in an double pointer structure (so we can execute an insert)
 * @param input - input to tokenize
 * @param delimiter - delimiter
 * @param valuesOrNot - 1 if the input are values, 0 otherwise
 * @return new double pointer structure with tokens
 */
char** AK_recovery_tokenize(const char* input, char* delimiter, int valuesOrNot){
    char* str = strdup(input);
    int count = 0;
    char** result = AK_malloc(MAX_ATTRIBUTES*sizeof(*result));

    char* tok=strtok(str, delimiter); 

    while(1){
        result[count++] = tok? strdup(tok) : tok;

        if (!tok) break;

        tok = strtok(NULL, delimiter);
    } 
    AK_free(str);

    int i = 0;
    int j = 0;

    char** values = AK_malloc(MAX_ATTRIBUTES*sizeof(*result));
    if(valuesOrNot == 1){
        for(i = count - 2, j = 0; i >= 0, j < count - 1; i--, j++){
            values[j] = result[i];
        }
        AK_free(result);
        return values;
    } else{
        return result;
    }
}
