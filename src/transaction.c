/**
@file transaction.c Defines functions for transaction execution
 */
/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 *
 * transaction.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * transaction.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "transaction.h"

AK_transaction_list LockTable[NUMBER_OF_KEYS];
pthread_mutex_t accessLockMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t transactionThreads[NUMBER_OF_THREADS];
/**
 * @author Frane Jakelić
 * @brief Calculates hash value for a given memory address. Hash values are used to identify location of locked resources.
 * @todo The current implementation is very limited it doesn't cope well with collision. recommendation use some better version of hash calculation. Maybe Knuth's memory address hashing function.
 * @param memoryAddress integer representation of memory address, the hash value is calculated from this parameter.
 * @return integer containing the hash value of the passed memory address
 */
int AK_calculate_hash(int memoryAddress) {
    int tmp = memoryAddress % NUMBER_OF_KEYS;
    return tmp >= 0 ? tmp : (-1 * tmp);

}

/**
 * @author Frane Jakelić
 * @brief Adds an element to the doubly linked list.
 * @param memoryAddress integer representation of memory address.
 * @param type of lock issued to the provided memory address.
 * @return pointer to the newly created doubly linked element.
 */
AK_transaction_elem AK_add_hash_entry_list(int memoryAddress, int type) {

    int hash = AK_calculate_hash(memoryAddress);
    AK_transaction_elem bucket = (AK_transaction_elem) malloc(sizeof (struct transaction_list_elem));
    AK_transaction_elem root = LockTable[hash].DLLHead;
    memset(bucket, 0, sizeof (struct transaction_list_elem));

    if (root == NULL) {

        LockTable[hash].DLLHead = bucket;
        bucket->prevBucket = bucket;
        bucket->nextBucket = bucket;

    } else {

        bucket->nextBucket = root;
        bucket->prevBucket = root->prevBucket;
        (*root->prevBucket).nextBucket = bucket;
        root->prevBucket = bucket;

    }

    bucket->id = memoryAddress;
    bucket->lock_type = type;

    return bucket;

}

/**
 * @author Frane Jakelić
 * @brief Searches for a specific entry in the LockTable hash list using the memory address as it's key.
 * @param memoryAddress integer representation of memory address.
 * @return NULL pointer if the element is not found otherwise it returns a pointer to the found element
 */
AK_transaction_elem AK_search_hash_entry_list_by_key(int memoryAddress) {
    int hash = AK_calculate_hash(memoryAddress);
    AK_transaction_elem tmpElem = LockTable[hash].DLLHead;

    if (tmpElem == NULL || tmpElem->id == memoryAddress) {
        return tmpElem;
    }

    while (tmpElem->nextBucket != LockTable[hash].DLLHead) {

        tmpElem = tmpElem->nextBucket;

        if (tmpElem->id == memoryAddress) {
            return tmpElem;
        }
    }
    return NULL;
}

/**
 * @author Frane Jakelić
 * @brief Deletes a specific element in the lockTable doubly linked list.
 * @param memoryAddress integer representation of memory address.
 * @return integer OK or NOT_OK based on success of finding the specific element in the list.
 */
int AK_delete_hash_entry_list(int memoryAddress) {
    int hash = AK_calculate_hash(memoryAddress);
    AK_transaction_elem elemDelete = AK_search_hash_entry_list_by_key(memoryAddress);

    if (elemDelete != NULL) {

        (*elemDelete->prevBucket).nextBucket = elemDelete->nextBucket;
        (*elemDelete->nextBucket).prevBucket = elemDelete->prevBucket;


        if (elemDelete == LockTable[hash].DLLHead && elemDelete->nextBucket != elemDelete) {
            LockTable[hash].DLLHead = elemDelete->nextBucket;
        } else {
            LockTable[hash].DLLHead = NULL;
        }

        elemDelete->prevBucket = NULL;
        elemDelete->nextBucket = NULL;

        return OK;

    } else {
        return NOT_OK;
    }
}

/**
 * @author Frane Jakelić
 * @brief Searches for a specific entry in the Locks doubly linked list using the transaction id as it's key.
 * @param memoryAddress integer representation of memory address.
 * @param id integer representation of transaction id.
 * @return NULL pointer if the element is not found otherwise it returns a pointer to the found element
 */
AK_transaction_lock_elem AK_search_lock_entry_list_by_key(int memoryAddress, int id) {

    AK_transaction_lock_elem tmpElem = AK_search_hash_entry_list_by_key(memoryAddress)->DLLLocksHead;
    AK_transaction_lock_elem anchor = tmpElem;

    if (tmpElem == NULL || tmpElem->TransactionId == id) {
        return tmpElem;
    }

    while (tmpElem != anchor) {

        if (tmpElem->TransactionId == id) {
            return tmpElem;
        }

        tmpElem = tmpElem->nextLock;

    }
    return NULL;
}

