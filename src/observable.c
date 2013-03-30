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
    self = (AK_observable*) calloc(1, sizeof(*self));
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
static inline int * AK_notify(AK_observer *observer, void *observable_type)
{
    observer->AK_observer_type_event_handler(observer->AK_observer_type, observable_type);
    return OK;
}

/** 
 * @author Ivan Pusic
 * @brief Function for initializing observer object
 * 
 * @return Pointer to new observer object
 */
AK_observer * AK_init_observer(void *observer_type, void (*observer_type_event_handler)(void*, void*))
{
    AK_observer *self;
    self = calloc(1, sizeof(*self));
    self->AK_destroy_observer = AK_destroy_observer;
    self->AK_observer_type = observer_type;
    self->AK_observer_type_event_handler = observer_type_event_handler;
    self->AK_notify = AK_notify;
    Ak_dbg_messg(LOW, GLOBAL, "NEW OBSERVER OBJECT INITIALIZED!");    
    return self;
}

/**
 * @author Ivan Pusic
 * @brief OBSERVABLE MANUAL
 */

// First define you observable type 
struct TypeObservable {
    // Then specify your type variables
    char *message;
    // And your methods
    char* (*AK_get_message) (struct TypeObservable*);
    
    // You should define methods for adding and removing observers. You can also define some other method for achieveing this
    int (*AK_custom_register_observer) (struct TypeObservable*, AK_observer*);
    int (*AK_custom_unregister_observer) (struct TypeObservable*, AK_observer*);
    // Define observable variable, nedded for observable pattern
    AK_observable *observable;
};
typedef struct TypeObservable AK_TypeObservable;

// Implement method for getting some message from observable custom type
char * AK_get_message(AK_TypeObservable *self) {
    return self->message;
}

// Implement method for adding observer to custom observable type
int * AK_custom_register_observer(AK_TypeObservable* self, AK_observer* observer) {
    self->observable->AK_register_observer(self->observable, observer);
    return OK;
}

// Implement method for removing observer from custom observable type
int * AK_custom_unregister_observer(AK_TypeObservable * self, AK_observer* observer) {
	self->observable->AK_unregister_observer(self->observable, observer);
    return OK;
}

// You should have some kind of method for initializing observable type
AK_TypeObservable * init_observable_type() {
    AK_TypeObservable *self;
    self = calloc(1, sizeof(AK_TypeObservable));
    self->AK_get_message = AK_get_message;
    self->AK_custom_register_observer = AK_custom_register_observer;
    self->AK_custom_unregister_observer = AK_custom_unregister_observer;
    self->message = "This is some message from my custom observable type";

    // Very important!!! Call method for initializing AK_Observable and pass instance of custom observable type
    self->observable = AK_init_observable(self);
    return self;
}

struct TypeObserver {
    // This is observable type we will track.
    // You can declare instance of custom observable type here if you want, but it isn't necessary
    AK_TypeObservable *observable;
    
    // Nedded for observable pattern
    AK_observer *observer;
}; 
typedef struct TypeObserver AK_TypeObserver;

void * custom_observer_event_handler(AK_TypeObserver *observer, AK_TypeObservable *observable) {
    // Handle event, and call some method from observable type. In this case that is method for getting some message from observable type.
    // You can also define your custom methods and call that method in event handler (here).
    char *message = observable->AK_get_message(observable);
    printf ("%s\n",message);
}

// Define some method for init observer type
AK_TypeObserver * init_observer_type(void *observable) {
    AK_TypeObserver *self;
    self = calloc(1, sizeof(AK_TypeObserver));
    //self->observable = observable;
    // Init AK_Observer type. This is very important!!! Pass custom type observer instance as first parameter, and
    // pointer to event handler function of custom observer type
    self->observer = AK_init_observer(self, custom_observer_event_handler);
    return self;
}

// Define some method for init observer type. This method is without passing custom observable type to observer.
// This is also correct, if you don't want to have instance of custom observable type in custom observer type
AK_TypeObserver * init_observer_type_second() {
    AK_TypeObserver *self;
    self = calloc(1, sizeof(AK_TypeObserver));
    // Init AK_Observer type. This is very important!!! Pass custom type observer instance as first parameter, and
    // pointer to event handler function of custom observer type
    self->observer = AK_init_observer(self, custom_observer_event_handler);
    return self;
}

/**
 * @author Ivan Pusic
 * @brief Function that runs tests for observable pattern
 */
void AK_observable_test()
{
    printf ("\n========== OBSERVABLE PATTERN BEGIN ==========\n");

    // Fucntion for init custom observable type
    AK_TypeObservable *observable_type = init_observable_type();
    // Init observer type with passing observable type instance
    AK_TypeObserver *observer_first = init_observer_type(observable_type);
    // You can also initialize obsever type without passing observable type instance
    AK_TypeObserver *observer_second = init_observer_type_second();

    // Register out observers to observable type
    observable_type->AK_custom_register_observer(observable_type, observer_first->observer);
    observable_type->AK_custom_register_observer(observable_type, observer_second->observer);

    // Notify all observers
    observable_type->observable->AK_notify_observers(observable_type->observable);
    // Notify specified observer
    observable_type->observable->AK_notify_observer(observable_type->observable, observer_first->observer);

    // Search for observer by ID
    AK_observer *requested_observer = observable_type->observable->AK_get_observer_by_id(observable_type->observable, 1);
    if(requested_observer) {
        printf ("Observer was found. Observer adress: %d\n", requested_observer);
    }
    else
        printf ("Requested observer was not found!\n");
    
    printf ("\n========== OBSERVABLE PATTERN END ==========\n");
}
