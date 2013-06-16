/*
 * redo_log.h
 *
 *  Created on: Apr 30, 2012
 *      Author: gigai
 */

#ifndef REDOLOG
#define REDOLOG

#include "../auxi/auxiliary.h"
#include "../mm/memoman.h"
#include "../auxi/constants.h"
#include "../auxi/configuration.h"
#include "../auxi/debug.h"
#include "../rec/archive_log.h"
#include "../file/table.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#endif /* REDOLOG */

int AK_add_to_redolog(char command[6], AK_list *row_root);
void AK_printout_redolog();
char* AK_check_attributes(char *attributes);
