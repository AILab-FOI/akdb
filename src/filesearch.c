/**
@file filesearch.c Provides functions for file searching
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "filesearch.h"
#include "memoman.h"
#include "configuration.h"

void filesearch_test()
{
	printf( "filesearch_test: Present!\n" );
}

/**
 @author Miroslav Policki
 
 Searches through unsorted values of multiple attributes in a segment. Only tuples that are equal on all given attribute values are returned.
 Only one value per attribute allowed - use search_params.pData_lower.

 @param iSegment_address address of segment to search
 @param aspParams array of search parameters
 @param iNum_search_params number of search parameters

 @return search_result structure defined in filesearch.h. Use KK_deallocate_search_result to deallocate.
*/

search_result KK_equisearch_unsorted(int iSegment_address, search_params *aspParams, int iNum_search_params) {
    int           iBlock;
    KK_mem_block  *mem_block;
    int           i, j;
    int           iTupleMatches;
    search_result srResult;
    
    srResult.aiTuple_addresses = NULL;
    srResult.iNum_tuple_addresses = 0;
    srResult.aiSearch_attributes = NULL;

    if(aspParams == NULL || iNum_search_params == 0)
        return srResult;
    
    /// iterate through all blocks in segment, assuming 1 extent per segment and constant INITIAL_EXTENT_SIZE blocks per extent
    for(iBlock = iSegment_address; iBlock < iSegment_address + INITIAL_EXTENT_SIZE; iBlock++) {
        mem_block = KK_get_block(iBlock);
        
        /// count number of attributes in segment/relation
        srResult.iNum_tuple_attributes = 0;
        for(i = 0; i < MAX_ATTRIBUTES; i++) {
            if(mem_block->block->header[i].att_name[0] == FREE_CHAR)
                break;
            srResult.iNum_tuple_attributes++;
        }

        srResult.aiSearch_attributes = (int *) malloc(iNum_search_params * sizeof(int));
        if(srResult.aiSearch_attributes == NULL) {
            printf("KK_equisearch_unsorted: ERROR. Cannot allocate srResult.aiAttributes_searched.\n");
            exit(EXIT_ERROR);
        }
        srResult.iNum_search_attributes = 0;

        /// determine index of attributes on which search will be performed
        for(j = 0; j < iNum_search_params; j++) {
            for(i = 0; i < MAX_ATTRIBUTES; i++) {
                if(!strcmp(mem_block->block->header[i].att_name, aspParams[j].szAttribute)) {
                    srResult.aiSearch_attributes[j] = i;
                    srResult.iNum_search_attributes++;
                    break;
                }
            }
        }

        /// if any of the provided attributes are not found in the relation, return empty result
        if(srResult.iNum_search_attributes != iNum_search_params)
            return srResult;
 
        /// in every tuple, for all required attributes, compare attribute value with searched-for value and store matched tuple addresses
        for(i = 0; i < DATA_BLOCK_SIZE; i += srResult.iNum_tuple_attributes) {
            iTupleMatches = 1;
            
            for(j = 0; j < iNum_search_params; j++) {
                if(mem_block->block->tuple_dict[i + srResult.aiSearch_attributes[j]].type != aspParams[j].iType
                   || mem_block->block->tuple_dict[i + srResult.aiSearch_attributes[j]].size != KK_type_size(aspParams[j].iType, (char *) aspParams[j].pData_lower)
                   || memcmp(mem_block->block->data + mem_block->block->tuple_dict[i + srResult.aiSearch_attributes[j]].address, aspParams[j].pData_lower, KK_type_size(aspParams[j].iType, (char *) aspParams[j].pData_lower))) {
                    iTupleMatches = 0;
                }
            }

            if(iTupleMatches) {
                srResult.aiTuple_addresses = (int *) realloc(srResult.aiTuple_addresses, srResult.iNum_tuple_addresses++ * sizeof(int));
                if(srResult.aiTuple_addresses == NULL) {
                    printf("KK_equisearch_unsorted: ERROR. Cannot reallocate srResult.aiTuple_addresses, iteration %d.\n", i);
                    exit(EXIT_ERROR);
                }

                srResult.aiTuple_addresses[srResult.iNum_tuple_addresses - 1] = i;
            }
        }
    }

    return srResult;
}

/**
 @author Miroslav Policki
 
 Searches through unsorted values of multiple attributes in a segment.
 Only tuples whose attribute values fall within the given range (inclusive), for every given attribute value, are returned.
 Only one value range per attribute allowed. Supported types: TYPE_INT, TYPE_FLOAT, TYPE_NUMBER, TYPE_DATE, TYPE_DATETIME, TYPE_TIME.

 @param iSegment_address address of segment to search
 @param aspParams array of search parameters
 @param iNum_search_params number of search parameters

 @return search_result structure defined in filesearch.h. Use KK_deallocate_search_result to deallocate.
*/

