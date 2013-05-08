/**
 *
@file expression_check.c Provides functions for constraint checking used in selection and theta-join
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

/**
 * @author Dino Laktašić, abstracted by Tomislav Mikulček
 * @brief  Function compares values according to their data type, checks aritmetic statement which is part of expression given in   	       the function below. For every type of arithmetic operator, there is switch-case statement which examines type of el and
           casts void operands to this type.
 * @param el list element, last element put in list temp which holds elements of row ordered according to expression and results of 		     their evaluation
 * @param *op comparison operator 
 * @param *a left operand
 * @param *b right operand
 * @return 0 if arithmetic statement is false, 1 if arithmetic statement is true
 */

#include "expression_check.h"

int AK_check_arithmetic_statement(AK_list_elem el, const char *op, const char *a, const char *b) {
    
    char **numericStringEnd; //A pointer to the location where the numeric part of the string ends

	if(strcmp(op, "<") == 0){

		switch (el->type) {

			case TYPE_INT:
				return *(int *) a < *(int *) b;
			case TYPE_FLOAT:
				return *((float *) a) < *((float *) b);
			case TYPE_NUMBER:
				return *((double *) a) < *((double *) b);
			case TYPE_VARCHAR:
				return strcmp((const char *) a, (const char *) b) < 0;

		}
	}else if(strcmp(op, ">") == 0){

//		switch (el->type) {
//
//			case TYPE_INT:
//				return *(int *) a > *(int *) b;
//			case TYPE_FLOAT:
//				return *((float *) a) > *((float *) b);
//			case TYPE_NUMBER:
//				return *((double *) a) > *((double *) b);
//			case TYPE_VARCHAR:
//				return strcmp((const char *) a, (const char *) b) > 0;
//
//		}
            
		switch (el->type) {

			case TYPE_INT:
				return strtol(a, numericStringEnd, 10)  > strtol(b, numericStringEnd, 10);
			case TYPE_FLOAT:
				return strtod(a, numericStringEnd) > strtod(b, numericStringEnd);
			case TYPE_NUMBER:
				return strtod(a, numericStringEnd) > strtod(b, numericStringEnd);
			case TYPE_VARCHAR:
				return strcmp((const char *) a, (const char *) b) > 0;

		}
            
            
	}else if(strcmp(op, "<=") == 0){

		switch (el->type) {

			case TYPE_INT:
				return strtol(a, numericStringEnd, 10)  <= strtol(b, numericStringEnd, 10);
			case TYPE_FLOAT:
				return strtod(a, numericStringEnd) <= strtod(b, numericStringEnd);
			case TYPE_NUMBER:
				return strtod(a, numericStringEnd) <= strtod(b, numericStringEnd);
			case TYPE_VARCHAR:
				return strcmp((const char *) a, (const char *) b) <= 0;
		}
	}else if(strcmp(op, ">=") == 0){

		switch (el->type) {

			case TYPE_INT:
				return strtol(a, numericStringEnd, 10)  >= strtol(b, numericStringEnd, 10);
			case TYPE_FLOAT:
				return strtod(a, numericStringEnd) >= strtod(b, numericStringEnd);
			case TYPE_NUMBER:
				return strtod(a, numericStringEnd) >= strtod(b, numericStringEnd);
			case TYPE_VARCHAR:
				return strcmp((const char *) a, (const char *) b) >= 0;
		}
	}else if(strcmp(op, "+") == 0){

		switch (el->type) {

			case TYPE_INT:
				return strtol(a, numericStringEnd, 10) + strtol(b, numericStringEnd, 10);
			case TYPE_FLOAT:
				return strtod(a, numericStringEnd) + strtod(b, numericStringEnd);
			case TYPE_NUMBER:
				return strtod(a, numericStringEnd) + strtod(b, numericStringEnd);
			case TYPE_VARCHAR:
				return EXIT_ERROR;
//				return (int)memcpy((char *)a + sizeof(a), b, sizeof(b));
		}
	}else if(strcmp(op, "-") == 0){

		switch (el->type) {

			case TYPE_INT:
				return strtol(a, numericStringEnd, 10) - strtol(b, numericStringEnd, 10);
			case TYPE_FLOAT:
				return strtod(a, numericStringEnd) - strtod(b, numericStringEnd);
			case TYPE_NUMBER:
				return strtod(a, numericStringEnd) - strtod(b, numericStringEnd);
			case TYPE_VARCHAR:
				return EXIT_ERROR;
		}
	}else if(strcmp(op, "*") == 0){

		switch (el->type) {

			case TYPE_INT:
				return strtol(a, numericStringEnd, 10) * strtol(b, numericStringEnd, 10);
			case TYPE_FLOAT:
				return strtod(a, numericStringEnd) * strtod(b, numericStringEnd);
			case TYPE_NUMBER:
				return strtod(a, numericStringEnd) * strtod(b, numericStringEnd);
			case TYPE_VARCHAR:
				return EXIT_ERROR;
		}
	}else if(strcmp(op, "/") == 0){

		switch (el->type) {

			case TYPE_INT:
				return strtol(a, numericStringEnd, 10) / strtol(b, numericStringEnd, 10);
			case TYPE_FLOAT:
				strtod(a, numericStringEnd) / strtod(b, numericStringEnd);
			case TYPE_NUMBER:
				strtod(a, numericStringEnd) / strtod(b, numericStringEnd);
			case TYPE_VARCHAR:
				return EXIT_ERROR;
		}
	}
}

