/**
@file selection.h Provides data structures for relational selection operation
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


#ifndef SELECTION_H
#define SELECTION_H

#include<stdio.h>
#include<stdlib.h>

#include "configuration.h"
#include "projection.h"
#include "dbman.h"
#include "memoman.h"

struct list_elem{
    int type;
    int size;
    char data[ MAX_VARCHAR_LENGHT ];
    struct list_elem *next;
};

typedef struct list_elem AK_list;
typedef struct list_elem* AK_list_elem;

void InitL( AK_list *L ){
    L->next = NULL;
}

AK_list_elem FirstL( AK_list *L ){
    return L->next;
}

AK_list_elem EndL( AK_list *L ){
    AK_list_elem current = FirstL( L );
    if( current == NULL )
        return NULL;
    while( current->next != NULL )
        current = current->next;
    return current;
}

AK_list_elem NextL( AK_list_elem current, AK_list *L ){
    if( current == NULL )
        return NULL;
    return current->next;
}

AK_list_elem PreviousL( AK_list_elem current, AK_list *L ){
    if( current == NULL || current == L )
        return NULL;
    AK_list_elem previous = L;
    while( previous->next != current )
        previous = previous->next;
    return previous;
}

int IsEmptyL( AK_list *L ){
    return FirstL( L ) == NULL ? 1 : 0;
}

void InsertBeforeL( int type, char* data, int size, AK_list_elem current, AK_list *L ){
    AK_list_elem new_elem = (AK_list_elem)malloc( sizeof( struct list_elem ));
    new_elem->size = size;
    new_elem->type = type;
    memcpy( new_elem->data, data, new_elem->size );
    if( current == NULL ){
        L->next = new_elem;
        new_elem->next = NULL;
        return;
    }
    AK_list_elem previous = PreviousL( current, L );
    previous->next = new_elem;
    new_elem->next = current;
}

void InsertAfterL( int type, char* data, int size, AK_list_elem current, AK_list *L ){
    AK_list_elem new_elem = (AK_list_elem)malloc( sizeof( struct list_elem ));
    new_elem->size = size;
    new_elem->type = type;
    memcpy( new_elem->data, data, new_elem->size );
    if( current == NULL ){
        L->next = new_elem;
        new_elem->next = NULL;
        return;
    }
    AK_list_elem next = NextL( current, L );
    new_elem->next = current->next;
    current->next = new_elem;
}

void InsertAtBeginL( int type, char* data, int size, AK_list *L ){
    InsertBeforeL( type, data, size, FirstL( L), L );
}

void InsertAtEndL( int type, char* data, int size, AK_list *L ){
    InsertAfterL( type, data, size, EndL( L), L );
}

void DeleteL( AK_list_elem current, AK_list *L){
    if( current == NULL )
        return;
    AK_list_elem previous = PreviousL( current, L );
    AK_list_elem next = NextL( current, L );

    previous->next = next;
    free( current );
}

void DeleteAllL( AK_list *L ){
    AK_list_elem current;
    while( current = FirstL(L)){
        L->next = current->next;
        free( current );
    }
}

int SizeL( AK_list *L ){
    int size = 0;
    AK_list_elem current = FirstL( L );
    while( current ){
        size++;
        current = NextL( current, L );
    }
    return size;
}

char* RetrieveL( AK_list_elem current, AK_list *L ){
    if( current == NULL )
        return NULL;
    char *data = (char*)malloc(current->size);
    memcpy( data, current->data, current->size );
    return data;
}

int GetTypeL( AK_list_elem current, AK_list *L ){
    return (current ==NULL ) ? 0:current->type;
}

int GetSizeL( AK_list_elem current, AK_list *L ){
    return (current ==NULL ) ? 0:current->size;
}

#endif
