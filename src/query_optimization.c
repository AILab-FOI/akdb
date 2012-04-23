/**
@file query_optimization.c Provides functions for general query optimization
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

#include "query_optimization.h"

/**
 * @author Dino Laktašić.
 * @brief Print optimization table for testing purposes
 * @param *list_query optimized RA expresion list 
 * @result list output
 */
void AK_print_optimized_query(AK_list *list_query) {
    AK_list_elem list_elem = (AK_list_elem) Ak_First_L(list_query);

    int length;
    int len[] = {strlen("Type"), strlen("Size"), strlen("Data")};
    list_elem = (AK_list_elem) Ak_First_L(list_query);

    printf("==>");
    while (list_elem != NULL) {
        length = AK_chars_num_from_number(list_elem->type, 10);
        if (len[0] < length) {
            len[0] = length;
        }

        length = AK_chars_num_from_number(list_elem->size, 10);
        if (len[1] < length) {
            len[1] = length;
        }

        if (len[2] < strlen(list_elem->data)) {
            len[2] = strlen(list_elem->data);
        }

        if (list_elem->type == TYPE_ATTRIBS || list_elem->type == TYPE_CONDITION) {
            printf("(%s)", list_elem->data);
        } else {
            printf(" %s", list_elem->data);
        }
        list_elem = list_elem->next;
    }

    //7 is number of char | + space in printf
    //set offset to change the box size
    length = len[0] + len[1] + len[2] + 3 * TBL_BOX_OFFSET + 7;

    printf("\n%-*sREL_EQ OPTIMIZED QUERY TABLE", (length / 2) - (strlen("REL_EQ OPTIMIZED QUERY TABLE") / 2), " ");
    printf("\n| %-*s| %-*s| %-*s|\n", len[0] + TBL_BOX_OFFSET, "Type",
            len[1] + TBL_BOX_OFFSET, "Size", len[2] + TBL_BOX_OFFSET, "Data");
    AK_print_row_spacer(len, length);

    list_elem = (AK_list_elem) Ak_First_L(list_query);

    while (list_elem != NULL) {
        printf("\n| %-*i| %-*i| %-*s|\n", len[0] + TBL_BOX_OFFSET, list_elem->type,
                len[1] + TBL_BOX_OFFSET, list_elem->size, len[2] + TBL_BOX_OFFSET, list_elem->data);
        AK_print_row_spacer(len, length);
        list_elem = list_elem->next;
    }
}

/**
 * @author Dino Laktašić.
 * @brief Call and execute relation equivalence
 * /* RELATION EQUIVALENCE RULES FLAGS
 * c - commutation
 * a - associativity
 * p - projection
 * s - selection	
 * @param *list_query RA expresion list where we need to apply relational equivalences rules
 * @param rel_eq rel_eq to execute 
 * @param *FLAGS flags for relation equivalences (execute rel_eq for given flags) 
 * @return returns AK_list (RA expresion list) optimized by given relational equivalence rule 
 */
AK_list *AK_execute_rel_eq(AK_list *list_query, const char rel_eq, const char *FLAGS) {
    Ak_dbg_messg(LOW, REL_EQ, "\nATTEMPT TO EXECUTE '%c' AS RELATIONAL EQUIVALENCE\n", rel_eq);
    Ak_dbg_messg(LOW, REL_EQ, "=================================================\n");

    if (strchr(FLAGS, rel_eq) != NULL) {
        switch (rel_eq) {
            case 'c':
                Ak_dbg_messg(LOW, REL_EQ, "\napply rel_eq_commute.\n");
                //return (AK_list *)AK_rel_eq_comut(list_query);
                break;

            case 'a':
                Ak_dbg_messg(LOW, REL_EQ, "\napply rel_eq_assoc.\n");
                return (AK_list *) AK_rel_eq_assoc(list_query);
                break;

            case 's':
                Ak_dbg_messg(LOW, REL_EQ, "\napply rel_eq_selection.\n");
                return (AK_list *) AK_rel_eq_selection(list_query);
                break;

            case 'p':
                Ak_dbg_messg(LOW, REL_EQ, "\napply rel_eq_projection.\n");
                return (AK_list *) AK_rel_eq_projection(list_query);
                break;

            default:
                Ak_dbg_messg(LOW, REL_EQ, "Invalid relational equivalence flag: %c", rel_eq);
                return list_query;
                break;
        }
    }
}