search_result KK_rangesearch_unsorted(int iSegment_address, search_params *aspParams, int iNum_search_params) {
    int           iBlock;
    KK_mem_block  *mem_block;
    int           i, j;
    int           iTupleMatches;
    search_result srResult;
    
    srResult.aiTuple_addresses = NULL;
    srResult.iNum_tuple_addresses = 0;
    srResult.aiSearch_attributes = NULL;

    if(aspParams == NULL || iNum_search_params == 0)
        return srResult;
    
    /// iterate through all blocks in segment, assuming 1 extent per segment and constant INITIAL_EXTENT_SIZE blocks per extent
    for(iBlock = iSegment_address; iBlock < iSegment_address + INITIAL_EXTENT_SIZE; iBlock++) {
        mem_block = KK_get_block(iBlock);
        
        /// count number of attributes in segment/relation
        srResult.iNum_tuple_attributes = 0;
        for(i = 0; i < MAX_ATTRIBUTES; i++) {
            if(mem_block->block->header[i].att_name[0] == FREE_CHAR)
                break;
            srResult.iNum_tuple_attributes++;
        }

        srResult.aiSearch_attributes = (int *) malloc(iNum_search_params * sizeof(int));
        if(srResult.aiSearch_attributes == NULL) {
            printf("KK_rangesearch_unsorted: ERROR. Cannot allocate srResult.aiAttributes_searched.\n");
            exit(EXIT_ERROR);
        }
        srResult.iNum_search_attributes = 0;

        /// determine index of attributes on which search will be performed
        for(j = 0; j < iNum_search_params; j++) {
            for(i = 0; i < MAX_ATTRIBUTES; i++) {
                if(!strcmp(mem_block->block->header[i].att_name, aspParams[j].szAttribute)) {
                    srResult.aiSearch_attributes[j] = i;
                    srResult.iNum_search_attributes++;
                    break;
                }
            }
        }

        /// if any of the provided attributes are not found in the relation, return empty result
        if(srResult.iNum_search_attributes != iNum_search_params)
            return srResult;
 
        /// in every tuple, for all required attributes, compare attribute value with searched-for value range and store matched tuple addresses
        for(i = 0; i < DATA_BLOCK_SIZE; i += srResult.iNum_tuple_attributes) {
            iTupleMatches = 1;
            
            for(j = 0; j < iNum_search_params; j++) {
                if(mem_block->block->tuple_dict[i + srResult.aiSearch_attributes[j]].type != aspParams[j].iType) {
                    iTupleMatches = 0;
                }
                else {
                    switch(aspParams[j].iType) {
                        case TYPE_INT:
                        case TYPE_DATE:
                        case TYPE_DATETIME:
                        case TYPE_TIME:
                            if(*((int *) mem_block->block->data + mem_block->block->tuple_dict[i + srResult.aiSearch_attributes[j]].address) < *((int *) aspParams[j].pData_lower)
                               || *((int *) mem_block->block->data + mem_block->block->tuple_dict[i + srResult.aiSearch_attributes[j]].address) > *((int *) aspParams[j].pData_upper)) {
                                iTupleMatches = 0;
                            }
                            break;
                        case TYPE_FLOAT:
                        case TYPE_NUMBER:
                            if(*((double *) mem_block->block->data + mem_block->block->tuple_dict[i + srResult.aiSearch_attributes[j]].address) < *((double *) aspParams[j].pData_lower)
                               || *((double *) mem_block->block->data + mem_block->block->tuple_dict[i + srResult.aiSearch_attributes[j]].address) > *((double *) aspParams[j].pData_upper)) {
                                iTupleMatches = 0;
                            }
                            break;
                        default: // other types unsupported
                            iTupleMatches = 0;
                    }
                }
            }

            if(iTupleMatches) {
                srResult.aiTuple_addresses = (int *) realloc(srResult.aiTuple_addresses, srResult.iNum_tuple_addresses++ * sizeof(int));
                if(srResult.aiTuple_addresses == NULL) {
                    printf("KK_rangesearch_unsorted: ERROR. Cannot reallocate srResult.aiTuple_addresses, iteration %d.\n", i);
                    exit(EXIT_ERROR);
                }

                srResult.aiTuple_addresses[srResult.iNum_tuple_addresses - 1] = i;
            }
        }
    }

    return srResult;
}

/**
 @author Miroslav Policki
 
 Deallocates memory used by search result returned by KK_equisearch_unsorted and KK_rangesearch_unsorted.

 @param srResult search result
*/

void KK_deallocate_search_result(search_result srResult) {
    free(srResult.aiTuple_addresses);
    free(srResult.aiSearch_attributes);
}
