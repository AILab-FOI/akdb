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
//string restrictions must be in lowercase

/**
 * @author Mislav Jurinić
 * @brief Returns system tables address by name
 * @param _systemTableName table name
 * @return int
 **/
int AK_find_table_address(char* _systemTableName) {
    AK_block *tempBlock = (AK_block *) AK_read_block(0);
    int i = -1;
    int ret;
    int tuple_dict_size = sizeof(tempBlock->tuple_dict) / sizeof(AK_tuple_dict);
    char systemTableName[50];

    AK_PRO;

    while (++i + 1 <= tuple_dict_size) {
        memset(systemTableName, FREE_CHAR, sizeof(systemTableName));
        memmove(systemTableName, tempBlock->data + tempBlock->tuple_dict[i].address, strlen(tempBlock->data + tempBlock->tuple_dict[i].address));

        systemTableName[strlen(tempBlock->data + tempBlock->tuple_dict[i].address) - 1] = FREE_CHAR;

        if (strcmp(systemTableName, _systemTableName) == 0) {
            memmove(&ret, tempBlock->data + tempBlock->tuple_dict[i + 1].address, tempBlock->tuple_dict[i + 1].size);

            AK_EPI;

            return ret;
        }
    }

    AK_free(tempBlock);
    AK_EPI;

    return -1;
}

/**
 * @author Saša Vukšić, updated by Mislav Jurinić
 * @brief Function sets between constraints on particulary attribute, string constraint should be writen in lowercase.
      It searches for AK_free space. Then it inserts id, name of table, name of constraint, name of attribute, start and
       end value in temporary block.      
 * @param tableName table name
 * @param constraintName name of constraint
 * @param attName name of attribute
 * @param startValue initial constraint
 * @param endValue final constraint
 * @return No return value
 **/
void AK_set_constraint_between(char* tableName, char* constraintName, char* attName, char* startValue, char* endValue) {
    char* systemTableName = AK_CONSTRAINTS_BEWTEEN;

    int systemTableAddress = AK_find_table_address(systemTableName);
    int AK_freeSpaceFound = 0;
    int tupleDictID = -1;
    int id;

    AK_block *tempBlock;

    AK_PRO;

    if (systemTableAddress != -1) {
        Ak_dbg_messg(HIGH, CONSTRAINTS, "System table to insert: %s, address: %i\n", systemTableName, systemTableAddress);

        tempBlock = (AK_block *) AK_read_block(systemTableAddress);

        while (AK_freeSpaceFound == 0) {
            ++tupleDictID;

            if (tempBlock->tuple_dict[tupleDictID].size == FREE_INT) {
                AK_freeSpaceFound = 1;
            }
        }

        if (AK_freeSpaceFound == 1) {
            id = AK_get_table_id(systemTableName);

            AK_insert_entry(tempBlock, TYPE_INT, &id, tupleDictID);
            AK_insert_entry(tempBlock, TYPE_VARCHAR, tableName, tupleDictID + 1);
            AK_insert_entry(tempBlock, TYPE_VARCHAR, constraintName, tupleDictID + 2);
            AK_insert_entry(tempBlock, TYPE_VARCHAR, attName, tupleDictID + 3);
            AK_insert_entry(tempBlock, TYPE_VARCHAR, startValue, tupleDictID + 4);
            AK_insert_entry(tempBlock, TYPE_VARCHAR, endValue, tupleDictID + 5);

            AK_write_block(tempBlock);
        }
    }

    AK_free(tempBlock);

    AK_EPI;
}

/**
 * @author Saša Vukšić, updated by Mislav Jurinić
 * @brief Checks if the given value is between lower and upper bounds of the "between" constraint.
 * @param tableName table name
 * @param newValue value we want to insert
 * @param attNamePar attribute name
 * @return EXIT_SUCCESS or EXIT_ERROR
 **/
