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
 * @author Tomislav Fotak
 *
 * Function initializes new segment and writes its start and finish address in
 * system catalog table
 * For creting new table, index, temporary table, etc. call this function
 * @param name (char*) - segment name
 * @param type (int) - segment type
 * @param header (KK_header*) - pointer to header that should be written to the new extent (all blocks)
 *
 * @return
 */
int KK_initialize_new_segment(char *name, int type, KK_header *header)
{
	int start_address = -1;
	int end_address = INITIAL_EXTENT_SIZE;
	int i = 0;
	int search = 1; //search if 1, zero (0) otherwise
	int j = 0;
	char systemTableName[20];
	int systemTableAddress;
	int freeSpaceFound = 0; //find free space in system table for writing at it
	int tupleDictID = -1;
	int objectID = 79; //will be defined later

	//KK_mem_block *tempBlock;
	KK_block *tempBlock;

	if ((start_address = KK_new_segment(name, type, header)) == EXIT_ERROR)
	{
		if (DEBUG)
			printf("KK_init_new_segment__ERROR: Cannot initialize segment!\n");
		return EXIT_ERROR;
	}
	else
	{
		end_address += start_address;
		/// find system catalog
		while ((i < DATA_BLOCK_SIZE) && (search == 1))
		{
			for(j; j<20; j++)
				systemTableName[j]= FREE_CHAR;

			if(DEBUG)
				printf("KK_init_new_segment__NOTIFICATION: Searching....\n");

			//tempBlock = KK_get_block(0);
			tempBlock = KK_read_block(0);

			memcpy(systemTableName, tempBlock->data + tempBlock->tuple_dict[i].address, tempBlock->tuple_dict[i].size);
			if (DEBUG)
				printf("KK_init_new_segment__Getting: %s\n", systemTableName);

			i += 1;

			//get next tuple_dict where the start address might be
			memcpy(&systemTableAddress, tempBlock->data + tempBlock->tuple_dict[i].address, tempBlock->tuple_dict[i].size);

			switch(type)
			{
				case SEGMENT_TYPE_TABLE:
					if(strcmp(systemTableName,"KK_relation") == 0)
					{
						if(DEBUG)
							printf("KK_init_new_segment__NOTIFICATION: KK_relation found at %d \n", systemTableAddress);
						search = 0;
					}
					break;
				case SEGMENT_TYPE_INDEX:
					if(strcmp(systemTableName,"KK_index") == 0)
					{
						if(DEBUG)
							printf("KK_init_new_segment__NOTIFICATION: KK_index found at %d \n", systemTableAddress);
						search = 0;
					}
					break;
				default:
					break;
			}

			i += 1;
		}

		if (search == 0)
		{
			//tempBlock = KK_get_block(systemTableAddress);
			tempBlock = KK_read_block(systemTableAddress);

			while (freeSpaceFound == 0)
			{
				tupleDictID += 1;
				if (tempBlock->tuple_dict[tupleDictID].size == FREE_INT)
					freeSpaceFound = 1;
			}
			if (freeSpaceFound == 1)
			{
				KK_insert_entry(tempBlock, TYPE_INT, &objectID, tupleDictID);
				KK_insert_entry(tempBlock, TYPE_VARCHAR, name, tupleDictID + 1);
				KK_insert_entry(tempBlock, TYPE_INT, &start_address, tupleDictID + 2);
				KK_insert_entry(tempBlock, TYPE_INT, &end_address, tupleDictID + 3);
				KK_write_block(tempBlock);
			}
		}
		if (DEBUG)
			printf("KK_init_new_segment__NOTIFICATION: New segment initialized at %d\n", start_address);
		return start_address;
	}
}


void files_test()
{
	KK_header header;
	int i;
	header.integrity[0] = 21;
	header.type = 1;
	char *ime = "Tomo";
	i = KK_initialize_new_segment( ime, SEGMENT_TYPE_TABLE, &header);
	if (i != EXIT_ERROR)
		printf("KK_init_new_segment: Test succeded!\n");
}
