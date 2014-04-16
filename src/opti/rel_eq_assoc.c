/**
@file rel_eq_assoc.c Provides functions for for relational equivalences regarding associativity
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

#include "rel_eq_assoc.h"

/**
 * @author Dino Laktašić
 * @brief Function for Struct cost_eval comparison 
 * @param *a first value
 * @param *b second value
 * @return returns result of comparison
 */
int AK_compare(const void *a, const void *b) {
    return ((*(struct cost_eval_t*) a).value - (*(struct cost_eval_t*) b).value);
}

/**
 * @author Dino Laktašić.
 * @brief Main function for generating RA expresion according to associativity equivalence rules 
 * @param *list_rel_eq RA expresion as the AK_list
 * @return optimised RA expresion as the AK_list
 */
AK_list *AK_rel_eq_assoc(AK_list *list_rel_eq) {
    //int exit_cond[5] = {0};
    int next_cost, i;

    //Initialize temporary linked list
    AK_list *temp = (AK_list *) AK_malloc(sizeof (AK_list));
    Ak_Init_L(temp);

    AK_list_elem tmp, temp_elem, temp_elem_prev, temp_elem_next;
    AK_list_elem list_elem_next, list_elem = (AK_list_elem) Ak_First_L(list_rel_eq);

    //Iterate through all the elements of RA linked list
    while (list_elem != NULL) {
        //printf("read > %s\n", list_elem->data);

        switch (list_elem->type) {

            case TYPE_OPERATOR:
                Ak_dbg_messg(LOW, REL_EQ, "\nOPERATOR '%c' SELECTED\n", list_elem->data[0]);
				Ak_dbg_messg(LOW, REL_EQ, "----------------------\n");
                temp_elem = (AK_list_elem) Ak_End_L(temp);
                temp_elem_prev = (AK_list_elem) Ak_Previous_L(temp_elem, temp);
                list_elem_next = (AK_list_elem) Ak_Next_L(list_elem);

                switch (list_elem->data[0]) {
                    //Associativity of set operations: Union and Intersection are associative but Set Difference is not
                    case RO_UNION:
                    case RO_INTERSECT:
                        if (temp_elem != NULL) {
                            if (temp_elem->type == TYPE_OPERAND && temp_elem_prev->type == TYPE_OPERAND &&
                                    list_elem_next->type == TYPE_OPERAND && (list_elem_next->next)->type == TYPE_OPERATOR &&
                                    strcmp(list_elem->data, (list_elem_next->next)->data) == 0) {

                                cost_eval cost[3];

                                //We can later consider some other options than number of table records
                                //to get heuristic values for table reordering in association construction
                                //Getting table rows count requires loop through all rows in table (very expansive)
                                cost[0].value = AK_get_num_records(temp_elem->data);
                                cost[1].value = AK_get_num_records(temp_elem_prev->data);
                                cost[2].value = AK_get_num_records(list_elem_next->data);

                                strcpy(cost[0].data, temp_elem->data);
                                strcpy(cost[1].data, temp_elem_prev->data);
                                strcpy(cost[2].data, list_elem_next->data);
                                qsort(cost, 3, sizeof (cost_eval), AK_compare);

                                //printf("ROW_COUNT: (%s) %i rows\n", cost[0].data, cost[0].value);
                                //printf("ROW_COUNT: (%s) %i rows\n", cost[1].data, cost[1].value);
                                //printf("ROW_COUNT: (%s) %i rows\n", cost[2].data, cost[2].value);

                                //Change inserted relation to largest table
								Ak_dbg_messg(MIDDLE, REL_EQ, "::table_name (%s) in temp list changed to %s\n", temp_elem_prev->data, cost[2].data); 
                                temp_elem_prev->size = strlen(cost[2].data) + 1;
                                memset(temp_elem_prev->data, '\0', MAX_VARCHAR_LENGTH);
                                strcpy(temp_elem_prev->data, cost[2].data);

                                //Change last inserted relation
                                Ak_dbg_messg(MIDDLE, REL_EQ, "::table_name (%s) in temp list changed to %s\n", temp_elem->data, cost[1].data);
                                temp_elem->size = strlen(cost[1].data) + 1;
                                memset(temp_elem->data, '\0', MAX_VARCHAR_LENGTH);
                                strcpy(temp_elem->data, cost[1].data);

                                //Insert smallest table at the end of temp list
                                Ak_InsertAtEnd_L(TYPE_OPERAND, cost[0].data, strlen(cost[0].data) + 1, temp);
								Ak_dbg_messg(MIDDLE, REL_EQ, "::table_name (%s) inserted in temp list\n", cost[0].data);
								
                                //Insert operator
								Ak_InsertAtEnd_L(list_elem->type, list_elem->data, list_elem->size, temp);
								Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted in temp list\n", list_elem->data);
								
                                list_elem = list_elem->next;
                            } else {
                            	Ak_InsertAtEnd_L(list_elem->type, list_elem->data, list_elem->size, temp);
								Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted in temp list\n", list_elem->data);
                            }
                        } else {
                        	Ak_InsertAtEnd_L(list_elem->type, list_elem->data, list_elem->size, temp);
                            Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted in temp list\n", list_elem->data);
                        }
                        break;

                        //Associativity of natural join
                    case RO_NAT_JOIN:
                        //Example: E1 E2 n[L1] E3 n[L2]
                        next_cost = i = 0;

                        //Set tmp to E3 (relation), move tmp for one place to get next natural join operator if exists
                        tmp = list_elem_next->next;
                        if (temp_elem_prev != NULL) {
                            while (tmp != NULL && (tmp->data[0] != RO_NAT_JOIN && tmp->type != TYPE_OPERATOR)) {
                                i++;
                                tmp = tmp->next;
                            }
                        }
                        //Current operator and condition
                        //list_elem >> n, list_elem_next >> [L1]

                        //Get next operator and condition if exist
                        //tmp >> n, tmp->next >> [L2]

                        //If there is valid move
                        if (i == 1) {
                            //Check for valid types (don't need to check operator, neither type for
                            //list_elem because it's alreacdy checked in the switch statement)
                            if ((list_elem->data[0] == tmp->data[0] && tmp->type == TYPE_OPERATOR) &&
                                    (list_elem_next->type == (tmp->next)->type)) {
                                cost_eval cost[3];

                                //read previous two relations and save their rows number to cost_eval struct,
                                //save also table name
                                while (temp_elem->type == TYPE_OPERAND) {
                                    cost[next_cost].value = AK_get_num_records(temp_elem->data);
                                    strcpy(cost[next_cost].data, temp_elem->data);
                                    next_cost++;
                                    temp_elem = (AK_list_elem) Ak_Previous_L(temp_elem, temp);
                                }

                                //see comment on the previous operator for getting heuristics values
                                //check for relation after natural join operator, if exists save data to cost_eval struct
                                //and then sort all three elements ascending (lower index -> less rows in table)
                                if ((list_elem_next->next)->type == TYPE_OPERAND) {
                                    cost[next_cost].value = AK_get_num_records((list_elem_next->next)->data);
                                    strcpy(cost[next_cost].data, (list_elem_next->next)->data);
                                    qsort(cost, 3, sizeof (cost_eval), AK_compare);
                                }

                                //if values for all three relations are saved, rearrange tables in list
                                if (next_cost-- == 2) {
                                    temp_elem = (AK_list_elem) Ak_End_L(temp);
                                    while (next_cost < 3) {
                                        if (temp_elem->type == TYPE_OPERAND) {
                                            temp_elem->size = strlen(cost[next_cost].data) + 1;
                                            memset(temp_elem->data, '\0', MAX_VARCHAR_LENGTH);
                                            strcpy(temp_elem->data, cost[next_cost].data);
                                            Ak_dbg_messg(MIDDLE, REL_EQ, "::table_name (%s) in temp list changed to %s\n", temp_elem->data, cost[next_cost].data);
                                            next_cost++;
                                        }
                                        temp_elem = (AK_list_elem) Ak_Previous_L(temp_elem, temp);
                                    }
                                    //insert final relation
                                    Ak_InsertAtEnd_L(TYPE_OPERAND, cost[0].data, strlen(cost[0].data) + 1, temp);
                                    Ak_dbg_messg(MIDDLE, REL_EQ, "::table_name (%s) inserted in temp list\n", cost[0].data);

                                    next_cost = 1;
                                } else {
                                    next_cost = 0;
                                }
                            }
                        }
                        //insert operator
                        Ak_InsertAtEnd_L(list_elem->type, list_elem->data, list_elem->size, temp);
                        Ak_InsertAtEnd_L(list_elem_next->type, list_elem_next->data, list_elem_next->size, temp);
                        Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted with attributes (%s) in temp list\n", list_elem->data, list_elem_next->data);

                        if (next_cost) {
                            //dbg_messg(MIDDLE, REL_EQ, "TMP: (%s), TMP_NEXT: (%s)\n", tmp->data, (tmp->next)->data);
                        	Ak_InsertAtEnd_L(tmp->type, tmp->data, tmp->size, temp);
                        	Ak_InsertAtEnd_L((tmp->next)->type, (tmp->next)->data, (tmp->next)->size, temp);
                            Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted with attributes (%s) in temp list\n", tmp->data, (tmp->next)->data);
                            list_elem = tmp->next;
                        } else {
                            list_elem = list_elem_next;
                        }
                        break;

                    case RO_THETA_JOIN:
                        //Example: E1 E2 t[F1] E3 t[F2]
                        next_cost = -1;
                        tmp = list_elem_next->next;

                        i = 0;
                        while (tmp != NULL && tmp->type != TYPE_CONDITION) {
                            tmp = tmp->next;
                            i++;
                        }
                        tmp = list_elem_next->next;

                        //if exists elements in range
                        if (temp_elem_prev != NULL && i == 2) {
                            char *cond_attr_first, *cond_attr_second;
                            char *tbl_attr_first, *tbl_attr_second, *tbl_attr_third;

                            //get attributes from condition F1 (E1 E2 t[F1])
                            cond_attr_first = NULL;
                            if (list_elem_next->type == TYPE_CONDITION) {
                                cond_attr_first = (char *) AK_rel_eq_cond_attributes(list_elem_next->data);
                            }

                            //get attributes from condition F2 (t[F2])
                            cond_attr_second = NULL;
                            if (((tmp->next)->data[0] == RO_THETA_JOIN && (tmp->next)->type == TYPE_OPERATOR) && (tmp->next->next)->type == TYPE_CONDITION) {
                                cond_attr_second = (char *) AK_rel_eq_cond_attributes((tmp->next->next)->data);
                            }

                            //get attributes from table E1
                            tbl_attr_first = NULL;
                            if (temp_elem->type == TYPE_OPERAND) {
                                tbl_attr_first = (char *) AK_rel_eq_get_attributes(temp_elem_prev->data);
                            }

                            //get attributes from table E2
                            tbl_attr_second = NULL;
                            if (temp_elem->type == TYPE_OPERAND) {
                                tbl_attr_second = (char *) AK_rel_eq_get_attributes(temp_elem->data);
                            }

                            //get attributes from table E3
                            tbl_attr_third = NULL;
                            if (temp_elem->type == TYPE_OPERAND) {
                                tbl_attr_third = (char *) AK_rel_eq_get_attributes(tmp->data);
                            }

                            if (cond_attr_first != NULL && cond_attr_second != NULL &&
                                    tbl_attr_first != NULL && tbl_attr_second != NULL && tbl_attr_third != NULL) {

                                //attributes from F1 are subset of attributes from E2
                                //and attributes from F1 are subset of attributes from E1, and they are not susbset of E3
                                if (AK_rel_eq_is_attr_subset(tbl_attr_first, cond_attr_first) == EXIT_SUCCESS &&
                                        AK_rel_eq_is_attr_subset(tbl_attr_second, cond_attr_first) == EXIT_SUCCESS &&
                                        AK_rel_eq_is_attr_subset(tbl_attr_third, cond_attr_first) == EXIT_FAILURE) {
                                    //first condition check completed successfully
                                    next_cost++;
                                }

                                //attributes from F2 are subset of attributes from E2
                                //and attributes from F2 are subset of attributes from E3, and they are not susbset of E1
                                if (AK_rel_eq_is_attr_subset(tbl_attr_second, cond_attr_second) == EXIT_SUCCESS &&
                                        AK_rel_eq_is_attr_subset(tbl_attr_third, cond_attr_second) == EXIT_SUCCESS &&
                                        AK_rel_eq_is_attr_subset(tbl_attr_first, cond_attr_second) == EXIT_FAILURE) {
                                    //second condition check completed successfully
                                    next_cost++;
                                }

                                AK_free(cond_attr_first);
                                AK_free(tbl_attr_first);
                                AK_free(tbl_attr_second);
                                AK_free(tbl_attr_third);

                            } //end of check for NULLs

                            if (next_cost) {
                                next_cost = 0;
                                cost_eval cost[3];

                                while (temp_elem->type == TYPE_OPERAND) {
                                    cost[next_cost].value = AK_get_num_records(temp_elem->data);
                                    strcpy(cost[next_cost].data, temp_elem->data);
                                    next_cost++;
                                    temp_elem = (AK_list_elem) Ak_Previous_L(temp_elem, temp);
                                }

                                if (next_cost > 1) {
                                    //see comment on the previous operator for getting heuristics values
                                    cost[next_cost].value = AK_get_num_records((list_elem_next->next)->data);
                                    strcpy(cost[next_cost].data, (list_elem_next->next)->data);
                                    qsort(cost, 3, sizeof (cost_eval), AK_compare);
                                    temp_elem = (AK_list_elem) Ak_End_L(temp);

                                    next_cost = 1;
                                    while (next_cost < 3) {
                                        if (temp_elem->type == TYPE_OPERAND) {
                                            temp_elem->size = strlen(cost[next_cost].data) + 1;
                                            memset(temp_elem->data, '\0', MAX_VARCHAR_LENGTH);
                                            strcpy(temp_elem->data, cost[next_cost].data);
                                            Ak_dbg_messg(MIDDLE, REL_EQ, "::table_name (%s) in temp list changed to %s\n", temp_elem->data, cost[next_cost].data);
                                            next_cost++;
                                        }
                                        temp_elem = (AK_list_elem) Ak_Previous_L(temp_elem, temp);
                                    }

                                    Ak_InsertAtEnd_L(TYPE_OPERAND, cost[0].data, strlen(cost[0].data) + 1, temp);
                                    Ak_dbg_messg(MIDDLE, REL_EQ, "::table_name (%s) inserted in temp list\n", cost[0].data);
                                    next_cost = 1;
                                } else {
                                    next_cost = -1;
                                }
                            }
                        }

                        Ak_InsertAtEnd_L(list_elem->type, list_elem->data, list_elem->size, temp);
                        Ak_InsertAtEnd_L(list_elem_next->type, list_elem_next->data, list_elem_next->size, temp);

                        if (next_cost > 0) {
                        	Ak_InsertAtEnd_L((tmp->next)->type, (tmp->next)->data, (tmp->next)->size, temp);
                            Ak_InsertAtEnd_L((tmp->next->next)->type, (tmp->next->next)->data, (tmp->next->next)->size, temp);
                            Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted with attributes (%s) in temp list\n", list_elem->data, list_elem_next->data);
                            Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted with attributes (%s) in temp list\n", (tmp->next)->data, (tmp->next->next)->data);
                            list_elem = tmp->next;
                        } else {
                            Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted with condition (%s) in temp list\n", list_elem->data, list_elem_next->data);
                            list_elem = list_elem->next;
                        }
                        break;
                    case RO_PROJECTION:
                    case RO_SELECTION:
                    	Ak_InsertAtEnd_L(list_elem->type, list_elem->data, list_elem->size, temp);
                        Ak_InsertAtEnd_L(list_elem_next->type, list_elem_next->data, list_elem_next->size, temp);
                        Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted with condition (%s) in temp list\n", list_elem->data, list_elem_next->data);
                        list_elem = list_elem->next;
                        break;

                    case RO_EXCEPT:
                    	Ak_InsertAtEnd_L(list_elem->type, list_elem->data, list_elem->size, temp);
                        Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted in temp list\n", list_elem->data);
                        break;

                    case RO_RENAME:
                    	Ak_InsertAtEnd_L(list_elem->type, list_elem->data, list_elem->size, temp);
                        Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted in temp list\n", list_elem->data);
                        break;

                    default:
                        Ak_dbg_messg(LOW, REL_EQ, "Invalid operator: %s", list_elem->data);
                        break;
                }
                break;

                //additional type definition included to distinguish beetween table name and attribute/s
            case TYPE_ATTRIBS:
                //printf("::attribute (%s) inserted in the temp list\n", list_elem->data);
                break;

                //additional type definition included to distinguish beetween attribute/s and condition
            case TYPE_CONDITION:
                //printf("::condition (%s) inserted in the temp list\n", list_elem->data);
                break;

            case TYPE_OPERAND:
                Ak_dbg_messg(MIDDLE, REL_EQ, "::table_name (%s) inserted in the temp list\n", list_elem->data);
                Ak_InsertAtEnd_L(TYPE_OPERAND, list_elem->data, list_elem->size, temp);
                break;

            default:
                Ak_dbg_messg(LOW, REL_EQ, "Invalid type: %s", list_elem->data);
                break;
        }

        list_elem = list_elem->next;
    }

    //====================================> IMPROVMENTS <=======================================
    //Recursive RA optimization (need to implement exit condition in place of each operator, ...)
    //If there is no new changes on the list return generated AK_lists
    //int iter_cond;
    //for (iter_cond = 0; iter_cond < sizeof(exit_cond); iter_cond++) {
    //	if (exit_cond[iter_cond] == 0) {
    ////	Edit function to return collection of the AK_lists
    ////	Generate next RA expr. (new plan)
    ////	temp += remain from the list_rel_eq
    //		AK_rel_eq_assoc(temp);
    //	}
    //}

    Ak_DeleteAll_L(list_rel_eq);
    return temp;
}
/**
 * @author Dino Laktašić.
 * @brief Function for printing RA expresion AK_list
 * @param *list_rel_eq RA expresion as the AK_list
 * @return optimised RA expresion as the AK_list
 */
