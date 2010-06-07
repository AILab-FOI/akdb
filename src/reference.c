/**
@file reference.c Provides functions for referential integrity
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
 17 */

#include "reference.h"


/*
    AK_header t_header[7] = {
                {TYPE_VARCHAR, "table", 0, '\0', '\0',},
		{TYPE_VARCHAR, "constraint", 0, '\0', '\0',},
		{TYPE_VARCHAR, "attribute", 0, '\0', '\0',},
		{TYPE_VARCHAR, "parent", 0, '\0', '\0',},
                {TYPE_VARCHAR, "parent_attribute", 0, '\0', '\0',},
		{TYPE_INT, "type", 0, '\0', '\0',},
		{0, '\0', 0, '\0', '\0'}
        };
*/

int AK_add_reference(char *childTable, char *childAttNames[], char *parentTable, char *parentAttNames[], int attNum, char *constraintName, int type) {
    /*char systemTableName[MAX_ATT_NAME];
    int systemTableAddress;
    int j, i = 0;
    int freeSpaceFound = 0;
    int tupleDictID = -1;
    //AK_block *tempBlock;
    AK_mem_block *tempBlock;*/

    int i;
    if (type != REF_TYPE_CASCADE && type != REF_TYPE_NO_ACTION && type!=REF_TYPE_RESTRICT && type!=REF_TYPE_SET_DEFAULT && type != REF_TYPE_SET_NULL) return;

    element row_root =  (element) malloc( sizeof(list) );
    InitializeList(row_root);

    for (i = 0;i<attNum;i++) {
        DeleteAllElements(row_root);
        InsertNewElement( TYPE_VARCHAR, childTable, "AK_reference", "table", row_root );
        InsertNewElement( TYPE_VARCHAR, constraintName, "AK_reference", "constraint", row_root );
        InsertNewElement( TYPE_VARCHAR, childAttNames[i], "AK_reference", "attribute", row_root );
        InsertNewElement( TYPE_VARCHAR, parentTable, "AK_reference", "parent", row_root );
        InsertNewElement( TYPE_VARCHAR, parentAttNames[i], "AK_reference", "parent_attribute", row_root );
        InsertNewElement( TYPE_INT, &type, "AK_reference", "type", row_root );
        insert_row( row_root );
    }

   /* tempBlock = AK_read_block(0);

    do
    {
   //     for(j=0; j<50; j++)
   //         systemTableName[j]= FREE_CHAR;

        memcpy(systemTableName, tempBlock->data + tempBlock->tuple_dict[i].address, tempBlock->tuple_dict[i].size);
        systemTableName[tempBlock->tuple_dict[i].size+1] = FREE_CHAR;
        memcpy(&systemTableAddress, tempBlock->data + tempBlock->tuple_dict[i+1].address, tempBlock->tuple_dict[i+1].size);
        if(strcmp(systemTableName,"AK_references") == 0)
            break;
        i+= 2;
    } while(strcmp(systemTableName,"\0") != 0);

    {
        if(DEBUG)
            printf("Sistemska tablica u koju se upisuje: %s,adresa: %i\n",systemTableName,systemTableAddress);

        //tempBlock = AK_read_block(systemTableAddress);
        tempBlock = AK_get_block(systemTableAddress);

        while (freeSpaceFound == 0)
        {
                tupleDictID += 1;
                if (tempBlock->tuple_dict[tupleDictID].size == FREE_INT)
                        freeSpaceFound = 1;
        }*/

 /*   AK_insert_entry(tempBlock, TYPE_VARCHAR, tableName, tupleDictID);
    AK_insert_entry(tempBlock, TYPE_VARCHAR, constraintName, tupleDictID + 1);
    AK_insert_entry(tempBlock, TYPE_VARCHAR, attName, tupleDictID + 2);
    AK_insert_entry(tempBlock, TYPE_VARCHAR, startValue, tupleDictID + 3);
    AK_insert_entry(tempBlock, TYPE_VARCHAR, endValue, tupleDictID + 4);
*/
//                AK_write_block(tempBlock);

    
  //  free(tempBlock);
    return 1;
}

AK_ref_item AK_check_reference_integrity(char *tableName, char *constraintName) {
    int i = 0;
    AK_list *list;

    AK_ref_item a;

    return a;
}

void reference_test() {
    printf("HELL YEAH!\n");

    char *att[2];
    att[0] = malloc(sizeof(char)*20);
    strcpy(att[0],"att");
    att[1] = malloc(sizeof(char)*20);
    strcpy(att[1],"att1");

    char *patt[2];
    patt[0] = malloc(sizeof(char)*20);
    strcpy(patt[0],"patt");
    patt[1] = malloc(sizeof(char)*20);
    strcpy(patt[1],"patt1");

    //AK_create_reference_table();
    AK_add_reference("child",att,"parent",patt,2,"constraint",1);
    AK_print_table("AK_reference");

}