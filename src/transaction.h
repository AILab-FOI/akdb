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
#include "constants.h"
#include "command.h"
#include <string.h>
extern int numberOfActiveTransactionThreads;
/**
 * @author Frane Jakelić
 * @struct transaction_locks_list_elem
 * @brief Structure that represents LockTable entry about transaction resource lock.
 */
struct transaction_locks_list_elem {
    int TransactionId;
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
    int id;
    int lock_type;
    int isWaiting;
    struct transaction_locks_list_elem *DLLLocksHead;
    struct transaction_list_elem *nextBucket;
    struct transaction_list_elem *prevBucket;
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
	int transactionId;
        int lengthOfArray;
	command *array;
};

typedef struct transactionData AK_transaction_data;
typedef struct memoryAddresses AK_memoryAddresses;
typedef struct memoryAddresses* AK_memoryAddresses_link;
typedef struct transaction_list_head AK_transaction_list;
typedef struct transaction_list_elem *AK_transaction_elem;
typedef struct transaction_locks_list_elem *AK_transaction_lock_elem;

#endif /* TRANSACTION_H_ */

int AK_calculate_hash(int memoryAddress) ;
AK_transaction_elem AK_add_hash_entry_list(int memoryAddress, int type);
AK_transaction_elem AK_search_hash_entry_list_by_key(int memoryAddress);
int AK_delete_hash_entry_list(int memoryAddress);
AK_transaction_lock_elem AK_search_lock_entry_list_by_key(int memoryAddress, int id);
int AK_delete_lock_entry_list(int memoryAddress, int id);
int AK_isLock_waiting(AK_transaction_elem lockHolder, int type, int transactionId, AK_transaction_lock_elem lock);
AK_transaction_lock_elem AK_add_lock(AK_transaction_elem LocksList, int type, int transactionId);
AK_transaction_lock_elem AK_create_lock(int memoryAddress, int type, int transactionId);
void AK_check_for_deadLock(int id);
int AK_acquire_lock(int memoryAddress, int type, int transactionId);
void AK_release_locks(AK_memoryAddresses_link addresses, int transactionId);
int AK_get_memory_blocks(char *tblName, AK_memoryAddresses *addressList);
int AK_execute_commands(command * commandArray, int lengthOfArray, int transactionID);
void AK_update_transaction_thread_count(int offset) ;
void AK_execute_transaction(void *params);
void AK_transaction_manager(command * commandArray, int lengthOfArray);
void AK_test_Transaction();
