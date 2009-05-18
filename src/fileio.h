/**
@file fileio.h Provides data structures for file input/output
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "configuration.h"

typedef struct{
	int address_from[200];
	int address_to[200];

}table_addresses;

typedef struct {
	///START row_element
	///structure that reperesents one row of table that is inserted, updated, or deleted
	int type;
	unsigned char data[200];
	char table[ MAX_ATT_NAME ];
	char attribute_name[ MAX_ATT_NAME ];
	int constraint;
//možda dodati dali je atribut ograničenje ili nova vrijednost treba kod update-a
	///END row_element
	//you can add here your own attributes, but then you must implement your own special functions
	struct list_structure *next;
}list_structure;

typedef list_structure *element;
typedef list_structure list;

///START GLOBAL FUNCTIONS

//Alocate empty list
void InitializeList(list *L)
{
	L->next = 0;
}
 
//returns frst list elementint type;
	char data[255];
	char table;
	char attribute_name;
element GetFirstElement(list *L)
{
	return L->next;
}

//returns last list element
element GetLastElement(list *L)
{
	list *CurrentElement;
	CurrentElement = L;
	while (CurrentElement->next) 
		CurrentElement = CurrentElement->next;
	if(CurrentElement!=L)
		return CurrentElement;
	else 
		return 0;
}

//returns next element of given current element 
element GetNextElement(element CurrentElement)
{
	if (CurrentElement->next == 0) {
//		cout << "Element ne postoji!" <<endl;
		return 0;
	} else {
		list *NextElement;
		NextElement = CurrentElement->next;
		return NextElement;
	}
}

//returns previous element of given current element, must alsa give list as second parameter
element GetPreviousElement(element CurrentElement, list *L)
{
	list *PreviousElement;
	PreviousElement = L;
	while (PreviousElement->next != 0 && PreviousElement->next != CurrentElement)
		PreviousElement = PreviousElement->next;
	if (PreviousElement->next != 0 && PreviousElement!=L) {
		return PreviousElement;
	} else {
//		cout << "Element ne postoji!" <<endl;
		return 0;
	}
}

//Retruns number positon of some element 
int GetPositionOfElement(element SearchedElement, list *L)
{
	list *CurrentElement;
	int i = 0;
	CurrentElement = L;
	while (CurrentElement->next != 0 && CurrentElement != SearchedElement) {
		CurrentElement = CurrentElement->next;
		i++;
	}
	return i;
}

///Delete element in list
void DeleteElement(element DeletedElement, list *L)
{
	element PreviousElement = GetPreviousElement(DeletedElement,L);
		if(PreviousElement!=0)
		{	
			PreviousElement->next = DeletedElement->next;
		}
		else
		{
			L->next=DeletedElement->next;
		}
	free(DeletedElement);
}

//delete all list elements
void DeleteAllElements(list *L)
{
	list *CurrentElement = L, *DeletedElement=L->next;	
	while (CurrentElement->next != 0) {
		CurrentElement->next = DeletedElement->next;
		//clear(DeletedElement);
		free(DeletedElement);
		DeletedElement = CurrentElement->next;
	}	
}

///END GLOBAL FUNCTIONS

///START SPECIAL FUNCTIONS FOR WORK WITH row_element_structure

///Insert new element after some element, to insert on first place give list as before element
void InsertNewElement(int newtype, void * data, char * table, char * attribute_name, element ElementBefore)
{
	list *newElement = (list *) malloc( sizeof(list) );
	newElement->type = newtype;
	memcpy(newElement->data, data, KK_type_size(newtype, data));
/*	int free=0;
	for(free;free< MAX_ATT_NAME;free++)
			newElement->table[free]='\0';*/
	memcpy(newElement->table, table, strlen(table));
	newElement->table[strlen(table)]='\0';
	memcpy(newElement->attribute_name, attribute_name, strlen(attribute_name));
	newElement->attribute_name[strlen(attribute_name)]='\0';
	if(newtype==TYPE_VARCHAR)
	{
		newElement->data[KK_type_size(newtype, data)]='\0';
	}
	newElement->next = ElementBefore->next;
	ElementBefore->next = newElement;
}

void InsertNewElementForUpdate(int newtype, void * data, char * table, char * attribute_name, element ElementBefore, int newconstraint)
{
	list *newElement = (list *) malloc( sizeof(list) );
	newElement->type = newtype;
	memcpy(newElement->data, data, KK_type_size(newtype, data));
/*	int free=0;
	for(free;free< MAX_ATT_NAME;free++)
			newElement->table[free]='\0';*/
	printf("\n u hederu: %s",table);
	memcpy(newElement->table, table, strlen(table));
	printf("\n u hederu: %s",newElement->table);
	newElement->table[strlen(table)]='\0';
	printf("\n u hederu: %s",newElement->table);
	memcpy(newElement->attribute_name, attribute_name, strlen(attribute_name));
	newElement->attribute_name[strlen(attribute_name)]='\0';
	if(newtype==TYPE_VARCHAR)
	{
		newElement->data[KK_type_size(newtype, data)]='\0';
	}
	newElement->constraint = newconstraint;
	newElement->next = ElementBefore->next;
	ElementBefore->next = newElement;
}
///END SPECIAL FUNCTIONS row_element_structure
