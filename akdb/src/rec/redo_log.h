/*
/**
@file redo_log.h Header file that provides functions and defines for redo_log.h
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

#include "../auxi/mempro.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"



int AK_add_to_redolog(int command, struct list_node *row_root);
int AK_add_to_redolog_select(int command, struct list_node *condition, char *srcTable);
int AK_check_redo_log_select(int command, struct list_node *condition, char *srcTable);
void AK_printout_redolog();
void AK_redolog_commit();
char* AK_check_attributes(char *attributes);

#endif /* REDOLOG */