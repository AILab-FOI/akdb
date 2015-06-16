/**
@file auxiliary.c Provides auxiliary functions
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

#include "auxiliary.h"

AK_graph G;
AK_stackHead S;
int indexCounter = 0;

/**
 * @author Dino Laktašić.
 * @brief Function gets number of digits for given number
 * @param number number to evaluate
 * @param int base mathematic base (e.g. 2, 10 etc.)
 * @return number of digits for given number
 */
int AK_chars_num_from_number(int number, int base) {
    int len = 0;
    AK_PRO;

    do {
        len++;
    } while ((double) (number /= base) > 0);

    AK_EPI;
    return len;
}

/**
 * @author Miroslav Policki
 * @brief Function returns size in bytes of the provided database type
 * @param iDB_type database data type (defined in constants.h)
 * @param szVarchar if iDB_type == TYPE_VARCHAR, pointer to the string, otherwise unused
 * @return size of provided data type in bytes if provided data type is valid, else 0
 */
size_t AK_type_size(int iDB_type, char *szVarchar) {
    AK_PRO;
    switch (iDB_type) {
        case TYPE_INTERNAL:
            AK_EPI;
            return sizeof (int);
        case TYPE_INT:
            AK_EPI;
            return sizeof (int);
        case TYPE_FLOAT:
            AK_EPI;
            return sizeof (double);
        case TYPE_NUMBER:
            AK_EPI;
            return sizeof (double);
        case TYPE_VARCHAR:
            AK_EPI;
            return (size_t) strlen(szVarchar);
        case TYPE_DATE:
            AK_EPI;
            return sizeof (int);
        case TYPE_DATETIME:
            AK_EPI;
            return sizeof (int);
        case TYPE_TIME:
            AK_EPI;
            return sizeof (int);
        case TYPE_BLOB:
            AK_EPI;
            return sizeof (void *);
        case TYPE_BOOL:
            AK_EPI;
            return (size_t) 1;
        default:
            AK_EPI;
            return (size_t) 0;
    }
}

/**
 * @author Dino Laktašić
 * @brief Function compares to Strings
 * @param *a  pointer of value to compare
 * @param *b pointer of value to compare
 * @return result of comparison according to strcmp function
 */
int AK_strcmp(const void *a, const void *b) {
    const char **ia = (const char **) a;
    const char **ib = (const char **) b;
    AK_PRO;
    AK_EPI;
    return strcmp(*ia, *ib);
}

/**
 * @author Ljiljana Pintarić
 * @brief  Function initializes empty list
 * @param L root of the list
 * @return NO return value
 */

void Ak_Init_L3(list_node **L){
    AK_PRO;
    assert(L != NULL);
    (*L)->next = NULL;
    AK_EPI;
}

/**
 * @author Ljiljana Pintarić
 * @brief  Transition function.
 * @param L root of the list
 * @return NO return value
 */

void Ak_Init_L2(AK_list *L) {
    AK_PRO;
    unsigned int insert_list_address= 1;

    int i;
    for(i = 0; i < lsa_counter; i++)
    {
      if(list_structures_array[i].bad == L)
      {
	insert_list_address = 0;
      }
    }

    list_node *list_node_L;
    list_node_L=NULL;

    if(insert_list_address)
    {
	list_node_L = (list_node*) AK_malloc(sizeof(list_node));
	assert(list_node_L != NULL);
	//list_node_L->next = NULL;
	Ak_Init_L3(&list_node_L);
	list_structures_array[lsa_counter].bad = L;
	list_structures_array[lsa_counter].good = list_node_L;
	lsa_counter++;
    }
    AK_EPI;
}

/**
 * @author Matija Šestak.
 * @brief  Function initializes empty list
 * @param L root of the list
 * @return NO return value
 */
void Ak_Init_L(AK_list *L) {
    AK_PRO;
    assert(L != NULL);
    L->next = NULL;
    Ak_Init_L2(L);
    AK_EPI;
}

/**
 * @author Ljiljana Pintarić.
 * @brief  Function gets the first element of the list
 * @param L root of the list
 * @return first element of the list
 */

list_node *Ak_First_L2(list_node *L){
  AK_PRO;
  if(L == NULL){
	AK_EPI;
	return NULL;
    }
  AK_EPI;
  return L->next;
}

/**
 * @author Matija Šestak.
 * @brief  Function gets the first element of the list
 * @param L root of the list
 * @return first element of the list
 */
AK_list_elem Ak_First_L(AK_list *L) {
    AK_PRO;
    if(L == NULL){
	AK_EPI;
	return NULL;
    }
    AK_EPI;
    return L->next;
}

/**
 * @author Ljiljana Pintarić.
 * @brief  Function gets the last element of the list
 * @param L  root of the list
 * @return last element of the list
 */

