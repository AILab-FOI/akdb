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

#define MAX_PERMUTATION 24
#define MAX_TABLE_BOX_OFFSET 4

void AK_print_row_spacer(int col_len[], int length, int offset) {
	int i, j, col, temp;
	
	j = col = temp = 0;
	
	for(i = 0; i < length; i++) {
		if (!i || i == temp + j) {
			j += offset;
			temp += col_len[col++] + 1; 
			printf("+");
		} else {
			printf("-");
		}
	}
}

int AK_chars_num_from_number(int number, int base) {
	int len = 0;
	
	do {
		len++;
	} while ((double)(number /= base) > 0);
	
	return len;
}

void AK_print_optimized_query(AK_list *list_query) {
	AK_list_elem list_elem = (AK_list_elem)FirstL(list_query);
	
	int offset = MAX_TABLE_BOX_OFFSET;
	int len[] = {strlen("Type"), strlen("Size"), strlen("Data")};   
	list_elem = (AK_list_elem)FirstL(list_query);
		
	while (list_elem != NULL) {
		if (len[0] < AK_chars_num_from_number(list_elem->type, 10)) {
			len[0] = AK_chars_num_from_number(list_elem->type, 10);		
		}

		if (len[1] < AK_chars_num_from_number(list_elem->size, 10)) {
			len[1] = AK_chars_num_from_number(list_elem->size, 10);		
		}

		if (len[2] < strlen(list_elem->data)) {
			len[2] = strlen(list_elem->data);			
		}
		list_elem = list_elem->next;
	}
	
	//4 is number of char | in printf
	//set offset to change the box size
	int length = len[0] + len[1] + len[2] + 3 * offset + 4;

	printf("\n%-*sREL_EQ OPTIMIZED QUERY TABLE", (length / 2) - (strlen("REL_EQ OPTIMIZED QUERY TABLE") / 2), " ");
	printf("\n|%-*s|%-*s|%-*s|\n", len[0] + offset, "Type", 
			len[1] + offset, "Size", len[2] + offset, "Data");		
	AK_print_row_spacer(len, length, offset);

	list_elem = (AK_list_elem)FirstL(list_query);
		
	while (list_elem != NULL) {
		printf("\n|%-*i|%-*i|%-*s|\n", len[0] + offset, list_elem->type, 
				len[1] + offset,  list_elem->size, len[2] + offset, list_elem->data);
		AK_print_row_spacer(len, length, offset);
		list_elem = list_elem->next;
	}
}

/* RELATION EQUIVALENCE RULES FLAGS
 * c - commutation
 * a - associativity
 * p - projection
 * s - selection	
 */
AK_list *AK_execute_rel_eq(AK_list *list_query, const char rel_eq, const char *FLAGS) {	
	printf("\nATTEMPT TO EXECUTE '%c' AS RELATIONAL EQUIVALENCE\n", rel_eq);
	printf("=================================================\n");
	
	if (strchr(FLAGS, rel_eq) != NULL) {
		switch (rel_eq) {
			case 'c':
				printf("\napply rel_eq_commute.\n");
				//return (AK_list *)AK_rel_eq_comut(list_query);
				break;
						
			case 'a':
				printf("\napply rel_eq_assoc.\n");
				return (AK_list *)AK_rel_eq_assoc(list_query);
				break;

			case 's':
				printf("\napply rel_eq_selection.\n");
				return (AK_list *)AK_rel_eq_selection(list_query);
				break;
				
			case 'p':
				printf("\napply rel_eq_projection.\n");
				return (AK_list *)AK_rel_eq_projection(list_query);	
				break;
				
			default: 
				printf("Invalid relational equivalence flag: %c", rel_eq);
				return list_query;
				break;
		}
	}
}

