/**
@file observable.c File that provides implementations of functions for observable pattern
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

#include "observable.h"

/******************** OBSERVABLE IMPLEMENTATION ********************/

/** 
 * @author Ivan Pusic
 * @brief Function to registering new observer. Observer is added to list of all observers
 * @param self Observable object instance
 * @param observer Observer object instance
 * 
 * @return Exit status 
 */
static inline int *AK_register_observer(AK_observable *self, AK_observer *observer)
{
    int i;
    for (i = 0; i < MAX_OBSERVABLE_OBSERVERS; ++i) {
        if(self->observers[i] == NULL) {
            // Assigning unique ID to new observer
            observer->observer_id = self->observer_id_counter;
            self->observer_id_counter++;
            self->observers[i] = observer;
            Ak_dbg_messg(LOW, GLOBAL, "NEW OBSERVER ADDED");
            return OK;
        }
    }
    Ak_dbg_messg(LOW, GLOBAL, "ERROR IN FUNCTION FOR ADDING NEW OBSERVER!");
    
    return NOT_OK;
}

/** 
 * @author Ivan Pusic
 * @brief Function to unregister observer. Observer will be removed from list of all observers
 * @param self Observable object instance
 * @param observer Observer object instance
 * 
 * @return Exit status
 */
static inline int *AK_unregister_observer(AK_observable *self, AK_observer *observer)
{
    int i;
    for(i = 0; i < MAX_OBSERVABLE_OBSERVERS; ++i) {
        if(observer == self->observers[i]) {
            free(observer);
            observer = NULL;
            Ak_dbg_messg(LOW, GLOBAL, "OBSERVER DELETED");
            return OK;
        }
    }
    
    Ak_dbg_messg(LOW, GLOBAL, "ERROR IN FUNCTION FOR DELETING OBSERVER!");    
    return NOT_OK;
}

/** 
 * @author Ivan Pusic
 * @brief Function to notiy certain observer
 * @param self Observable object instance
 * @param observer Observer object to notify
 * 
 * @return Exit status
 */
static inline int *AK_notify_observer(AK_observable *self, AK_observer *observer)
{
    int i;
    for(i = 0; i < MAX_OBSERVABLE_OBSERVERS; ++i) {
        if(self->observers[i] != NULL && self->observers[i] == observer) {
            observer->AK_notify(observer, self->AK_observable_type);
            Ak_dbg_messg(LOW, GLOBAL, "NOTIFICATION SENT TO OBSERVER");
            return OK;
        }
    }
    
    Ak_dbg_messg(LOW, GLOBAL, "ERROR IN FUNCTION FOR SENDING NOTIFICATION TO SPECIFIED OBSERVER!");
    return NOT_OK;
}

/** 
 * @author Ivan Pusic
 * @brief Function to notify all observers
 * @param self Observable object instance
 * 
 * @return Exit status
 */
static inline int *AK_notify_observers(AK_observable *self)
{
    int i, id;
    for(i = 0; i < MAX_OBSERVABLE_OBSERVERS; ++i) {
        if(self->observers[i] != NULL) {
            self->observers[i]->AK_notify(self->observers[i], self->AK_observable_type);
            id = self->observers[i]->observer_id;
            printf ("OBSERVER ID: %d\n", id);
        }
    }
    
    Ak_dbg_messg(LOW, GLOBAL, "OBSERVERS NOTIFIED");
    return OK;
}

/** 
 * @author Ivan Pusic
 * @brief Function for getting observer object from observable type by observer id
 * @param self Observable type instance
 * @param id Observer id
 * 
 * @return Requested observer instance
 */