int AK_read_constraint_between(char* tableName, char* newValue, char* attNamePar) {
    char* systemTableName = AK_CONSTRAINTS_BEWTEEN;
    char attName[50] = {FREE_CHAR};
    char constraintName[50] = {FREE_CHAR};
    char inMemoryTable[50] = {FREE_CHAR};
    char lowerBoundary[50] = {FREE_CHAR};
    char upperBoundary[50] = {FREE_CHAR};

    int systemTableAddress = AK_find_table_address(systemTableName);
    int tupleDictID = -1;
    int flag = EXIT_SUCCESS;

    AK_block *tempBlock;

    AK_PRO;

    if (systemTableAddress != -1) {
        tempBlock = (AK_block *) AK_read_block(systemTableAddress);

        while (tempBlock->tuple_dict[++tupleDictID].size != FREE_INT) {
            // clear all vars  
            memset(attName, FREE_CHAR, sizeof(attName));
            memset(constraintName, FREE_CHAR, sizeof(constraintName));
            memset(inMemoryTable, FREE_CHAR, sizeof(inMemoryTable));
            memset(lowerBoundary, FREE_CHAR, sizeof(lowerBoundary));
            memset(upperBoundary, FREE_CHAR, sizeof(upperBoundary));

            memmove(inMemoryTable, tempBlock->data + tempBlock->tuple_dict[tupleDictID].address, tempBlock->tuple_dict[tupleDictID].size);
            inMemoryTable[strlen(tempBlock->data + tempBlock->tuple_dict[tupleDictID].address) - 1] = FREE_CHAR;

            if (strcmp(inMemoryTable, tableName) == 0) {                
                Ak_dbg_messg(HIGH, CONSTRAINTS, "--------------------------------\n");                
                Ak_dbg_messg(HIGH, CONSTRAINTS, "Table name: %s\n", tableName);

                memmove(attName, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 2].address, tempBlock->tuple_dict[tupleDictID + 2].size);
                Ak_dbg_messg(HIGH, CONSTRAINTS, "Attribute name: %s\n", attName);

                memmove(constraintName, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 1].address, tempBlock->tuple_dict[tupleDictID + 1].size);
                Ak_dbg_messg(HIGH, CONSTRAINTS, "Constraint name: %s\n", constraintName);

                memmove(lowerBoundary, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 3].address, tempBlock->tuple_dict[tupleDictID + 3].size);
                Ak_dbg_messg(HIGH, CONSTRAINTS, "Low boundary: %s\n", lowerBoundary);

                memmove(upperBoundary, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 4].address, tempBlock->tuple_dict[tupleDictID + 4].size);
                Ak_dbg_messg(HIGH, CONSTRAINTS, "High boundary: %s\n", upperBoundary);
                Ak_dbg_messg(HIGH, CONSTRAINTS, "--------------------------------\n");

                if (strcmp(attName, attNamePar) == 0) {
                    if (strcmp(newValue, lowerBoundary) >= 0 && strcmp(newValue, upperBoundary) <= 0) {
                        printf("\nConstraint succeeded!\n\n");
                    } 
                    else {
                        printf("\nOn table %s and attribute %s exists constraint %s\n<lowerBoundary have to be in range between %s and %s>\n\n", tableName, attName, constraintName, lowerBoundary, upperBoundary);
                        flag = EXIT_ERROR;
                    }
                }
            }
        }
    }

    AK_free(tempBlock);
    AK_EPI;

    return flag;
}

/**
 * @author Maja Vračan
 * @brief Function for printing tables
 * @param tableName name of table 
 */