AK_list *AK_query_optimization(AK_list *list_query, const char *FLAGS, const int DIFF_PLANS) {
	int div, num_perms = 1;
	int next_perm, next_flag;
	int len_flags = strlen(FLAGS);
	
	AK_list *temp = (AK_list *)malloc(sizeof(AK_list));
	InitL(temp);
	
	temp = list_query;
	
	//Get total permutation without repetition number
	for (next_flag = 1; next_flag <= len_flags; num_perms *= next_flag++);
	
	//AK_list *temps[num_perms];
	
	if (num_perms > MAX_PERMUTATION) {
		printf("ERROR: max four flags are allowed!\n");
		return temp;
	}
	
	for (next_perm = 0; next_perm < num_perms; next_perm++) {
		char *perm = (char *)calloc(len_flags, sizeof(char));
		memcpy(perm, FLAGS, len_flags);
		
		printf("\n\t==============================\n\t\t%i. LOGIC PLAN\n\t==============================\n", next_perm + 1);
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
		printf("\nTOTAL REL_EQ OPTIMIZED PLANS: %i\n", num_perms);
		//temp = (AK_list *)realloc(temp, num_perms * sizeof(temps));
		//memcpy(temp, temps, sizeof(temps) * num_perms);
	}

	DeleteAllL(list_query);
	
	return temp;
}

