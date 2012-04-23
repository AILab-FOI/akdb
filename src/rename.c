/**
@file rename.c Provides functions for header atributes renaming
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


#include "rename.h"

/**
 * @author Mislav Čakarić
 * @brief Function for renaming table and/or attribute in table
 * @param old_table_name old name of the table
 * @param new_table_name new name of the table
 * @param old_attr name of the attribute to rename
 * @param new_attr new name for the attribute to rename
 * @return EXIT_ERROR or EXIT_SUCCESS
 */
int AK_rename(char *old_table_name, char *old_attr, char *new_table_name, char *new_attr) {
    table_addresses *adresses = (table_addresses *)AK_get_table_addresses(old_table_name);
    int tab_addresses[MAX_NUM_OF_BLOCKS];
    int num_extents = 0, num_blocks = 0;
    register int i, j;
    AK_mem_block *mem_block;
    AK_archive_log("AK_rename", old_table_name, old_attr, new_table_name, new_attr); //ARCHIVE_LOG
	if(strcmp(old_attr, new_attr) != 0){
		//SEARCH FOR ALL BLOCKS IN SEGMENT
		i=0;
		while(adresses->address_from[i]){
			for (j = adresses->address_from[i]; j <= adresses->address_to[i]; j++) {
				tab_addresses[num_blocks] = j;
				num_blocks++;
			}
			num_extents++;
			i++;
		}

		AK_header newHeader[MAX_ATTRIBUTES];
		mem_block = (AK_mem_block *)AK_get_block(tab_addresses[0]);
		memcpy(&newHeader, mem_block->block->header, sizeof (mem_block->block->header));

		for (i = 0; i < MAX_ATTRIBUTES; i++) {
			if (strcmp(newHeader[i].att_name, old_attr)==0) {
				Ak_dbg_messg(HIGH, REL_OP, "AK_rename: the attribute names are the same at position %d!\n", i);
				memset(&newHeader[i].att_name, 0, MAX_ATT_NAME);
				memcpy(&newHeader[i].att_name, new_attr, strlen(new_attr));
				break;
			} else if (strcmp(newHeader[i].att_name, "\0")==0) { //if there is no more attributes
				Ak_dbg_messg(MIDDLE, REL_OP, "AK_rename: ERROR: atribute: %s does not exist in this table\n", old_attr);
				return (EXIT_ERROR);
			}
		}

		//replacing old headers with new ones
		for (i = 0; i < num_blocks; i++) {
			mem_block = (AK_mem_block *)AK_get_block(tab_addresses[i]);
			memcpy(&mem_block->block->header, newHeader, sizeof (AK_header) * MAX_ATTRIBUTES);
			mem_block->dirty=BLOCK_DIRTY;
		}
	}

    if(strcmp(old_table_name, new_table_name) != 0){//new name is different than old, and old needs to be replaced
        AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
		Ak_Init_L(row_root);
		
		Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, old_table_name, "AK_relation", "name", row_root, 1);
		Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, new_table_name, "AK_relation", "name", row_root, 0);        
        Ak_update_row(row_root);
		free(row_root);
    }

    return EXIT_SUCCESS;
}

/**
  * @author Mislav Čakarić
  * @brief Function for rename operator testing
  * @return No return value
 */
void AK_op_rename_test() {
    //printf( "rename_test: Present!\n" );
    printf("\n********** RENAME TEST **********\n\n");

	AK_print_table("AK_relation");
    AK_rename("student", "weight", "student1", "weight");
    AK_print_table("student1");
    //AK_print_table("student2");

    AK_print_table("AK_relation");
}
