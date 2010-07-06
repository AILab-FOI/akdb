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

void InitializelistAd(list_ad *L) {
    L->next = 0;
}

element_ad GetFirstelementAd(list_ad *L) {
    return (element_ad) L->next;
}

element_ad GetLastelementAd(list_ad *L) {
    list_ad *Currentelement_op;
    Currentelement_op = L;
    while (Currentelement_op->next)
        Currentelement_op = (element_ad) Currentelement_op->next;
    if (Currentelement_op != L)
        return (element_ad) Currentelement_op;
    else
        return 0;
}

element_ad GetNextelementAd(element_ad Currentelement_op) {
    if (Currentelement_op->next == 0) {
        return 0;
    } else {
        list_ad *Nextelement_op;
        Nextelement_op = (element_ad) Currentelement_op->next;
        return (element_ad) Nextelement_op;
    }
}

element_ad GetPreviouselementAd(element_ad Currentelement_op, element_ad L) {
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

int GetPositionOfelementAd(element_ad Searchedelement_op, list_ad *L) {
    list_ad *Currentelement_op;
    int i = 0;
    Currentelement_op = L;
    while (Currentelement_op->next != 0 && Currentelement_op != Searchedelement_op) {
        Currentelement_op = (list_ad *) Currentelement_op->next;
        i++;
    }
    return i;
}

void DeleteelementAd(element_ad Deletedelement_op, list_ad *L) {
    element_ad Previouselement_op = (element_ad) GetPreviouselementAd(Deletedelement_op, L);
    if (Previouselement_op != 0) {
        Previouselement_op->next = Deletedelement_op->next;
    } else {
        L->next = Deletedelement_op->next;
    }
    free(Deletedelement_op);
}

void DeleteAllelementsAd(list_ad *L) {
    list_ad *Currentelement_op = L;
    list_ad *Deletedelement_op = (list_ad *) L->next;
    while (Currentelement_op->next != 0) {
        Currentelement_op->next = Deletedelement_op->next;
        free(Deletedelement_op);
        Deletedelement_op = (list_ad *) Currentelement_op->next;
    }
}

void InsertNewelementAd(int addBlock, int indexTd, char *attName, element_ad elementBefore) {
    list_ad *newelement_op = (list_ad *) malloc(sizeof (list_ad));
    newelement_op->add.addBlock = addBlock;
    newelement_op->add.indexTd = indexTd;
    newelement_op->attName = attName;
    newelement_op->next = elementBefore->next;
    elementBefore->next = newelement_op;
}