void query_optimization_test(AK_list *list_query) {
	printf("query_optimization.c: Present!\n");
	printf( "\n********** QUERY OPTIMIZATION TEST by Dino Laktašić **********\n");
	
	int t_id = 0;
    char *tblName[3];
	AK_header *temp[3];
	
	//---------------------------------------> CREATE TABLE 'PROFESOR' <---------------------------------------
    //create header
    AK_header t_header[MAX_ATTRIBUTES];
  
    temp[t_id] = (AK_header*) AK_create_header("id_prof", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header, temp[t_id], sizeof(AK_header));
    temp[t_id] = (AK_header*) AK_create_header("firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 1, temp[t_id], sizeof(AK_header));
    temp[t_id] = (AK_header*) AK_create_header("lastname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 2, temp[t_id], sizeof(AK_header));
	temp[t_id] = (AK_header*) AK_create_header("profession", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 3, temp[t_id], sizeof(AK_header));
    temp[t_id] = (AK_header*) AK_create_header("years", TYPE_FLOAT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 4, temp[t_id], sizeof(AK_header));
	temp[t_id] = (AK_header*) AK_create_header("email", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header + 5, temp[t_id], sizeof(AK_header));
	memset(t_header + 6, '\0', MAX_ATTRIBUTES - 6);
	
	//create table
	tblName[t_id] = "profesor";
    
	int startAddress = AK_initialize_new_segment(tblName[t_id], SEGMENT_TYPE_TABLE, t_header);
    
    if (startAddress != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName[t_id]);
    
    printf("rel_eq_selection_test: After segment initialization: %d\n", AK_num_attr(tblName[t_id++]));
	
	//---------------------------------------> CREATE TABLE 'STUDENT' <---------------------------------------
	//create header
    AK_header t_header2[MAX_ATTRIBUTES];

    temp[t_id] = (AK_header*) AK_create_header("id_stud", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2, temp[t_id], sizeof(AK_header));
    temp[t_id] = (AK_header*) AK_create_header("firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 1, temp[t_id], sizeof(AK_header));
    temp[t_id] = (AK_header*) AK_create_header("lastname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 2, temp[t_id], sizeof(AK_header));
	temp[t_id] = (AK_header*) AK_create_header("status", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 3, temp[t_id], sizeof(AK_header));
    temp[t_id] = (AK_header*) AK_create_header("avg_grade", TYPE_FLOAT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header2 + 4, temp[t_id], sizeof(AK_header));
	memset(t_header2 + 5, '\0', MAX_ATTRIBUTES - 5);
    
	//create table
	tblName[t_id] = "student";
    
    int startAddress2 = AK_initialize_new_segment(tblName[t_id], SEGMENT_TYPE_TABLE, t_header2);
    
    if (startAddress2 != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName[t_id]);
    
    printf("rel_eq_selection_test: After segment initialization: %d\n", AK_num_attr(tblName[t_id++]));
	
	//---------------------------------------> CREATE TABLE 'COURSE' <---------------------------------------
	//create header
    AK_header t_header3[MAX_ATTRIBUTES];

    temp[t_id] = (AK_header*) AK_create_header("id_course", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3, temp[t_id], sizeof(AK_header));
    temp[t_id] = (AK_header*) AK_create_header("name", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3 + 1, temp[t_id], sizeof(AK_header));
    temp[t_id] = (AK_header*) AK_create_header("category", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3 + 2, temp[t_id], sizeof(AK_header));
	temp[t_id] = (AK_header*) AK_create_header("lecturer", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3 + 3, temp[t_id], sizeof(AK_header));
    temp[t_id] = (AK_header*) AK_create_header("active", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy(t_header3 + 4, temp[t_id], sizeof(AK_header));
	memset(t_header3 + 5, '\0', MAX_ATTRIBUTES - 5);
    
	//create table
	tblName[t_id] = "course";
    
    int startAddress3 = AK_initialize_new_segment(tblName[t_id], SEGMENT_TYPE_TABLE, t_header3);
    
    if (startAddress3 != EXIT_ERROR)
        printf("\nTABLE %s CREATED!\n", tblName[t_id]);
    
    printf("rel_eq_selection_test: After segment initialization: %d\n", AK_num_attr(tblName[t_id++]));
	//------------------------------------------------------------------------------------------------------
	
	//Init list and insert elements (Query parser output)
	AK_list *expr = (AK_list *)malloc(sizeof(AK_list));
    InitL(expr);
	
	//*Commutativity of Selection and Projection
	InsertAtEndL( TYPE_OPERATOR, "s", sizeof("s"), expr );
	InsertAtEndL( TYPE_CONDITION, "`firstname` = 'Dino'", sizeof("`firstname` = 'Dino'"), expr );
	//...
	//*Cascade of Projection p[L1](p[L2](...p[Ln](R)...)) = p[L1](R)
	//[L1,...] < [L2,...] < [...,Ln-1,Ln]
	InsertAtEndL( TYPE_OPERATOR, "p", sizeof("p"), expr );
    InsertAtEndL( TYPE_ATTRIBS, "firstname;lastname", sizeof("firstname;lastname"), expr );
	InsertAtEndL( TYPE_OPERATOR, "p", sizeof("p"), expr );
    InsertAtEndL( TYPE_ATTRIBS, "firstname;lastname;avg_grade", sizeof("firstname;lastname;avg_grade"), expr );
	InsertAtEndL( TYPE_OPERAND, "student", sizeof("student"), expr );
	//---------------------------------------------------------------------------------------------------------
	//*/
	
	//*Cascade of Selection and Commutativity of Selection
	InsertAtEndL( TYPE_OPERATOR, "i", sizeof("u"), expr ); //u, i, e
	InsertAtEndL( TYPE_OPERATOR, "s", sizeof("s"), expr );
	InsertAtEndL( TYPE_CONDITION, "`avg_grade` 4.5 <", sizeof("`avg_grade` 4.5 <"), expr );
	//...
	//*Commutativity of Selection and set operations (Union, Intersection, and Set difference)
    InsertAtEndL( TYPE_OPERATOR, "s", sizeof("s"), expr );
	InsertAtEndL( TYPE_CONDITION, "`id_stud` 100 > `id_prof` 50 < OR", sizeof("`id_stud` 100 > `id_prof` 50 < OR"), expr );
    InsertAtEndL( TYPE_OPERAND, "profesor", sizeof("profesor"), expr );
    InsertAtEndL( TYPE_OPERAND, "student", sizeof("student"), expr );
	InsertAtEndL( TYPE_OPERATOR, "u", sizeof("u"), expr ); //u, i, e
	InsertAtEndL( TYPE_OPERATOR, "i", sizeof("i"), expr ); //u, i, e
	//---------------------------------------------------------------------------------------------------------
	//*/
	
	//*Commutativity of Selection and Theta join (or Cartesian product)
	InsertAtEndL( TYPE_OPERATOR, "s", sizeof("s"), expr );
    InsertAtEndL( TYPE_CONDITION, "`firstname` 10 < `category` 'Algorithms' AND", sizeof("`firstname` 10 < `category` 'Algorithms' AND"), expr );
	InsertAtEndL( TYPE_OPERAND, "profesor", sizeof("profesor"), expr );
    InsertAtEndL( TYPE_OPERAND, "course", sizeof("course"), expr );
	InsertAtEndL( TYPE_OPERATOR, "t", sizeof("t"), expr );
	InsertAtEndL( TYPE_CONDITION, "`category` `id_prof` =", sizeof("`category` `id_prof` ="), expr );
	InsertAtEndL( TYPE_OPERATOR, "e", sizeof("e"), expr ); //u, i, e
	//*/
	
	//*Commutativity of Projection and Theta join (or Cartesian product)
	InsertAtEndL( TYPE_OPERATOR, "p", sizeof("p"), expr );
    InsertAtEndL( TYPE_ATTRIBS, "id_stud;id_prof", sizeof("id_stud;id_prof"), expr );
	InsertAtEndL( TYPE_OPERAND, "profesor", sizeof("profesor"), expr );
    InsertAtEndL( TYPE_OPERAND, "student", sizeof("student"), expr );
	InsertAtEndL( TYPE_OPERATOR, "t", sizeof("t"), expr );
	InsertAtEndL( TYPE_CONDITION, "`id_stud` 10 > `id_prof` 5 < AND", sizeof("`id_stud` 10 > `id_prof` 5 < AND"), expr );
	//*/
	
	//*Associativity of natural joins
	InsertAtEndL( TYPE_OPERAND, "profesor", sizeof("profesor"), expr );
	InsertAtEndL( TYPE_OPERAND, "student", sizeof("student"), expr );
    InsertAtEndL( TYPE_OPERATOR, "n", sizeof("n"), expr );
	InsertAtEndL( TYPE_ATTRIBS, "id_stud;id_prof", sizeof("id_stud;id_prof"), expr );
    InsertAtEndL( TYPE_OPERAND, "course", sizeof("course"), expr );
	InsertAtEndL( TYPE_OPERATOR, "n", sizeof("n"), expr );
	InsertAtEndL( TYPE_ATTRIBS, "id_course", sizeof("id_course"), expr );
	//*/
	
	//*Associativity of union and intersection
	InsertAtEndL( TYPE_OPERAND, "profesor", sizeof("profesor"), expr );
	InsertAtEndL( TYPE_OPERAND, "student", sizeof("student"), expr );
    InsertAtEndL( TYPE_OPERATOR, "u", sizeof("u"), expr );
    InsertAtEndL( TYPE_OPERAND, "course", sizeof("course"), expr );
	InsertAtEndL( TYPE_OPERATOR, "u", sizeof("u"), expr );
	//*/
	
	//*Associativity of theta-joins
	InsertAtEndL( TYPE_OPERAND, "profesor", sizeof("profesor"), expr );
	InsertAtEndL( TYPE_OPERAND, "student", sizeof("student"), expr );
    InsertAtEndL( TYPE_OPERATOR, "t", sizeof("t"), expr );
	InsertAtEndL( TYPE_OPERAND, "`id_stud` 5 > `id_prof` 10 <", sizeof("`id_stud` 5 > `id_prof` 10 <"), expr );
    InsertAtEndL( TYPE_OPERAND, "course", sizeof("course"), expr );
	InsertAtEndL( TYPE_OPERATOR, "t", sizeof("t"), expr );
	InsertAtEndL( TYPE_CONDITION, "`id_course` 7 < `id_stud` 10 >", sizeof("`id_course` 7 < `id_stud` 10 >"), expr );
	//*/
	
	
	time_t start = clock();
	AK_print_optimized_query(AK_query_optimization(expr, "aps", 1));
	time_t end = clock();
    printf( "\n\nLOGIC PLAN GENERATED IN: %d μs, %d s\n", end - start, (end - start) / 1000000);
	
	if (DEBUG) {
		printf("\n------------------> TEST_REL_EQ_FUNCTIONS <------------------\n\n");
		
		char *test_attribs;	
		char *test_cond1, *test_cond2;
		char *test_table;
		char *cond_attr1, *cond_attr2;

		test_table = "profesor";
		test_attribs = "id_prof;firstname";
		test_cond1 = "`id_prof` 10 < `category` '10' = AND";
		test_cond2 = "`id_stud` 100 > `id_prof` 50 < OR";
		
		cond_attr1 = AK_rel_eq_cond_attributes(test_cond1);
		cond_attr2 = AK_rel_eq_cond_attributes(test_cond2);
		
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
		printf("GET_ALL_TABLE_ATTRIBUTES_TEST   : (%s)\n\n", AK_rel_eq_get_atrributes_char(test_table));
		printf("GET_CONDITION_ATTRIBUTES_TEST   : (%s)\n\n", AK_rel_eq_cond_attributes(test_cond1));
		printf("COMMUTE_WITH_THETA_JOIN_TEST    : (%s)\n\n", AK_rel_eq_commute_with_theta_join(test_cond1, test_table));
		printf("CONDITION_SHARE_ATTRIBUTES_TEST : (%i)\n", AK_rel_eq_share_attributes(cond_attr1, cond_attr2));
		/**/
	} else {
		printf("\n...\n");
	}
	
	DeleteAllL(expr);
}
