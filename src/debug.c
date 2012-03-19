/**
@file debug.c Provides functions for debuging
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
 
#include "debug.h"

/**
 * @brief Print debug message. Provide debug level, debug type and message with corresponding variables for the output.
 * @author Dino Laktašić
 * @param DEBUG_LEVEL level - level of debug information for a given DB module
 * @param DEBUG_TYPE type - the name of DB module for which to print debug information
 * const char *format - format for the output message
 * ... - variable number of (different) type args used in printf
 * @return int - if debug message is printed return 1, else return 0
 */
int Ak_dbg_messg(DEBUG_LEVEL level, DEBUG_TYPE type, const char *format, ...) {
	/*if (DEBUG_NONE) {
		return 0;
	}*/
	
	if ((level == 0 || type == 0 || format == NULL) && !DEBUG_ALL) {
		return 0;
	}
	
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	return 1;
}