list_node *Ak_End_L2(list_node *L) {
    AK_PRO;
    list_node *current;
    current=Ak_First_L2(L);

    if (current == NULL)
    {
      AK_EPI;
      return NULL;
    }

    int counter = 0;
    while (current->next != NULL)
    {
      current = current->next;
      counter++;
      assert(counter < MAX_LOOP_ITERATIONS);
    }

    AK_EPI;
    return current;
}

/**
 * @author Matija Šestak.
 * @brief  Function gets the last element of the list
 * @param L  root of the list
 * @return last element of the list
 */
AK_list_elem Ak_End_L(AK_list *L) {
    AK_PRO;
    AK_list_elem current = Ak_First_L(L);
    if (current == NULL)
    {
        AK_EPI;
        return NULL;
    }

    int counter = 0;
    while (current->next != NULL)
    {
      current = current->next;
      counter++;
      assert(counter < MAX_LOOP_ITERATIONS);
    }

    AK_EPI;
    return current;
}

/**
 * @author Ljiljana Pintarić.
 * @brief  Function gets the next element of the list
 * @param current  current element in the list
 * @return next element in the list
 */

list_node *Ak_Next_L2(list_node *current) {
    AK_PRO;
    if (current == NULL)
    {
        AK_EPI;
        return NULL;
    }
    AK_EPI;
    return current->next;
}

/**
 * @author Matija Šestak.
 * @brief  Function gets the next element of the list
 * @param current  current element in the list
 * @return next element in the list
 */
AK_list_elem Ak_Next_L(AK_list_elem current) {
    AK_PRO;
    if (current == NULL){
        AK_EPI;
        return NULL;
    }
    AK_EPI;
    return current->next;
}

/**
 * @author Ljiljana Pintarić.
 * @brief  Function gets the previous element of the list
 * @param current current element in the list
 * @param L root of the list
 * @return previous element in the list
 */

list_node *Ak_Previous_L2(list_node *current, list_node *L) {
    AK_PRO;

    if (current == NULL || current == L)
    {
        AK_EPI;
        return NULL;
    }

    list_node *previous = L;
    int counter = 0;
    while (previous->next != current)
    {
      previous = previous->next;
      counter++;
      assert(counter < MAX_LOOP_ITERATIONS);
    }

    AK_EPI;
    return previous;
}

/**
 * @author Matija Šestak.
 * @brief  Function gets the previous element of the list
 * @param current current element in the list
 * @param L root of the list
 * @return previous element in the list
 */
AK_list_elem Ak_Previous_L(AK_list_elem current, AK_list *L) {
    AK_PRO;

    if (current == NULL || current == L)
    {
        AK_EPI;
        return NULL;
    }

    AK_list_elem previous = L;
    int counter = 0;
    while (previous->next != current)
    {
        previous = previous->next;
	counter++;
	assert(counter < MAX_LOOP_ITERATIONS);
    }

    AK_EPI;
    return previous;
}

/**
 * @author Ljiljana Pintarić.
 * @brief  Function tests whether the list is empty
 * @param L root of the list
 * @return 1 if the list is empty, otherwise returns 0
 */

unsigned int Ak_IsEmpty_L2(list_node *L) {
    AK_PRO;
    unsigned int ret;
    ret = (Ak_First_L2(L) == NULL ? 1 : 0);
    AK_EPI;
    return ret;
}

/**
 * @author Matija Šestak.
 * @brief  Function tests whether the list is empty
 * @param L root of the list
 * @return 1 if the list is empty, otherwise returns 0
 */
int Ak_IsEmpty_L(AK_list *L) {
    AK_PRO;
    int ret;
    ret = (Ak_First_L(L) == NULL ? 1 : 0);
    AK_EPI;
    return ret;
}


/**
 * @author Ljiljana Pintarić.
 * @brief  Function inserts new element before the current element in the list.
 * @param data new data
 * @param current current element in the list
 * @param L root of the list
 * @return No return value
 */

void Ak_InsertBefore_L2(int type, char* data, int size, list_node **current, list_node **L) {
    AK_PRO;
    list_node *new_elem;

    new_elem = (list_node*) AK_malloc(sizeof(list_node));
    new_elem->size = size;
    new_elem->type = type;
    
    memcpy(new_elem->data, data, MAX_VARCHAR_LENGTH);

    if ((*current) == NULL)
    {
        (*L)->next = new_elem;
        new_elem->next = NULL;
        AK_EPI;
        return;
    }
    list_node *previous = Ak_Previous_L2((*current), (*L));
    previous->next = new_elem;
    new_elem->next = (*current);
    AK_EPI;
}

/**
 * @author Matija Šestak.
 * @brief  Function inserts new element before the current element in the list.
 * @param type data type of the new element
 * @param data new data
 * @param size data size of the new element in the list
 * @param current current element in the list
 * @param L root of the list
 * @return No return value
 */
