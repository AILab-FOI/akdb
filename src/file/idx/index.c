/**
@file index.c Provides functions for indexes
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
 17 */

#include "index.h"
#include <stdlib.h>

/**
 * @author Unknown 
 * @brief Function for initalizing linked list
 * @param list_ad *L linked list head
 * @return No return value
 * */
void Ak_InitializelistAd(list_ad *L) {
    L->next = 0;
}

/**
 * @author Unknown
 * @brief Function for finding first node of linked list
 * @param list_ad *L linked list head
 * @return Address of first node
 * */
element_ad Ak_Get_First_elementAd(list_ad *L) {
    return (element_ad) L->next;
}

/**
 * @author Unknown
 * @brief Function for finding last node of linked list  
 * @param list_ad *L linked list head
 * @return Address of last node or 0 if list is empty
 * */
element_ad Ak_Get_Last_elementAd(list_ad *L) {
    list_ad *Currentelement_op;
    Currentelement_op = L;
    while (Currentelement_op->next)
        Currentelement_op = (element_ad) Currentelement_op->next;
    if (Currentelement_op != L)
        return (element_ad) Currentelement_op;
    else
        return 0;
}

/**
 * @author Unknown
 * @brief Function for finding the next node of a node in linked list
 * @param Currentelement_op address of current node
 * @return Address of next node or 0 if current node is last in list
 * */
element_ad Ak_Get_Next_elementAd(element_ad Currentelement_op) {
    if (Currentelement_op->next == 0) {
        return 0;
    } else {
        list_ad *Nextelement_op;
        Nextelement_op = (element_ad) Currentelement_op->next;
        return (element_ad) Nextelement_op;
    }
}

/**
 * @author Unknown
 * @brief Function for finding the previous node of a node in linked list
 * @param Currentelement_op Address of current node
 * @param L previous element
 * @return Address of previous node or 0 if the current node is the head or the list is empty
 * */
element_ad Ak_Get_Previous_elementAd(element_ad Currentelement_op, element_ad L) {
    element_ad Previouselement_op;
    Previouselement_op = L;
    while ((Previouselement_op->next != 0) && ((element_ad) Previouselement_op->next != Currentelement_op))
        Previouselement_op = (element_ad) Previouselement_op->next;
    if (Previouselement_op->next != 0 && Previouselement_op != L) {
        return (element_ad) Previouselement_op;
    } else {
        return 0;
    }
}

/**
 * @author Unknown
 * @brief Function for finding the position of a node in linked list
 * @param Searchedelement_op address of current note
 * @param *L linked list head
 * @return Integer value of current node's order in the list
 * */
int Ak_Get_Position_Of_elementAd(element_ad Searchedelement_op, list_ad *L) {
    list_ad *Currentelement_op;
    int i = 0;
    Currentelement_op = L;
    while (Currentelement_op->next != 0 && Currentelement_op != Searchedelement_op) {
        Currentelement_op = (list_ad *) Currentelement_op->next;
        i++;
    }
    return i;
}

/**
 * @author Unknown
 * @brief Function for deleting a node in linked list
 * @param Deletedelement_op - address of node to delete
 * @param list_ad *L - list head
 * @return No return value
 * */
void Ak_Delete_elementAd(element_ad Deletedelement_op, list_ad *L) {
    element_ad Previouselement_op = (element_ad) Ak_Get_Previous_elementAd(Deletedelement_op, L);
    if (Previouselement_op != 0) {
        Previouselement_op->next = Deletedelement_op->next;
    } else {
        L->next = Deletedelement_op->next;
    }
    free(Deletedelement_op);
}

/**
 * @author Unknown
 * @brief Function for deleting all nodes in linked list  
 * @param L list head
 * @return No return value
 * */
void Ak_Delete_All_elementsAd(list_ad *L) {
    list_ad *Currentelement_op = L;
    list_ad *Deletedelement_op = (list_ad *) L->next;
    while (Currentelement_op->next != 0) {
        Currentelement_op->next = Deletedelement_op->next;
        free(Deletedelement_op);
        Deletedelement_op = (list_ad *) Currentelement_op->next;
    }
}


/**
 * @author Unknown
 * @brief Function for inserting a new element into linked list
 * @param addBlock address block
 * @param indexTd index table destination
 * @param *attname attribute name
 * @param elementBefore address of the node after which the new node will be inserted
 * @return No return value
 * */
void Ak_Insert_NewelementAd(int addBlock, int indexTd, char *attName, element_ad elementBefore) {
    element_ad newelement_op = (element_ad) malloc(sizeof (element_ad));
    newelement_op->add.addBlock = addBlock;
    newelement_op->add.indexTd = indexTd;
    newelement_op->attName = attName;
    newelement_op->next = elementBefore->next;
    elementBefore->next = newelement_op;
}

