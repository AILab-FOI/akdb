/**
@file rel_eq_comut.c Provides functions for relational equivalences regarding commutativity
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

#include "rel_eq_comut.h"


/**
 * @author Dino Laktašić.
 * @brief Check if selection can commute with theta-join or product
 * <ol>
 * <li>For each token (delimited by " ") in selection condition first check if token represents attribute/s and is subset in the given table</li>
 * <li>If token is a subset set variable id to 1</li>
 * <li>else set id to 0, else make no changes to variable id</li>
 * <li>if token differs from "AND" and "OR" and id equals to 1 append current token to result condition</li>
 * <li>else if token equals to "AND" or "OR" and id equals to 1 and there are two added tokens add "AND" or "OR" to condition string</li>
 * <li>When exits from loop, return pointer to char array that contains new condition for a given table
 * </ol>
 * @param *cond condition array that contains condition data 
 * @param *tblName name of the table
 * @result pointer to char array that contains new condition for a given table
 */


char *AK_rel_eq_commute_with_theta_join(char *cond, char *tblName) {
    int id = 0;
    int token_id = 0;
    int next_cond = 0;

    char *token_cond, *save_token_cond;
    char *ret_attributes = (char *) calloc(MAX_VARCHAR_LENGTH, sizeof (char));
    char *temp_cond = (char *) calloc(strlen(cond) + 1, sizeof (char));

    memcpy(temp_cond, cond, strlen(cond));
    memcpy(temp_cond + strlen(cond) + 1, "\0", 1);

    Ak_dbg_messg(HIGH, REL_EQ, "RULE - commute selection (%s) with theta-join, table name (%s)\n", temp_cond, tblName);

    for ((token_cond = strtok_r(temp_cond, " ", &save_token_cond)); token_cond;
            (token_cond = strtok_r(NULL, " ", &save_token_cond)), token_id++) {
        if (token_id < MAX_TOKENS - 1) {
            if (*token_cond == ATTR_ESCAPE) {
                char *tbl = AK_rel_eq_get_atrributes_char(tblName);
                char *attr = AK_rel_eq_cond_attributes(token_cond);

                if (attr != NULL && tbl != NULL) {
                    if (!AK_rel_eq_is_attr_subset(tbl, attr)) { //if is subset set id to 1
                        id = 1;
                    } else {
                        id = 0;
                    }
                }
                free(tbl);
                free(attr);
            }
            //`category` 'teacher' = `firstname` 'Dino' = AND `lastname` 'Laktasic' = OR
            if ((strcmp(token_cond, "AND") != 0 && strcmp(token_cond, "OR") != 0) && id > 0) {
                if (strlen(ret_attributes)) {
                    strcat(ret_attributes, " ");
                }
                strcat(ret_attributes, token_cond);
                next_cond++;
            } else if ((strcmp(token_cond, "AND") == 0 || strcmp(token_cond, "OR") == 0) && next_cond % 2 == 0 && id > 0) {
                strcat(ret_attributes, " ");
                strcat(ret_attributes, token_cond);
            }
        }
    }

    if (ret_attributes > 0) {
        Ak_dbg_messg(HIGH, REL_EQ, "RULE - commute selection with theta-join succeed.\n");
        return ret_attributes;
    } else {
        Ak_dbg_messg(HIGH, REL_EQ, "RULE - commute selection with theta-join failed!\n");
        free(ret_attributes);
        return NULL;
    }
}
/**
 * @author Dino Laktašić.
 * @brief relational equivalences regarding commutativity
 * @return No return vlaue
 */
void rel_eq_comut_test() {
    printf(AK_rel_eq_commute_with_theta_join("`mbr` 100 > `firstname` 50 < AND `id` 'A' > OR", "profesor"));
    printf("rel_eq_comut.c: Present!\n");
    printf("AK_rel_eq_commute_with_theta_join: Present!\n");
}