/**
 * @author Frane Jakelić
 * @brief Deletes a specific entry in the Locks doubly linked list using the transaction id as it's key.
 * @param memoryAddress integer representation of memory address.
 * @param id integer representation of transaction id.
 * @return int OK or NOT_OK based on success of finding the specific element in the list.
 */
int AK_delete_lock_entry_list(int memoryAddress, int id) {
    AK_transaction_lock_elem elemDelete = AK_search_lock_entry_list_by_key(memoryAddress, id);
    AK_transaction_elem elemListHolder = AK_search_hash_entry_list_by_key(memoryAddress);

    if (elemDelete != NULL) {
        
        do{
        printf("Lock released by transId:%i. Memory address:%i\n", elemDelete->TransactionId,elemListHolder->id);
        (*elemDelete->prevLock).nextLock = elemDelete->nextLock;
        (*elemDelete->nextLock).prevLock = elemDelete->prevLock;

        if (elemDelete == elemListHolder->DLLLocksHead && elemDelete->nextLock != elemDelete) {
            elemListHolder->DLLLocksHead = elemDelete->nextLock;
        } else if (elemDelete == elemListHolder->DLLLocksHead) {
            elemListHolder->DLLLocksHead = NULL;
        }

        elemDelete->prevLock = NULL;
        elemDelete->nextLock = NULL;
        
        elemDelete = AK_search_lock_entry_list_by_key(memoryAddress, id);
        }while(elemDelete != NULL);
        return OK;

    } else {
        return NOT_OK;
    }
}

/**
 * @author Frane Jakelić
 * @brief Based on the parameters puts an transaction action in waiting phase or let's the transaction do it's actions.
 * @param lockHolder pointer to the hash list entry that is entitled to the specific memory address.
 * @param type of lock issued to the provided memory address.
 * @param transactionId integer representation of transaction id.
 * @param lock pointer to the lock element that is being tested.
 * @return int PASS_LOCK_QUEUE or WAIT_FOR_UNLOCK based on the rules described inside the function.
 */
int AK_isLock_waiting(AK_transaction_elem lockHolder, int type, int transactionId, AK_transaction_lock_elem lock) {

    AK_transaction_lock_elem tmp = lockHolder->DLLLocksHead;

    if (tmp == lock) {
        lockHolder->lock_type = type;
        return PASS_LOCK_QUEUE;
    } else if (tmp->lock_type == SHARED_LOCK) {

        if (type == SHARED_LOCK) {
            while (tmp->nextLock != lock) {
                if (tmp->nextLock->TransactionId != transactionId && tmp->lock_type == EXCLUSIVE_LOCK) {
                    return WAIT_FOR_UNLOCK;
                }
                tmp = tmp->nextLock;
            }
            return PASS_LOCK_QUEUE;

        } else if (type == EXCLUSIVE_LOCK) {
            while (tmp->nextLock != lock) {

                if (tmp->nextLock->TransactionId != transactionId) {

                    return WAIT_FOR_UNLOCK;
                }
                tmp = tmp->nextLock;
            }
            lockHolder->lock_type = EXCLUSIVE_LOCK;
            return PASS_LOCK_QUEUE;

        }

    } else if (tmp->lock_type == EXCLUSIVE_LOCK) {
        while (tmp->nextLock != lock) {

            if (tmp->nextLock->TransactionId != transactionId) {

                return WAIT_FOR_UNLOCK;
            }
            tmp = tmp->nextLock;
        }
        return PASS_LOCK_QUEUE;
    }

    return WAIT_FOR_UNLOCK;
}

/**
 * @author Frane Jakelić
 * @brief Adds an element to the locks doubly linked list.
 * @param memoryAddress integer representation of memory address.
 * @param type of lock issued to the provided memory address.
 * @param transactionId integer representation of transaction id.
 * @return pointer to the newly created Locks doubly linked element.
 */
AK_transaction_lock_elem AK_add_lock(AK_transaction_elem LocksList, int type, int transactionId) {
    AK_transaction_lock_elem root = LocksList->DLLLocksHead;
    AK_transaction_lock_elem lock = (AK_transaction_lock_elem) malloc(sizeof (struct transaction_locks_list_elem));

    memset(lock, 0, sizeof (struct transaction_locks_list_elem));

    if (root == NULL) {
        LocksList->DLLLocksHead = lock;
        lock->prevLock = lock;
        lock->nextLock = lock;

    } else {
        lock->nextLock = root;
        lock->prevLock = root->prevLock;
        (*root->prevLock).nextLock = lock;
        root->prevLock = lock;
    }

    lock->TransactionId = transactionId;
    lock->lock_type = type;

    lock->isWaiting = AK_isLock_waiting(LocksList, type, transactionId, lock);

    return lock;
}