void Ak_InsertBefore_L(int type, char* data, int size, AK_list_elem current, AK_list *L) {
    AK_list_elem new_elem;
    AK_PRO;
    new_elem = (AK_list_elem) AK_malloc(sizeof ( struct list_elem));
    new_elem->size = size;
    new_elem->type = type;
    memcpy(new_elem->data, data, new_elem->size);
    if (current == NULL)
    {
        L->next = new_elem;
        new_elem->next = NULL;
        AK_EPI;
        return;
    }
    AK_list_elem previous = Ak_Previous_L(current, L);
    previous->next = new_elem;
    new_elem->next = current;
    AK_EPI;
}

/**
 * @author Ljiljana Pintarić.
 * @brief  Function inserts new element after the current element in the list
 * @param data new data
 * @param current current element in the list
 * @param L root of the list
 * @return No return value.
 */

void Ak_InsertAfter_L2(int type, char* data, int size, list_node **current,  list_node **L) {

    AK_PRO;
    list_node *new_elem;

    new_elem = (list_node*) AK_malloc(sizeof(list_node));
    new_elem->size = size;
    new_elem->type = type;
    memcpy(new_elem->data, data, MAX_VARCHAR_LENGTH);
    if ((*current) == NULL)
    {
	(*L)->next = new_elem;
        new_elem->next = NULL;
        AK_EPI;
        return;
    }
    new_elem->next = (*current)->next;
    (*current)->next = new_elem;
    AK_EPI;
}

/**
 * @author Matija Šestak.
 * @brief  Function inserts new element after the current element in the list
 * @param type data type of the new element
 * @param data new data
 * @param size data size of the new element in the list
 * @param current current element in the list
 * @param L root of the list
 * @return Address of new element
 */
void Ak_InsertAfter_L(int type, char* data, int size, AK_list_elem current, AK_list *L) {
    AK_list_elem new_elem;
    AK_PRO;

    new_elem = (AK_list_elem) AK_calloc(1, sizeof ( struct list_elem));

    new_elem->size = size;
    new_elem->type = type;
    new_elem->next = NULL;

    memcpy(new_elem->data, data, new_elem->size);
    if (current == NULL)
    {
        L->next = new_elem;
        AK_EPI;
        return;
    }
    new_elem->next = current->next;
    current->next = new_elem;
    AK_EPI;
    return;
}

/**
 * @author Ljiljana Pintarić.
 * @brief  Function inserts new element at the begin of the list. It uses function AK_InsertBefore_L
 * @param data new data
 * @param L root of the list
 * @return No return value
 */

void Ak_InsertAtBegin_L3(int type, char* data, int size, list_node *L) {
    AK_PRO;
    assert(L != NULL);

    list_node *current = Ak_First_L2(L);
    current = Ak_First_L2(L);
    Ak_InsertBefore_L2(type, data, size, &current, &L);

    AK_EPI;
}


/**
 * @author Ljiljana Pintarić.
 * @brief  Transition function.
 * @param data new data
 * @param L root of the list
 * @return No return value
 */

void Ak_InsertAtBegin_L2(int type, char* data, int size, AK_list *L) {
    AK_PRO;
    assert(L != NULL);
    list_node * list_node_L, *current;
    int i;
    for(i = 0; i < lsa_counter; i++)
    {
      if(list_structures_array[i].bad == L)
      {
	list_node_L=list_structures_array[i].good;
      }
    }

    current=Ak_First_L2(list_node_L);
    //Ak_InsertAtBegin_L3(data, &list_node_L);
    Ak_InsertAtBegin_L3(type,data, size, &list_node_L);     
    Ak_InsertBefore_L(type, data, size, Ak_First_L(L), L);

    AK_EPI;
}

/**
 * @author Matija Šestak.
 * @brief  Function inserts new element at the begin of the list. It uses function AK_InsertBefore_L
 * @param type data type of the new element
 * @param data new data
 * @param size data size of the new element in the list
 * @param L root of the list
 * @return No return value
 */
void Ak_InsertAtBegin_L(int type, char* data, int size, AK_list *L) {
    AK_PRO;
    Ak_InsertAtBegin_L2(type,data, size, L);
    //Ak_InsertBefore_L(type, data, size, Ak_First_L(L), L);
    AK_EPI;
}

/**
 * @author Ljiljana Pintarić.
 * @brief  Function inserts new element at the end of the list. It uses function Ak_InsertAfter_L
 * @param data new data
 * @param L root of the list
 * @return No return value.
 */

void Ak_InsertAtEnd_L3(int type, char* data, int size, list_node *L) {
    AK_PRO;
    assert(L != NULL);
    list_node *current;
    current = NULL;
    current = Ak_End_L2(L);
    Ak_InsertAfter_L2(type,data, size, &current, &L);

    AK_EPI;
}

/**
 * @author Ljiljana Pintarić.
 * @brief  Transition function.
 * @param type data type of the new element
 * @param data new data
 * @param size data size of the new element in the list
 * @param L root of the list
 * @return No return value.
 */

