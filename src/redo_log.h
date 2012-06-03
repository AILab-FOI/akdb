/*
 * redo_log.h
 *
 *  Created on: Apr 30, 2012
 *      Author: gigai
 */

#ifndef REDOLOG
#define REDOLOG

#include "auxiliary.h"
#include "memoman.h"
#include "configuration.h"
#include "debug.h"

#endif /* REDOLOG */

void AK_clean_redolog();
int AK_add_to_redolog(char *srcTableName, AK_list *expr);
int AK_is_in_redolog(AK_list *expr);
void AK_printout_redolog();
