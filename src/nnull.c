/**
@file between.c Provides functions for not null constaint
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

void AK_set_constraint_not_null(char* tableName, char* constraintName, char* attName)
{
    char systemTableName[50];
    int systemTableAddress;
    int j, i = 0;
    int freeSpaceFound = 0;
    int tupleDictID = -1;
    AK_block *tempBlock;
    int itis = 1;
    int id;

    tempBlock = AK_read_block(0);

    while(itis)
    {
        for(j=0; j<50; j++)
            systemTableName[j]= FREE_CHAR;

        memcpy(systemTableName, tempBlock->data + tempBlock->tuple_dict[i].address, tempBlock->tuple_dict[i].size);
        memcpy(&systemTableAddress, tempBlock->data + tempBlock->tuple_dict[i+1].address, tempBlock->tuple_dict[i+1].size);
        if(strcmp(systemTableName,"AK_constraints_not_null") == 0)
        {
            if(DEBUG)
                printf("Sistemska tablica u koju se upisuje: %s,adresa: %i\n",systemTableName,systemTableAddress);

            tempBlock = AK_read_block(systemTableAddress);

            while (freeSpaceFound == 0)
            {
                    tupleDictID += 1;
                    if (tempBlock->tuple_dict[tupleDictID].size == FREE_INT)
                            freeSpaceFound = 1;
            }

            //printf("ddddddddddd %i",tupleDictID);

            if (freeSpaceFound == 1)
            {
                    id = AK_get_id();
                    AK_insert_entry(tempBlock, TYPE_INT, &id, tupleDictID);
                    AK_insert_entry(tempBlock, TYPE_VARCHAR, tableName, tupleDictID + 1 );
                    AK_insert_entry(tempBlock, TYPE_VARCHAR, constraintName, tupleDictID + 2);
                    AK_insert_entry(tempBlock, TYPE_VARCHAR, attName, tupleDictID + 3);

                    AK_write_block(tempBlock);

            }
            else
            {
                    if (DEBUG)
                            //printf("AK_init_new_segment__ERROR: Cannot initialize segment, no more space in last block!\n");
                    return EXIT_ERROR;
            }
            itis = 0;
        }
        i++;
    }
    free(tempBlock);
}


int AK_read_constraint_not_null(char* tableName, char* newValue, char* attNamePar)
{
    char systemTableName[50];
    int systemTableAddress;
    int j, i = 0;
    AK_block *tempBlock;
    int itis = 1;
    int freeSpaceFound = 0;
    int tupleDictID = -1;
    int flag = EXIT_SUCCESS;

    tempBlock = AK_read_block(0);

    while(itis)
    {
        for(j=0; j<50; j++)
            systemTableName[j]= FREE_CHAR;

        memcpy(systemTableName, tempBlock->data + tempBlock->tuple_dict[i].address, tempBlock->tuple_dict[i].size);
        memcpy(&systemTableAddress, tempBlock->data + tempBlock->tuple_dict[i+1].address, tempBlock->tuple_dict[i+1].size);
        if(strcmp(systemTableName,"AK_constraints_not_null") == 0)
        {
            if(DEBUG)
                printf("Sistemska tablica iz koje se čita: %s,adresa: %i\n",systemTableName,systemTableAddress);
            itis = 0;
        }
        i++;
    }
    tempBlock = AK_read_block(systemTableAddress);


    char value[50];
    char att[50];

    double nValue;

    while (freeSpaceFound == 0)
    {
        tupleDictID += 1;
        if (tempBlock->tuple_dict[tupleDictID].size == FREE_INT)
        {
                freeSpaceFound = 1;
        }else
        {
            for(j=0; j<50; j++)
            {
                value[j]= FREE_CHAR;
                att[j] = FREE_CHAR;
            }

            memcpy(value, tempBlock->data + tempBlock->tuple_dict[tupleDictID].address, tempBlock->tuple_dict[tupleDictID].size);            
            if(strcmp(value,tableName) == 0)
            {
                memcpy(att, tempBlock->data + tempBlock->tuple_dict[tupleDictID+2].address, tempBlock->tuple_dict[tupleDictID+2].size);          
                if(strcmp(att,attNamePar) == 0)
                {
                    if(is_number(newValue) == 0)
                    {
                        if(DEBUG)
                            printf("NIje broj\n");
                        if(strlen(newValue) == 0)
                        {
                            flag = EXIT_ERROR;
                            printf("Postoji null ograničenje nad atributm %s u tablici %s\n",att,value);
                        }
                    }
                    else
                    {
                        if(DEBUG)
                            printf("Broj je\n");
                        nValue = atof(newValue);
                        if(nValue == 0)
                        {
                          flag = EXIT_ERROR;
                          printf("Postoji null ograničenje nad atributm %s u tablici %s\n",att,value);
                        }
                    }
                }
                else
                   {
                        flag = EXIT_SUCCESS;
                   }
                }
            }
        }
    return flag;
}



int is_number(char* s)
{
    int i;
    for(i=0; i< strlen(s); i++)
    {
        if(isdigit(s[i]) == 0)
            return 0;
    }
    return 1;
}