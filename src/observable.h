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
#define MAX_OBSERVABLE_SERVERS 10

#include "constants.h"

/**
 * @author Ivan Pusic
 * @struct Observer
 * @brief Structure defines functions for observer object
 */
struct Observer {
    void (*AK_destroy_observer) (struct Observer*);
    void (*AK_notify_handler) (struct Observer*, void*);
};
typedef struct Observer AK_observer;

/**
 * @author Ivan Pusic
 * @struct Observable
 * @brief Structure defines functions for observable object
 */
struct Observable {
    // Members
    AK_observer observers[MAX_OBSERVABLE_SERVERS];

    // Methods
    int (*AK_destroy_observable) (struct Observable*);
    int (*AK_register_observer) (struct Observable*, struct AK_observer*);
    int (*AK_unregister_observer) (struct Observable*, struct AK_observer*);
    int (*AK_run_action) ();
    int (*AK_notify_observer) (struct Observable*, struct AK_observer*);
    int (*AK_notify_observers) (struct Observable*);
};
typedef struct Observable AK_observable;

#endif

extern AK_observer *AK_init_observer();
extern AK_observable *AK_init_observable();

extern void AK_observable_test();
