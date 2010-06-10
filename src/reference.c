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

AK_ref_item AK_get_reference(char *tableName, char *constraintName) {
    int i = 0;
    AK_list *list;
    AK_ref_item reference;

    reference.attributes_number = 0;

    while ((list = AK_get_row(i,"AK_reference")) != NULL ) {
        if (strcmp(list->next->data,tableName) == 0 &&
                strcmp(list->next->next->data,constraintName)==0) {
            strcpy(reference.table, tableName);
            strcpy(reference.constraint, constraintName);
            strcpy(reference.attributes[reference.attributes_number], list->next->next->next->data);
            strcpy(reference.parent, list->next->next->next->next->data);
            strcpy(reference.parent_attributes[reference.attributes_number], list->next->next->next->next->next->data);
            memcpy(&reference.type, list->next->next->next->next->next->next->data, sizeof(int));
            reference.attributes_number++;
        }
        i++;
    }
    return reference;
}

int AK_reference_check_attribute(char *tableName, char *attribute, char *value) {
    int i;
    int att_index;
    AK_list *list_row, *list_col;
    while ((list_row = AK_get_row(i,"AK_reference")) != NULL ) {
        if (strcmp(list_row->next->data,tableName) == 0 &&
                strcmp(list_row->next->next->next->data,attribute) == 0) {
            att_index = AK_get_attr_index(list_row->next->next->next->next->data, list_row->next->next->next->next->next->data);
            list_col = AK_get_column(att_index, list_row->next->next->next->next->data);
            while (strcmp(list_col->data,value) != 0) {
                list_col = list_col->next;
                if (list_col == NULL)
                    return EXIT_ERROR;
            }
        }
        i++;
    }
    return EXIT_SUCCESS;
}

int AK_reference_check_if_update_needed(list *lista, int delete) {
    element temp;
    int i = 0,j,con_num=0;
    AK_list *row;

     while ((row = AK_get_row(i,"AK_reference")) != NULL ) {
        if (strcmp(row->next->next->next->next->data, lista->next->table) == 0) {
            temp = GetFirstElement(lista);
            while (temp != NULL) {
                if (delete == 0 && temp->constraint == 0 && strcmp(row->next->next->next->next->next->data, temp->attribute_name) == 0)
                    return EXIT_SUCCESS;
                else if (delete == 1 && strcmp(row->next->next->next->next->next->data, temp->attribute_name) == 0)
                    return EXIT_SUCCESS;
                temp = GetNextElement(temp);
            }
        }
        i++;
     }

    return EXIT_ERROR;
}

int AK_reference_check_restricion(list *lista, int delete) {
    element temp;
    int i = 0,j,con_num=0;
    AK_list *row;

     while ((row = AK_get_row(i,"AK_reference")) != NULL ) {
        if (strcmp(row->next->next->next->next->data, lista->next->table) == 0) {
            temp = GetFirstElement(lista);
            while (temp != NULL) {
                if (delete == 0 && temp->constraint == 0 && strcmp(row->next->next->next->next->next->data, temp->attribute_name) == 0 && (int)*row->next->next->next->next->next->next->data == REF_TYPE_RESTRICT)
                    return EXIT_ERROR;
                else if (delete == 1 && strcmp(row->next->next->next->next->next->data, temp->attribute_name) == 0 && (int)*row->next->next->next->next->next->next->data == REF_TYPE_RESTRICT)
                    return EXIT_ERROR;
                temp = GetNextElement(temp);
            }
        }
        i++;
     }

    return EXIT_SUCCESS;
}

int AK_get_type_size(int type, char data[]) {
    switch (type) {
        case TYPE_INT:
            return sizeof(int);
        case TYPE_FLOAT:
            return sizeof(float);
        case TYPE_NUMBER:
            return sizeof(double);
        case TYPE_VARCHAR:
            return strlen(data);
        case TYPE_DATE:
            //?
        case TYPE_DATETIME:
            //?
        case TYPE_TIME:
            //?
        case TYPE_BLOB:
            //?
        case TYPE_BOOL:
            return sizeof(int);
    }
}

