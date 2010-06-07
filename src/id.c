/**
@file between.c Provides functions for creating id of objects
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

#include "id.h"

int AK_get_id()
{
    char systemTableName[50];
    int systemTableAddress;
    int j, i = 0;
    int freeSpaceFound = 0;
    int tupleDictID = -1;
    KK_block *tempBlock;
    int itis = 1;
    int currValue;
    int nullID = 0;

    char* currentValue[50];

    tempBlock = KK_read_block(0);

    while(itis)
    {
        for(j=0; j<50; j++)
            systemTableName[j]= FREE_CHAR;

        memcpy(systemTableName, tempBlock->data + tempBlock->tuple_dict[i].address, tempBlock->tuple_dict[i].size);
        memcpy(&systemTableAddress, tempBlock->data + tempBlock->tuple_dict[i+1].address, tempBlock->tuple_dict[i+1].size);
        if(strcmp(systemTableName,"KK_sequence") == 0)
        {
            if(DEBUG)
                printf("Sistemska tablica u koju se upisuje: %s, adresa: %i\n",systemTableName,systemTableAddress);
                
            tempBlock = KK_read_block(systemTableAddress);

            while (freeSpaceFound == 0)
            {
                    tupleDictID += 1;
                    if (tempBlock->tuple_dict[tupleDictID].size == FREE_INT)
                            freeSpaceFound = 1;
            }
            if(tupleDictID < 1)
            {
                tupleDictID = 0;
                currValue = 0;
                KK_insert_entry(tempBlock, TYPE_INT, &nullID, tupleDictID);
                KK_insert_entry(tempBlock, TYPE_VARCHAR, "objectID", tupleDictID + 1);
                KK_insert_entry(tempBlock, TYPE_VARCHAR, "1", tupleDictID + 2);
                KK_insert_entry(tempBlock, TYPE_VARCHAR, "1", tupleDictID + 3);
                KK_insert_entry(tempBlock, TYPE_VARCHAR, "null", tupleDictID + 4);
                KK_insert_entry(tempBlock, TYPE_VARCHAR, "null", tupleDictID + 5);
                KK_insert_entry(tempBlock, TYPE_VARCHAR, "null" , tupleDictID + 6);

                KK_write_block(tempBlock);
            }
            else
            {
                tupleDictID = 0;
                tempBlock = KK_read_block(systemTableAddress);
                for(j=0; j<50; j++)
                    currentValue[j]= FREE_CHAR;
                memcpy(&currentValue, tempBlock->data + tempBlock->tuple_dict[2].address, tempBlock->tuple_dict[2].size);
                
                currValue = atoi(currentValue);
                currValue++;
                sprintf(currentValue,"%i",currValue);
                KK_insert_entry(tempBlock, TYPE_VARCHAR, currentValue, tupleDictID + 2);
                KK_write_block(tempBlock);

                //printf("Current value: %s\n",currentValue);
            }
            itis = 0;
        }
    i++;
    }

    free(tempBlock);

    return atoi(currentValue);
}