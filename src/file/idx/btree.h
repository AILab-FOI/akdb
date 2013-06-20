/**
@file btree.h Header file that provides data strucures for BTree indices
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

#ifndef BTREE
#define BTREE

#define B 3
//order of the tree 6 - it can be lowered to usual 4-5 IF the B is increased!
#define ORDER 6
//now we have place for ((B+1)^(0RDER-1))*B = ((3+1)^(6-1))*3 = (4^5)*3 = 1024*3 = 3072 elements

//types for tuple_dict
#define LEAF 0
#define NODE 1

#include "index.h"
#include "../../file/table.h"
#include "../../auxi/constants.h"
#include "../../auxi/configuration.h"

typedef struct{
	//B values
	int values[B];
	/*
	B + 1 pointers
	IF type == 1 -> it points to other B+1 nodes/leafs
	IF type == 0 -> first B points to addresses in tblName
	and last pointer points to the next leaf (rangeSearch)
	*/
	struct_add pointers[B+1];
}btree_node;

/*
root_info->level[index] represents the number of nodes in 
that level from leafs to root, where root is always 1 (root doesn't
have to be on index 5 - if order of the tree is 3 it can be on 2 so we
distinct root lvl by value == 1). This is just theoretical representation
of the full usage of the array for B=3 and 0=6.

index|	     0 |       1 |       2 |       3 |       4 |       5 |
value| (B+1)^5 | (B+1)^4 | (B+1)^3 | (B+1)^2 | (B+1)^1 | (B+1)^0 |
level|   leaf  |         |         |         |         |   root  |
*/
typedef struct{
	//tuple_dic[ID] of the root node in the block
	int root;
	//array size of the 0RDER
	int level[ORDER]; 
}root_info;

#endif

int AK_btree_create(char *tblName, AK_list *attributes, char *indexName);
int AK_btree_delete(char *indexName);
void AK_btree_search_delete(char *indexName,int *searchValue,int *toDo);
int AK_btree_insert(char *indexName,int *insertValue, int *insertTd, int *insertBlock);
void Ak_btree_test() ;