int AK_reference_update(list *lista, int delete) {
    int parent_i,i,j,ref_i,con_num =0;
    AK_list *parent_row;
    AK_list *ref_row;
    element temp;
    AK_list_elem tempcell;
    AK_ref_item reference;
    char constraints[MAX_CHILD_CONSTRAINTS][MAX_VARCHAR_LENGHT];
    char child_tables[MAX_CHILD_CONSTRAINTS][MAX_VARCHAR_LENGHT];

    element row_root =  (element) malloc( sizeof(list) );
    InitializeList(row_root);

    while ((ref_row = AK_get_row(ref_i,"AK_reference")) != NULL ) {
        if (strcmp(ref_row->next->next->next->next->data,lista->next->table) == 0) { // we're searching for PARENT table here
            for (j=0;j<con_num;j++) {
                if (strcmp(constraints[j],ref_row->next->next->data) == 0 && strcmp(child_tables[j], ref_row->next->data)==0) {
                    break;
                }
            }
            if (j==con_num) {
                strcpy(constraints[con_num],ref_row->next->next->data);
                strcpy(child_tables[con_num],ref_row->next->data);
                con_num++;
            }
        }
        ref_i++;
     }

    AK_list expr;
    InitL( &expr );

    // selecting only affected rows in parent table..
    i=0;
    temp = GetFirstElement(lista);
    while (temp != NULL) {
        if (delete == 1 || temp->constraint == 0) {
            InsertAtEndL(TYPE_OPERAND, temp->attribute_name, strlen(temp->attribute_name), &expr);
            InsertAtEndL(temp->type, temp->data, AK_get_type_size(temp->type, temp->data), &expr);
            InsertAtEndL(TYPE_OPERATOR, "=", 1, &expr);
            i++;
        }
        temp = GetNextElement(temp);
    }
    for (j=0;j<i-1;j++) {
        InsertAtEndL(TYPE_OPERAND, "AND", 3, &expr);
    }
    char tempTable[MAX_ATT_NAME];
    sprintf(tempTable, "ref_tmp_%s", lista->next->table);
    //AK_selection( lista->next->table, "ref_update_temp", &expr );
    AK_selection( lista->next->table, tempTable, &expr );
    DeleteAllL(&expr);
   
    
    // browsing through affected rows..
    //while ((parent_row = AK_get_row(parent_i,"ref_update_temp")) != NULL) { // walkin' thgough the parent table
    while ((parent_row = AK_get_row(parent_i,tempTable)) != NULL) {
       // while ((ref_row = AK_get_row(ref_i,"AK_reference")) != NULL ) { // looking for the parent table in references...
         //   if (strcmp(ref_row->next->next->next->next->data, lista->next->table) == 0) {
        for (i = 0;i < con_num;i++) {
            reference = AK_get_reference(child_tables[i],constraints[i]);
            DeleteAllL(&expr);

            for (j = 0;j<reference.attributes_number;j++) {
                tempcell = GetNthL(AK_get_attr_index(reference.parent, reference.parent_attributes[j]), parent_row); // from the row of parent table, take the value of attribute with name from parent_attribute
                InsertNewElementForUpdate(tempcell->type, tempcell->data , reference.table, reference.attributes[j], row_root,1);

                switch (reference.type) {
                    case REF_TYPE_CASCADE:
                        if (delete == 0) {
                            temp = GetFirstElement(lista);
                            while (temp != NULL) {
                                if (strcmp(temp->attribute_name,reference.parent_attributes[j]) == 0 && temp->constraint == 0) {
                                    InsertNewElementForUpdate(tempcell->type, temp->data, reference.table, reference.attributes[j], row_root,0);
                                    break;
                                }
                                temp = GetNextElement(temp);
                            }
                        }
                        break;
                    case REF_TYPE_NO_ACTION:
                        break;
                   // case REF_TYPE_RESTRICT: // handled by function AK_reference_check_restrict
                    case REF_TYPE_SET_DEFAULT: // default values are currently unsupported by this DBMS... reserved for future use
                        break;
                    case REF_TYPE_SET_NULL:
                        if (delete == 1) {
                            InsertNewElementForUpdate(0, "", reference.table, reference.attributes[j], row_root,0);
                        }
                        else {
                            temp = GetFirstElement(lista);
                            while (temp != NULL) {
                                if (strcmp(temp->attribute_name,reference.parent_attributes[j]) == 0 && temp->constraint == 0) {
                                    InsertNewElementForUpdate(0, "", reference.table, reference.attributes[j], row_root,0);
                                    break;
                                }
                                temp = GetNextElement(temp);
                            }
                        }
                        break;
                }
            }
            
            //InsertNewElementForUpdate(tempcell->type, "" ,ref_row->next->data, ref_row->next->next->next->data, row_root,0);
            if (delete == 0 || reference.type == REF_TYPE_SET_NULL)
                update_row(row_root);
            else
                delete_row(row_root);
            

              /*  InsertAtEndL(TYPE_OPERAND, ref_row->next->next->next->next->next->data, ref_row->next->next->next->next->next->size, &expr);
                InsertAtEndL(ref_row->next->next->next->next->next->type, tempcell, AK_get_type_size(ref_row->next->next->next->next->next->type, tempcell), &expr);
                InsertAtEndL(TYPE_OPERATOR, "=", 1, &expr);
                AK_selection(ref_row->next->data , "ref_update_temp_2", &expr );
                DeleteAllL(&expr);

                child_head = AK_get_header("ref_update_temp_2");
                child_att_num = AK_num_attr("ref_update_temp_2");

                while ((child_row = AK_get_row(child_i,"ref_update_temp_2")) != NULL) {
                    while (child_head.)

                    tempcell2 = FirstL(child_row);
                    DeleteAllElements(row_root);
                    while (tempcell2 != NULL) {

                        InsertNewElementForUpdate(tempcell2->type, tempcell2->data ,"ref_update_temp_2", , row_root,1);
                        update_row(row_root);

                        tempcell2 = NextL(tempcell2, child_row);
                    }
                }

                AK_delete_table("ref_update_temp_2");*/
        }
       //     }
      //      ref_i++;
     //   }
        
        parent_i++;
    }

    //AK_delete_table("ref_update_temp");
    //AK_delete_table(tempTable);
    return EXIT_SUCCESS;
}

