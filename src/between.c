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
void AK_set_constraint_between(char* tableName, char* constraintName, char* attName, char* startValue, char* endValue)
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
        if(strcmp(systemTableName,"AK_constraints_between") == 0)
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
                AK_insert_entry(tempBlock, TYPE_VARCHAR, tableName, tupleDictID + 1);
                AK_insert_entry(tempBlock, TYPE_VARCHAR, constraintName, tupleDictID + 2);
                AK_insert_entry(tempBlock, TYPE_VARCHAR, attName, tupleDictID + 3);
                AK_insert_entry(tempBlock, TYPE_VARCHAR, startValue, tupleDictID + 4);
                AK_insert_entry(tempBlock, TYPE_VARCHAR, endValue, tupleDictID + 5);

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


//ako je flag 0 onda nije dobro, ako je 1 onda je dobro
int AK_read_constraint_between(char* tableName, char* newValue, char* attNamePar)
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
        if(strcmp(systemTableName,"AK_constraints_between") == 0)
        {
            if(DEBUG)
                printf("Sistemska tablica iz koje se čita: %s,adresa: %i\n",systemTableName,systemTableAddress);
            itis = 0;
        }
        i++;
    }
    tempBlock = AK_read_block(systemTableAddress);


    char attName[50];
    char constraintName[50];
    char value[50];
    char valueF[50];
    char valueS[50];
    for(j=0; j<50; j++)
    {
            attName[j]= FREE_CHAR;
            constraintName[j] = FREE_CHAR;   
            value[j] = FREE_CHAR;
            valueF[j] = FREE_CHAR;
            valueS[j] = FREE_CHAR;
    }

    double startValue;
    double endValue;

    while (freeSpaceFound == 0)
    {
        tupleDictID += 1;        
        if (tempBlock->tuple_dict[tupleDictID].size == FREE_INT)
        {
                freeSpaceFound = 1;
        }else
        {
            for(j=0; j<50; j++)
                value[j]= FREE_CHAR;

            memcpy(value, tempBlock->data + tempBlock->tuple_dict[tupleDictID].address, tempBlock->tuple_dict[tupleDictID].size);
            if(strcmp(value,tableName) == 0 )
            {

                if(DEBUG)
                    printf("--------------------------------\n");
                   if(DEBUG)
                        printf("Naziv tablice: %s\n",tableName);
                    memcpy(attName, tempBlock->data + tempBlock->tuple_dict[tupleDictID+2].address, tempBlock->tuple_dict[tupleDictID+2].size);
                    if(DEBUG)
                    printf("Naziv atributa: %s\n",attName);
                    for(j=0; j<50; j++)
                        value[j]= FREE_CHAR;
                    memcpy(constraintName, tempBlock->data + tempBlock->tuple_dict[tupleDictID+1].address, tempBlock->tuple_dict[tupleDictID+1].size);
                    if(DEBUG)
                    printf("Naziv ograničenja: %s\n",constraintName);
                    memcpy(valueF, tempBlock->data + tempBlock->tuple_dict[tupleDictID+3].address, tempBlock->tuple_dict[tupleDictID+3].size);
                    startValue = atof(valueF);
                    for(j=0; j<50; j++)
                        value[j]= FREE_CHAR;
                    if(DEBUG)
                    printf("Donja granica: %f\n",startValue);
                    memcpy(valueS, tempBlock->data + tempBlock->tuple_dict[tupleDictID+4].address, tempBlock->tuple_dict[tupleDictID+4].size);
                    endValue = atof(valueS);
                    if(DEBUG)
                    printf("Gornja granica: %f\n",endValue);
                    if(DEBUG)
                    printf("--------------------------------\n");
                

                if(strcmp(attName,attNamePar) == 0)
                {
                    if((startValue != 0) && (endValue != 0))
                    {
                        if(atof(newValue) != 0)
                        {
                            if((atof(newValue) >= startValue) && (atof(newValue) <= endValue))
                            {
                         
                                    printf("Ograničenje zadovoljeno!!!\n");
                            }
                            else
                            {
                           
                                    printf("Nad tablicom %s i atributom %s postoji ograničenje %s\n vrijednosti moraju biti izmedju %f i %f\n",tableName,attName,constraintName,startValue,endValue);
                                flag = EXIT_ERROR;
                            }
                        }
                    }
                    else
                    {
                        if(atof(newValue) == 0)
                        {
                            //printf("ASCI:%i",valueF[0]);
                            if(((int)newValue[0] >= 97) && ((int)newValue[0] <= 122))
                            {
                                if(( (int)newValue[0] >= (int)valueF[0]) && ( (int)newValue[0] <= (int)valueS[0] ) )
                                {
                                 
                                    printf("Ograničenje zadovoljeno: %s\n",newValue);
                                }
                                else
                                {
                              
                                    printf("Nad tablicom %s i atributom %s postoji ograničenje %s\n vrijednosti moraju biti izmedju %s i %s\n",tableName,attName,constraintName,valueF,valueS);
                                    flag = EXIT_ERROR;
                                }
                            }
                            else if(((int)newValue[0] >= 65) && ((int)newValue[0] <= 90))
                            {
                                if(( (int)newValue[0] >= (int)valueF[0]-32) && ( (int)newValue[0] <= (int)valueS[0]-32 ) )
                                {
                                    printf("Ograničenje zadovoljeno: %s\n",newValue);
                                }
                                else
                                {
                              
                                    printf("Nad tablicom %s i atributom %s postoji ograničenje %s\n vrijednosti moraju biti izmedju %s i %s\n",tableName,attName,constraintName,valueF,valueS);
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