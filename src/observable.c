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

/** 
 * @author Ivan Pusic
 * @brief Functions to run some action on receiving message from some observer
 * 
 * @return Exit status
 */
static inline int *AK_run_action()
{
    printf("Hello from observable pattern :]");
    return OK;
}

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
    return OK;
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
    return OK;
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
    return OK;
}

/** 
 * @author Ivan Pusic
 * @brief Function to notify all observers
 * @param self Observable object instance
 * 
 * @return Exit status
 */
static inline int *AK_notify_observers(AK_observable self)
{
    return OK;
}

/** 
 * @author Ivan Pusic
 * @brief Function for initializing observable object
 *
 * @return Pointer to new observable object
 */
AK_observable *AK_init_observable()
{
    AK_observable *self;
    self = (AK_observable*) calloc(1, sizeof(self));
    self->AK_register_observer = AK_register_observer;
    self->AK_unregister_observer = AK_unregister_observer;
    self->AK_run_action = AK_run_action;
    self->AK_notify_observer = AK_notify_observer;
    self->AK_notify_observers = AK_notify_observers;

    return self;
}

/**
 * @author Ivan Pusic
 * @brief Function that runs tests for observable pattern
 */
void AK_observable_test()
{
    printf ("\n========== OBSERVABLE PATTERN BEGIN ==========\n");
    
    AK_observable *observableObject = AK_init_observable();
    observableObject->AK_run_action();
    
    printf ("\n========== OBSERVABLE PATTERN END ==========\n");
}