int AK_reference_check_entry(list *lista) {
    element temp;
    int i = 0,j,k,con_num=0,success;
    char constraints[10][MAX_VARCHAR_LENGHT]; // this 10 should probably be a constant... how many foreign keys can one table have..
    char attributes[MAX_REFERENCE_ATTRIBUTES][MAX_ATT_NAME];
    int is_att_null[MAX_REFERENCE_ATTRIBUTES]; //this is a workaround... when proper null value implementation is in place, this should be solved differently
    AK_list *row;
    AK_ref_item reference;

    AK_list_elem temp1;

    temp = GetFirstElement(lista);
    while (temp != NULL) {
        if (temp->constraint == 1)
            return EXIT_SUCCESS;
        temp = GetNextElement(temp);
    }

     while ((row = AK_get_row(i,"AK_reference")) != NULL ) {
        if (strcmp(row->next->data,lista->next->table) == 0) {
            for (j=0;j<con_num;j++) {
                if (strcmp(constraints[j],row->next->next->data) == 0) {
                    break;
                }
            }
            if (j==con_num) {
                //constraints[con_num] = malloc(sizeof(char)*MAX_VARCHAR_LENGHT);
                strcpy(constraints[con_num],row->next->next->data);
                con_num++;
            }
        }
        i++;
     }

    if (con_num == 0)
        return EXIT_SUCCESS;

    for (i = 0; i< con_num;i++) { // reference
        reference = AK_get_reference(lista->next->table,constraints[i]);

        // fetching relevant attributes from entry list...
       // attributes = malloc(sizeof(char)*MAX_VARCHAR_LENGHT*reference.attributes_number);
        for (j = 0;j < reference.attributes_number; j++) {
            temp = lista->next;
            while (temp != NULL) {

                if (temp->constraint == 0 && strcmp(temp->attribute_name,reference.attributes[j]) == 0) {
                    strcpy(attributes[j],temp->data);
                    if (reference.type == REF_TYPE_SET_NULL && strcmp(temp->data,"\0") == 0) //if type is 0, the value is PROBABLY null
                        is_att_null[j] = 1;
                    else
                        is_att_null[j] = 0;
                    break;
                }
                temp = GetNextElement(temp);
            }
        }

        if (reference.attributes_number == 1) {
            if (AK_reference_check_attribute(reference.table, reference.attributes[0], attributes[0]) == EXIT_ERROR) {
                return EXIT_ERROR;
            }
            else continue;
        }


        j = 0;
        while ((row = AK_get_row(j,reference.parent)) != NULL ) { // rows in parent table
            success = 1;
            for (k=0;k<reference.attributes_number;k++) { // attributes in reference
                temp1 = GetNthL(AK_get_attr_index(reference.parent,reference.parent_attributes[k]),row);
                if (is_att_null[k] || strcmp(temp1->data,attributes[k]) != 0) {
                    success = 0;
                    break;
                }
            }
            if (success == 1) {
                //free(attributes);
                return EXIT_SUCCESS;
            }
            j++;
        }
       // free(attributes);
    }

    return EXIT_ERROR;
}