void AK_print_constraints(char* tableName) {
    char* systemTableName = AK_CONSTRAINTS_BEWTEEN;
    char attName[50] = {FREE_CHAR};
    char constraintName[50] = {FREE_CHAR};
    char inMemoryTable[50] = {FREE_CHAR};
    char lowerBoundary[50] = {FREE_CHAR};
    char upperBoundary[50] = {FREE_CHAR};

    int systemTableAddress = AK_find_table_address(systemTableName);
    int tupleDictID = -1;
    int flag = EXIT_SUCCESS;

    AK_block *tempBlock;

    AK_PRO;

    printf("\n============= CONSTRAINTS LISTING =============\n");
    if (systemTableAddress != -1) {
        tempBlock = (AK_block *) AK_read_block(systemTableAddress);

        while (tempBlock->tuple_dict[++tupleDictID].size != FREE_INT) {
            // clear all vars  
            memset(attName, FREE_CHAR, sizeof(attName));
            memset(constraintName, FREE_CHAR, sizeof(constraintName));
            memset(inMemoryTable, FREE_CHAR, sizeof(inMemoryTable));
            memset(lowerBoundary, FREE_CHAR, sizeof(lowerBoundary));
            memset(upperBoundary, FREE_CHAR, sizeof(upperBoundary));

            memmove(inMemoryTable, tempBlock->data + tempBlock->tuple_dict[tupleDictID].address, tempBlock->tuple_dict[tupleDictID].size);
            inMemoryTable[strlen(tempBlock->data + tempBlock->tuple_dict[tupleDictID].address) - 1] = FREE_CHAR;

            if (strcmp(inMemoryTable, tableName) == 0) {                
                Ak_dbg_messg(HIGH, CONSTRAINTS, "--------------------------------\n");                
                Ak_dbg_messg(HIGH, CONSTRAINTS, "Table name: %s\n", tableName);

                memmove(attName, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 2].address, tempBlock->tuple_dict[tupleDictID + 2].size);
                Ak_dbg_messg(HIGH, CONSTRAINTS, "Attribute name: %s\n", attName);

                memmove(constraintName, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 1].address, tempBlock->tuple_dict[tupleDictID + 1].size);
                Ak_dbg_messg(HIGH, CONSTRAINTS, "Constraint name: %s\n", constraintName);

                memmove(lowerBoundary, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 3].address, tempBlock->tuple_dict[tupleDictID + 3].size);
                Ak_dbg_messg(HIGH, CONSTRAINTS, "Low boundary: %s\n", lowerBoundary);

                memmove(upperBoundary, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 4].address, tempBlock->tuple_dict[tupleDictID + 4].size);
                Ak_dbg_messg(HIGH, CONSTRAINTS, "High boundary: %s\n", upperBoundary);
                Ak_dbg_messg(HIGH, CONSTRAINTS, "--------------------------------\n");

                // DEBUG
                printf("\ninMemoryTable: %s\n", inMemoryTable);
                printf("Table name: %s\n", tableName);
                printf("Attribute name: %s\n", attName);
                printf("Constraint name: %s\n", constraintName);
                printf("Low boundary: %s\n", lowerBoundary);
                printf("High boundary: %s\n", upperBoundary);
            }
        }
    }

    AK_free(tempBlock);
    AK_EPI;
}

/**
 * @author Maja Vračan
 * @brief Function for deleting specific between constraint
 * @param tableName name of table on which constraint refers
 * @param attName name of attribute on which constraint is declared
 * @param constraintName name of constraint 
 * @return EXIT_SUCCESS when constraint is deleted, else EXIT_ERROR
 */
int AK_delete_constraint_between(char* tableName, char* constraintNamePar, char* attNamePar){
    char* systemTableName = AK_CONSTRAINTS_BEWTEEN;
    char attName[50] = {FREE_CHAR};
    char constraintName[50] = {FREE_CHAR};
    char inMemoryTable[50] = {FREE_CHAR};
    char lowerBoundary[50] = {FREE_CHAR};
    char upperBoundary[50] = {FREE_CHAR};

    int systemTableAddress = AK_find_table_address(systemTableName);
    int tupleDictID = -1;
    int flag = EXIT_FAILURE;

    AK_block *tempBlock;

    AK_PRO;

    if (systemTableAddress != -1) {
        tempBlock = (AK_block *) AK_read_block(systemTableAddress);

        while (tempBlock->tuple_dict[++tupleDictID].size != FREE_INT) {
            // clear all vars  
            memset(attName, FREE_CHAR, sizeof(attName));
            memset(constraintName, FREE_CHAR, sizeof(constraintName));
            memset(inMemoryTable, FREE_CHAR, sizeof(inMemoryTable));
            memset(lowerBoundary, FREE_CHAR, sizeof(lowerBoundary));
            memset(upperBoundary, FREE_CHAR, sizeof(upperBoundary));

            memmove(inMemoryTable, tempBlock->data + tempBlock->tuple_dict[tupleDictID].address, tempBlock->tuple_dict[tupleDictID].size);
            inMemoryTable[strlen(tempBlock->data + tempBlock->tuple_dict[tupleDictID].address) - 1] = FREE_CHAR;

            if (strcmp(inMemoryTable, tableName) == 0) {                
                memmove(attName, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 2].address, tempBlock->tuple_dict[tupleDictID + 2].size);
                memmove(constraintName, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 1].address, tempBlock->tuple_dict[tupleDictID + 1].size);
                memmove(lowerBoundary, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 3].address, tempBlock->tuple_dict[tupleDictID + 3].size);
                memmove(upperBoundary, tempBlock->data + tempBlock->tuple_dict[tupleDictID + 4].address, tempBlock->tuple_dict[tupleDictID + 4].size);
                
                if (strcmp(constraintName, constraintNamePar) == 0 && strcmp(attName, attNamePar) == 0) {
                    tempBlock->tuple_dict[tupleDictID].size = FREE_CHAR;
                    AK_write_block(tempBlock);
                    flag = EXIT_SUCCESS;
                }
            }
        }
    }

    AK_free(tempBlock);
    AK_EPI;

    return flag;
}

