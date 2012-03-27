/**
@file files.c Provides functions for file management
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
#include "files.h"

/**
 * @author Tomislav Fotak, updated by Matija Šestak (function now uses caching)
 *
 * Function initializes new segment and writes its start and finish address in
 * system catalog table
 * For creting new table, index, temporary table, etc. call this function
 * @param name (char*) - segment name
 * @param type (int) - segment type
 * @param header (AK_header*) - pointer to header that should be written to the new extent (all blocks)
 *
 * @return
 */

int AK_initialize_new_segment(char *name, int type, AK_header *header) {
    int start_address = -1;
    int end_address = INITIAL_EXTENT_SIZE;
    int objectID = AK_get_id();
    char *sys_table;

    AK_archive_log("AK_initialize_new_segment", name, type); //ARCHIVE_LOG
    
    if ((start_address = AK_new_segment(name, type, header)) == EXIT_ERROR) {
        Ak_dbg_messg(LOW, FILE_MAN, "AK_init_new_segment__ERROR: Cannot initialize segment!\n");
        return EXIT_ERROR;
    } else {
        end_address += start_address;
		
        switch (type) {
            case SEGMENT_TYPE_TABLE:
                sys_table = "AK_relation";
                break;
            case SEGMENT_TYPE_INDEX:
                sys_table = "AK_index";
                break;
            default:
                break;
        }
		
        AK_list_elem row_root = (AK_list_elem) malloc(sizeof (AK_list));
        Ak_Init_L(row_root);
        //DeleteAllElements(row_root);
        Ak_Insert_New_Element(TYPE_INT, &objectID, sys_table, "obj_id", row_root);
        Ak_Insert_New_Element(TYPE_VARCHAR, name, sys_table, "name", row_root);
        Ak_Insert_New_Element(TYPE_INT, &start_address, sys_table, "start_address", row_root);
        Ak_Insert_New_Element(TYPE_INT, &end_address, sys_table, "end_address", row_root);
        Ak_insert_row(row_root);

        Ak_dbg_messg(LOW, FILE_MAN, "AK_init_new_segment__NOTIFICATION: New segment initialized at %d\n", start_address);
        return start_address;
    }
}

void Ak_files_test() {
    AK_header header[MAX_ATTRIBUTES], header1[MAX_ATTRIBUTES];
    memset(header, '\0', MAX_ATTRIBUTES);
    memset(header1, '\0', MAX_ATTRIBUTES);

    int i, i1;
    char *ime = "Tomo";
    char *ime1 = "Tomo1";

    header[0].integrity[0] = 21;
    header[0].type = 1;

    header1[0].integrity[0] = 21;
    header1[0].type = 1;

    i = AK_initialize_new_segment(ime, SEGMENT_TYPE_TABLE, header);
    if (i != EXIT_ERROR)
        printf("AK_init_new_segment: Test succeded!\n");
    i1 = AK_initialize_new_segment(ime1, SEGMENT_TYPE_TABLE, header1);
    if (i1 != EXIT_ERROR)
        printf("AK_init_new_segment: Test1 succeded!\n");
}