/**
 * @author Dino Laktašić.
 * @brief Execute all relational equivalences provided by FLAGS (one or more), 
 * if DIFF_PLANS turned on execute permutations without repetition on given RA list from SQL parser output
 * @param *list_query RA expresion list where we need to apply relational equivalences rules
 * @param *FLAGS flags for relation equivalences (execute rel_eq for given flags) 
 * @return returns AK_list (RA expresion list) optimized by all relational equivalence rules provided by FLAGS 
 * (commented code can be edited so AK_list can return the list of lists (lists of different optimization plans),
 * with permutation switched on (DIFF_PLANS = 1) time for execution will be significantly increased
 * Current implementation without uncommenting code doesn't produce list of list, 
 * it rather apply all permutations on the same list
 * 
 * For futher development consider to implement cost estimation for given plan based on 
 * returned heuristicly optimized list
 */
AK_list *AK_query_optimization(AK_list *list_query, const char *FLAGS, const int DIFF_PLANS) {
    int div, num_perms = 1;
    int next_perm, next_flag;
    int len_flags = strlen(FLAGS);

    AK_list *temp = (AK_list *) malloc(sizeof (AK_list));
    Ak_Init_L(temp);

    temp = list_query;

    //Get total permutation without repetition number
    for (next_flag = 1; next_flag <= len_flags; num_perms *= next_flag++);

    //AK_list *temps[num_perms];

    if (num_perms > MAX_PERMUTATION) {
        Ak_dbg_messg(LOW, REL_EQ, "ERROR: max four flags are allowed!\n");
        return temp;
    }

    for (next_perm = 0; next_perm < num_perms; next_perm++) {
        char *perm = (char *) calloc(len_flags, sizeof (char));
        memcpy(perm, FLAGS, len_flags);

        Ak_dbg_messg(LOW, REL_EQ, "\n\t==============================\n\t\t%i. LOGIC PLAN\n\t==============================\n", next_perm + 1);
        for (next_flag = len_flags, div = num_perms; next_flag > 0; next_flag--) {
            div /= next_flag;
            int index = (next_perm / div) % next_flag;

            //if (DIFF_PLANS) {
            //	temps[next_perm] = (AK_list *)malloc(sizeof(AK_list));
            //	InitL(temps[next_perm]);
            //	temps[next_perm] = AK_execute_rel_eq(temp, perm[index], FLAGS);
            //AK_print_optimized_query(temps[next_perm]);
            //} else {
            temp = AK_execute_rel_eq(temp, perm[index], FLAGS);
            //AK_print_optimized_query(temp);
            //}

            memmove(perm + index, perm + index + 1, len_flags - index);
        }

        if (!DIFF_PLANS) {
            break;
        }

        free(perm);
    }

    if (DIFF_PLANS) {
        Ak_dbg_messg(LOW, REL_EQ, "\nTOTAL REL_EQ OPTIMIZED PLANS: %i\n", num_perms);
        //temp = (AK_list *)realloc(temp, num_perms * sizeof(temps));
        //memcpy(temp, temps, sizeof(temps) * num_perms);
    }

    Ak_DeleteAll_L(list_query);

    return temp;
}
/**
  * @author Dino Laktašić
  * @param *list_query query to be optimized
  * @return No return value
  */
