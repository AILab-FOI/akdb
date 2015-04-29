/**
@file nnull.c Provides functions for not null constaint
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

#include "nnull.h"

/**
 * @author Saša Vukšić, updated by Nenad Makar
 * @brief Function that sets null constraint on attribute
 * @param char* tableName name of table
 * @param char* constraintName name of constraint
 * @param char* attName name of attribute
 * @return No return value
 **/
void AK_set_constraint_not_null(char* tableName, char* constraintName, char* attName) {
    char systemTableName[50];
    char table[50];
    char attribute[50];
    int systemTableAddress;
    int j, i = 0;
    int AK_freeSpaceFound = 0;
    int tupleDictID = -1;
    AK_block *tempBlock;
    int itis = 1;
    int id;
    int skip = 0;
    AK_PRO;
    tempBlock = (AK_block *)AK_read_block(0);

    while (itis) {
        for (j = 0; j < 50; j++)
			systemTableName[j] = FREE_CHAR;

        memcpy(systemTableName, tempBlock->data + tempBlock->tuple_dict[i].address, tempBlock->tuple_dict[i].size);
        
        if (strcmp(systemTableName, "AK_constraints_not_null") == 0) {
			memcpy(&systemTableAddress, tempBlock->data + tempBlock->tuple_dict[i + 1].address, tempBlock->tuple_dict[i + 1].size);
            Ak_dbg_messg(HIGH, CONSTRAINTS, "System table to insert: %s, address: %i\n", systemTableName, systemTableAddress);
            tempBlock = (AK_block *)AK_read_block(systemTableAddress);

            while (AK_freeSpaceFound == 0 && skip == 0) {
                tupleDictID += 1;
		
				if (tempBlock->tuple_dict[tupleDictID].size == FREE_INT) {
							AK_freeSpaceFound = 1;
				}
				else
				{
					for (j = 0; j < 50; j++) {
						table[j] = FREE_CHAR;
						attribute[j] = FREE_CHAR;
					}			

					memcpy(table, tempBlock->data + tempBlock->tuple_dict[tupleDictID].address, tempBlock->tuple_dict[tupleDictID].size);
					
					if (strcmp(table, tableName) == 0) {
						memcpy(attribute, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 2].address, tempBlock->tuple_dict[tupleDictID + 2].size);
						if (strcmp(attribute, attName) == 0) {
							printf("\nNOT NULL constraint is already set on attribute %s of table %s!\n\n", attName, tableName);   
							skip = 1;
						}

					}
				}

			}

            if (AK_freeSpaceFound == 1) {
                id = AK_get_id();
                AK_insert_entry(tempBlock, TYPE_INT, &id, tupleDictID);
                AK_insert_entry(tempBlock, TYPE_VARCHAR, tableName, tupleDictID + 1);
                AK_insert_entry(tempBlock, TYPE_VARCHAR, constraintName, tupleDictID + 2);
                AK_insert_entry(tempBlock, TYPE_VARCHAR, attName, tupleDictID + 3);
                AK_write_block(tempBlock); 
		
				printf("\nNOT NULL constraint is now set on attribute %s of table %s.\n\n", attName, tableName); 
            } 
			else if (skip == 0){
				AK_EPI;
                exit (EXIT_ERROR);
            }
            itis = 0;
        }
        i++;
    }
    AK_free(tempBlock);
    AK_EPI;
}

/**
 * @author Saša Vukšić, updated by Nenad Makar
 * @brief Function checks if value is not null 
 * @param char* tableName name of table
 * @param char* newValue new value
 * @param char* attNamePar name of attribute
 * @return EXIT_ERROR or EXIT_SUCCESS
 **/

int AK_read_constraint_not_null(char* tableName, char newValue[], char* attNamePar) {
    char systemTableName[50];
    int systemTableAddress;
    int j, i = 0;
    AK_block *tempBlock;
    int itis = 1;
    int AK_freeSpaceFound = 0;
    int tupleDictID = -1;
    int flag = EXIT_SUCCESS;
    AK_PRO;
    tempBlock = (AK_block *)AK_read_block(0);

    while (itis) {
        for (j = 0; j < 50; j++)
            systemTableName[j] = FREE_CHAR;

        memcpy(systemTableName, tempBlock->data + tempBlock->tuple_dict[i].address, tempBlock->tuple_dict[i].size);
        
        if (strcmp(systemTableName, "AK_constraints_not_null") == 0) {
			memcpy(&systemTableAddress, tempBlock->data + tempBlock->tuple_dict[i + 1].address, tempBlock->tuple_dict[i + 1].size);
            Ak_dbg_messg(HIGH, CONSTRAINTS, "System table for reading: %s, address: %i\n", systemTableName, systemTableAddress);
            itis = 0;
        }
        i++;
    }
    tempBlock = (AK_block *)AK_read_block(systemTableAddress);

    char value[50];
    char att[50];

    while (AK_freeSpaceFound == 0) {
        tupleDictID += 1;
        if (tempBlock->tuple_dict[tupleDictID].size == FREE_INT) {
            AK_freeSpaceFound = 1;
        } 
		else {
			for (j = 0; j < 50; j++) {
                value[j] = FREE_CHAR;
                att[j] = FREE_CHAR;
            }

            memcpy(value, tempBlock->data + tempBlock->tuple_dict[tupleDictID].address, tempBlock->tuple_dict[tupleDictID].size);
            if (strcmp(value, tableName) == 0) {
                memcpy(att, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 2].address, tempBlock->tuple_dict[tupleDictID + 2].size);
                if (strcmp(att, attNamePar) == 0) {
                    if (strcmp(newValue, NULLL) == 0) {
                        flag = EXIT_ERROR;
                        printf("\nAttribute %s of table %s can't contain NULL!\n\n", att, value);
                    }

                } 
				else {
                    flag = EXIT_SUCCESS;
                }
            }
        }
    }
    AK_EPI;
    return flag;
}

/**
  * @author Saša Vukšić, updated by Nenad Makar
  * @brief Function for testing testing not null constraint
  * @return No return value
  */
void AK_null_test() {
    char* tableName = "studenti";
    char* constraintName = "studentiNotNull";
    char* attName = "ime";
    char* newValue = NULLL;
    AK_PRO;
    printf("\n\nTrying to set NOT NULL constraint on attribute %s of table %s...\n\n", attName, tableName);
    AK_set_constraint_not_null(tableName, constraintName, attName);
    printf("\nTrying to set NOT NULL constraint on same attribute of same table again...\n\n");
    AK_set_constraint_not_null(tableName, constraintName, attName);	
    printf("\nChecking if attribute %s of table %s can contain NULL sign...\n\n", attName, tableName);
    AK_read_constraint_not_null(tableName, newValue, attName);
    printf("\nTest succeeded.\nNOT NULL constraint is set on attribute %s of table %s.\n", attName, tableName);
    AK_EPI;
}