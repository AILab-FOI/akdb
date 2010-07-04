/**
@file aggregation.h Provides data structures for aggregation and grouping
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

#ifndef AGGREGATION
#define AGGREGATION

#include "auxiliary.h"
#include "dbman.h"
#include "selection.h"
#include "projection.h"
#include "filesearch.h"
#include "fileio.h"

#define AGG_TASK_GROUP 1
#define AGG_TASK_COUNT 2
#define AGG_TASK_SUM 3
#define AGG_TASK_MAX 4
#define AGG_TASK_MIN 5
#define AGG_TASK_AVG 6
#define AGG_TASK_AVG_COUNT 10 //used internaly
#define AGG_TASK_AVG_SUM 11 //used internaly

typedef struct {
    char att_name[MAX_ATT_NAME];
    char data[ MAX_VARCHAR_LENGTH ];
    int agg_task;
} AK_agg_value;

typedef struct {
    AK_header attributes[MAX_ATTRIBUTES];
    int tasks[MAX_ATTRIBUTES];
    int counter;
} AK_agg_input;

#endif