/**
 * @author Matija Šestak, updated by Dino Laktašić, abstracted by Tomislav Mikulček
 * @brief  Function evaluates whether one record (row) satisfies logical expression. It goes through
           given row. If it comes to logical operator, it evaluates by itself. For arithmetic operators
           function AK_check_arithmetic_statement() is called.
 * @param row_root beginning of the row that is to be evaluated
 * @param *expr list with the logical expression in postfix notation
 * @result 0 if row does not satisfy, 1 if row satisfies expression
 */
int AK_check_if_row_satisfies_expression(AK_list_elem row_root, AK_list *expr) {
    if (expr == 0) return 1;
	int true = 1, false = 0;
    int found, result;

    AK_list *temp = (AK_list *) malloc(sizeof (AK_list));
    Ak_Init_L(temp);

    AK_list_elem el = (AK_list_elem) Ak_First_L(expr);
    AK_list_elem row;
    AK_list_elem a, b;

    char data[MAX_VARCHAR_LENGTH];

    while (el) {
     
        if (el->type == TYPE_ATTRIBS) {

            found = 0;
            row = row_root;

            while (row) {

                if (strcmp(el->data, row->attribute_name) == 0) {
                    found = 1;
                    break;
                }
                row = row->next;
            }

            if (!found) {
            	Ak_dbg_messg(MIDDLE, REL_OP, "Expression ckeck was not able to find column: %s\n", el->data);
                return 0;

            } else {

                int type = row->type;
                memset(data, 0, MAX_VARCHAR_LENGTH);
                memcpy(data, &row->data, sizeof(row->data));
                Ak_InsertAtEnd_L(type, data, sizeof(row->data), temp);
            }

        } else if (el->type == TYPE_OPERATOR) {
            //operators implementation
            b = (AK_list_elem) Ak_End_L(temp);
            a = (AK_list_elem) Ak_Previous_L(b, temp);

            if (strcmp(el->data, "=") == 0) {
                if (memcmp(a->data, b->data, sizeof(a->type)) == 0)
                	Ak_InsertAtEnd_L(TYPE_INT, &true, sizeof (int), temp);
                else
                	Ak_InsertAtEnd_L(TYPE_INT, &false, sizeof (int), temp);

            } else if (strcmp(el->data, "<>") == 0) {
                if (memcmp(a->data, b->data, a->size) != 0)
                	Ak_InsertAtEnd_L(TYPE_INT, &true, sizeof (int), temp);
                else
                	Ak_InsertAtEnd_L(TYPE_INT, &false, sizeof (int), temp);

            } else if (strcmp(el->data, "OR") == 0) {
                int val_a, val_b;
                memcpy(&val_a, a->data, sizeof (int));
                memcpy(&val_b, b->data, sizeof (int));

                if (val_a || val_b)
                	Ak_InsertAtEnd_L(TYPE_INT, &true, sizeof (int), temp);
                else
                	Ak_InsertAtEnd_L(TYPE_INT, &false, sizeof (int), temp);

            } else if (strcmp(el->data, "AND") == 0) {
                int val_a, val_b;
                memcpy(&val_a, a->data, sizeof (int));
                memcpy(&val_b, b->data, sizeof (int));

                if (val_a && val_b)
                	Ak_InsertAtEnd_L(TYPE_INT, &true, sizeof (int), temp);
                else
                	Ak_InsertAtEnd_L(TYPE_INT, &false, sizeof (int), temp);

            } else {

                int rs;
                rs = AK_check_arithmetic_statement(b, el->data, a->data, b->data);
                Ak_InsertAtEnd_L(TYPE_INT, &rs, sizeof (int), temp);
            }

            Ak_Delete_L(a, temp);
            Ak_Delete_L(b, temp);

        } else {
        	Ak_InsertAtEnd_L(el->type, el->data, el->size, temp);
        }
        el = el->next;
    }

    memcpy(&result, ((AK_list_elem) Ak_First_L(temp))->data, sizeof (int));
    Ak_DeleteAll_L(temp);
    free(temp);

    return result;
}

void Ak_expression_check_test()
{
    AK_list_elem elem = malloc(sizeof(AK_list_elem));
    elem->type = TYPE_INT;
    const char *op = "+";
    const char *a = "800";
    const char *b = "400";
    int outcome;
    
    outcome = AK_check_arithmetic_statement(elem, op, a, b);
    
    printf("The outcome is: %d", outcome);
    
    free(elem);
    
}
