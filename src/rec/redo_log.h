/*
 * redo_log.h
 *
 *  Created on: Apr 30, 2012
 *      Author: gigai
 */

#ifndef REDOLOG
#define REDOLOG

#include "../aux/auxiliary.h"
#include "../mm/memoman.h"
#include "../aux/constants.h"
#include "../aux/configuration.h"
#include "../aux/debug.h"

#endif /* REDOLOG */

void AK_clean_redolog();
int AK_add_to_redolog(char *srcTableName, AK_list *expr);
int AK_is_in_redolog(AK_list *expr);
void AK_printout_redolog();