/**
 * @author Frane Jakelić
 * @brief Helper function that determines if there is a hash LockTable entry that corresponds to the given memory address. And if there isn't an entry the function calls for the creation of the Locks list holder.
 * @param memoryAddress integer representation of memory address.
 * @param type of lock issued to the provided memory address.
 * @param transactionId integer representation of transaction id.
 * @return pointer to the newly created Locks doubly linked element.
 */
AK_transaction_lock_elem AK_create_lock(int memoryAddress, int type, int transactionId) {
    AK_transaction_elem elem = AK_search_hash_entry_list_by_key(memoryAddress);

    if (elem != NULL) {
        return AK_add_lock(elem, type, transactionId);

    } else {
        elem = AK_add_hash_entry_list(memoryAddress, type);
        return AK_add_lock(elem, type, transactionId);
    }
    return NULL;
}

/**
 * @author Frane Jakelić
 * @brief Main interface function for the transaction API. It is responsible for the whole process of creating a new lock.
 * @todo Implement a better deadlock detection. This method uses a very simple approach. It waits for 60sec before it restarts a transaction.
 * @param memoryAddress integer representation of memory address.
 * @param type of lock issued to the provided memory address.
 * @param transactionId integer representation of transaction id.
 * @return OK or NOT_OK based on the success of the function.
 */

int AK_acquire_lock(int memoryAddress, int type, int transactionId) {
    pthread_mutex_lock(&accessLockMutex);
    AK_transaction_lock_elem lock = AK_create_lock(memoryAddress, type, transactionId);
    pthread_mutex_unlock(&accessLockMutex);

    int counter = 0;
    while (!lock->isWaiting) {
        if (counter == 0) {
            printf("Transaction %i waiting for lock-a.Locktype:%i Memory Address:%i\n", lock->TransactionId,lock->lock_type,memoryAddress);

        }
        counter++;
        sleep(1);
        if (counter == 10)return NOT_OK;
    }
    if (counter > 0) {
        printf("Lock granted after wait.Transaction:%i Memory Address:%i\n", lock->TransactionId,memoryAddress);
    } else {
        printf("Lock granted to Transaction:%i lockType:%i Memory Address:%i\n", lock->TransactionId,lock->lock_type,memoryAddress);
    }
    return OK;
}

/**
 * @author Frane Jakelić
 * @brief Main interface function for the transaction API. It is responsible for the whole process releasing locks acquired by a transaction. The locks are released either by COMMIT or ABORT .
 * @param adresses linked list of memory addresses locked by the transaction.
 * @param transactionId integer representation of transaction id.
 */
void AK_release_locks(AK_memoryAddresses_link addresses, int transactionId) {
    pthread_mutex_lock(&accessLockMutex);
    AK_memoryAddresses_link addressesTmp = addresses;
    while(addressesTmp->nextElement !=NULL) {
        addressesTmp = addressesTmp->nextElement;
        
        
        AK_delete_lock_entry_list(addressesTmp->adresa, transactionId);
        
        AK_transaction_lock_elem tmp = AK_search_hash_entry_list_by_key(addressesTmp->adresa)->DLLLocksHead;
        AK_transaction_lock_elem anchor = tmp;
        if (tmp != NULL && tmp->nextLock != anchor && tmp->nextLock->TransactionId != transactionId) {
            while (tmp->nextLock->isWaiting != WAIT_FOR_UNLOCK && tmp->nextLock != anchor) {

                tmp->nextLock->isWaiting = AK_isLock_waiting(AK_search_hash_entry_list_by_key(addressesTmp->adresa),
                        tmp->nextLock->lock_type, tmp->nextLock->TransactionId,
                        tmp->nextLock);
                tmp = tmp->nextLock;
            };
        }
       
    }
    pthread_mutex_unlock(&accessLockMutex);
}
/**
 * @author Frane Jakelić
 * @brief Method that appends all addresses affected by the transaction
 * @param addressList pointer to the linked list where the addresses are stored.
 * @param tblName table name used in the transaction
 * @return OK or NOT_OK based on the success of the function.
 */