void AK_query_optimization_test(AK_list *list_query) {
    printf("query_optimization.c: Present!\n");
    printf("\n********** QUERY OPTIMIZATION TEST by Dino Laktašić **********\n");

    //Init list and insert elements (Query parser output)
    AK_list *expr = (AK_list *) malloc(sizeof (AK_list));
    Ak_Init_L(expr);

    //*Commutativity of Selection and Projection
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "s", sizeof ("s"), expr);
    Ak_InsertAtEnd_L(TYPE_CONDITION, "`firstname` = 'Dino'", sizeof ("`firstname` = 'Dino'"), expr);
    //...
    //*Cascade of Projection p[L1](p[L2](...p[Ln](R)...)) = p[L1](R)
    //[L1,...] < [L2,...] < [...,Ln-1,Ln]
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "p", sizeof ("p"), expr);
    Ak_InsertAtEnd_L(TYPE_ATTRIBS, "firstname;lastname", sizeof ("firstname;lastname"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "p", sizeof ("p"), expr);
    Ak_InsertAtEnd_L(TYPE_ATTRIBS, "firstname;lastname;year", sizeof ("firstname;lastname;year"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "student", sizeof ("student"), expr);
    //---------------------------------------------------------------------------------------------------------
    //*/

    //*Cascade of Selection and Commutativity of Selection
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "i", sizeof ("u"), expr); //u, i, e
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "s", sizeof ("s"), expr);
    Ak_InsertAtEnd_L(TYPE_CONDITION, "`year` 2008 <", sizeof ("`year` 2008 <"), expr);
    //...
    //*Commutativity of Selection and set operations (Union, Intersection, and Set difference)
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "s", sizeof ("s"), expr);
    Ak_InsertAtEnd_L(TYPE_CONDITION, "`mbr` 35895 < `id_prof` 35897 < OR", sizeof ("`mbr` 35895 < `id_prof` 35897 < OR"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "professor", sizeof ("professor"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "student", sizeof ("student"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "u", sizeof ("u"), expr); //u, i, e
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "i", sizeof ("i"), expr); //u, i, e
    //---------------------------------------------------------------------------------------------------------
    //*/

    //*Commutativity of Selection and Theta join (or Cartesian product)
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "s", sizeof ("s"), expr);
    Ak_InsertAtEnd_L(TYPE_CONDITION, "`name` 'FOBP' = `firstname` 'Alen' = AND `lastname` 'Lovrencic' = OR", sizeof ("`name` 'FOBP' = `firstname` 'Alen' = AND `lastname` 'Lovrencic' = OR"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "professor", sizeof ("professor"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "course", sizeof ("course"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "t", sizeof ("t"), expr);
    Ak_InsertAtEnd_L(TYPE_CONDITION, "`name` 'FOBP' = `firstname` 'Alen' = AND `lastname` 'Lovrencic' = OR", sizeof ("`name` 'FOBP' = `firstname` 'Alen' = AND `lastname` 'Lovrencic' = OR"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "e", sizeof ("e"), expr); //u, i, e
    //*/

    //*Commutativity of Projection and Theta join (or Cartesian product)
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "p", sizeof ("p"), expr);
    Ak_InsertAtEnd_L(TYPE_ATTRIBS, "mbr;id_prof", sizeof ("mbr;id_prof"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "professor", sizeof ("professor"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "student", sizeof ("student"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "t", sizeof ("t"), expr);
    Ak_InsertAtEnd_L(TYPE_CONDITION, "`mbr` 35891 > `id_prof` 35897 < AND", sizeof ("`mbr` 35891 > `id_prof` 35897 < AND"), expr);
    //*/

    //*Associativity of natural joins
    Ak_InsertAtEnd_L(TYPE_OPERAND, "professor", sizeof ("professor"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "student", sizeof ("student"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "n", sizeof ("n"), expr);
    Ak_InsertAtEnd_L(TYPE_ATTRIBS, "mbr;id_prof", sizeof ("mbr;id_prof"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "course", sizeof ("course"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "n", sizeof ("n"), expr);
    Ak_InsertAtEnd_L(TYPE_ATTRIBS, "id_course", sizeof ("id_course"), expr);
    //*/

    //*Associativity of union and intersection
    Ak_InsertAtEnd_L(TYPE_OPERAND, "professor", sizeof ("professor"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "student", sizeof ("student"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "u", sizeof ("u"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "course", sizeof ("course"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "u", sizeof ("u"), expr);
    //*/

    //*Associativity of theta-joins
    Ak_InsertAtEnd_L(TYPE_OPERAND, "professor", sizeof ("professor"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "student", sizeof ("student"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "t", sizeof ("t"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "`mbr` 35891 > `id_prof` 35897 <", sizeof ("`mbr` 35891 > `id_prof` 35897 <"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERAND, "course", sizeof ("course"), expr);
    Ak_InsertAtEnd_L(TYPE_OPERATOR, "t", sizeof ("t"), expr);
    Ak_InsertAtEnd_L(TYPE_CONDITION, "`id_course` 7 < `mbr` 35891 > AND", sizeof ("`id_course` 7 < `mbr` 35891 > AND"), expr);
    //*/

    time_t start = clock();
    AK_print_optimized_query(AK_query_optimization(expr, "aps", 1));
    time_t end = clock();
    printf("\n\nLOGIC PLAN GENERATED IN: %d μs, %d s\n", end - start, (end - start) / 1000000);

    if (DEBUG_ALL) {
        printf("\n------------------> TEST_REL_EQ_FUNCTIONS <------------------\n\n");

        char *test_attribs;
        char *test_cond1, *test_cond2;
        char *test_table;
        char *cond_attr1, *cond_attr2;

        test_table = "professor";
        test_attribs = "id_prof;firstname";
        test_cond1 = "`id_prof` 35897 < `category` '10' = AND";
        test_cond2 = "`mbr` 35891 > `id_prof` 35895 < OR";

        cond_attr1 = (char *) AK_rel_eq_cond_attributes(test_cond1);
        cond_attr2 = (char *) AK_rel_eq_cond_attributes(test_cond2);

        //Initialize list elements...
        //AK_list_elem list_elem_set, list_elem_subset;
        //AK_list_elem list_elem_cond, list_elem_attr;

        printf("Test table: (%s) | Test attributes: (%s)\n", test_table, test_attribs);
        printf("Test first condition       : (%s)\n", test_cond1);
        printf("Test second condition      : (%s)\n\n", test_cond2);
        printf("First condition attributes : (%s)\n", cond_attr1);
        printf("Second condition attributes: (%s)\n", cond_attr2);
        printf("-------------------------------------------------------------\n\n");

        //printf("IS_SET_SUBSET_OF_LARGER_SET_TEST  : (%i)\n\n", AK_rel_eq_is_subset(list_elem_set, list_elem_subset));
        //printf("COMMUTE_PROJECTION_SELECTION_TEST : (%i)\n\n", AK_rel_eq_can_commute(list_elem_attr, list_elem_cond));
        ////printf("GET_TABLE_ATTRIBUTES_TEST       : (%s)\n\n", (AK_rel_eq_get_attributes(test_table))->data);
        //printf("GET_PROJECTION_ATTRIBUTES_TEST    : (%s)\n\n", AK_rel_eq_projection_attributes(test_attribs, test_table));
        //printf("GET_ATTRIBUTES_FROM_CONDITION_TEST: (%s)\n\n", AK_rel_eq_collect_cond_attributes(list_elem_cond));
        //printf("REMOVE_DUPLICATE_ATTRIBUTES_TEST  : (%s)\n", AK_rel_eq_remove_duplicates(test_attribs));

        printf("IS_SET_SUBSET_OF_LARGER_SET_TEST: (%i)\n\n", AK_rel_eq_is_attr_subset(cond_attr1, cond_attr2));
        printf("GET_ALL_TABLE_ATTRIBUTES_TEST   : (%s)\n\n", (char *) AK_rel_eq_get_atrributes_char(test_table));
        printf("GET_CONDITION_ATTRIBUTES_TEST   : (%s)\n\n", (char *) AK_rel_eq_cond_attributes(test_cond1));
        printf("COMMUTE_WITH_THETA_JOIN_TEST    : (%s)\n\n", (char *) AK_rel_eq_commute_with_theta_join(test_cond1, test_table));
        printf("CONDITION_SHARE_ATTRIBUTES_TEST : (%i)\n", AK_rel_eq_share_attributes(cond_attr1, cond_attr2));
        /**/
    } else {
        printf("\n...\n");
    }

    Ak_DeleteAll_L(expr);
}
