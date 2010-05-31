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

/**
 * @brief  Determine the number of attributes in the table
 * <ol>
 * <li>Read addresses of extents</li>
 * <li>If there is no extents in the table, return -1</li>
 * <li>else read the first block</li>
 * <li>while  header tuple exists in the block, increment num_attr</li>
 * </ol>
 * @author Matija Šestak.
 * @param char* - table name
 * @result int - number of attributes in the table
 */
int AK_num_attr( char * tblName ){
    int num_attr = 0;
    table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );
    if( addresses->address_from[0] == 0 )
        num_attr = -1;
    else{
        AK_block *temp = (AK_block *)AK_read_block( addresses->address_from[0] );
        int i = 0;
        while( strcmp( temp->header[i].att_name,"\0" ) != 0 ){
            num_attr++;
            i++;
        }
    }
    return num_attr;
}

/**
 * @brief  Determine number of rows in the table
 * <ol>
 * <li>Read addresses of extents</li>
 * <li>If there is no extents in the table, return -1</li>
 * <li>For each extent from table</li>
 * <li>For each block in the extent</li>
 * <li>Get a block</li>
 * <li>Exit if there is no records in block</li>
 * <li>Count tuples in block</li>
 * <li>Return the number of tuples divided by number of attributes</li>
 * </ol>
 * @author Matija Šestak.
 * @param char* - table name
 * @result int - number of rows in the table
 */
int AK_get_num_records( char *tblName ){
    int num_rec = 0;
    table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );
    if( addresses->address_from[0] == NULL )
        return -1;
    int i = 0, j, k;
     AK_mem_block *temp = (AK_mem_block*)AK_get_block( addresses->address_from[0]);
        while( addresses->address_from[ i ] != 0 ){
            for( j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++ ){
                temp = (AK_mem_block*) AK_get_block( j );
                if( temp->block->last_tuple_dict_id == 0)
                    break;
                for( k = 0; k < DATA_BLOCK_SIZE; k++ ){
                    if( temp->block->tuple_dict[ k ].size > 0 ){
                        num_rec++;
                    }
                }
            }
            i++;
        }

    int num_head = AK_num_attr( tblName );
    return num_rec / num_head;
}

/**
 * @brief  Get table header
 * <ol>
 * <li>Read addresses of extents</li>
 * <li>If there is no extents in the table, return -1</li>
 * <li>else read the first block</li>
 * <li>allocate array</li>
 * <li>copy table header to the array</li>
 * </ol>
 * @author Matija Šestak.
 * @param char* - table name
 * @result AK_header* - array of table header
 */
AK_header* AK_get_header( char *tblName ){
    table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );
    if( addresses->address_from[0] == NULL )
        return 0;
    AK_mem_block *temp = (AK_mem_block*) AK_get_block( addresses->address_from[0] );

    int num_attr = AK_num_attr( tblName );
    AK_header *head = (AK_header*)malloc( num_attr * sizeof(AK_header));
    memcpy( head, temp->block->header, num_attr * sizeof(AK_header));
    return head;
}

/**
 * @author Matija Šestak.
 * @brief  Get attribute name for some zero-based index
 * @param char* - table name
 * @param int - zero-based index
 * @result char* - attribute name
 */
char* AK_get_attr_name( char *tblName, int index ){
    int num_attr = AK_num_attr( tblName );
    if( index >= num_attr )
        return NULL;
    else{
        AK_header *header = AK_get_header( tblName );
        return (header+index)->att_name;
    }
}

/**
 * @author Matija Šestak.
 * @brief  Get zero-based index for atrribute
 * @param char* - table name
 * @param char* - attribute name
 * @result int - zero-based index
 */
int AK_get_attr_index( char *tblName, char *attrName ){
    if ( tblName == NULL || attrName == NULL )
        return -1;
    int num_attr = AK_num_attr(tblName);
    AK_header *header = AK_get_header(tblName);
    int index = 0;
    while( index < num_attr ){
        if( strcmp( attrName, (header+index)->att_name)== 0)
            return index;
        index++;
    }
    return -1;
}

