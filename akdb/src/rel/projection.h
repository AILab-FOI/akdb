/**
@file projection.h Header file that provides data structures, functions and defines for relational projection operation
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
 
#ifndef PROJECTION
#define PROJECTION

#include "../auxi/test.h"
#include "expression_check.h"
#include "../file/table.h"
#include "../file/fileio.h"
#include "../auxi/mempro.h"

 struct AK_operand {
	char value[MAX_VARCHAR_LENGTH];
	int type;
};


/*
void AK_create_block_header(int old_block, char *dstTable, AK_list *att);
void AK_copy_block_projection(AK_block *old_block, AK_list *att, char *dstTable);
int AK_projection(char *srcTable, char *dstTable, AK_list *att);
*/

/**
 * @author Matija Novak, rewritten and optimized by Dino Laktašić to support AK_list  
 * @brief  Function that creates a new header for the projection table
 * @param old_block_add address of the block from which we copy headers we need
 * @param dstTable name of the new table
 * @param att list of the attributes which should the projeciton table contain
 * @return No return value
 */
void AK_create_block_header(int old_block, char *dstTable, struct list_node *att);

/** 
  * @author Matija Novak, rewrited and optimized by Dino Laktašić to support AK_list
  * @brief  Function that copies the data from old table block to the new projection table
  * @param old_block block from which we copy data
  * @param dstTable name of the new table
  * @param att list of the attributes which should the projeciton table contain
  * @retrun No return value
 */
void AK_copy_block_projection(AK_block *old_block, struct list_node *att, char *dstTable, struct list_node *expr);
int AK_projection(char *srcTable, char *dstTable, struct list_node *att, struct list_node *expr);

/**
    * @Author Leon Palaić
    * @brief Determines the new header type
    * @param a operand type
    * @param b operand type
    * @result header type

*/
int AK_determine_header_type(int a,int b);

/**
    * @Author Leon Palaić
    * @brief  Function that creates new header name from passed operand names and operator
    * @param first operand name
    * @param second operand name
    * @param operator 
    * @result new sequence of charachters

*/
char *AK_create_header_name(char * first,char *operator, char * second);

/**
    * @Author Leon Palaić
    * @brief Function that performes arithmetics operation on operand data
    * @param ab first operand
    * @param bb second operand 
    * @param op operator 
    * @result result of arithmetics

*/
char *AK_perform_operatrion(char *op,struct AK_operand *a, struct AK_operand *b,int type);

/**
    * @Author Leon Palaić
    * @brief Function that removes specified part of string
    * @param s input string 
    * @param toremove remove string
    * @result new sequence of charachters

*/
void removeSubstring(char *s,const char *toremove);
TestResult AK_op_projection_test();

/**
    * @Author Leon Palaić
    * @brief Function that fetches arithmetics operator
    * @param exp input expression string
    * @result operator 

*/
char *AK_get_operator(char *exp);

#endif
