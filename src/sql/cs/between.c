/**
@file between.c Provides functions for between constaint
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

#include "between.h"
//string ograničenja moraju biti malim slovima

/**
 * @author Saša Vukšić
 * @brief Function sets between constraints on particulary attribute, string constraint should be writen in lowercase.
	  It searches for free space. Then it inserts id, name of table, name of constraint, name of attribute, start and
	   end value in temporary block.	  
 * @param tableName table name
 * @param constraintName name of constraint
 * @param attName name of attribute
 * @param startValue initial constraint
 * @param endValue final constraint
   @return No return value
 **/
void AK_set_constraint_between(char* tableName, char* constraintName, char* attName, char* startValue, char* endValue) {
    char systemTableName[50];
    int systemTableAddress;
    int j, i = 0;
    int freeSpaceFound = 0;
    int tupleDictID = -1;
    AK_block *tempBlock;
    int itis = 1;
    int id;

    tempBlock = (AK_block *)AK_read_block(0);

    while (itis) {
        for (j = 0; j < 50; j++)
            systemTableName[j] = FREE_CHAR;

        memcpy(systemTableName, tempBlock->data + tempBlock->tuple_dict[i].address, tempBlock->tuple_dict[i].size);
        memcpy(&systemTableAddress, tempBlock->data + tempBlock->tuple_dict[i + 1].address, tempBlock->tuple_dict[i + 1].size);
        if (strcmp(systemTableName, "AK_constraints_between") == 0) {
            Ak_dbg_messg(HIGH, CONSTRAINTS, "System table to insert: %s, address: %i\n", systemTableName, systemTableAddress);

            tempBlock = (AK_block *)AK_read_block(systemTableAddress);

            while (freeSpaceFound == 0) {
                tupleDictID += 1;
                if (tempBlock->tuple_dict[tupleDictID].size == FREE_INT)
                    freeSpaceFound = 1;
            }

            //printf("ddddddddddd %i",tupleDictID);

            if (freeSpaceFound == 1) {
                id = AK_get_id();
                AK_insert_entry(tempBlock, TYPE_INT, &id, tupleDictID);
                AK_insert_entry(tempBlock, TYPE_VARCHAR, tableName, tupleDictID + 1);
                AK_insert_entry(tempBlock, TYPE_VARCHAR, constraintName, tupleDictID + 2);
                AK_insert_entry(tempBlock, TYPE_VARCHAR, attName, tupleDictID + 3);
                AK_insert_entry(tempBlock, TYPE_VARCHAR, startValue, tupleDictID + 4);
                AK_insert_entry(tempBlock, TYPE_VARCHAR, endValue, tupleDictID + 5);

                AK_write_block(tempBlock);

            } else {
                //if (DEBUG)
                    //printf("AK_init_new_segment__ERROR: Cannot initialize segment, no more space in last block!\n");
                    exit (EXIT_ERROR);
            }
            itis = 0;
        }
        i++;
    }
    free(tempBlock);
}


//ako je flag 0 onda nije dobro, ako je 1 onda je dobro

/**
 * @author Saša Vukšić
 * @brief Function checks if this value is between constraints. First it gets table name, attribute name, constraint name,
	  low boundary and high boundary from temporary block and than checks between constraint.
 * @param tableName name of table
 * @param newValue new value
 * @param attNamePar name of attribute
 * @return EXIT_ERROR or EXIT_SUCCESS
 **/