void AK_print_rel_eq_assoc(AK_list *list_rel_eq) {
    AK_list_elem list_elem = (AK_list_elem) Ak_First_L(list_rel_eq);

    printf("\n");
    while (list_elem != NULL) {
        printf("Type: %i, size: %i, data: %s\n", list_elem->type, list_elem->size, list_elem->data);
        list_elem = list_elem->next;
    }
}
/**
 * @author Dino Laktašić.
 * @brief Function for testing relational equivalences regarding associativity
 * @return No return value
 */
void AK_rel_eq_assoc_test() {
    printf("rel_eq_assoc.c: Present!\n");
    printf("\n********** REL_EQ_ASSOCIATIVITY TEST by Dino Laktašić **********\n");

    //create header
    AK_header t_header[MAX_ATTRIBUTES];
    AK_header *temp;

    temp = (AK_header*) AK_create_header("id", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header, temp, sizeof (AK_header));
    temp = (AK_header*) AK_create_header("firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 1, temp, sizeof (AK_header));
    temp = (AK_header*) AK_create_header("job", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 2, temp, sizeof (AK_header));
    temp = (AK_header*) AK_create_header("year", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 3, temp, sizeof (AK_header));
    temp = (AK_header*) AK_create_header("weight", TYPE_FLOAT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 4, temp, sizeof (AK_header));
    memset(t_header + 5, '\0', MAX_ATTRIBUTES - 5);

    //create table
    char *tblName = "profesor";

    int startAddress = AK_initialize_new_segment(tblName, SEGMENT_TYPE_TABLE, t_header);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName);

    printf("rel_eq_associativity_test: After segment initialization: %d\n", AK_num_attr(tblName));

    AK_list_elem row_root = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L(row_root);

    int id = 35890, year = 1999;
    float weight = 80.00;

    //insert rows in table student
    id++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id, tblName, "id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Mirko", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Sestak", tblName, "job", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    id++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id, tblName, "id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Igor", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Mesaric", tblName, "job", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    id++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id, tblName, "id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Dalibor", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Slunjski", tblName, "job", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    id++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id, tblName, "id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Dino", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Alagic", tblName, "job", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    id++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id, tblName, "id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Miroslav", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Zver", tblName, "job", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    id++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root);
    Ak_Insert_New_Element(TYPE_INT, &id, tblName, "id", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Josip", tblName, "firstname", row_root);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Vincek", tblName, "job", row_root);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName, "year", row_root);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName, "weight", row_root);
    Ak_insert_row(row_root);

    printf("rel_eq_associativity_test: After data insertion: %d\n", AK_num_attr(tblName));

    //create header
    AK_header t_header2[MAX_ATTRIBUTES];
    AK_header *temp2;

    temp2 = (AK_header*) AK_create_header("mbr", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2, temp2, sizeof (AK_header));
    temp2 = (AK_header*) AK_create_header("firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 1, temp2, sizeof (AK_header));
    temp2 = (AK_header*) AK_create_header("lastname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 2, temp2, sizeof (AK_header));
    temp2 = (AK_header*) AK_create_header("year", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 3, temp2, sizeof (AK_header));
    temp2 = (AK_header*) AK_create_header("weight", TYPE_FLOAT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 4, temp2, sizeof (AK_header));
    memset(t_header2 + 5, '\0', MAX_ATTRIBUTES - 5);

    //create table
    char *tblName2 = "student";

    int startAddress2 = AK_initialize_new_segment(tblName2, SEGMENT_TYPE_TABLE, t_header2);

    if (startAddress2 != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName2);

    printf("rel_eq_associativity_test: After segment initialization: %d\n", AK_num_attr(tblName2));

    AK_list_elem row_root2 = (AK_list_elem) AK_malloc(sizeof (AK_list));
    Ak_Init_L(row_root2);

    int mbr = 35890;
    year = 1999;
    weight = 80.00;

    //insert rows in table student
    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root2);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName2, "mbr", row_root2);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Matija", tblName2, "firstname", row_root2);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Sestak", tblName2, "lastname", row_root2);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName2, "year", row_root2);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName2, "weight", row_root2);
    Ak_insert_row(row_root2);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root2);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName2, "mbr", row_root2);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Igor", tblName2, "firstname", row_root2);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Mesaric", tblName2, "lastname", row_root2);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName2, "year", row_root2);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName2, "weight", row_root2);
    Ak_insert_row(row_root2);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root2);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName2, "mbr", row_root2);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Dalibor", tblName2, "firstname", row_root2);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Slunjski", tblName2, "lastname", row_root2);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName2, "year", row_root2);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName2, "weight", row_root2);
    Ak_insert_row(row_root2);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root2);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName2, "mbr", row_root2);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Dino", tblName2, "firstname", row_root2);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Alagic", tblName2, "lastname", row_root2);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName2, "year", row_root2);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName2, "weight", row_root2);
    Ak_insert_row(row_root2);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root2);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName2, "mbr", row_root2);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Miroslav", tblName2, "firstname", row_root2);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Zver", tblName2, "lastname", row_root2);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName2, "year", row_root2);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName2, "weight", row_root2);
    Ak_insert_row(row_root2);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root2);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName2, "mbr", row_root2);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Josip", tblName2, "firstname", row_root2);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Vincek", tblName2, "lastname", row_root2);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName2, "year", row_root2);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName2, "weight", row_root2);
    Ak_insert_row(row_root2);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root2);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName2, "mbr", row_root2);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Netko", tblName2, "firstname", row_root2);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Netkic", tblName2, "lastname", row_root2);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName2, "year", row_root2);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName2, "weight", row_root2);
    Ak_insert_row(row_root2);

    mbr++;
    year++;
    weight += 0.75;
    Ak_DeleteAll_L(row_root2);
    Ak_Insert_New_Element(TYPE_INT, &mbr, tblName2, "mbr", row_root2);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Dino", tblName2, "firstname", row_root2);
    Ak_Insert_New_Element(TYPE_VARCHAR, "Laktašić", tblName2, "lastname", row_root2);
    Ak_Insert_New_Element(TYPE_INT, &year, tblName2, "year", row_root2);
    Ak_Insert_New_Element(TYPE_FLOAT, &weight, tblName2, "weight", row_root2);
    Ak_insert_row(row_root2);

    printf("rel_eq_associativity_test: After data insertion: %d\n", AK_num_attr(tblName2));
    //-----------------------------------------------------------------------------------------

    AK_list *expr = (AK_list *) AK_malloc(sizeof (AK_list));
    Ak_Init_L(expr);

    Ak_InsertAtEnd_L(TYPE_OPERATOR, "p", sizeof ("p"), expr);
    Ak_InsertAtEnd_L(TYPE_ATTRIBS, "L1;L2;L3;L4", sizeof ("L1;L2;L3;L4"), expr); //projection attribute
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "p", sizeof ("p"), expr);
    Ak_InsertAtEnd_L(TYPE_ATTRIBS, "L1;L4;L3;L2;L5", sizeof ("L1;L4;L3;L2;L5"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "s", sizeof ("s"), expr);
    Ak_InsertAtEnd_L(TYPE_CONDITION, "`L1` > 100 OR `L2` < 50", sizeof ("`L1` > 100 OR `L2` < 50"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "student", sizeof ("student"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "profesor", sizeof ("profesor"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "u", sizeof ("u"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "student", sizeof ("student"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "u", sizeof ("u"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "p", sizeof ("p"), expr);
    Ak_InsertAtEnd_L(TYPE_ATTRIBS, "mbr;firstname;job", sizeof ("mbr;firstname;job"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "student", sizeof ("student"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "profesor", sizeof ("profesor"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "t", sizeof ("t"), expr);
    Ak_InsertAtEnd_L(TYPE_CONDITION, "`mbr` = `id`", sizeof ("`mbr` = `id`"), expr); //theta join attribute

    Ak_InsertAtEnd_L(TYPE_OPERAND, "student", sizeof ("student"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "profesor", sizeof ("profesor"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "n", sizeof ("n"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "mbr;job", sizeof ("mbr;job"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "profesor", sizeof ("profesor"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "n", sizeof ("n"), expr);
    Ak_InsertAtEnd_L(TYPE_CONDITION, "mbr;job", sizeof ("mbr;job"), expr);

    //printf("\nRA expr. before rel_eq optimization:\n");
    //AK_print_rel_eq_projection(expr);
    AK_print_rel_eq_assoc(AK_rel_eq_assoc(expr));

    printf("\n------------------> TEST_ASSOCIATIVITY_FUNCTIONS <------------------\n\n");
    printf("...");

    Ak_DeleteAll_L(expr);
    //dealocate variables ;)
}