void Ak_InsertAtEnd_L2(int type, char* data, int size, AK_list *L) {
    AK_PRO;
    assert(L != NULL);

    list_node * list_node_L, *current;
    list_node_L=NULL;
    current=NULL;
    int i;

    for(i=0; i<lsa_counter; i++)
    {
      if(list_structures_array[i].bad == L)
      {
	list_node_L=list_structures_array[i].good;
      }
    }

   // Ak_InsertAtEnd_L3(data, list_node_L);
           
    Ak_InsertAtEnd_L3(type, data, size, list_node_L);    
    Ak_InsertAfter_L(type, data, size, Ak_End_L(L), L);

    AK_EPI;
}

/**
 * @author Matija Šestak.
 * @brief  Function inserts new element at the end of the list. It uses function Ak_InsertAfter_L
 * @param type data type of the new element
 * @param data new data
 * @param size data size of the new element in the list
 * @param L root of the list
 * @return No return value.
 */
void Ak_InsertAtEnd_L(int type, char* data, int size, AK_list *L) {
    AK_PRO;
    Ak_InsertAtEnd_L2(type, data, size, L);
    AK_EPI;
}


/**
 * @author Ljiljana Pintarić.
 * @brief  Function deletes the current element in the list
 * @param current current element in the list
 * @param L root of the list
 * @retrun No return value
 */

void Ak_Delete_L3(list_node **current, list_node **L) {
    AK_PRO;
    if ((*current) == NULL || (*L) == NULL)
    {
        AK_EPI;
        return;
    }
    list_node * previous = Ak_Previous_L2((*current), (*L));
    list_node * next = (*current)->next; //NextL( current, L );

    previous->next = next;
    AK_free((*current));
    AK_EPI;
}

void Ak_Delete_L2(AK_list_elem current, AK_list *L) {
    AK_PRO;

    if (current == NULL || L == NULL)
    {
        AK_EPI;
        return;
    }

    int i;
    list_node *list_node_L, *current2, *current3;

    for(i=0; i<lsa_counter; i++)
    {
      if(list_structures_array[i].bad==L)
      {
	list_node_L=list_structures_array[i].good;
      }
    }

    int counter = 0;
    while ( (current3 = Ak_First_L2(list_node_L)) != NULL )
    {
      if(strcmp(current3->data, current->data)) //loše, moguće da podaci nisu unique
      {
	current2=current3;
      }
      list_node_L->next = current3->next;
      counter++;
      assert(counter < MAX_LOOP_ITERATIONS);
    }

    Ak_Delete_L3(&current2, &list_node_L);

    AK_EPI;
}


/**
 * @author Matija Šestak.
 * @brief  Function deletes the current element in the list
 * @param current current element in the list
 * @param L root of the list
 * @retrun No return value
 */
void Ak_Delete_L(AK_list_elem current, AK_list *L) {
    AK_PRO;

    if (current == NULL || L == NULL)
    {
        AK_EPI;
        return;
    }
    Ak_Delete_L2(current, L);
    AK_list_elem previous = Ak_Previous_L(current, L);
    AK_list_elem next = current->next; //NextL( current, L );

    previous->next = next;
    AK_free(current);
    AK_EPI;
}

/**
 * @author Ljiljana Pintarić.
 * @brief  Function empties list
 * @param L root of the list
 * @return No return value
 */

void Ak_DeleteAll_L3(list_node **L) {
    AK_PRO;

    list_node *current;
    if((*L) == NULL)
    {
	AK_EPI;
	return;
    }

    int counter = 0;
    while ( (current = Ak_First_L2(*L)) != NULL )
    {
	(*L)->next = current->next;
	AK_free(current);
	counter++;
	assert(counter < MAX_LOOP_ITERATIONS);
    }

    AK_EPI;
}

/**
 * @author Ljiljana Pintarić.
 * @brief  Transition function.
 * @param L root of the list
 * @return No return value
 */

void Ak_DeleteAll_L2(AK_list *L){
  AK_PRO;

  int i;
  int position = 10000;
  list_node * list_node_L;

  for(i = 0; i < lsa_counter; i++)
  {
    if(list_structures_array[i].bad == L)
    {
      position=i;
    }
  }


  if(position<lsa_counter)
  {
    list_node_L = list_structures_array[position].good;
    Ak_DeleteAll_L3(&list_node_L);
  }

  AK_EPI;
}

/**
 * @author Matija Šestak.
 * @brief  Function empties list
 * @param L root of the list
 * @return No return value
 */
void Ak_DeleteAll_L(AK_list *L) {
    AK_list_elem current;
    AK_PRO;
    if(L == NULL)
    {
	AK_EPI;
	return;
    }

    int counter = 0;
    while ( (current = Ak_First_L(L)) != NULL )
    {
        L->next = current->next;
        AK_free(current);
	counter++;
	assert(counter < MAX_LOOP_ITERATIONS);
    }
    Ak_DeleteAll_L2(L);
    AK_EPI;
}

