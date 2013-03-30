/**
   @file observable.h Header file that provides data structures and declarations of functions for observable pattern
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

#ifndef OBSERVABLE
#define OBSERVABLE

#include "constants.h"
#include "debug.h"

/**
 * @author Ivan Pusic
 * @enum ObservableType
 * @brief Enumeration which defines possible observable types. You can also add your custom observable type here
 */
typedef enum {
	AK_TRANSACTION,
	AK_TRIGGER,
	AK_CUSTOM,
} AK_ObservableType_Enum;

/**
 * @author Ivan Pusic
 * @struct Observer
 * @brief Structure defines functions for observer object
 */
struct Observer {
    // Members
    int observer_id;
    void * AK_observer_type;
    
    // Methods
    int (*AK_observer_type_event_handler) (void *, void *, AK_ObservableType_Enum);
    int (*AK_notify) (struct Observer*, void *observable_type, AK_ObservableType_Enum);
    int (*AK_destroy_observer) (struct Observer*);
};
typedef struct Observer AK_observer;


/**
 * @author Ivan Pusic
 * @struct Observable
 * @brief Structure defines functions for observable object
 */
struct Observable {
    // Members
    AK_observer *observers[MAX_OBSERVABLE_OBSERVERS];
    int observer_id_counter;
    void * AK_observable_type;
    int AK_ObservableType_Def;
    
    // Methods
    int (*AK_destroy_observable) (struct Observable*);
    int (*AK_register_observer) (struct Observable*, struct AK_observer*);
    int (*AK_unregister_observer) (struct Observable*, struct AK_observer*);
    int (*AK_notify_observer) (struct Observable*, struct AK_observer*);
    int (*AK_notify_observers) (struct Observable*);
    AK_observer* (*AK_get_observer_by_id) (struct AK_observable*, int id);
};
typedef struct Observable AK_observable;

#endif

extern AK_observer * AK_init_observer(void *observable_type, void (*observable_type_event_handler)(void*, void*));
extern AK_observable * AK_init_observable(void *AK_observable_type, AK_ObservableType_Enum AK_ObservableType_Def);
extern void AK_observable_test();