static inline AK_observer * AK_get_observer_by_id(AK_observable *self, int id)
{
    int i;
    for(i = 0; i < MAX_OBSERVABLE_OBSERVERS; ++i) {
        if(self->observers[i] != NULL && self->observers[i]->observer_id == id) {
            Ak_dbg_messg(LOW, GLOBAL, "REQUESTED OBSERVER FOUND. RETURNING OBSERVER BY ID");
            return self->observers[i];
        }
    }
    Ak_dbg_messg(LOW, GLOBAL, "OBSERVER WITH ID:%d DOESN'T EXIST!", id);
    return NULL;
}

/** 
 * @author Ivan Pusic
 * @brief Function for initializing observable object
 *
 * @return Pointer to new observable object
 */
AK_observable * AK_init_observable(void *AK_observable_type)
{
    AK_observable *self;
    self = (AK_observable*) calloc(1, sizeof(AK_observable));
    self->AK_register_observer = AK_register_observer;
    self->AK_unregister_observer = AK_unregister_observer;
    self->AK_notify_observer = AK_notify_observer;
    self->AK_notify_observers = AK_notify_observers;
    self->AK_get_observer_by_id = AK_get_observer_by_id;

    self->observer_id_counter = 1;
    self->AK_observable_type = AK_observable_type;
    Ak_dbg_messg(LOW, GLOBAL, "NEW OBSERVABLE OBJECT INITIALIZED!");
    
    return self;
}

/******************** OBSERVER IMPLEMENTATION ********************/

/** 
 * @author Ivan Pusic
 * @brief Function for destroying observer object
 *
 * @return Exit status
 */
static inline int *AK_destroy_observer(AK_observer *self)
{
    if(self != NULL) {
        free(self);
        self = NULL;
        Ak_dbg_messg(LOW, GLOBAL, "OBSERVER DESTROYED!");        
        return OK;
    }
    
    Ak_dbg_messg(LOW, GLOBAL, "ERROR WHILE DESTROYING OBSERVER");
    return NOT_OK;
}

/** 
 * @author Ivan Pusic
 * @brief Function for calling event handler of observer object
 * @param observer Observer object instance
 * @param observable_type Observable object instance
 * 
 * @return Exit status
 */
static inline int AK_notify(AK_observer *observer, void *observable_type)
{
    return OK;
}

/** 
 * @author Ivan Pusic
 * @brief Function for initializing observer object
 * 
 * @return Pointer to new observer object
 */
AK_observer * AK_init_observer(void *observable_type, void (*observable_type_event_handler)(void*, void*))
{
    AK_observer *self;
    self = calloc(1, sizeof(AK_observer));
    self->AK_destroy_observer = AK_destroy_observer;
    self->AK_observable_type = observable_type;
    self->AK_observable_type_event_handler = observable_type_event_handler;
    self->AK_notify = AK_notify;
    
    Ak_dbg_messg(LOW, GLOBAL, "NEW OBSERVER OBJECT INITIALIZED!");    
    return self;
}

/**
 * @author Ivan Pusic
 * @brief Function that runs tests for observable pattern
 */
void AK_observable_test()
{
    printf ("\n========== OBSERVABLE PATTERN BEGIN ==========\n");
    AK_observable *observableObject = AK_init_observable(NULL);

    AK_observer *observer_first = AK_init_observer(NULL, NULL);
    AK_observer *observer_second = AK_init_observer(NULL, NULL);
    
    observableObject->AK_register_observer(observableObject, observer_first);
    observableObject->AK_register_observer(observableObject, observer_second);

    observableObject->AK_notify_observers(observableObject);
    AK_observer *requested_observer = observableObject->AK_get_observer_by_id(observableObject, observer_second->observer_id);
    printf("Returned observer id: %d", requested_observer->observer_id);
    
    observableObject->AK_unregister_observer(observableObject, observer_first);
    observableObject->AK_unregister_observer(observableObject, observer_second);
    
    printf ("\n========== OBSERVABLE PATTERN END ==========\n");
}

/**
 * @author Ivan Pusic
 * @brief OBSERVABLE MANUAL
 *
 *
 *
 *
 *
 *
 *
 * 
 */

