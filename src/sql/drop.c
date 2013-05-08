/**
@file drop.c Provides DROP functions
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
 17 */

#include "drop.h"

// DROP table

#define DROP_TABLE 0


void AK_drop(int type, AK_drop_arguments* drop_arguments) 
{
  AK_print_table("AK_relation");
  AK_list_elem row_root;
  
  char* tblName;
  
  switch (type) 
  {
    case DROP_TABLE:
      
     
     tblName = (char*)drop_arguments->value;


    table_addresses *addresses2;
    addresses2 = AK_get_segment_addresses(tblName, SEGMENT_TYPE_TABLE);

    AK_mem_block *mem_block2;
    int from = 0, to = 0, j1 = 0, i1 = 0;
    int c;

    for (j1 = 0; j1 < MAX_EXTENTS_IN_SEGMENT; j1++) {
        if (addresses2->address_from != 0) {
            from = addresses2->address_from[j1];
            to = addresses2->address_to[j1];
	    
	    if (from == 0 || to == 0) break;
            for (i1 = from; i1 <= to; i1++) {
                mem_block2 = (AK_mem_block *) AK_get_block(i1);
		mem_block2->block->type = BLOCK_TYPE_FREE;
		
		for (c = 0; c < DATA_BLOCK_SIZE; c++) mem_block2->block->tuple_dict[c].type = FREE_INT;
		
								mem_block2->block->data[0] = FREE_CHAR;
            }
        } else break;
    }
    
    
    
    




   int i = 0;
    int freeVar = 0;
    int data_adr = 0;
    int data_size = 0;
    int data_type = 0;
    int address_sys;
    char name_sys[MAX_ATT_NAME];
    char *sys_table;

  //  switch (segmentType) {
    //    case SEGMENT_TYPE_TABLE:
            sys_table = "AK_relation";
      //      break;
      //  case SEGMENT_TYPE_INDEX:
       //     sys_table = "AK_index";
         //   break;
    //}

	//Ak_dbg_messg(HIGH, MEMO_MAN,"get_segment_addresses: Serching for %s table \n", sys_table);
    AK_mem_block *mem_block = (AK_mem_block *) AK_get_block(0);

    for (i = 0; i < DATA_BLOCK_SIZE; i++) {
        memset(name_sys, 0, MAX_ATT_NAME);

        if (mem_block->block->tuple_dict[i].address == FREE_INT) {
            break;
        }

        data_adr = mem_block->block->tuple_dict[i].address;
        data_size = mem_block->block->tuple_dict[i].size;
        data_type = mem_block->block->tuple_dict[i].type;
        memcpy(name_sys, mem_block->block->data + data_adr, data_size);

        i++;
        data_adr = mem_block->block->tuple_dict[i].address;
        data_size = mem_block->block->tuple_dict[i].size;
        data_type = mem_block->block->tuple_dict[i].type;
        memcpy(&address_sys, mem_block->block->data + data_adr, data_size);

        if (strcmp(name_sys, sys_table) == 0) {
	//		Ak_dbg_messg(HIGH, MEMO_MAN, "get_segment_addresses: Found the address of the %s table: %d \n", sys_table, address_sys);
            break;
        }
    }

    mem_block = (AK_mem_block *) AK_get_block(address_sys);
    table_addresses * addresses = (table_addresses *) malloc(sizeof (table_addresses));

    //memset(addresses->address_from, 0, MAX_EXTENTS_IN_SEGMENT);
    //memset(addresses->address_to, 0, MAX_EXTENTS_IN_SEGMENT);

    for (freeVar = 0; freeVar < MAX_EXTENTS_IN_SEGMENT; freeVar++) {
        addresses->address_from[freeVar] = 0;
        addresses->address_to[freeVar] = 0;
    }

    char name[MAX_VARCHAR_LENGTH];

    for (i = 0; i < DATA_BLOCK_SIZE; i++) {
        if (mem_block->block->tuple_dict[i].type == FREE_INT)
            break;
        i++;
        memcpy(name, &(mem_block->block->data[mem_block->block->tuple_dict[i].address]), mem_block->block->tuple_dict[i].size);
        name[ mem_block->block->tuple_dict[i].size] = '\0';
        if (strcmp(name, tblName) == 0) {
	  //  mem_block->block->data[mem_block->block->tuple_dict[i].address] = 0;
	    i++;
	    mem_block->block->data[mem_block->block->tuple_dict[i].address] = 0;
	    i++;
	    mem_block->block->data[mem_block->block->tuple_dict[i].address] = 0;
	}
   /*     i++;
        memcpy(&address_from, &(mem_block->block->data[mem_block->block->tuple_dict[i].address]), mem_block->block->tuple_dict[i].size);
        i++;
        memcpy(&address_to, &(mem_block->block->data[mem_block->block->tuple_dict[i].address]), mem_block->block->tuple_dict[i].size);

        //if found the table that addresses we need
        if (strcmp(name, tblName) == 0) {
            addresses->address_from[j] = address_from;
            addresses->address_to[j] = address_to;
            j++;
        }
        if (segmentType == SEGMENT_TYPE_INDEX) {
            i += 2;
        }*/
    }
 //   return addresses;
    
    
    
    
    
    
          //tblName = "AK_relation";
   
       row_root = (AK_list_elem) malloc(sizeof (AK_list));
      Ak_Init_L(row_root);
      Ak_DeleteAll_L(row_root);
   
   //char *name2 = (char*) drop_arguments->next->value;
      Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, tblName, "AK_relation", "name", row_root, 1);
  // Ak_Insert_New_Element_For_Update(TYPE_VARCHAR, "FOI", tblName, "firstname", row_root, 0);

    Ak_delete_row(row_root);
    
    
    
   //AK_flush_cache();
    
    AK_print_table("AK_relation");
    AK_print_table(tblName);
    
      
    break;
    default:
      break;
  }
}



void AK_drop_test()
{
  AK_drop_arguments* drop_arguments = malloc(sizeof (AK_drop_arguments));
  drop_arguments->value = "employee";
  AK_drop(DROP_TABLE, drop_arguments);
}