void reference_test_table() {
    int i;
    //create header
    AK_header t_header[ MAX_ATTRIBUTES ];
    AK_header* temp;

    temp = (AK_header*)AK_create_header( "mbr", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header, temp, sizeof( AK_header ));

    temp = (AK_header*)AK_create_header( "firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header + 1, temp, sizeof( AK_header ));

    temp = (AK_header*)AK_create_header( "lastname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header + 2, temp, sizeof( AK_header ));

    temp = (AK_header*)AK_create_header( "year", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header + 3, temp, sizeof( AK_header ));

    temp = (AK_header*)AK_create_header( "tezina", TYPE_FLOAT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header + 4, temp, sizeof( AK_header ));

    for( i = 5; i < MAX_ATTRIBUTES; i++ )
    {
	memcpy( t_header + i, "\0", sizeof( "\0" ));
    }

    //create table
    char *tblName = "student";

    printf("op_selection_test: Before segment initialization: %d\n", AK_num_attr( tblName ) );

    int startAddress = AK_initialize_new_segment( tblName, SEGMENT_TYPE_TABLE, t_header);

    if( startAddress != EXIT_ERROR )
        printf( "\nTABLE %s CREATED!\n", tblName );


    printf("op_selection_test: After segment initialization: %d\n", AK_num_attr( tblName ) );

    element row_root =  (element) malloc( sizeof(list) );
    InitializeList(row_root);

    int mbr = 100, year = 1999;
    float weight = 80.00;

    //insert rows in table student
    mbr++; year++; weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Matija", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Sestak", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Igor", tblName, "firstname", row_root);
    InsertNewElement( TYPE_VARCHAR, "Mesaric", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Master", tblName, "firstname", row_root);
    InsertNewElement( TYPE_VARCHAR, "Slunjski", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Dino", tblName, "firstname", row_root);
    InsertNewElement( TYPE_VARCHAR, "Alagic", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Igor", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Netkic", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Josip", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Vincek", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Igor", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Netkic", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );


    AK_header t_header2[4] = {
                {TYPE_INT, "FK", 0, '\0', '\0',},
		{TYPE_VARCHAR, "Value", 0, '\0', '\0',},
                {TYPE_VARCHAR, "Rnd", 0, '\0', '\0',},
		{0, '\0', 0, '\0', '\0'}
        };

    int startAddress1 = AK_initialize_new_segment( "ref_test", SEGMENT_TYPE_TABLE, t_header2);

    if( startAddress1 != EXIT_ERROR )
        printf( "\nTABLE %s CREATED!\n", "ref_test" );

}

void reference_test() {
    printf("HELL YEAH!\n");
    int a;
    reference_test_table();

    char *att[2];
    att[0] = malloc(sizeof(char)*20);
    strcpy(att[0],"FK");
    att[1] = malloc(sizeof(char)*20);
    strcpy(att[1],"Value");


    char *patt[2];
    patt[0] = malloc(sizeof(char)*20);
    strcpy(patt[0],"mbr");
    patt[1] = malloc(sizeof(char)*20);
    strcpy(patt[1],"firstname");

    //AK_create_reference_table();
    AK_add_reference("ref_test",att,"student",patt,2,"constraint", REF_TYPE_RESTRICT);
    AK_print_table("AK_reference");
    AK_print_table("student");


    element row_root =  (element) malloc( sizeof(list) );
    a = 102;
    InitializeList(row_root);
     DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &a , "ref_test", "FK", row_root );
    InsertNewElement( TYPE_VARCHAR, "Dude", "ref_test", "Value", row_root );
    InsertNewElement( TYPE_VARCHAR, "TheRippah", "ref_test", "Rnd", row_root );
    insert_row( row_root );
    
    a=101;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &a , "ref_test", "FK", row_root );
    InsertNewElement( TYPE_VARCHAR, "Matija", "ref_test", "Value", row_root );
    InsertNewElement( TYPE_VARCHAR, "TheMutilator", "ref_test", "Rnd", row_root );
    insert_row( row_root );

    AK_print_table("ref_test");

    a=101;
    DeleteAllElements(row_root);
    InsertNewElementForUpdate(TYPE_INT, &a, "student", "mbr", row_root, 1);
    delete_row(row_root);

   /* DeleteAllElements(row_root);
    a = 101;
    InsertNewElementForUpdate(TYPE_INT, &a, "ref_test", "FK", row_root, 1);
    a = 102;
    InsertNewElementForUpdate(TYPE_INT, &a, "ref_test", "FK", row_root, 0);
    InsertNewElementForUpdate(TYPE_VARCHAR, "Matija", "ref_test", "Value", row_root, 1);
    InsertNewElementForUpdate(TYPE_VARCHAR, "Igor", "ref_test", "Value", row_root, 0);
    update_row(row_root);*/

//    AK_print_table("student");
    AK_print_table("ref_test");
}