/**
 * @author Ljiljana Pintarić.
 * @brief  Function gets a number of the elements in the list.
 * @param L root of the list
 * @return Size of the list
 */

int Ak_Size_L2(list_node *L) {
    AK_PRO;

    int size = 0;
    list_node *current;

    current = Ak_First_L(L);

    int counter = 0;
    while (current)
    {
        size++;
        current = Ak_Next_L(current);
	counter++;
	assert(counter < MAX_LOOP_ITERATIONS);
    }
    AK_EPI;
    return size;
}


/**
 * @author Matija Šestak.
 * @brief  Function gets a number of the elements in the list.
 * @param L root of the list
 * @return Size of the list
 */
int Ak_Size_L(AK_list *L) {
    int size = 0;
    AK_list_elem current;
    AK_PRO;

    int counter = 0;
    current = Ak_First_L(L);
    while (current)
    {
        size++;
        current = Ak_Next_L(current);
	counter++;
	assert(counter < MAX_LOOP_ITERATIONS);
    }
    AK_EPI;
    return size;
}


/**
 * @author Ljiljana Pintarić.
 * @brief  Function retrieves data from the current element in the list
 * @param current current element in the list
 * @param L root of the list
 * @return data from the list element
 */

char* Ak_Retrieve_L2(list_node *current, list_node *L) {
    char *data;
    AK_PRO;

    if (current == NULL || L == NULL)
    {
        return NULL;
    }

    data = (char*) AK_malloc(MAX_VARCHAR_LENGTH);
    memcpy(data, current->data, MAX_VARCHAR_LENGTH);

    AK_EPI;
    return data;
}

/**
 * @author Matija Šestak.
 * @brief  Function retrieves data from the current element in the list
 * @param current current element in the list
 * @param L root of the list
 * @return data from the list element
 */
char* Ak_Retrieve_L(AK_list_elem current, AK_list *L) {
    char *data;
    AK_PRO;
    if (current == NULL)
    {
        return NULL;
    }

    data = (char*) AK_malloc(current->size);
    memcpy(data, current->data, current->size);
    AK_EPI;
    return data;
}

/**
 * @author Matija Šestak.
 * @brief  Function gets a type of the current list element
 * @param current current list element
 * @param L root of the list
 * @return data type  of the current list element
 */

// 0 uses found
/*
int Ak_GetType_L(AK_list_elem current, AK_list *L) {
    int ret;
    AK_PRO;
    ret = (current == NULL) ? 0 : current->type;
    AK_EPI;
    return ret;
}
*/

/**
 * @author Matija Šestak.
 * @brief  Function gets a data size of the element
 * @param current current list element
 * @param L - root of the list
 * @return data size of the current list element
 */
// 0 uses found
/*
int Ak_GetSize_L(AK_list_elem current, AK_list *L) {
    int ret;
    AK_PRO;
    ret = (current == NULL) ? 0 : current->size;
    AK_EPI;
    return ret;
}
*/

/**
 * @author Ljiljana Pintarić
 * @brief Function for fetching nth element in row
 * @param pos position of element in row
 * @param row list of elements of row in table
 * @return element of list of elements of row in table

 */

list_node * Ak_GetNth_L2(int pos, list_node *row) {
    AK_PRO;
    int i;
    list_node *temp_elem;

    assert(pos > -1);
    temp_elem = Ak_First_L2(row);
    for(i = 1; temp_elem != NULL; ++i)
    {
        if (i == pos)
	{
            AK_EPI;
            return temp_elem;
        }
        temp_elem = Ak_Next_L2(temp_elem);
    };
    AK_EPI;
    return NULL;
}

/**
 * @author Mislav Čakarić
 * @brief Function for fetching nth element in row
 * @param pos position of element in row
 * @param row list of elements of row in table
 * @return element of list of elements of row in table

 */
AK_list_elem Ak_GetNth_L(int pos, AK_list *row) {
    int i;
    AK_list_elem temp_elem;
    AK_PRO;
    assert(pos > -1);
    temp_elem = Ak_First_L(row);
    for(i = 1; temp_elem != NULL; ++i){
        if (i == pos){
            AK_EPI;
            return temp_elem;
        }
        temp_elem = Ak_Next_L(temp_elem);
    };
    AK_EPI;
    return NULL;
}

/**
 * @author Matija Novak
 * @brief Function gets the position of given element
 * @param SearchElement element whose posititon we search for
 * @param L root of the list
 * @return returns the posititon number of some elelemnt
 */
// 0 uses found
/*
int Ak_Get_Position_Of_Element(AK_list_elem SearchedElement, AK_list *L) {
    AK_list *CurrentElement;
    int i = 0;
    AK_PRO;
    CurrentElement = L;
    while (CurrentElement->next != 0 && CurrentElement != SearchedElement) {
        CurrentElement = (AK_list *) CurrentElement->next;
        i++;
    }
    AK_EPI;
    return i;
}
*/