/**
 * @author Matija Šestak.
 * @brief  Get all values in some column and put on the list
 * @param int - zero-based column index
 * @param char* - table name
 * @result AK_list* - column values list
 */
AK_list * AK_get_column( int num, char *tblName ){
    int num_attr = AK_num_attr( tblName );
    if( num >= num_attr || num < 0 )
        return NULL;
     AK_list *row_root =  (AK_list*) malloc( sizeof(AK_list) );
     InitL(row_root);

     table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );
     int i, j, k;
     char data[ MAX_VARCHAR_LENGHT ];

     i=0;
     while( addresses->address_from[i] != 0 ){
         for( j = addresses->address_from[i]; j < addresses->address_to[i]; j++ ){
          AK_mem_block *temp = (AK_mem_block*) AK_get_block( j );
          if( temp->block->last_tuple_dict_id == 0) break;
	  for( k = num; k < DATA_BLOCK_SIZE; k+=num_attr ){
              if( temp->block->tuple_dict[k].type != FREE_INT ){
                  int type = temp->block->tuple_dict[k].type;
                  int size = temp->block->tuple_dict[k].size;
                  int address = temp->block->tuple_dict[k].address;
		    memcpy( data, &(temp->block->data[address]), size );
		    data[ size ] = '\0';
		    InsertAtEndL( type, &data, size, row_root );
              }
          }
         }
         i++;
     }
     return row_root;
}

/**
 * @author Markus Schatten, Matija Šestak.
 * @brief  Get all values in some row and put on the list
 * @param int - zero-based row index
 * @param char* - table name
 * @result AK_list* - row values list
 */
AK_list * AK_get_row( int num, char * tblName )
{
  int num_rows = AK_get_num_records( tblName );
  
  table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );
  AK_header *t_header = AK_get_header( tblName );
  int num_attr = AK_num_attr( tblName );
  
  AK_list *row_root =  (AK_list*) malloc( sizeof(AK_list) );
  InitL(row_root);
  

  int i, j, k, l, counter;
  char data[ MAX_VARCHAR_LENGHT ];
  
  i = 0;
  counter = -1;
  while( addresses->address_from[ i ] != 0 ){
      for( j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++ ){
	  AK_mem_block *temp = (AK_mem_block*) AK_get_block( j );
          if( temp->block->last_tuple_dict_id == 0) break;
	  for( k = 0; k < DATA_BLOCK_SIZE; k+=num_attr ){
	      if( temp->block->tuple_dict[k].size > 0 )
		 counter++;
	      if( counter == num )
	      {
		for( l = 0; l < num_attr; l++ ){
		    int type = temp->block->tuple_dict[ k+l ].type;
		    int size = temp->block->tuple_dict[ k+l ].size;
		    int address = temp->block->tuple_dict[ k + l ].address;
		    memcpy( data, &(temp->block->data[address]), size );
		    data[ size ] = '\0';
		    InsertAtEndL( type, &data, size, row_root );
		}
               return row_root;
	      }
	  }
      }
      i++;
  }
  return NULL;
}

/**
 * @author Matija Šestak.
 * @brief  Get value in some row and column
 * @param int - zero-based row index
 * @param int - zero-based column index
 * @param char* - table name
 * @result AK_list* - value in the list
 */
AK_list * AK_get_tuple( int row, int column, char *tblName )
{
  int num_rows = AK_get_num_records( tblName );
  int num_attr = AK_num_attr( tblName );

  if( row >= num_rows || column >= num_attr )
      return NULL;

  table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );

  AK_list *row_root =  (AK_list*) malloc( sizeof(AK_list) );
  InitL(row_root);

  int i, j, k, counter;
  char data[ MAX_VARCHAR_LENGHT ];

  i = 0;
  counter = -1;
  while( addresses->address_from[ i ] != 0 ){
      for( j = addresses->address_from[ i ]; j < addresses->address_to[ i ]; j++ ){
	  AK_mem_block *temp = (AK_mem_block*) AK_get_block( j );
          if( temp->block->last_tuple_dict_id == 0) break;
	  for( k = 0; k < DATA_BLOCK_SIZE; k+=num_attr ){
	      if( temp->block->tuple_dict[k].size > 0 )
		 counter++;
	      if( counter == row) {
                int type = temp->block->tuple_dict[ k+column ].type;
                int size = temp->block->tuple_dict[ k+column ].size;
                int address = temp->block->tuple_dict[ k + column ].address;
                memcpy( data, &(temp->block->data[address]), size );
                data[ size ] = '\0';
                InsertAtEndL( type, &data, size, row_root );
                return FirstL(row_root);
	      }
	  }
      }
      i++;
  }
  return NULL;
}

