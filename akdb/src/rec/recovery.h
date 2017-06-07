/*
 * recovery.h
 *
 *  Created on: June 12, 2013
 *      Author: Drazen Bandic, 
 *              updated by Tomislav Turek on March 30, 2016
 */

#ifndef RECOVERY
#define RECOVERY

#include "../auxi/auxiliary.h"
#include "../mm/memoman.h"
#include "../auxi/constants.h"
#include "../auxi/configuration.h"
#include "../auxi/debug.h"
#include "../rec/archive_log.h"
#include "../file/table.h"
#include "../file/fileio.h"
#include "../file/test.h"
#include "../opti/rel_eq_selection.h"
#include "../auxi/mempro.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "signal.h"

void AK_recover_archive_log(char* fileName);
void AK_recovery_insert_row(char* table, int commandNumber);
char** AK_recovery_tokenize(char* input, char* delimiter, int valuesOrNot);
void AK_recovery_test();
void AK_recover_operation(int sig);

#endif /* RECOVERY */
