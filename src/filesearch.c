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
#include "filesearch.h"

/**
 @author Miroslav Policki

 Searches through unsorted values of multiple attributes in a segment.
 Only tuples that are equal on all given attribute values are returned (A == 1 AND B == 7 AND ...).
 SEARCH_RANGE is inclusive. Only one value (or range) per attribute allowed - use search_params.pData_lower for SEARCH_PARTICULAR.
 Supported types for SEARCH_RANGE: TYPE_INT, TYPE_FLOAT, TYPE_NUMBER, TYPE_DATE, TYPE_DATETIME, TYPE_TIME.
 Do not provide the wrong data types in the array of search parameters. There is no way to test for that and it could cause a memory access violation.

 @param szRelation relation name
 @param aspParams array of search parameters
 @param iNum_search_params number of search parameters

 @return search_result structure defined in filesearch.h. Use AK_deallocate_search_result to deallocate.
 */

search_result AK_search_unsorted(char *szRelation, search_params *aspParams, int iNum_search_params) {
    AK_flush_cache();
    int iBlock;
    AK_mem_block *mem_block = NULL, tmp;
    int i, j, k;
    int iTupleMatches;
    search_result srResult;
    table_addresses *taAddresses;

    srResult.aiTuple_addresses = NULL;
    srResult.iNum_tuple_addresses = 0;
    srResult.aiSearch_attributes = NULL;
    srResult.aiBlocks = NULL;

    if (aspParams == NULL || iNum_search_params == 0)
        return srResult;

    taAddresses = get_table_addresses(szRelation);

    /// iterate through all the blocks
    for (k = 0; k < MAX_EXTENTS_IN_SEGMENT && taAddresses->address_from[k] > 0; k++) { // 200 == Novak's magic number :)
        for (iBlock = taAddresses->address_from[k]; iBlock <= taAddresses->address_to[k]; iBlock++) {
            //mem_block = AK_get_block(iBlock);
            mem_block = &tmp;
            mem_block->block = AK_read_block(iBlock);

            /// count number of attributes in segment/relation
            srResult.iNum_tuple_attributes = 0;
            for (i = 0; i < MAX_ATTRIBUTES; i++) {
                if (mem_block->block->header[i].att_name[0] == FREE_CHAR)
                    break;
                srResult.iNum_tuple_attributes++;
            }

            srResult.aiSearch_attributes = (int *) malloc(iNum_search_params * sizeof (int));
            if (srResult.aiSearch_attributes == NULL) {
                printf("AK_search_unsorted: ERROR. Cannot allocate srResult.aiAttributes_searched.\n");
                exit(EXIT_ERROR);
            }
            srResult.iNum_search_attributes = 0;

            /// determine index of attributes on which search will be performed
            for (j = 0; j < iNum_search_params; j++) {
                for (i = 0; i < MAX_ATTRIBUTES; i++) {
                    if (!strcmp(mem_block->block->header[i].att_name, aspParams[j].szAttribute)) {
                        srResult.aiSearch_attributes[j] = i;
                        srResult.iNum_search_attributes++;
                        break;
                    }
                }
            }

            /// if any of the provided attributes are not found in the relation, return empty result
            if (srResult.iNum_search_attributes != iNum_search_params)
                return srResult;

            /// in every tuple, for all required attributes, compare attribute value with searched-for value and store matched tuple addresses
            for (i = 0; i < DATA_BLOCK_SIZE && mem_block->block->tuple_dict[i].type != FREE_INT; i += srResult.iNum_tuple_attributes) {
                iTupleMatches = 1;

                for (j = 0; j < iNum_search_params && iTupleMatches; j++) {
                    switch (aspParams[j].iSearchType) {
                        case SEARCH_PARTICULAR:
                        {
                            size_t iSearchAttributeValueSize = AK_type_size(mem_block->block->header[srResult.aiSearch_attributes[j]].type, (char *) aspParams[j].pData_lower);

                            if (mem_block->block->tuple_dict[i + srResult.aiSearch_attributes[j]].size != iSearchAttributeValueSize
                                    || memcmp(mem_block->block->data + mem_block->block->tuple_dict[i + srResult.aiSearch_attributes[j]].address, aspParams[j].pData_lower, iSearchAttributeValueSize)) {
                                iTupleMatches = 0;
                            }
                        }
                            break;

                        case SEARCH_RANGE:
                        {
                            size_t iSearchAttributeValueSize = AK_type_size(mem_block->block->header[srResult.aiSearch_attributes[j]].type, (char *) aspParams[j].pData_lower);

                            switch (mem_block->block->tuple_dict[i + srResult.aiSearch_attributes[j]].type) {
                                case TYPE_INT:
                                case TYPE_DATE:
                                case TYPE_DATETIME:
                                case TYPE_TIME:
                                {
                                    int iAttributeValue = *((int *) (mem_block->block->data + mem_block->block->tuple_dict[i + srResult.aiSearch_attributes[j]].address));
                                    if (iAttributeValue < *((int *) aspParams[j].pData_lower)
                                            || iAttributeValue > *((int *) aspParams[j].pData_upper)) {
                                        iTupleMatches = 0;
                                    }
                                }
                                    break;

                                case TYPE_FLOAT:
                                case TYPE_NUMBER:
                                    if (*((double *) (mem_block->block->data + mem_block->block->tuple_dict[i + srResult.aiSearch_attributes[j]].address)) < *((double *) aspParams[j].pData_lower)
                                            || *((double *) (mem_block->block->data + mem_block->block->tuple_dict[i + srResult.aiSearch_attributes[j]].address)) > *((double *) aspParams[j].pData_upper)) {
                                        iTupleMatches = 0;
                                    }
                                    break;

                                default: // other types unsupported
                                    iTupleMatches = 0;
                            }
                        }
                            break;

                        case SEARCH_ALL: // iTupleMatches is already == 1, no action needed
                            break;

                        case SEARCH_NULL:
                        {
                            size_t iSearchAttributeValueSize = AK_type_size(mem_block->block->header[srResult.aiSearch_attributes[j]].type, (char *) aspParams[j].pData_lower);

                            if (mem_block->block->tuple_dict[i + srResult.aiSearch_attributes[j]].type != TYPE_VARCHAR
                                    || iSearchAttributeValueSize != strlen("NULL")
                                    || memcmp(mem_block->block->data + mem_block->block->tuple_dict[i + srResult.aiSearch_attributes[j]].address, "NULL", strlen("NULL")))
                                iTupleMatches = 0;
                        }
                            break;

                        default:
                            iTupleMatches = 0;
                    }
                }

                if (iTupleMatches) {
                    /*if(DEBUG) {
                        printf("realloc(srResult.aiTuple_addresses = %p, srResult.iNum_tuple_addresses++ * sizeof(int) = %d)\n", srResult.aiTuple_addresses, (srResult.iNum_tuple_addresses + 1) * sizeof(int));
                        if(srResult.iNum_tuple_addresses == 7)
                            puts("8");
                    }*/
                    srResult.iNum_tuple_addresses++;
                    srResult.aiTuple_addresses = (int *) realloc(srResult.aiTuple_addresses, srResult.iNum_tuple_addresses * sizeof (int));

                    if (srResult.aiTuple_addresses == NULL) {
                        printf("AK_search_unsorted: ERROR. Cannot reallocate srResult.aiTuple_addresses, iteration %d.\n", i);
                        exit(EXIT_ERROR);
                    }

                    /*if(DEBUG)
                        puts("realloc srResult.aiBlocks");*/
                    srResult.aiBlocks = (int *) realloc(srResult.aiBlocks, srResult.iNum_tuple_addresses * sizeof (int));
                    if (srResult.aiBlocks == NULL) {
                        printf("AK_search_unsorted: ERROR. Cannot reallocate srResult.aiBlocks, iteration %d.\n", i);
                        exit(EXIT_ERROR);
                    }

                    srResult.aiTuple_addresses[srResult.iNum_tuple_addresses - 1] = i;
                    srResult.aiBlocks[srResult.iNum_tuple_addresses - 1] = iBlock;
                }
            }
        }
    }

    return srResult;
}

