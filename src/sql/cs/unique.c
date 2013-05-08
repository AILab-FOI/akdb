
#include "unique.h"

/**
 * @author Domagoj Tuličić
 * @brief Function that sets unique constraint on attribute
 * @param char* tableName name of table
 * @param char* constraintName name of constraint
 * @param char* attName name of attribute
 * @return No return value
 **/


void Ak_set_constraint_unique(char* tableName, char* constraintName, char* attName){
    
    char systemTableName[50];
    int systemTableAddress;
    int j, i = 0;
    int freeSpaceFound = 0;
    int tupleDictID = -1;
    AK_block *tempBlock;
    int itis = 1;
    int id;

    tempBlock = (AK_block *)AK_read_block(0);

   // printf("%s\n",tempBlock->data + tempBlock->tuple_dict[30].address);


    while (itis) {
        for (j = 0; j < 50; j++)
            systemTableName[j] = FREE_CHAR;

        memcpy(systemTableName, tempBlock->data + tempBlock->tuple_dict[i].address, tempBlock->tuple_dict[i].size);
        memcpy(&systemTableAddress, tempBlock->data + tempBlock->tuple_dict[i + 1].address, tempBlock->tuple_dict[i + 1].size);
        if (strcmp(systemTableName, "AK_constraints_unique") == 0) {
            Ak_dbg_messg(HIGH, CONSTRAINTS, "System table to insert: %s, address: %i\n", systemTableName, systemTableAddress);

            tempBlock = (AK_block *)AK_read_block(systemTableAddress);

            while (freeSpaceFound == 0) {
                tupleDictID += 1;
                if (tempBlock->tuple_dict[tupleDictID].size == FREE_INT)
                    freeSpaceFound = 1;
            }

         //  printf("ddddddddddd %i",tupleDictID);
            //printf("%d\n",tempBlock->last_tuple_dict_id);
            //printf("%d\n",tempBlock->tuple_dict[2]);

            if (freeSpaceFound == 1) {
                id = AK_get_id();
                AK_insert_entry(tempBlock, TYPE_INT, &id, tupleDictID);
                AK_insert_entry(tempBlock, TYPE_VARCHAR, tableName, tupleDictID + 1);
                AK_insert_entry(tempBlock, TYPE_VARCHAR, constraintName, tupleDictID + 2);
                AK_insert_entry(tempBlock, TYPE_VARCHAR, attName, tupleDictID + 3);

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
    //printf("///////////////////%d\n", i);
   // printf("%d", tempBlock->address);
    free(tempBlock);
}

/**
 * @author Domagoj Tuličić
 * @brief Function reads if unique constraint exist
 * @param char* tableName name of table
 * @param char* newvalue new value
 * @param char* attNamePar name of attribute
 * @return EXIT_ERROR or EXIT_SUCCESS
 **/

int AK_read_constraint_unique(char* tableName, char *newvalue, char* attNamepar){

    char systemTableName[50];
    int systemTableAddress;
    int j, i = 0;
    AK_block *tempBlock;
    int freeSpaceFound = 0;
    int TupleDictID = -1;
    int flag = EXIT_SUCCESS;
    int itis = 1;

    char value[50];
    char attval[50];

    tempBlock = (AK_block*)AK_read_block(0);

    while(itis){

        for(j = 0;j < 50;j++)
            systemTableName[j] = FREE_CHAR;

                memcpy(systemTableName, tempBlock->data + tempBlock->tuple_dict[i].address, tempBlock->tuple_dict[i].size);
                memcpy(&systemTableAddress, tempBlock->data + tempBlock->tuple_dict[i + 1].address, tempBlock->tuple_dict[i + 1].size);
                if(strcmp(systemTableName,"AK_constraints_unique") == 0){
                    Ak_dbg_messg(HIGH, CONSTRAINTS, "System table for reading: %s, address: %i\n", systemTableName, systemTableAddress);
                    itis = 0;
                }
                i++;
    }

    tempBlock = (AK_block*)AK_read_block(systemTableAddress);
  

  

    ////////////////////////////////////////////////////////
     

    while (freeSpaceFound == 0) {
        TupleDictID += 1;
        if (tempBlock->tuple_dict[TupleDictID].size == FREE_INT) {
            freeSpaceFound = 1;
        } else {
            for (j = 0; j < 50; j++){
                value[j] = FREE_CHAR;
                attval[j] = FREE_CHAR;

            }

            memcpy(value, tempBlock->data + tempBlock->tuple_dict[TupleDictID].address, tempBlock->tuple_dict[TupleDictID].size);
            if (strcmp(value, tableName) == 0) {
                memcpy(attval, tempBlock->data + tempBlock->tuple_dict[TupleDictID + 2].address, tempBlock->tuple_dict[TupleDictID + 2].size);
                if(strcmp(attval, attNamepar) == 0){

                    if (strcmp(newvalue, "Unique") == 0) {
                        flag = EXIT_ERROR;
                        printf("Unique constraint");
                    }
                }
                else{
                    flag = EXIT_SUCCESS;
                }

             }
        }
    }

    free(tempBlock);
    return flag;
}
 

/**
  * @author Domagoj Tuličić
  * @brief Function for testing unique constraint
  * @return No return value
  */


void AK_unique_test() {
    char* tableName = "student";
    char* constraintName = "AK_constraints_unique";
    char* attName = "firstname";
    char* newValue = "Unique";

    Ak_set_constraint_unique(tableName, constraintName, attName);

    AK_read_constraint_unique(tableName, newValue, attName);

}