/**
 * @author Dino Laktašić.
 * @brief  Get all permutations without repetition (currently not used, but it can be helpful)
 * @param arr array of chars to permute
 * @return char pointer to array of pointers to permuted char arrays
 */
char *AK_get_array_perms(char *arr) {
    int div, num_perms = 1;
    int next_perm, next_chr;
    int len_arr = strlen(arr);
    AK_PRO;
    //Get total permutation without repetition number
    for (next_chr = 1; next_chr <= len_arr; num_perms *= next_chr++);

    char *res_perm, *perms[num_perms];

    for (next_perm = 0; next_perm < num_perms; next_perm++) {
        char *perm = (char *) AK_calloc(len_arr, sizeof (char));
        memcpy(perm, arr, len_arr);

        res_perm = (char *) AK_calloc(len_arr, sizeof (char));

        for (next_chr = len_arr, div = num_perms; next_chr > 0; next_chr--) {
            div /= next_chr;
            int index = (next_perm / div) % next_chr;
            //printf("%c", perm[index] );

            memcpy(res_perm + (len_arr - next_chr), &perm[index], 1);
            memmove(perm + index, perm + index + 1, len_arr - index);
        }
        //printf("\n");

        perms[next_perm] = res_perm;
        //printf("%s\n", perms[next_perm]);

        AK_free(perm);
        //perm = NULL;
    }

    res_perm = (char *) AK_calloc(num_perms, sizeof (char*));
    memcpy(res_perm, perms, sizeof (perms));
    AK_EPI;
    return res_perm;
}

/**
 * @author Frane Jakelić
 * @briefFunction that searches for a specific graph node by its ID
 * @param id of the vertex that needs to be found
 * @param graphRoot root node of the graph structure
 * @return found graph nod or null
 */
AK_vertex AK_search_vertex(int id) {
    AK_vertex tmp;
    AK_PRO;
    tmp = &G;

    while (tmp->nextVertex != NULL) {
        if (tmp->nextVertex->vertexId == id) {
            AK_EPI;
            return tmp->nextVertex;
        }
        tmp = tmp->nextVertex;

    }
    AK_EPI;
    return NULL;
}
/**
 * @author Frane Jakelić
 * @briefLooks for empty link for a new graph node
 * @param graphRoot oot node of the graph structure
 * @return empty link for new graph node
 */
AK_vertex AK_search_empty_link() {

    AK_vertex tmp;
    AK_PRO;
    tmp = &G;

    while (tmp->nextVertex != NULL) {
        tmp = tmp->nextVertex;
    }
    AK_EPI;
    return tmp;
}

/**
 * @author Frane Jakelić
 * @brief Function that adds a new graph node
 * @param id of the vertex that needs to be added
 * @param graphRoot root node of the graph structure
 * @return pointer to the newly created node
 */
AK_vertex AK_add_vertex(int id) {
    AK_vertex node;
    AK_PRO;
    node = (AK_vertex) AK_malloc(sizeof (struct Vertex));
    memset(node, 0, sizeof (struct Vertex));

    AK_search_empty_link(&G)->nextVertex = node;
    node->vertexId = id;
    node->index = -1;
    node->lowLink = -1;
    AK_EPI;
    return node;
}
/**
 * @author Frane Jakelić
 * @brief Creates a edge between two nodes
 * @param succesorId id of a newly created edge
 * @param succesorOf source of the newly created edge
 * @return pointer to the newly created edge
 */
AK_succesor AK_add_succesor(int succesorId, int succesorOf) {
    AK_succesor edge;
    AK_vertex root;
    AK_succesor suc;
    AK_PRO;
    edge = (AK_succesor) AK_malloc(sizeof (struct Succesor));
    memset(edge, 0, sizeof (struct Succesor));
    root = AK_search_vertex(succesorOf);
    suc = root->nextSuccesor;

    if (root->nextSuccesor == NULL) {
        root->nextSuccesor = edge;
    } else {
        while (suc->nextSuccesor != NULL) {
            suc = suc->nextSuccesor;
        }
        suc->nextSuccesor = edge;
    }

    edge->link = AK_search_vertex(succesorId);
    AK_EPI;
    return edge;
}

/**
 * @author Frane Jakelić
 * @brief Returns a empty link for the stack
 * @param stackRoot root node of the selected stack
 * @return pointer to the empty link
 */
AK_stack AK_search_empty_stack_link(AK_stack stackRoot) {

    AK_stack tmp;
    AK_PRO;
    tmp = stackRoot;

    while (tmp->nextElement != NULL) {
        tmp = tmp->nextElement;
    }
    AK_EPI;
    return tmp;
}
/**
 * @author Frane Jakelić
 * @brief Adds a entry to the stack
 * @param id of the element that is being added to the stack
 * @return pointer to the newly added stack node
 */