int AK_get_memory_blocks(char *tblName, AK_memoryAddresses *addressList) {
    table_addresses *addresses = (table_addresses*) AK_get_table_addresses(tblName);
    if (addresses->address_from[0] == 0)
        return NOT_OK;

    int i = 0, j;
    AK_mem_block *temp = (AK_mem_block*) AK_get_block(addresses->address_from[0]);

    for (j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++) {
        temp = (AK_mem_block*) AK_get_block(j);

        AK_memoryAddresses_link address = malloc(sizeof (struct memoryAddresses));
        memset(address, 0, sizeof (struct memoryAddresses));

        AK_memoryAddresses_link tmp = addressList;
        if(tmp==NULL)return NOT_OK;
        while (tmp->nextElement != NULL) {
            tmp = tmp->nextElement;
        }
        tmp->nextElement = address;
        address->adresa = temp->block->address;

        if (temp->block->last_tuple_dict_id == 0)
            break;

    }

    return OK;

}
/**
 * @author Frane Jakelić
 * @brief Method that is called in a seperate thread that is responsible for acquiring locks releasing them and finding the associated block addresses
 * @todo Check multithreading, check if it's working correctly
 * @param commandArray array filled with commands that need to be secured using transactions
 * @param lengthOfArray length of commandArray
 * @param transactionId associated with the transaction
 * @return ABORT or COMMIT based on the success of the function.
 */
int AK_execute_commands(command * commandArray, int lengthOfArray, int transactionID) {
    int i = 0, status;
    AK_memoryAddresses addresses;
    memset(&addresses, 0, sizeof (struct memoryAddresses));
    AK_memoryAddresses_link address;

    for (i = 0; i < lengthOfArray; i++) {

        if (!AK_get_memory_blocks(commandArray[i].tblName, &addresses)) {
            printf("Error reading block Addresses. Aborting\n");
            AK_release_locks(&addresses,transactionID);
            return ABORT;
        };

        address = &addresses;
        while (address->nextElement != NULL) {
            address = address->nextElement;
            switch (commandArray[i].id_command) {
                case UPDATE:
                    status = AK_acquire_lock(address->adresa, EXCLUSIVE_LOCK, transactionID);
                    break;
                case DELETE:
                    status = AK_acquire_lock(address->adresa, EXCLUSIVE_LOCK, transactionID);
                    break;
                case INSERT:
                    status = AK_acquire_lock(address->adresa, EXCLUSIVE_LOCK, transactionID);
                    break;
                case SELECT:
                    status = AK_acquire_lock(address->adresa, SHARED_LOCK, transactionID);
                    break;
                default:
                    break;
            }

            if (status == NOT_OK) {
                printf("Error acquiring lock. Aborting\n");
                AK_release_locks(&addresses,transactionID);
                return ABORT;
            }
        }

    }
    //TODO dodat poziv prave funkcije koja vrši ove promjene
    AK_release_locks(&addresses,transactionID);
    return COMMIT;
}

/**
 * @author Frane Jakelić
 * @brief checks if there is any free thread spaces.
 * @return empty index else return -1
 */
int AK_get_free_transaction_slot(){
    int i = 0;
    for (i=0;i<NUMBER_OF_THREADS;i++){
        if(transactionThreads[i]==NULL){
            return i;
        }
    }
    return -1;
}
/**
 * @author Frane Jakelić
 * @brief thread start point all relevant functions are called from this function. It acts as an intermidary between the main thread and other threads
 * @param data transmited to the thread from the main thread
 */
void AK_execute_transaction(void* params){
    int status;
    AK_transaction_data *data = (AK_transaction_data*) params;
    status = AK_execute_commands(data->array,data->lengthOfArray,data->transactionId);
    if(status==ABORT){
        printf("Transaction ABORTED!\n");
    }else{
        printf("Transaction COMMITED!\n");
    }
}


/**
 * @author Frane Jakelić
 * @brief method that receives all the data and gives an id to that data and starts a thread that executes the transaction 
 * @param commandArray array filled with commands that need to be secured using transactions
 * @param lengthOfArray length of commandArray
 */
void AK_transaction_manager(command * commandArray, int lengthOfArray) {
    memset(LockTable, 0, NUMBER_OF_KEYS * sizeof (struct transaction_list_head));
    int transactionSlot = AK_get_free_transaction_slot();
    
    while(transactionSlot == -1){
        transactionSlot = AK_get_free_transaction_slot();
        sleep(1);
    }
    
    AK_transaction_data params;
    params.array = commandArray;
    params.lengthOfArray = lengthOfArray;
    params.transactionId = transactionSlot;
    AK_execute_transaction(&params);
    //pthread_create(&transactionThreads[transactionSlot], NULL, AK_execute_transaction, &params);
}


void AK_test_Transaction(){
   printf("***Test Transaction***\n");
   int brojkomandi;

   command *komande = malloc(sizeof(command)*5);
   komande[0].id_command = 2;
   komande[0].tblName = "student";


   komande[1].id_command = 0;
   komande[1].tblName = "student";

   
   komande[2].id_command = 1;
   komande[2].tblName = "professor";

    komande[3].id_command = 3;
    komande[3].tblName = "professor";

    brojkomandi = 4;

    AK_transaction_manager(komande, brojkomandi);
      printf("***End test Transaction***\n");
    }