/**
 * @author Matija Šestak.
 * @brief  Convert tuple value to string
 * @param AK_list* - tuple in the list
 * @result char* - string
 */
char * AK_tuple_to_string( AK_list *tuple ){
  int temp_int;
  float temp_float;
  char temp_char[ MAX_VARCHAR_LENGHT ];

  char *buff = (char*)malloc(MAX_VARCHAR_LENGHT);

    switch( tuple->type ){
      case TYPE_INT:
	  memcpy( &temp_int, tuple->data, tuple->size);
          sprintf( buff, "%d", temp_int);
          return buff;
	  break;
      case TYPE_FLOAT:
	  memcpy( &temp_float, tuple->data, tuple->size);
          sprintf( buff, "%f", temp_float);
          return buff;
	  break;
      case TYPE_VARCHAR:
	  memcpy( temp_char, tuple->data, tuple->size);
	  temp_char[ tuple->size ] = '\0';
          sprintf( buff, "%s", temp_char);
          return buff;
	  break;
    }
    return NULL;
}

/**
 * @author Matija Šestak.
 * @brief  Print values in the AK_list
 * @param AK_list* - list of the values
 * @param int - 0-horizontal, 1 - vertical
 * @result void
 */
void AK_print_list( AK_list * L, int how)
{
  int temp_int;
  float temp_float;
  char temp_char[ MAX_VARCHAR_LENGHT ];
  AK_list_elem e = FirstL( L );
  while( e )
  {
    int type = GetTypeL( e, L );
    int size = GetSizeL( e, L );
    char * data = RetrieveL( e, L );
    
    switch( type )
    {
      case TYPE_INT:
	  memcpy( &temp_int, data, size);
	  printf( "%d", temp_int );
          if( how )
              printf("\n");
          else
              printf( ", ");
	  break;
      case TYPE_FLOAT:
	  memcpy( &temp_float, data, size);
	  printf( "%f", temp_float );
          if( how )
              printf("\n");
          else
              printf( ", ");
	  break;
      case TYPE_VARCHAR:
	  memcpy( temp_char, data, size);
	  temp_char[ size ] = '\0';
	  printf( "%s", temp_char );
          if( how )
              printf("\n");
          else
              printf( ", ");
	  break;
    }
    e = NextL( e, L );
  }
  printf( "\n" );
  
}

/**
 * @author Matija Šestak.
 * @brief  Print values in the row
 * @param AK_list* - list of the values
 * @result void
 */
void AK_print_row( AK_list *row ){
    AK_print_list( row, 0 );
}

/**
 * @author Matija Šestak.
 * @brief  Print values in the column
 * @param AK_list* - list of the values
 * @result void
 */
void AK_print_column( AK_list *row ){
    AK_print_list( row, 1 );
}

/**
 * @author Matija Šestak.
 * @brief  Print table header
 * @param char* - table name
 * @result void
 */
