/**
@file transaction.h Header file that defines includes and datastructures for the transaction execution
 */
/*
 *
 * transaction.h is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * transaction.h is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRANSACTION_H_
#define TRANSACTION_H_
#include <pthread.h> //TRANSACTIONS
#include "../auxi/constants.h"
#include "../auxi/configuration.h"
#include "../mm/memoman.h"
#include "../sql/command.h"
#include "../auxi/observable.h"
#include "../file/table.h"
#include "../file/fileio.h"
#include <string.h>
#include "../auxi/mempro.h"

/**
 * @author Ivan Pusic
 * @enum NoticeType
 * @brief Enumeration which define notice types for transactions
 */
typedef enum {
    AK_LOCK_RELEASED,
    AK_TRANSACTION_FINISHED,
    AK_ALL_TRANSACTION_FINISHED,
} NoticeType;

/**
 * @author Ivan Pusic
 * @struct observable_transaction
 * @brief Structure which defines transaction observable type
 */
struct observable_transaction_struct {
    int (*AK_transaction_register_observer) (struct observable_transaction_struct*, AK_observer*);
    int (*AK_transaction_unregister_observer) (struct observable_transaction_struct*, AK_observer*);
    void (*AK_lock_released) ();
    void (*AK_transaction_finished) ();
    void (*AK_all_transactions_finished) ();
    AK_observable *observable;
};
typedef struct observable_transaction_struct AK_observable_transaction;

/**
 * @author Ivan Pusic
 * @struct observer_lock
 * @brief Structure which defines transaction lock observer type
 */
struct observer_lock {
    AK_observer *observer;
};
typedef struct observer_lock AK_observer_lock;

/**
 * @author Frane Jakelić
 * @struct transaction_locks_list_elem
 * @brief Structure that represents LockTable entry about transaction resource lock.
 */
struct transaction_locks_list_elem {
	pthread_t TransactionId;
    int lock_type;
    int isWaiting;
    struct transaction_locks_list_elem *nextLock;
    struct transaction_locks_list_elem *prevLock;
};

/**
 * @author Frane Jakelić
 * @struct transaction_list_elem
 * @brief Structure that represents LockTable entry about transaction lock holder.Element indexed by Hash table.
 */
struct transaction_list_elem {
	int address;
    int lock_type;
    int isWaiting;
    struct transaction_locks_list_elem *DLLLocksHead;
    struct transaction_list_elem *nextBucket;
    struct transaction_list_elem *prevBucket;

    // Observer 
    AK_observer_lock *observer_lock;
};

/**
 * @author Frane Jakelić
 * @struct transaction_list_head
 * @brief Structure that represents LockTable entry about doubly linked list of collision in Hash table.
 */
struct transaction_list_head {
    struct transaction_list_elem *DLLHead;
};

/**
 * @author Frane Jakelić
 * @struct memoryAddresses
 * @brief Structure that represents a linked list of locked addresses.
 */
struct memoryAddresses{
	int adresa;
	struct memoryAddresses *nextElement;
};
/**
 * @author Frane Jakelić
 * @struct transactionData
 * @brief Structure used to transport transaction data to the thread.
 */
struct transactionData{
    int lengthOfArray;
	command *array;
};

typedef struct transactionData AK_transaction_data;
typedef struct memoryAddresses AK_memoryAddresses;
typedef struct memoryAddresses* AK_memoryAddresses_link;

typedef struct transaction_list_head AK_transaction_list;
typedef struct transaction_list_elem* AK_transaction_elem_P;
typedef struct transaction_list_elem AK_transaction_elem;

typedef struct transaction_locks_list_elem* AK_transaction_lock_elem_P;
typedef struct transaction_locks_list_elem AK_transaction_lock_elem;

 /**     
  * @author Frane Jakelić    
  * @struct threadContainer      
  * @brief Structure that represents a linked list of threads.   
  */     
 struct threadContainer{     
         pthread_t thread;   
         struct threadContainer *nextThread;     
 };      
     
     
 typedef struct threadContainer *AK_thread_elem;     
 typedef struct threadContainer AK_thread_Container;


int AK_memory_block_hash(int) ;
AK_transaction_elem_P AK_search_existing_link_for_hook(int);
AK_transaction_elem_P AK_search_empty_link_for_hook(int);
AK_transaction_elem_P AK_add_hash_entry_list(int, int);
int AK_delete_hash_entry_list(int);
AK_transaction_lock_elem_P AK_search_lock_entry_list_by_key(AK_transaction_elem_P, int, pthread_t);
int AK_delete_lock_entry_list(int, pthread_t);
int AK_isLock_waiting(AK_transaction_elem_P, int, pthread_t, AK_transaction_lock_elem_P);
AK_transaction_lock_elem_P AK_add_lock(AK_transaction_elem_P, int, pthread_t);
AK_transaction_lock_elem_P AK_create_lock(int, int, pthread_t);
int AK_acquire_lock(int, int, pthread_t);
void AK_release_locks(AK_memoryAddresses_link, pthread_t);
int AK_get_memory_blocks(char*, AK_memoryAddresses_link);
int AK_execute_commands(command* , int);
void * AK_execute_transaction(void*);
void AK_transaction_manager(command*, int);
void AK_test_Transaction();
int AK_create_new_transaction_thread(AK_transaction_data*);
int AK_remove_transaction_thread(pthread_t);
void handle_transaction_notify(AK_observer_lock*);
void AK_on_observable_notify(void*, void*, AK_ObservableType_Enum);
void AK_on_transaction_end(pthread_t);
void AK_on_lock_release();
void AK_on_all_transactions_end();
void AK_handle_observable_transaction_action(NoticeType*);
void AK_lock_released();
void AK_transaction_finished();
void AK_all_transactions_finished();
int AK_transaction_register_observer(AK_observable_transaction*, AK_observer*);
int AK_transaction_unregister_observer(AK_observable_transaction*, AK_observer*);
AK_observable_transaction * AK_init_observable_transaction();
AK_observer_lock * AK_init_observer_lock();

#endif /* TRANSACTION_H_ */
