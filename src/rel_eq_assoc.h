/**
@file rel_eq_assoc.h Header file that provides data structures for relational equivalences regarding associativity
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

#ifndef REL_EQ_ASSOC
#define REL_EQ_ASSOC

#include "table.h"

/**
 * @author Dino Laktašić
 * @struct cost_eval_t
 * @brief Stucture for cost estimation on relations. It contains value (number of rows in table) and data (used to store table name)
 * 
 */
struct cost_eval_t {
    int value;
    char data[MAX_VARCHAR_LENGTH];
};

typedef struct cost_eval_t cost_eval;

#endif