void AK_print_header( char *tblName ){
    int num_attr = AK_num_attr( tblName );
    AK_header *head = AK_get_header( tblName );

    int *rowWidth = (int*)malloc( num_attr*sizeof(int));
    int i;
    for( i = 0; i < num_attr; i++ ){
        rowWidth[i] = strlen((head+i)->att_name)+6;
    }
    char spacer[255];
    spacer[0]='+';
    spacer[1]='\0';
    for( i = 0; i < num_attr; i++ ){
        int j;
        for( j = 0; j < rowWidth[i]; j++ )
            strcat( spacer, "-");
        strcat( spacer, "+");
    }

    puts(spacer);
    printf( "|");
    for( i = 0; i < num_attr; i++ ){
        char pattern[ 20 ];
        char buff[20];
        pattern[ 0 ] = '%';
        pattern[ 1 ] = '-';
        pattern[ 2 ] = '\0';
        sprintf( buff, "%ds",rowWidth[i]);
        strcat( pattern, buff);
        printf( pattern, (head+i)->att_name);
        printf("|");
    }
    printf("\n");
    puts( spacer );
    free( rowWidth );
}

/**
 * @brief  Print table
 * @author Matija Šestak.
 * @param char* - table name
 * @result void
 */
void AK_print_table( char *tblName ){
    int temp_int;
    float temp_float;
    char temp_char[ MAX_VARCHAR_LENGHT ];

    table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );
    AK_mem_block *temp = (AK_mem_block*)AK_get_block( addresses->address_from[0]);

    int num_attr = AK_num_attr( tblName );
    AK_header *head = AK_get_header( tblName );

    int *rowWidth = (int*)malloc( num_attr*sizeof(int));
    int i;
    for( i = 0; i < num_attr; i++ ){
        rowWidth[i] = strlen((head+i)->att_name)+6;
    }
    char spacer[255];
    spacer[0]='+';
    spacer[1]='\0';
    for( i = 0; i < num_attr; i++ ){
        int j;
        for( j = 0; j < rowWidth[i]; j++ )
            strcat( spacer, "-");
        strcat( spacer, "+");
    }

    time_t start = clock();

    printf( "Table: %s\n", tblName );
    AK_print_header(tblName);
    int numRows = AK_get_num_records( tblName );
    for( i = 0; i < numRows; i++ ){
        int j;
        printf("|");
        AK_list * L = AK_get_row( i, tblName );
        AK_list_elem e = FirstL( L );
        for( j =0; j < num_attr; j++ ){
            char pattern[ 20 ];
            char buff[20];
            pattern[ 0 ] = '%';
            pattern[ 1 ] = '-';
            pattern[ 2 ] = '\0';
            sprintf( buff, "%d",rowWidth[j]);
            strcat( pattern, buff);

            int type = GetTypeL( e, L );
            int size = GetSizeL( e, L );
            char * data = RetrieveL( e, L );

            switch( temp->block->tuple_dict[j].type){
                case TYPE_INT:
                    strcat( pattern, "d");
                    memcpy( &temp_int, data, size);
                    printf( pattern, temp_int );
                    break;
                case TYPE_FLOAT:
                    strcat( pattern, ".3f");
                    memcpy( &temp_float, data, size);
                    printf( pattern, temp_float );
                    break;
                case TYPE_VARCHAR:
                    strcat( pattern, "s");
                    memcpy( temp_char, data, size);
                    temp_char[size]='\0';
                    printf( pattern, temp_char );
                    break;
            }
            e=NextL(e,L);
            printf("|");
        }
        printf("\n");
        puts(spacer);
    }
    time_t end = clock();
    printf( "%d records found, duration: %d μs\n\n", numRows, end-start);

    free( rowWidth );
}

/**
 * @author Matija Šestak.
 * @brief  Check if table empty
 * @param char* - table name
 * @result int - ture/false
 */
int AK_table_empty( char *tblName ){
     table_addresses *addresses = (table_addresses* ) get_table_addresses( tblName );
     AK_mem_block *temp = (AK_mem_block*) AK_get_block( addresses->address_from[0] );
     return (temp->block->last_tuple_dict_id==0)?1:0;
}

/**
 * @author Matija Šestak.
 * @brief  Create test table
 * @param void
 * @result void
 */