AK_stack AK_push_to_stack(int id) {

    AK_stack node;
    AK_PRO;
    node = (AK_stack) AK_malloc(sizeof (struct Stack));
    memset(node, 0, sizeof (struct Stack));

    AK_search_empty_stack_link(&S)->nextElement = node;
    node->link = AK_search_vertex(id);
    AK_EPI;
    return node;
}

/**
 * @author Frane Jakelić
 * @brief Pops a entry to the stack
 * @return pointer to the popped stack node
 */
AK_stack AK_pop_from_stack() {

    AK_stack node;
    AK_stack tmp;
    AK_PRO;
    node = AK_search_empty_stack_link(&S);
    tmp = &S;


    if (node == tmp){
      AK_EPI;
      return NULL;
    }
    while (tmp->nextElement != node) {
        tmp = tmp->nextElement;
    }
    tmp->nextElement = NULL;
    AK_EPI;
    return node;
}
/**
 * @author Frane Jakelić
 * @brief Finds an element in the stack
 * @param id  of the node that needs to be found in the stac
 * @return pointer to the found stack node
 */
AK_stack AK_search_in_stack(int id) {

    AK_stack tmp;
    AK_PRO;
    tmp = &S;

    while (tmp->nextElement != NULL) {
        if (tmp->nextElement->link->vertexId == id) {
            AK_EPI;
            return tmp->nextElement;
        }
        tmp = tmp->nextElement;

    }
    AK_EPI;
    return NULL;
}

int MIN(int X, int Y) {
    int ret;
    AK_PRO;
    ret = X > Y ? Y : X;
    AK_EPI;
    return ret;
}

/**
 * @author Frane Jakelić
 * @brief Tarjan algorithm that looks for a strongly connected component inside all subgraphs; using DFS
 * @param id of the element on which the algorithm looks for a id of a strongly connected component
 */
void AK_tarjan(int id) {
    AK_vertex node;
    AK_succesor succ;
    AK_PRO;
    node = AK_search_vertex(id);
    node->index = indexCounter;
    node->lowLink = indexCounter;
    indexCounter = indexCounter + 1;
    succ = node->nextSuccesor;

    if (succ != NULL)AK_push_to_stack(id);

    while (succ != NULL) {

        if (succ->link->index == -1) {

            AK_tarjan(succ->link->vertexId);
            node->lowLink = MIN(node->lowLink, succ->link->lowLink);

        } else if (AK_search_in_stack(succ->link->vertexId) != NULL) {
            node->lowLink = MIN(node->lowLink, succ->link->index);
        }

        succ = succ->nextSuccesor;
    }

    if (node->lowLink == node->index) {
        AK_vertex loop = NULL;
        printf("############\nStrongy connected component. Edges:\n");
        do {
            AK_stack elem = AK_pop_from_stack();
            if (elem == NULL)break;
            loop = elem->link;
            printf("%i\n", loop->vertexId);
        } while (loop->vertexId != node->vertexId);
        printf("############\n");
    }
    AK_EPI;
}

void AK_tarjan_test() {
    AK_vertex root;
    AK_vertex ro_ot;
    AK_PRO;
    root = G.nextVertex;
    ro_ot = G.nextVertex;
    while (root != NULL) {
        if (root->index == -1) {

            AK_tarjan(root->vertexId);
        }
        while (ro_ot != NULL) {
            ro_ot->index = -1;
            ro_ot->lowLink = -1;
            ro_ot = ro_ot->nextVertex;
        }
        root = root->nextVertex;
    }
    AK_EPI;
}

/**
 * @author Marko Sinko
 * @brief Initializes an AK_synchronization_info structure and returns an owned
 *        pointer that must later be passed to AK_destroy_critical_section.
 * @return Initialized synchronization object
 */
AK_synchronization_info* AK_init_critical_section() {
    AK_synchronization_info* info = AK_calloc(1, sizeof(AK_synchronization_info));
    if (info == NULL)
        return NULL;

#ifdef _WIN32
    if (InitializeCriticalSectionAndSpinCount(
        &info->critical_section, 0x00000400) == 0){
        free(info);
        return NULL;
    }
#endif
#ifdef __linux__
    pthread_mutex_init(&info->mutex, NULL);
#endif
    info->init = 1;
    info->ready = 1;
    return info;
}

/**
 * @author Marko Sinko
 * @param info Synchronization info structure
 * @brief Destroys a synchronization object when it is no longer necessary and
 *        frees the pointer.
 * @return void
 */
void AK_destroy_critical_section(AK_synchronization_info* info) {
    if (info == NULL)
        return;
    if (info->init != 1)
        return;
#ifdef _WIN32
    DeleteCriticalSection(&info->critical_section);
#endif
#ifdef __linux__
    pthread_mutex_destroy(&info->mutex);
#endif
    AK_free(info);
    info = NULL;
}