/**
  * @author Saša Vukšić, updated by Mislav Jurinić
  * @brief Tests the functionality of implemented between constraint.
  * @return No return value
  */
TestResult Ak_constraint_between_test() {
    // TEST #1
    char* tableName_1 = "department";
    char* attName_1 = "dep_name";
    char* constraintName_1 = "dep_name_between";
    char* newValue_1 = "Department of Zoo";
    char* startValue_1 = "Department of Economy";
    char* endValue_1 = "Department of Organization";
    //int test_status_1 = 0;
    int failed=0;
    int passed=0;
    AK_PRO;

    printf("============= Running Test #1 =============\n");

    AK_set_constraint_between(tableName_1, constraintName_1, attName_1, startValue_1, endValue_1);
    AK_print_constraints(tableName_1);
    
    if (AK_read_constraint_between(tableName_1, newValue_1, attName_1) == EXIT_ERROR) {
        printf("============= Test successful! ==============\n\n");
        passed++;
    } 
    else {
        printf("============\tTest failed.\t============\n\n");
        failed++;
    }
    

    // TEST #2
    char* tableName_2 = "department";
    char* attName_2 = "manager";
    char* constraintName_2 = "manager_surname_between";
    char* newValue_2 = "Kero";
    char* startValue_2 = "Hutinski";    
    char* endValue_2 = "Redep";
    //int test_status_2 = 0;

    AK_PRO;

    printf("\n============== Running Test #2 ==============\n");

    AK_set_constraint_between(tableName_2, constraintName_2, attName_2, startValue_2, endValue_2);
    AK_print_constraints(tableName_2);
    
    if (AK_read_constraint_between(tableName_2, newValue_2, attName_2) != EXIT_ERROR) {
        printf("============= Test successful! ==============\n\n");
        passed++;
    } 
    else {
        printf("============\tTest failed.\t============\n\n");
        failed++;
    }
    
      // delete test
    printf("\n============== Running Test #3 - Delete between constraint ==============\n");

    if(AK_delete_constraint_between(tableName_2, constraintName_2, attName_2) == EXIT_SUCCESS) {
        AK_print_constraints(tableName_2);
        printf("============= Test successful! ==============\n\n");
        passed++;
    } 
    else {
        AK_print_constraints(tableName_2);
        printf("============\tTest failed.\t============\n\n");
        failed++;
    }
    AK_EPI;
    return TEST_result(passed,failed);
}

/*
TEST #3
    char* tableName_3 = "student";
    char* attName_3 = "weight";
    char* constraintName_3 = "weight_between";
    char* newValue_3 = "80.751";
    char* startValue_3 = "80.750";    
    char* endValue_3 = "90.000";
    int test_status_3 = 0;

    AK_PRO;

    printf("\n============== Running Test #3 ==============\n");

    AK_set_constraint_between(tableName_3, constraintName_3, attName_3, startValue_3, endValue_3);
    
    if (AK_read_constraint_between(tableName_3, newValue_3, attName_3) != EXIT_ERROR) {
        printf("============= Test successful! ==============\n\n");
passed++;
    } 
    else {
        printf("============\tTest failed.\t============\n\n");
failed++;
    }
*/