int AK_read_constraint_between(char* tableName, char* newValue, char* attNamePar) {
    char systemTableName[50];
    int systemTableAddress;
    int j, i = 0;
    AK_block *tempBlock;
    int itis = 1;
    int freeSpaceFound = 0;
    int tupleDictID = -1;
    int flag = EXIT_SUCCESS;

    tempBlock = (AK_block *)AK_read_block(0);

    while (itis) {
        for (j = 0; j < 50; j++)
            systemTableName[j] = FREE_CHAR;

        memcpy(systemTableName, tempBlock->data + tempBlock->tuple_dict[i].address, tempBlock->tuple_dict[i].size);
        memcpy(&systemTableAddress, tempBlock->data + tempBlock->tuple_dict[i + 1].address, tempBlock->tuple_dict[i + 1].size);
        if (strcmp(systemTableName, "AK_constraints_between") == 0) {
            Ak_dbg_messg(HIGH, CONSTRAINTS, "System table for reading: %s, address: %i\n", systemTableName, systemTableAddress);
            itis = 0;
        }
        i++;
    }
    tempBlock = (AK_block *)AK_read_block(systemTableAddress);


    char attName[50];
    char constraintName[50];
    char value[50];
    char valueF[50];
    char valueS[50];
    for (j = 0; j < 50; j++) {
        attName[j] = FREE_CHAR;
        constraintName[j] = FREE_CHAR;
        value[j] = FREE_CHAR;
        valueF[j] = FREE_CHAR;
        valueS[j] = FREE_CHAR;
    }

    double startValue;
    double endValue;

    while (freeSpaceFound == 0) {
        tupleDictID += 1;
        if (tempBlock->tuple_dict[tupleDictID].size == FREE_INT) {
            freeSpaceFound = 1;
        } else {
            for (j = 0; j < 50; j++)
                value[j] = FREE_CHAR;

            memcpy(value, tempBlock->data + tempBlock->tuple_dict[tupleDictID].address, tempBlock->tuple_dict[tupleDictID].size);
            if (strcmp(value, tableName) == 0) {

                Ak_dbg_messg(HIGH, CONSTRAINTS, "--------------------------------\n");
                Ak_dbg_messg(HIGH, CONSTRAINTS, "Table name: %s\n", tableName);
                memcpy(attName, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 2].address, tempBlock->tuple_dict[tupleDictID + 2].size);
                Ak_dbg_messg(HIGH, CONSTRAINTS, "Attribute name: %s\n", attName);
                for (j = 0; j < 50; j++)
                    value[j] = FREE_CHAR;
                memcpy(constraintName, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 1].address, tempBlock->tuple_dict[tupleDictID + 1].size);
                Ak_dbg_messg(HIGH, CONSTRAINTS, "Constraint name: %s\n", constraintName);
                memcpy(valueF, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 3].address, tempBlock->tuple_dict[tupleDictID + 3].size);
                startValue = atof(valueF);
                for (j = 0; j < 50; j++)
                    value[j] = FREE_CHAR;
                Ak_dbg_messg(HIGH, CONSTRAINTS, "Low boundary: %f\n", startValue);
                memcpy(valueS, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 4].address, tempBlock->tuple_dict[tupleDictID + 4].size);
                endValue = atof(valueS);
				Ak_dbg_messg(HIGH, CONSTRAINTS, "High boundary: %f\n", endValue);
                Ak_dbg_messg(HIGH, CONSTRAINTS, "--------------------------------\n");

                if (strcmp(attName, attNamePar) == 0) {
                    if ((startValue != 0) && (endValue != 0)) {
                        if (atof(newValue) != 0) {
                            if ((atof(newValue) >= startValue) && (atof(newValue) <= endValue)) {
                                printf("Constraint successed!!!\n");
                            } else {
                                printf("On table %s and attribute %s exists constraint %s\n <values have to be in range between %f and %f>\n", tableName, attName, constraintName, startValue, endValue);
                                flag = EXIT_ERROR;
                            }
                        }
                    } else {
                        if (atof(newValue) == 0) {
                            //printf("ASCI:%i",valueF[0]);
                            if (((int) newValue[0] >= 97) && ((int) newValue[0] <= 122)) {
                                if (((int) newValue[0] >= (int) valueF[0]) && ((int) newValue[0] <= (int) valueS[0])) {
                                    printf("Constraint successed: %s\n", newValue);
                                } else {
                                    printf("On table %s and attribute %s exists constraint %s\n <values have to be in range between %s and %s>\n", tableName, attName, constraintName, valueF, valueS);
                                    flag = EXIT_ERROR;
                                }
                            } else if (((int) newValue[0] >= 65) && ((int) newValue[0] <= 90)) {
                                if (((int) newValue[0] >= (int) valueF[0] - 32) && ((int) newValue[0] <= (int) valueS[0] - 32)) {
                                    printf("Constraint successed: %s\n", newValue);
                                } else {

                                    printf("On table %s and attribute %s exists constraint %s\n <values have to be in range between %s and %s>\n", tableName, attName, constraintName, valueF, valueS);
                                    flag = EXIT_ERROR;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    free(tempBlock);
    return flag;
}
/**
  * @author Saša Vukšić
  * @brief Function that tests the functionality of implemented between constrain
  * @return No return value
*/

void Ak_constraint_between_test() {
    char* tableName = "studenti";
    char* attName = "ime";
    char* constraintName = "imeBetween";
    char* newValue = "Krunoslav";
    char* startValue = "ivica";
    char* endValue = "marica";

    AK_set_constraint_between(tableName, constraintName, attName, startValue, endValue);
    AK_read_constraint_between(tableName, newValue, attName);

    tableName = "studenti";
    attName = "godine";
    constraintName = "godineBetween";
    newValue = "5";
    startValue = "18";
    endValue = "27";

    AK_set_constraint_between(tableName, constraintName, attName, startValue, endValue);
    AK_read_constraint_between(tableName, newValue, attName);
}
