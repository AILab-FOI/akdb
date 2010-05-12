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
 * @param header (AK_header*) - pointer to header that should be written to the new extent (all blocks)
 *
 * @return
 */
int AK_initialize_new_segment(char *name, int type, AK_header *header)
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

	//AK_mem_block *tempBlock;
	AK_block *tempBlock;

	if ((start_address = AK_new_segment(name, type, header)) == EXIT_ERROR)
	{
		if (DEBUG)
			printf("AK_init_new_segment__ERROR: Cannot initialize segment!\n");
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
				printf("AK_init_new_segment__NOTIFICATION: Searching....\n");

			//tempBlock = AK_get_block(0);
			tempBlock = AK_read_block(0);

			memcpy(systemTableName, tempBlock->data + tempBlock->tuple_dict[i].address, tempBlock->tuple_dict[i].size);
			if (DEBUG)
				printf("AK_init_new_segment__Getting: %s\n", systemTableName);

			i += 1;

			//get next tuple_dict where the start address might be
			memcpy(&systemTableAddress, tempBlock->data + tempBlock->tuple_dict[i].address, tempBlock->tuple_dict[i].size);

			switch(type)
			{
				case SEGMENT_TYPE_TABLE:
					if(strcmp(systemTableName,"AK_relation") == 0)
					{
						if(DEBUG)
							printf("AK_init_new_segment__NOTIFICATION: AK_relation found at %d \n", systemTableAddress);
						search = 0;
					}
					break;
				case SEGMENT_TYPE_INDEX:
					if(strcmp(systemTableName,"AK_index") == 0)
					{
						if(DEBUG)
							printf("AK_init_new_segment__NOTIFICATION: AK_index found at %d \n", systemTableAddress);
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
			//tempBlock = AK_get_block(systemTableAddress);
			tempBlock = AK_read_block(systemTableAddress);

			while (freeSpaceFound == 0)
			{
				tupleDictID += 1;
				if (tempBlock->tuple_dict[tupleDictID].size == FREE_INT)
					freeSpaceFound = 1;
			}
			if (freeSpaceFound == 1)
			{
				AK_insert_entry(tempBlock, TYPE_INT, &objectID, tupleDictID);
				AK_insert_entry(tempBlock, TYPE_VARCHAR, name, tupleDictID + 1);
				AK_insert_entry(tempBlock, TYPE_INT, &start_address, tupleDictID + 2);
				AK_insert_entry(tempBlock, TYPE_INT, &end_address, tupleDictID + 3);
				if( DEBUG )
					printf("AK_init_new_segment__NOTIFICATION: Writing block at address %d\n", start_address );
				AK_write_block(tempBlock);
				
				AK_new_extent( start_address, 0, type, header );
			}
			else
			{
				if (DEBUG)
					printf("AK_init_new_segment__ERROR: Cannot initialize segment, no more space in last block!\n");
				return EXIT_ERROR;
			}
			
		}
		if (DEBUG)
			printf("AK_init_new_segment__NOTIFICATION: New segment initialized at %d\n", start_address);
		return start_address;
	}
}


void files_test()
{
	AK_header header;
	int i;
	header.integrity[0] = 21;
	header.type = 1;
	char *ime = "Tomo";
	AK_header header1;
	int i1;
	header1.integrity[0] = 21;
	header1.type = 1;
	char *ime1 = "Tomo1";
	i = AK_initialize_new_segment(ime, SEGMENT_TYPE_TABLE, &header);
	if (i != EXIT_ERROR)
		printf("AK_init_new_segment: Test succeded!\n");
	i1 = AK_initialize_new_segment(ime1, SEGMENT_TYPE_TABLE, &header1);
	if (i1 != EXIT_ERROR)
		printf("AK_init_new_segment: Test1 succeded!\n");
}