/**
 @author Miroslav Policki

 Deallocates memory used by search result returned by AK_search_unsorted.

 @param srResult search result
 */

void AK_deallocate_search_result(search_result srResult) {
    free(srResult.aiTuple_addresses);
    free(srResult.aiSearch_attributes);
    free(srResult.aiBlocks);
}

void filesearch_test() {
    int i;
    double f;
    AK_mem_block *mem_block, tmp;
    AK_header hBroj_int[4], *hTmp;
    AK_list *row_root;

    // create table and fill it for testing purposes
    hTmp = (AK_header *) AK_create_header("Number int", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    hBroj_int[0] = *hTmp;
    hTmp = (AK_header *) AK_create_header("Number float", TYPE_FLOAT, FREE_INT, FREE_CHAR, FREE_CHAR);
    hBroj_int[1] = *hTmp;
    hTmp = (AK_header *) AK_create_header("Varchar column", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    hBroj_int[2] = *hTmp;
    memset(&hBroj_int[3], 0, sizeof (AK_header));

    if (EXIT_ERROR == AK_initialize_new_segment("filesearch test table", SEGMENT_TYPE_TABLE, hBroj_int)) {
        printf("filesearch_test: ERROR. Unable to create \"filesearch test table \"\n");
        exit(EXIT_ERROR);
    }

    row_root = malloc(sizeof (AK_list));
    if (row_root == NULL) {
        printf("filesearch_test: ERROR. Cannot allocate row_root.\n", i);
        exit(EXIT_ERROR);
    }

    for (i = -10, f = -i; i < 10; i++, f = -i) {
        InitL(row_root);
        InsertNewElement(TYPE_INT, &i, "filesearch test table", "Number int", row_root);
        InsertNewElement(TYPE_FLOAT, &f, "filesearch test table", "Number float", row_root);
        InsertNewElement(TYPE_VARCHAR, "test text", "filesearch test table", "Varchar column", row_root);
        insert_row(row_root);
        DeleteAllL(row_root);
    }

    free(row_root);

    // filesearch usage example starts here
    {
        char *szTmp;
        search_params sp[3];
        search_result sr;
        int iLower, iUpper;
        double dTmp;

        sp[0].szAttribute = "Varchar column";
        sp[0].iSearchType = SEARCH_ALL;

        sp[1].szAttribute = "Number int";
        sp[1].iSearchType = SEARCH_RANGE;
        iLower = -5;
        iUpper = 5;
        sp[1].pData_lower = &iLower;
        sp[1].pData_upper = &iUpper;

        sp[2].szAttribute = "Number float";
        sp[2].iSearchType = SEARCH_PARTICULAR;
        dTmp = 2;
        sp[2].pData_lower = &dTmp;

        dbg_messg(LOW, FILE_MAN, "Calling AK_search_unsorted");
        sr = AK_search_unsorted("filesearch test table", sp, 3);

        for (i = 0; i < sr.iNum_tuple_addresses; i++) {
            //mem_block = AK_get_block (sr.aiBlocks[i]); // caching should be used when available
            mem_block = &tmp;
            mem_block->block = (AK_block *) AK_read_block(sr.aiBlocks[i]);

            printf("Found:%d\n", *((int *) (mem_block->block->data + mem_block->block->tuple_dict[sr.aiTuple_addresses[i] + 0].address)));
            printf("Found:%f\n", *((double *) (mem_block->block->data + mem_block->block->tuple_dict[sr.aiTuple_addresses[i] + 1].address)));

            szTmp = malloc(mem_block->block->tuple_dict[sr.aiTuple_addresses[i] + 2].size + 1);
            mempcpy(szTmp, mem_block->block->data + mem_block->block->tuple_dict[sr.aiTuple_addresses[i] + 2].address, mem_block->block->tuple_dict[sr.aiTuple_addresses[i] + 2].size);
            szTmp[mem_block->block->tuple_dict[sr.aiTuple_addresses[i] + 2].size] = '\0';
            printf("Found:%s\n", szTmp);
            free(szTmp);
        }

        AK_deallocate_search_result(sr);
    }
}
