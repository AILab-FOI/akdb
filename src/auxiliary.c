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


/**
 * @brief Get number of chars for given number
 * @author Dino Laktašić.
 * @param int number - number to evaluate 
 * @param int base - mathematic base (e.g. 2, 10 etc.)  
 * @result characters number for given number
 */
int AK_chars_num_from_number(int number, int base) {
    int len = 0;

    do {
        len++;
    } while ((double) (number /= base) > 0);

    return len;
}

/** 
 @author Miroslav Policki
 
 Returns size in bytes of the provided database type

 @param iDB_type database data type (defined in configuration.h)
 @param szVarchar if iDB_type == TYPE_VARCHAR, pointer to the string, otherwise unused

 @return size of provided data type in bytes if provided data type is valid, else 0
 */
size_t AK_type_size(int iDB_type, char *szVarchar) {
    switch (iDB_type) {
        case TYPE_INTERNAL:
            return sizeof (int);
        case TYPE_INT:
            return sizeof (int);
        case TYPE_FLOAT:
            return sizeof (double);
        case TYPE_NUMBER:
            return sizeof (double);
        case TYPE_VARCHAR:
            return (size_t) strlen(szVarchar);
        case TYPE_DATE:
            return sizeof (int);
        case TYPE_DATETIME:
            return sizeof (int);
        case TYPE_TIME:
            return sizeof (int);
        case TYPE_BLOB:
            return sizeof (void *);
        case TYPE_BOOL:
            return (size_t) 1;
        default:
            return (size_t) 0;
    }
}

/**
 * @brief String comparison function
 * @author Dino Laktašić
 * @param const void *a - value to compare
 * @param const void *b - value to compare
 * @return int - returns result of comparison
 */
int AK_strcmp(const void *a, const void *b) {
    const char **ia = (const char **) a;
    const char **ib = (const char **) b;
    return strcmp(*ia, *ib);
}

/**
 * @author Matija Šestak.
 * @brief  Alocate empty list
 * @param AK_list* - root of the list
 * @result void
 */
void Ak_Init_L(AK_list *L) {
    L->next = NULL;
}

/**
 * @author Matija Šestak.
 * @brief  Get the first element of the list
 * @param AK_list* - root of the list
 * @result AK_list_elem - first element of the list
 */
AK_list_elem Ak_First_L(AK_list *L) {
    return L->next;
}

/**
 * @author Matija Šestak.
 * @brief  Get the last element of the list
 * @param AK_list* - root of the list
 * @result AK_list_elem - last element of the list
 */
AK_list_elem Ak_End_L(AK_list *L) {
    AK_list_elem current = Ak_First_L(L);
    if (current == NULL)
        return NULL;
    while (current->next != NULL)
        current = current->next;
    return current;
}

/**
 * @author Matija Šestak.
 * @brief  Get the next element of the list
 * @param AK_list_elem - current element in the list
 * @param AK_list* - root of the list
 * @result AK_list_elem - next element in the list
 */
AK_list_elem Ak_Next_L(AK_list_elem current) {
    if (current == NULL)
        return NULL;
    return current->next;
}

/**
 * @author Matija Šestak.
 * @brief  Get the previous element of the list
 * @param AK_list_elem - current element in the list
 * @param AK_list* - root of the list
 * @result AK_list_elem - previous element in the list
 */
AK_list_elem Ak_Previous_L(AK_list_elem current, AK_list *L) {
    if (current == NULL || current == L)
        return NULL;
    AK_list_elem previous = L;
    while (previous->next != current)
        previous = previous->next;
    return previous;
}

/**
 * @author Matija Šestak.
 * @brief  Test whether the list is empty
 * @param AK_list* - root of the list
 * @result int - returns 1 if the list is empty, otherwise returns 0
 */
int Ak_IsEmpty_L(AK_list *L) {
    return Ak_First_L(L) == NULL ? 1 : 0;
}

/**
 * @author Matija Šestak.
 * @brief  Inserts new element before the current element in the list
 * @param int - data type of the new element
 * @param char* - new data
 * @param int - data size of the new element in the list
 * @param AK_list_elem - current element in the list
 * @param AK_list* - root of the list
 * @result void
 */
void Ak_InsertBefore_L(int type, char* data, int size, AK_list_elem current, AK_list *L) {
    AK_list_elem new_elem = (AK_list_elem) malloc(sizeof ( struct list_elem));
    new_elem->size = size;
    new_elem->type = type;
    memcpy(new_elem->data, data, new_elem->size);
    if (current == NULL) {
        L->next = new_elem;
        new_elem->next = NULL;
        return;
    }
    AK_list_elem previous = Ak_Previous_L(current, L);
    previous->next = new_elem;
    new_elem->next = current;
}

/**
 * @author Matija Šestak.
 * @brief  Inserts new element after the current element in the list
 * @param int - data type of the new element
 * @param char* - new data
 * @param int - data size of the new element in the list
 * @param AK_list_elem - current element in the list
 * @param AK_list* - root of the list
 * @result void
 */
void Ak_InsertAfter_L(int type, char* data, int size, AK_list_elem current, AK_list *L) {
    AK_list_elem new_elem = (AK_list_elem) malloc(sizeof ( struct list_elem));
    new_elem->size = size;
    new_elem->type = type;
    memcpy(new_elem->data, data, new_elem->size);
    if (current == NULL) {
        L->next = new_elem;
        new_elem->next = NULL;
        return;
    }
    new_elem->next = current->next;
    current->next = new_elem;
}