/**
 * @author Marko Sinko
 * @param info Synchronization info structure
 * @brief Enters a critical section.
 * @return void
 */
void AK_enter_critical_section(AK_synchronization_info* info) {
    assert(info != NULL && info->init == 1);
#ifdef __linux__
    pthread_mutex_lock(&info->mutex);
#endif
#ifdef _WIN32
    EnterCriticalSection(&info->critical_section);
#endif
    while (info->ready != 1); /* wait loop */
    info->ready = 0;
#ifdef __linux__
    pthread_mutex_unlock(&info->mutex);
#endif
#ifdef _WIN32
    LeaveCriticalSection(&info->critical_section);
#endif
}

/**
 * @author Marko Sinko
 * @param info Synchronization info structure
 * @brief Leaves a critical section
 * @return void
 */
void AK_leave_critical_section(AK_synchronization_info* info) {
    assert(info != NULL && info->init == 1);
    info->ready = 1;
}

/**
 * @author Krunoslav Bilić
 * @brief Function used to copy one "AK_list_elem" to another.
 * @param first "AK_list_elem" to be compared - source
 * @param second "AK_list_elem" to be compared - destination
 * @return No return value.
 */
// 0 uses found
/*
void AK_copy_L_Ele(AK_list_elem srcElem, AK_list_elem destElem) {
        AK_PRO;
	destElem->size = srcElem->size;
	destElem->type = srcElem->type;
	memcpy(destElem->data, srcElem->data, srcElem->size);
	memcpy(destElem->table, srcElem->table, sizeof(srcElem->table));
        AK_EPI;
}
*/

/**
 * @author Krunoslav Bilić
 * @brief Function used to copy one "AK_list" to another. Uses AK_copy_L_Ele(..) for copying.
 * @param first "AK_list" to be compared - source
 * @param second "AK_list" to be compared - destination
 * @return No return value.
 */
// 0 uses found
/*
void AK_copy_L(AK_list *src, AK_list *dest) {

	AK_list_elem srcElem;
        AK_list_elem destElem;
        AK_PRO;
        //srcElem = (AK_list_elem) AK_malloc(sizeof(struct list_elem));
	srcElem = (AK_list_elem) Ak_First_L(src);
	destElem = (AK_list_elem) Ak_First_L(dest);

	do {
		AK_copy_L_Ele(srcElem, destElem);

		srcElem = (AK_list_elem) Ak_Next_L(srcElem);

		if (destElem->next == NULL && srcElem != NULL) {
			destElem->next = (AK_list_elem) AK_malloc(sizeof(struct list_elem));
			destElem = (AK_list_elem) Ak_Next_L(destElem);
			destElem->next = NULL;
		}
	} while (srcElem != NULL);
        AK_EPI;
}
*/

/**
 * @author Krunoslav Bilić
 * @brief Function compares two "AK_list" lists by DATA, SIZE and TYPE
 * @param first "AK_list" to be compared
 * @param second "AK_list" to be compared
 * @return EXIT_SUCCESS if content is same, otherwise EXIT_FAILURE
 */
// 0 uses found
/*
int AK_compare_L(AK_list *srcInput, AK_list *srcOriginal) {
        AK_list_elem srcElemInp;
        AK_list_elem srcElemOrig;
        AK_PRO;
	// inicijalna provjera, da li su velicine identicne, ukoliko nisu : izlaz
	if (Ak_Size_L(srcInput) != Ak_Size_L(srcOriginal)){
                AK_EPI;
		return EXIT_FAILURE;
        }

	srcElemInp = (AK_list_elem) Ak_First_L(srcInput);
	srcElemOrig = (AK_list_elem) Ak_First_L(srcOriginal);

	// provjera za elemente data, size, type, table
	do {
		int compare = 0;
		compare = srcElemInp->size == srcElemOrig->size ? 0 : 1;
		if (compare != 0){
                        AK_EPI;
			return EXIT_FAILURE;
                }
		compare = srcElemInp->type == srcElemOrig->type ? 0 : 1;
		if (compare != 0){
                        AK_EPI;
			return EXIT_FAILURE;
                }
		compare = strcmp(srcElemInp->data, srcElemOrig->data);
		if (compare != 0){
                        AK_EPI;
			return EXIT_FAILURE;
                }
		compare = strcmp(srcElemInp->table, srcElemOrig->table);
		if (compare != 0){
                        AK_EPI;
			return EXIT_FAILURE;
                }


		//dohvaca novi element liste
		srcElemInp = (AK_list_elem) Ak_Next_L(srcElemInp);
		srcElemOrig = (AK_list_elem) Ak_Next_L(srcElemOrig);
	} while (srcElemOrig != NULL);
        AK_EPI;
	return EXIT_SUCCESS;

}
*/
