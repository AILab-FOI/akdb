/**
@file mempro.h Data structures, includes, macros and declarations for the
memory wrappers and debug mode of Kalashnikov DB.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301, USA
 */

#ifndef MEMPRO_H
#define MEMPRO_H

#if !(defined _WIN32 || defined __linux__)
#error "Unsupported platform."
#endif

#ifdef __cplusplus
#define NEW(type, type_size) ((type *)calloc(type_size, sizeof(type)))
#define AK_INLINE inline
#else
#define NEW(type, type_size) (calloc(type_size, sizeof(type)))
#ifdef _WIN32
#define AK_INLINE __inline
#else
#define AK_INLINE __inline__
#endif
#endif

#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#include <Windows.h>
#include <process.h>
#endif
#ifdef __linux__
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/mman.h>
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdarg.h>

/**
  * @def AK_DEBMOD_ON
  * @brief Zero to switch memory protection and debug mode off.
  */
#define AK_DEBMOD_ON 1

/**
  * @def AK_DEBMOD_PRINT
  * @brief Defines if debug mode messages are going to be printed
  */
#define AK_DEBMOD_PRINT 0

/**
  * @def AK_DEBMOD_PAGES_NUM
  * @brief Defines the total of available memory pages for allocation
  */
#define AK_DEBMOD_PAGES_NUM 8192

/**
  * @def AK_DEBMOD_MAX_WRITE_DETECTIONS
  * @brief Defines the maximum number of memory write detections
  */
#define AK_DEBMOD_MAX_WRITE_DETECTIONS (AK_DEBMOD_PAGES_NUM * 10)

/**
  * @def AK_DEBMOD_STACKSIZE
  * @brief Defines the monitored functions stack
  */
#define AK_DEBMOD_STACKSIZE AK_DEBMOD_PAGES_NUM

/**
  * @def AK_DEBMOD_MAX_FUNCTIONS
  * @brief Defines the maximum number of function names in the application
  */
#define AK_DEBMOD_MAX_FUNCTIONS 500

/**
  * @def AK_DEBMOD_MAX_FUNC_NAME
  * @brief Defines maximum function name length possible
  */
#define AK_DEBMOD_MAX_FUNC_NAME 80

#ifdef _WIN32
#define __func__ __FUNCTION__
#endif

/**
  * @def AK_PRO
  * @brief Mandatory function prologue for all functions (AK_debmod and
  *        related functions are excluded). Put this macro after variable
  *        declarations, before any function instruction.
  */
#define AK_PRO AK_debmod_function_prologue(__func__, __FILE__, __LINE__);

/**
  * @def AK_EPI
  * @brief Mandatory function epilogue for all functions (AK_debmod and
  *        related functions are excluded). Put this macro after last
  *        function instruction, before every return statement.
  */
#define AK_EPI AK_debmod_function_epilogue(__func__, __FILE__, __LINE__);

#ifdef __linux__
static pthread_mutex_t AK_debmod_critical_section = PTHREAD_MUTEX_INITIALIZER;
#endif

/**
  * @author Marin Rukavina, Mislav Bozicevic
  * @struct AK_debmod_state
  * @brief Global structure that holds all relevant information for the debug
  *        mode and related functionality.
  */
typedef struct {
    /* Is it initialized? 1 = yes it is */
    uint8_t  init;
    /* Size of one page in bytes (system-dependent) */
    uint32_t page_size;
    /* Is this global structure in use by some thread? 1 = ready, 0 = !ready */
    uint8_t  ready;
    /* Memory contents as pointers to pages */
    void*    page[AK_DEBMOD_PAGES_NUM];
    /* Is selected page in use? 0 = not used (free) */
    uint8_t  used[AK_DEBMOD_PAGES_NUM];
    /* Nominal size of allocation in bytes (this is what user believes) */
    uint32_t nomi[AK_DEBMOD_PAGES_NUM];
    /* Real size for allocation in bytes (this is what is really allocated) */
    uint32_t real[AK_DEBMOD_PAGES_NUM];
    /* Has the selected page been written from the last check? 1 = yes */
    uint8_t  dirty[AK_DEBMOD_PAGES_NUM];
    /* (function id, function name) pairs */
    char     function[AK_DEBMOD_MAX_FUNCTIONS][AK_DEBMOD_MAX_FUNC_NAME];
    /* Last used function id. -1 = empty/none used */
    int32_t  last_function_id;
    /* Allocation owner (function id) */
    int32_t  alloc_owner[AK_DEBMOD_PAGES_NUM];
    /* Free owner (function id) */
    int32_t  free_owner[AK_DEBMOD_PAGES_NUM];
    /* (function id, function id) pairs where function dependency is stored */
    int8_t   func_used_by[AK_DEBMOD_MAX_FUNCTIONS][AK_DEBMOD_MAX_FUNCTIONS];
    /* Are messages printed? 0 = no messages are printed */
    uint8_t  print;
    /* Function stack size */
    int32_t fstack_size;
    /* Function stack (contains function id) */
    int32_t fstack_items[AK_DEBMOD_STACKSIZE];
#ifdef _WIN32
    CRITICAL_SECTION critical_section;
#endif
#ifdef __linux__
    /* WriteWatch - has this page been unlocked for writing after a SIGSEGV */
    uint8_t  unlocked[AK_DEBMOD_PAGES_NUM];
    /* Is this memory protected (in which case writing should exit the app) */
    uint8_t  protected[AK_DEBMOD_PAGES_NUM];
#endif
} AK_debmod_state;

AK_debmod_state* AK_DEBMOD_STATE;

void AK_debmod_d(AK_debmod_state*, const char *);
void AK_debmod_dv(AK_debmod_state*, const char *, ...);
void AK_debmod_enter_critical_sec(AK_debmod_state*);
void AK_debmod_leave_critical_sec(AK_debmod_state*);
AK_debmod_state* AK_debmod_init(void);
void AK_debmod_die(AK_debmod_state*);
void* AK_debmod_calloc(AK_debmod_state*, uint32_t);
void AK_debmod_free(AK_debmod_state*, void*);

void* AK_calloc(size_t, size_t);
void* AK_malloc(size_t);
void AK_free(void*);
void* AK_realloc(void*, size_t);

void AK_write_protect(void*);
void AK_write_unprotect(void*);

void AK_check_for_writes(void);

int32_t AK_debmod_func_id(AK_debmod_state*, const char *);
const char * AK_debmod_func_get_name(AK_debmod_state*, int32_t);
int32_t AK_debmod_func_add(AK_debmod_state*, const char *);
void AK_debmod_fstack_push(AK_debmod_state*, int32_t);
int32_t AK_debmod_fstack_pop(AK_debmod_state*);
void AK_debmod_function_current(AK_debmod_state*, int32_t);

void AK_debmod_function_prologue(const char *, const char *, int);
void AK_debmod_function_epilogue(const char *, const char *, int);

void AK_debmod_log_memory_alloc(int32_t);

void AK_debmod_print_function_use(const char *, uint8_t);
void AK_print_function_use(const char *);
void AK_print_function_uses();
void AK_print_active_functions();

#endif