/**
 * @author Matija Šestak.
 * @brief  Inserts new element at the begin of the list
 * @param int - data type of the new element
 * @param char* - new data
 * @param int - data size of the new element in the list
 * @param AK_list* - root of the list
 * @result void
 */
void Ak_InsertAtBegin_L(int type, char* data, int size, AK_list *L) {
    Ak_InsertBefore_L(type, data, size, Ak_First_L(L), L);
}

/**
 * @author Matija Šestak.
 * @brief  Inserts new element at the end of the list
 * @param int - data type of the new element
 * @param char* - new data
 * @param int - data size of the new element in the list
 * @param AK_list* - root of the list
 * @result void
 */
void Ak_InsertAtEnd_L(int type, char* data, int size, AK_list *L) {
    Ak_InsertAfter_L(type, data, size, Ak_End_L(L), L);
} //Ak_InsertAtEnd_L  Ak_Insert_AtEnd_L

/**
 * @author Matija Šestak.
 * @brief  Deletes the current element in the list
 * @param AK_list_elem - current element in the list
 * @param AK_list* - root of the list
 * @result void
 */
void Ak_Delete_L(AK_list_elem current, AK_list *L) {
    if (current == NULL)
        return;
    AK_list_elem previous = Ak_Previous_L(current, L);
    AK_list_elem next = current->next; //NextL( current, L );

    previous->next = next;
    free(current);
}

/**
 * @author Matija Šestak.
 * @brief  Makes list empty.
 * @param AK_list* - root of the list
 * @result void
 */
void Ak_DeleteAll_L(AK_list *L) {
    AK_list_elem current;
    while (current = Ak_First_L(L)) {
        L->next = current->next;
        free(current);
    }
}

/**
 * @author Matija Šestak.
 * @brief  Get a number of the elements in the list
 * @param AK_list* - root of the list
 * @result int - size of the list
 */
int Ak_Size_L(AK_list *L) {
    int size = 0;
    AK_list_elem current = Ak_First_L(L);
    while (current) {
        size++;
        current = Ak_Next_L(current);
    }
    return size;
}

/**
 * @author Matija Šestak.
 * @brief  Retrieves data from the current element in the list
 * @param AK_list_elem - current element in the list
 * @param AK_list* - root of the list
 * @result char* - data from the list element
 */
char* Ak_Retrieve_L(AK_list_elem current, AK_list *L) {
    if (current == NULL)
        return NULL;
    char *data = (char*) malloc(current->size);
    memcpy(data, current->data, current->size);
    return data;
}

/**
 * @author Matija Šestak.
 * @brief  Get a type of the current list element
 * @param AK_list_elem - current list element
 * @param AK_list* - root of the list
 * @result int - data type  of the current list element
 */
int Ak_GetType_L(AK_list_elem current, AK_list *L) {
    return (current == NULL) ? 0 : current->type;
}

/**
 * @author Matija Šestak.
 * @brief  Get a data size of the element
 * @param AK_list_elem - current list element
 * @param AK_list* - root of the list
 * @result int - data size of the current list element
 */
int Ak_GetSize_L(AK_list_elem current, AK_list *L) {
    return (current == NULL) ? 0 : current->size;
}

/**
 Function for fetching nth element in row
 @param pos position of element in row
 @param row list of elements of row in table
 @return element of list of elements of row in table
 @author Mislav Čakarić
 */
AK_list_elem Ak_GetNth_L(int pos, AK_list *row) {
    int i = 0;
    AK_list_elem temp_elem;

    temp_elem = Ak_First_L(row);
    do {
        if (pos == i)
            return temp_elem;
        temp_elem = Ak_Next_L(temp_elem);
        i++;
    } while (temp_elem);
    return NULL;
}

/**
 * @author Matija Novak
 * Get the posititn of given element
 * @param SearchElement - element whose posititon we search for
 * @param L - root of the list
 * @result returns the posititon number of some elelemnt
 */
int Ak_Get_Position_Of_Element(AK_list_elem SearchedElement, AK_list *L) {
    AK_list *CurrentElement;
    int i = 0;
    CurrentElement = L;
    while (CurrentElement->next != 0 && CurrentElement != SearchedElement) {
        CurrentElement = (AK_list *) CurrentElement->next;
        i++;
    }
    return i;
}

/**
 * @author Dino Laktašić.
 * @brief  Get all permutations without repetition (currently not used, but it can be helpful)
 * @param char *arr - array of chars to permute
 * @result char* - pointer to array of pointers to permuted char arrays
 */
char *AK_get_array_perms(char *arr) {
    int div, num_perms = 1;
    int next_perm, next_chr;
    int len_arr = strlen(arr);

    //Get total permutation without repetition number
    for (next_chr = 1; next_chr <= len_arr; num_perms *= next_chr++);

    char *res_perm, *perms[num_perms];

    for (next_perm = 0; next_perm < num_perms; next_perm++) {
        char *perm = (char *) calloc(len_arr, sizeof (char));
        memcpy(perm, arr, len_arr);

        res_perm = (char *) calloc(len_arr, sizeof (char));

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

        free(perm);
		//perm = NULL;
    }

    res_perm = (char *) calloc(num_perms, sizeof (char*));
    memcpy(res_perm, perms, sizeof (perms));

    return res_perm;
}
