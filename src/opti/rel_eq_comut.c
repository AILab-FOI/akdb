/**
@file rel_eq_comut.c Provides functions for relational equivalences regarding commutativity
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

#include "rel_eq_comut.h"
 

/**
 * @author Davor Tomala
 * @brief Function for printing optimized relation equivalence expression list regarding commutativity
 * @param *list_rel_eq RA expresion as the AK_list
 */
void AK_print_rel_eq_comut(AK_list *list_rel_eq) {
    AK_list_elem list_elem = (AK_list_elem) Ak_First_L(list_rel_eq);

    printf("\n");
    while (list_elem != NULL) {
        printf("Type: %i, size: %i, data: %s\n", list_elem->type, list_elem->size, list_elem->data);
        list_elem = list_elem->next;
    }
}

/**
 * @author Davor Tomala
 * @brief Main function for generating RA expresion according to commutativity equivalence rules 
 * @param *list_rel_eq RA expresion as the AK_list
 * @return optimised RA expresion as the AK_list
 */
AK_list *AK_rel_eq_comut(AK_list *list_rel_eq) {
    int step; // exit_cond[5] = {0};

    //Initialize temporary linked list
    AK_list *temp = (AK_list *) malloc(sizeof (AK_list));
    Ak_Init_L(temp);

    AK_list_elem tmp, temp_elem, temp_elem_prev, temp_elem_next;
    AK_list_elem list_elem_next, list_elem = (AK_list_elem) Ak_First_L(list_rel_eq);

    //Iterate through all the elements of RA linked list
    while (list_elem != NULL) {

        switch (list_elem->type) {

            case TYPE_OPERATOR:
                Ak_dbg_messg(LOW, REL_EQ, "\nOPERATOR '%c' SELECTED\n", list_elem->data[0]);
                Ak_dbg_messg(LOW, REL_EQ, "----------------------\n");
                temp_elem = (AK_list_elem) Ak_End_L(temp);
                temp_elem_prev = (AK_list_elem) Ak_Previous_L(temp_elem, temp);
                list_elem_next = (AK_list_elem) Ak_Next_L(list_elem);

                switch (list_elem->data[0]) {
                    //Commutativity of Selection and Projection.
                    //Cascade of Selection and Commutativity of Selection
                    case RO_SELECTION:
                        //Join cascade selection conditions to one
                        if (temp_elem != NULL && temp_elem_prev != NULL && temp_elem->type == TYPE_CONDITION &&
                                temp_elem_prev->data[0] == RO_SELECTION && temp_elem_prev->type == TYPE_OPERATOR) {
                            temp_elem->size = temp_elem->size + list_elem_next->size + strlen(" AND") + 1; //edit to (" AND ")
                            //strcat(temp_elem->data, " AND "); //uncomment for infix use
                            strcat(temp_elem->data, " "); //remove for infix
                            strcat(temp_elem->data, list_elem_next->data);
                            strcat(temp_elem->data, " AND"); //comment if using infix format
                            memcpy(temp_elem->data, temp_elem->data, temp_elem->size);
                            Ak_dbg_messg(MIDDLE, REL_EQ, "::selection cascade - condition changed to (%s) in temp list\n", temp_elem->data);
                        } else {
                            Ak_InsertAtEnd_L(list_elem->type, list_elem->data, list_elem->size, temp);
                            Ak_InsertAtEnd_L(list_elem_next->type, list_elem_next->data, list_elem_next->size, temp);
                            Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted with attributes (%s) in temp list\n", list_elem->data, list_elem_next->data);
                        }

                        list_elem = list_elem->next;
                        break;
                    
                    case RO_PROJECTION:
                        step = -1;

                        if (temp_elem != NULL) {
                            while (temp_elem != NULL) {
                                if (temp_elem->type == TYPE_OPERAND || temp_elem->type == TYPE_CONDITION) {
                                    if (temp_elem->type == TYPE_CONDITION) {
                                        temp_elem_prev = (AK_list_elem) Ak_Previous_L(temp_elem, temp);

                                        if ((AK_rel_eq_can_commute(list_elem_next, temp_elem) == EXIT_FAILURE) &&
                                                (temp_elem_prev->data[0] == RO_SELECTION) && (temp_elem_prev->type == TYPE_OPERATOR)) {
                                            Ak_InsertAtEnd_L(list_elem->type, list_elem->data, list_elem->size, temp);
                                            Ak_InsertAtEnd_L(list_elem_next->type, list_elem_next->data, list_elem_next->size, temp);
                                            Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted with condition (%s) in temp list\n", list_elem->data, list_elem_next->data);
                                            step++;
                                            break;
                                        } else if ((AK_rel_eq_can_commute(list_elem_next, temp_elem) == EXIT_SUCCESS) &&
                                                (temp_elem_prev->data[0] == RO_SELECTION) && (temp_elem_prev->type == TYPE_OPERATOR)) {
                                            Ak_InsertBefore_L(list_elem->type, list_elem->data, list_elem->size, temp_elem_prev, temp);
                                            Ak_InsertBefore_L(list_elem_next->type, list_elem_next->data, list_elem_next->size, temp_elem_prev, temp);
                                            Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted with condition (%s) in temp list\n", list_elem->data, list_elem_next->data);
                                            step++;
                                            break;
                                        }
                                    }
                                }
                                temp_elem = (AK_list_elem) Ak_Previous_L(temp_elem, temp);
                            }
                        }

                        if (temp_elem == NULL || step != 0) {
                            Ak_InsertAtEnd_L(list_elem->type, list_elem->data, list_elem->size, temp);
                            Ak_InsertAtEnd_L(list_elem_next->type, list_elem_next->data, list_elem_next->size, temp);
                            Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted with condition (%s) in temp list\n", list_elem->data, list_elem_next->data);
                        }

                        list_elem = list_elem->next;
                        break;

                    //Commutativity of Selection and set operations (Union, Intersection, and Set difference)
                    case RO_UNION:
                    case RO_INTERSECT:
                    case RO_EXCEPT:
                        step = -1;

                        while (temp_elem != NULL) {
                            if (temp_elem->type == TYPE_OPERAND || temp_elem->type == TYPE_CONDITION) {
                                step++;
                                temp_elem_prev = (AK_list_elem) Ak_Previous_L(temp_elem, temp);

                                if (temp_elem_prev->data[0] == RO_SELECTION && temp_elem_prev->type == TYPE_OPERATOR) {
                                    if (step > 1) {
                                        tmp = temp_elem;
                                        while (tmp->type != TYPE_OPERAND) {
                                            tmp = tmp->next;
                                        }
                                        Ak_InsertAfter_L(temp_elem->type, temp_elem->data, temp_elem->size, tmp, temp);
                                        Ak_InsertAfter_L(temp_elem_prev->type, temp_elem_prev->data, temp_elem_prev->size, tmp, temp);
                                        Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted with attributes (%s) in temp list\n", temp_elem_prev->data, temp_elem->data);
                                    }
                                    break;
                                }
                            } else {
                                break;
                            }
                            temp_elem = (AK_list_elem) Ak_Previous_L(temp_elem, temp);
                        }
                        Ak_InsertAtEnd_L(list_elem->type, list_elem->data, list_elem->size, temp);
                        Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted in temp list\n", list_elem->data);
                        break;

                    case RO_NAT_JOIN:
                        Ak_InsertAtEnd_L(list_elem->type, list_elem->data, list_elem->size, temp);
                        Ak_InsertAtEnd_L(list_elem_next->type, list_elem_next->data, list_elem_next->size, temp);
                        Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted in temp list\n", list_elem->data);
                        list_elem = list_elem->next;
                        break;

                    //Commutativity of Selection and Theta join (or Cartesian product)
                    case RO_THETA_JOIN:
                        step = -1;

                        while (temp_elem != NULL) {
                            if (temp_elem->type == TYPE_OPERAND || temp_elem->type == TYPE_CONDITION) {
                                step++;
                                temp_elem_prev = (AK_list_elem) Ak_Previous_L(temp_elem, temp);

                                if (temp_elem_prev->data[0] == RO_SELECTION && temp_elem_prev->type == TYPE_OPERATOR) {
                                    if (step > 1) {
                                        tmp = temp_elem;
                                        temp_elem_next = temp_elem->next;

                                        char *data1, *data2;
                                        char *cond_attr1, *cond_attr2;
                                        char op_selected[2];
                                        memcpy(op_selected, temp_elem_prev->data, 2);

                                        data1 = AK_rel_eq_commute_with_theta_join(temp_elem->data, temp_elem_next->data);
                                        cond_attr1 = AK_rel_eq_cond_attributes(data1);

                                        data2 = AK_rel_eq_commute_with_theta_join(temp_elem->data, (temp_elem_next->next)->data);
                                        cond_attr2 = AK_rel_eq_cond_attributes(data2);

                                        //Debug lines - can be removed later
                                        //printf("CONDITION DATA : data: (%s),(%s), cond: (%s),(%s)\n", data1, data2, cond_attr1, cond_attr2);
                                        //printf("SHARE ATTRIBUTE: (%i)\n", AK_rel_eq_share_attributes(cond_attr1, cond_attr2));

                                        if (AK_rel_eq_share_attributes(cond_attr1, cond_attr2)) {
                                            if (cond_attr1 != NULL) {
                                                //memset(temp_elem->data, '\0', MAX_VARCHAR_LENGHT);
                                                temp_elem->size = strlen(data1) + 1;
                                                memcpy(temp_elem->data, data1, temp_elem->size);
                                                memset(temp_elem->data + temp_elem->size, '\0', MAX_VARCHAR_LENGTH - temp_elem->size);
                                                Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted with attributes (%s) in temp list\n", temp_elem_prev->data, temp_elem->data);
                                            } else {
                                                AK_list_elem temp_elem_prevprev = (AK_list_elem) Ak_Previous_L(temp_elem_prev, temp);
                                                temp_elem_prevprev->next = temp_elem;
                                                free(temp_elem_prev);
                                                AK_list_elem temp_elem_prev = temp_elem_prevprev;

                                                temp_elem_prev->next = temp_elem_next;
                                                free(temp_elem);
                                                AK_list_elem temp_elem = temp_elem_next;
                                                temp_elem_next = temp_elem->next;
                                                tmp = temp_elem;
                                            }

                                            while (tmp->type != TYPE_OPERAND) {
                                                tmp = tmp->next;
                                            }

                                            if (cond_attr2 != NULL) {
                                                memset(data2 + strlen(data2), '\0', 1);
                                                Ak_InsertAfter_L(temp_elem->type, data2, strlen(data2) + 1, tmp, temp);
                                                Ak_InsertAfter_L(TYPE_OPERATOR, op_selected, 2, tmp, temp);
                                                Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted with attributes (%s) in temp list\n", op_selected, data2);
                                            }
                                        }

                                        free(data1);
                                        free(data2);
                                        free(cond_attr1);
                                        free(cond_attr2);
                                        break;
                                    }
                                }
                            } else {
                                break;
                            }
                            temp_elem = (AK_list_elem) Ak_Previous_L(temp_elem, temp);
                        }

                        Ak_InsertAtEnd_L(list_elem->type, list_elem->data, list_elem->size, temp);
                        Ak_InsertAtEnd_L(list_elem_next->type, list_elem_next->data, list_elem_next->size, temp);
                        Ak_dbg_messg(MIDDLE, REL_EQ, "::operator %s inserted with condition (%s) in temp list\n", list_elem->data, list_elem_next->data);
                        list_elem = list_elem->next;
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
                //printf("::attribute '%s' inserted in the temp list\n", list_elem->data);
                break;

                //additional type definition included to distinguish beetween attribute/s and condition
            case TYPE_CONDITION:
                //printf("::condition '%s' inserted in the temp list\n", list_elem->data);
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
    //  if (exit_cond[iter_cond] == 0) {
    ////    Edit function to return collection of the AK_lists
    ////    Generate next RA expr. (new plan)
    ////    temp += remain from the list_rel_eq
    //      AK_rel_eq_comut(temp);
    //  }
    //}

    Ak_DeleteAll_L(list_rel_eq);
    return temp;
}

/**
 * @author Dino Laktašić.
 * @brief Check if selection can commute with theta-join or product
 * <ol>
 * <li>For each token (delimited by " ") in selection condition first check if token represents attribute/s and is subset in the given table</li>
 * <li>If token is a subset set variable id to 1</li>
 * <li>else set id to 0, else make no changes to variable id</li>
 * <li>if token differs from "AND" and "OR" and id equals to 1 append current token to result condition</li>
 * <li>else if token equals to "AND" or "OR" and id equals to 1 and there are two added tokens add "AND" or "OR" to condition string</li>
 * <li>When exits from loop, return pointer to char array that contains new condition for a given table
 * </ol>
 * @param *cond condition array that contains condition data 
 * @param *tblName name of the table
 * @result pointer to char array that contains new condition for a given table
 */

char *AK_rel_eq_commute_with_theta_join(char *cond, char *tblName) {
    int id = 0;
    int token_id = 0;
    int next_cond = 0;

    char *token_cond, *save_token_cond;
    char *ret_attributes = (char *) calloc(MAX_VARCHAR_LENGTH, sizeof (char));
    char *temp_cond = (char *) calloc(strlen(cond) + 1, sizeof (char));

    memcpy(temp_cond, cond, strlen(cond));
    memcpy(temp_cond + strlen(cond) + 1, "\0", 1);

    Ak_dbg_messg(HIGH, REL_EQ, "RULE - commute selection (%s) with theta-join, table name (%s)\n", temp_cond, tblName);

    for ((token_cond = strtok_r(temp_cond, " ", &save_token_cond)); token_cond;
            (token_cond = strtok_r(NULL, " ", &save_token_cond)), token_id++) {
        if (token_id < MAX_TOKENS - 1) {
            if (*token_cond == ATTR_ESCAPE) {
                char *tbl = AK_rel_eq_get_atrributes_char(tblName);
                char *attr = AK_rel_eq_cond_attributes(token_cond);

                if (attr != NULL && tbl != NULL) {
                    if (!AK_rel_eq_is_attr_subset(tbl, attr)) { //if is subset set id to 1
                        id = 1;
                    } else {
                        id = 0;
                    }
                }
                free(tbl);
                free(attr);
            }
            //`category` 'teacher' = `firstname` 'Dino' = AND `lastname` 'Laktasic' = OR
            if ((strcmp(token_cond, "AND") != 0 && strcmp(token_cond, "OR") != 0) && id > 0) {
                if (strlen(ret_attributes)) {
                    strcat(ret_attributes, " ");
                }
                strcat(ret_attributes, token_cond);
                next_cond++;
            } else if ((strcmp(token_cond, "AND") == 0 || strcmp(token_cond, "OR") == 0) && next_cond % 2 == 0 && id > 0) {
                strcat(ret_attributes, " ");
                strcat(ret_attributes, token_cond);
            }
        }
    }

    if (ret_attributes > 0) {
        Ak_dbg_messg(HIGH, REL_EQ, "RULE - commute selection with theta-join succeed.\n");
        return ret_attributes;
    } else {
        Ak_dbg_messg(HIGH, REL_EQ, "RULE - commute selection with theta-join failed!\n");
        free(ret_attributes);
        return NULL;
    }
}

/**
 * @author Dino Laktašić (AK_rel_eq_commute_with_theta_join), Davor Tomala (AK_rel_eq_comut)
 * @brief relational equivalences regarding commutativity
 * @return No return vlaue
 */
void AK_rel_eq_comut_test() {
    printf("AK_rel_eq_commute_with_theta_join: Present!\n");
    printf(AK_rel_eq_commute_with_theta_join("`mbr` 100 > `firstname` 50 < AND `id` 'A' > OR", "profesor"));
    printf("\n");

    printf("rel_eq_comut.c: Present!\n");
    printf("\n********** REL_EQ_COMMUTATION TEST **********\n");
    //create header
    AK_header t_header[MAX_ATTRIBUTES];
    AK_header* temp;

    temp = (AK_header*) AK_create_header("id", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 1, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("job", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 2, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("year", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 3, temp, sizeof ( AK_header));
    temp = (AK_header*) AK_create_header("weight", TYPE_FLOAT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 4, temp, sizeof ( AK_header));
    memset(t_header + 5, '\0', MAX_ATTRIBUTES - 5);

    //create table
    char *tblName = "profesor";

    int startAddress = AK_initialize_new_segment(tblName, SEGMENT_TYPE_TABLE, t_header);

    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName);

    printf("rel_eq_comut_test: After segment initialization: %d\n", AK_num_attr(tblName));

    AK_list *expr = (AK_list *) malloc(sizeof (AK_list));
    Ak_Init_L(expr);

    Ak_InsertAtEnd_L(TYPE_OPERATOR, "p", sizeof ("p"), expr);
    /*
     * The comut is only made up of a one or more of attributes
     */
    Ak_InsertAtEnd_L(TYPE_ATTRIBS, "L1;L2;L3;L4", sizeof ("L1;L2;L3;L4"), expr); //comut attribute
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "p", sizeof ("p"), expr);
    Ak_InsertAtEnd_L(TYPE_ATTRIBS, "L1;L4;L3;L2;L5", sizeof ("L1;L4;L3;L2;L5"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "s", sizeof ("s"), expr);

    /* The selection condition is made up of a number of clauses of the form
     * <attribute name> <comparison op> <constant value> OR
     * <attribute name 1> <comparison op> <attribute name 2>
     * In the clause, the comparison operations could be one of the following: ≤, ≥, ≠, =, >, < .
     * Clauses are connected by Boolean operators : and, or , not
     */
    Ak_InsertAtEnd_L(TYPE_CONDITION, "`L1` 100 > `L2` 50 < OR", sizeof ("`L1` 100 > `L2` 50 < OR"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "R", sizeof ("R"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "S", sizeof ("S"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "u", sizeof ("u"), expr);

    Ak_InsertAtEnd_L(TYPE_OPERATOR, "p", sizeof ("p"), expr);
    Ak_InsertAtEnd_L(TYPE_ATTRIBS, "mbr;firstname;job", sizeof ("mbr;firstname;job"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "student", sizeof ("student"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "profesor", sizeof ("profesor"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "t", sizeof ("t"), expr);
    Ak_InsertAtEnd_L(TYPE_CONDITION, "`mbr` `job` =", sizeof ("`mbr` `job` ="), expr); //theta join attribute

    /*
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
    */

    //printf("\nRA expr. before rel_eq optimization:\n");
    //AK_print_rel_eq_comut(expr);
    AK_print_rel_eq_comut(AK_rel_eq_comut(expr));

    if (DEBUG_ALL) {
        printf("\n------------------> TEST_COMUT_FUNCTIONS <------------------\n\n");

        //Initialize list elements...
        //AK_list_elem list_elem_set, list_elem_subset;
        //AK_list_elem list_elem_cond, list_elem_attr;

        char *test_cond1, *test_cond2;
        char *test_table;
        char *test_attribs;
        char *cond_attr1, *cond_attr2;

        test_table = "profesor";
        test_cond1 = "`mbr` 100 > `firstname` 'Markus' = AND `id` 1000 > OR";
        test_cond2 = "`id` 100 > `firstname` 50 < AND `job` 'teacher' = AND";
        test_attribs = "id;mbr";

        cond_attr1 = AK_rel_eq_cond_attributes(test_cond1);
        cond_attr2 = AK_rel_eq_cond_attributes(test_cond2);

        printf("GET_ALL_TABLE_ATTRIBUTES_TEST   : (%s)\n\n", AK_rel_eq_get_atrributes_char(test_table));
        printf("GET_CONDITION_ATTRIBUTES_TEST   : (%s)\n\n", AK_rel_eq_cond_attributes(test_cond1));
        //printf("COMMUTE_PROJECTION_SELECTION_TEST : (%i)\n\n", AK_rel_eq_can_commute(list_elem_attr, list_elem_cond));
        printf("COMMUTE_WITH_THETA_JOIN_TEST    : (%s)\n\n", AK_rel_eq_commute_with_theta_join(test_cond1, test_table));
        /**/
    } else {
        printf("...\n");
    }

    Ak_DeleteAll_L(expr);
    //dealocate variables ;)
}

