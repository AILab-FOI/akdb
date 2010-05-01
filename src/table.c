/**
@file table.c Provides functions for table abstraction
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

#include "table.h"

AK_list * AK_get_row( int num, char * tblName )
{
  int num_rows = AK_get_num_records( tblName );
  
  if( DEBUG_TABLE )
  {
    printf( "AK_get_row: Accessing row %d in table %s (has %d rows).\n", num, tblName, num_rows );
  }
  
  if( num > num_rows || num < 0 )
  {
    if( DEBUG_TABLE )
    {
      printf( "AK_get_row: Trying to access non existing row %d in table %s.\n", num, tblName );
    }
    return( EXIT_ERROR );
  }
  
  table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );
  KK_header *t_header = AK_get_header( tblName );
  int num_attr = AK_num_attr( tblName );
  
  AK_list *row_root =  (AK_list*) malloc( sizeof(AK_list) );
  InitL(row_root);
  

  int i, j, k, l, counter;
  char data[ MAX_VARCHAR_LENGHT ];
  
  i = 0;
  counter = 0;
  while( addresses->address_from[ i ] != 0 ){
      for( j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++ ){
	  KK_block *temp = (KK_block*) KK_read_block( j );
	  for( k = 0; k < DATA_BLOCK_SIZE; k+=num_attr ){
	      counter++;
	      if( temp->tuple_dict[k].type == FREE_INT )
		  break;
	      if( counter == num )
	      {
		for( l = 0; l < num_attr; l++ ){
		    int type = temp->tuple_dict[ k+l ].type;
		    int size = temp->tuple_dict[ k+l ].size;
		    int address = temp->tuple_dict[ k + l ].address;
		    memcpy( data, &(temp->data[address]), size );
		    data[ size ] = '\0';
		    InsertAtEndL( type, &data, size, row_root );
		}
	      }
	  }
      }
      i++;
  }
  if( DEBUG_TABLE )
  {
    printf( "AK_get_row: Size of list is %d\n", SizeL( row_root ) );
  }
  return row_root;
  
}

void AK_print_row( AK_list * row )
{
  int temp_int;
  float temp_float;
  char temp_char[ MAX_VARCHAR_LENGHT ];
  AK_list_elem e = FirstL( row );
  while( e )
  {
    int type = GetTypeL( e, row );
    int size = GetSizeL( e, row );
    char * data = RetrieveL( e, row );
    
    switch( type )
    {
      case TYPE_INT:
	  memcpy( &temp_int, data, size);
	  printf( "%-10d", temp_int );
	  break;
      case TYPE_FLOAT:
	  memcpy( &temp_float, data, size);
	  printf( "%-10.2f", temp_float );
	  break;

      case TYPE_VARCHAR:
	  memcpy( temp_char, data, size);
	  temp_char[ size ] = '\0';
	  
	  printf( "%-10s", temp_char );
	  break;
    }
    e = NextL( e, row );
  }
  printf( "\n" );
  
}

void table_test() 
{
  printf("table.c: Present!\n");
  
  AK_list * test = AK_get_row( 1, "student" );
  
  AK_print_row( test );
  
  test = AK_get_row( 20, "student" );
  test = AK_get_row( -1, "student" );
}