void create_test_table(){
    int i;
    //create header
    AK_header t_header[ MAX_ATTRIBUTES ];
    AK_header* temp;

    temp = (AK_header*)AK_create_header( "mbr", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header, temp, sizeof( AK_header ));

    temp = (AK_header*)AK_create_header( "firstname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header + 1, temp, sizeof( AK_header ));

    temp = (AK_header*)AK_create_header( "lastname", TYPE_VARCHAR, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header + 2, temp, sizeof( AK_header ));

    temp = (AK_header*)AK_create_header( "year", TYPE_INT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header + 3, temp, sizeof( AK_header ));

    temp = (AK_header*)AK_create_header( "tezina", TYPE_FLOAT, FREE_INT, FREE_CHAR, FREE_CHAR);
    memcpy( t_header + 4, temp, sizeof( AK_header ));

    for( i = 5; i < MAX_ATTRIBUTES; i++ )
    {
    	memcpy( t_header + i, "\0", sizeof( "\0" ));
    }

    //create table
    char *tblName = "student";

    printf("op_selection_test: Before segment initialization: %d\n", AK_num_attr( tblName ) );

    int startAddress = AK_initialize_new_segment( tblName, SEGMENT_TYPE_TABLE, t_header);

    if( startAddress != EXIT_ERROR )
        printf( "\nTABLE %s CREATED!\n", tblName );


    printf("op_selection_test: After segment initialization: %d\n", AK_num_attr( tblName ) );

    element row_root =  (element) malloc( sizeof(list) );
    InitializeList(row_root);

    int mbr = 35890, year = 1999;
    float weight = 80.00;

    //insert rows in table student
    mbr++; year++; weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Matija", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Sestak", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Igor", tblName, "firstname", row_root);
    InsertNewElement( TYPE_VARCHAR, "Mesaric", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Dalibor", tblName, "firstname", row_root);
    InsertNewElement( TYPE_VARCHAR, "Slunjski", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Dino", tblName, "firstname", row_root);
    InsertNewElement( TYPE_VARCHAR, "Alagic", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Miroslav", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Zver", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );

    mbr++; year++;weight += 0.75;
    DeleteAllElements(row_root);
    InsertNewElement( TYPE_INT, &mbr, tblName, "mbr", row_root );
    InsertNewElement( TYPE_VARCHAR, "Josip", tblName, "firstname", row_root );
    InsertNewElement( TYPE_VARCHAR, "Vincek", tblName, "lastname", row_root );
    InsertNewElement( TYPE_INT, &year, tblName, "year", row_root );
    InsertNewElement( TYPE_FLOAT, &weight, tblName, "tezina", row_root );
    insert_row( row_root );
}

void table_test() 
{
  printf("table.c: Present!\n");

  create_test_table();

  printf( "\n********** TABLE ABSTRACTION TEST by Matija Šestak **********\n");

  printf("\n");
  printf( "Table \"student\":AK_print_header\n");
  AK_print_header( "student" );
  printf("\n");

  printf( "Table \"student\":AK_print_table\n");
  AK_print_table( "student" );
  printf("\n");

  printf( "Table \"student\": AK_table_empty: ");
  if( AK_table_empty( "student"))
      printf( "true\n");
  else
      printf( "false\n");
  printf("\n");

  printf( "Table \"student\":AK_num_attr: ");
  printf( "%d\n", AK_num_attr("student"));
  printf( "\n");

  printf( "Table \"student\":AK_get_num_records: ");
  printf( "%d\n", AK_get_num_records("student"));
  printf( "\n");

  printf( "Table \"student\":AK_get_row: First row: \n");
  AK_list * row = AK_get_row( 0, "student" );
  AK_print_row( row );
  printf("\n");
  
  printf("Table \"student\": AK_get_column: Second column: \n");
  AK_list * column = AK_get_column( 1, "student" );
  AK_print_column( column );

  printf("Table \"student\": AK_get_attr_name for index 3: ");
  printf( "%s\n", AK_get_attr_name( "student", 3 ));
  printf("\n");

  printf("Table \"student\": AK_get_attr_index of \"year\": ");
  printf( "%d\n", AK_get_attr_index( "student", "year" ));
  printf("\n");

  printf( "Table \"student\": AK_get_tuple for row=0, column=1:");
  printf( "%s\n", AK_tuple_to_string( AK_get_tuple(0,1,"student")));
}