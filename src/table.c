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
int AK_num_attr( char * tblName ) {
    int num_attr = 0;
	
    table_addresses *addresses = (table_addresses*)get_table_addresses(tblName);
    if (addresses->address_from[0] == 0)
        num_attr = -1;
    else {
        AK_block *temp_block = (AK_block*)AK_read_block(addresses->address_from[0]);
		
        while (strcmp(temp_block->header[num_attr].att_name,"\0") != 0)  {
            num_attr++;
        }
		free(temp_block);
    }
	free(addresses);

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
    AK_header *head = (AK_header*)calloc( num_attr, sizeof(AK_header));
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
AK_list * AK_get_row(int num, char * tblName) {
	table_addresses *addresses = (table_addresses* ) get_table_addresses(tblName);
	AK_header *t_header = AK_get_header(tblName);
   
	AK_list *row_root = (AK_list*)malloc(sizeof(AK_list));
	InitL(row_root);
	
	int num_rows = AK_get_num_records( tblName );
	int num_attr = AK_num_attr( tblName );
	int i, j, k, l, counter;
	i = 0;
	
	char data[MAX_VARCHAR_LENGHT];
	counter = -1;
	while (addresses->address_from[i] != 0){
		for (j = addresses->address_from[i]; j < addresses->address_to[i]; j++) {
			AK_mem_block *temp = (AK_mem_block*) AK_get_block(j);
			if (temp->block->last_tuple_dict_id == 0) 
				break;
			for (k = 0; k < DATA_BLOCK_SIZE; k+=num_attr) {
				if (temp->block->tuple_dict[k].size > 0)
					counter++;
				if (counter == num) {
					for (l = 0; l < num_attr; l++) {
						int type = temp->block->tuple_dict[k + l].type;
						int size = temp->block->tuple_dict[k + l].size;
						int address = temp->block->tuple_dict[k + l].address;
						memcpy(data, &(temp->block->data[address]), size);
						data[size] = '\0';
						InsertAtEndL(type, &data, size, row_root);
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
AK_list_elem AK_get_tuple( int row, int column, char *tblName ) {
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
					return (AK_list_elem)FirstL(row_root);
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
 * @brief Print row spacer
 * @author Dino Laktašić.
 * @param int col_len[] - max lengths for each attribute cell 
 * @param int length -  total table width 
 * @result printed row spacer
 */
void AK_print_row_spacer(int col_len[], int length) {
	int i, j, col, temp;
	
	j = col = temp = 0;
	
	for(i = 0; i < length; i++) {
		if (!i || i == temp + j) {
			j += TBL_BOX_OFFSET + 1;
			temp += col_len[col++] + 1; 
			printf("+");
		} else {
			printf("-");
		}
	}
}

/**
 * @brief  Print table row
 * @author Dino Laktašić
 * @param int col_len[] - array of max lengths for each attribute
 * @param AK_list *row - list with row elements
 * @return void
 */
AK_print_row(int col_len[], AK_list *row) {
	AK_list_elem el = (AK_list_elem)FirstL(row);
	
	int i = 0;	
	void  *data = (void *)calloc(MAX_VARCHAR_LENGHT, sizeof(void));
	
	printf("\n|");
	while (el != NULL) {			
		memset(data, 0, MAX_VARCHAR_LENGHT);
		switch (el->type) {
			case FREE_CHAR:
			//case FREE_INT:
				printf(" %-*s|", col_len[i] + TBL_BOX_OFFSET, "null");
				break;
			case TYPE_INT:
				memcpy(data, el->data, sizeof(int));
				printf("%*i |", col_len[i] + TBL_BOX_OFFSET, *((int *)data));	
				break;
			case TYPE_FLOAT:
				memcpy(data, el->data, sizeof(float));
				printf("%*.3f |", col_len[i] + TBL_BOX_OFFSET, *((float *)data));	
				break;
			//Currently this header types are handled like TYPE_VARCHAR (print as string left aligned)
			//case TYPE_INTERNAL:
			//case TYPE_NUMBER:
			//case TYPE_DATE:
			//case TYPE_DATETIME:
			//case TYPE_TIME:
			//case TYPE_BLOB:
			//case TYPE_BOOL:
			case TYPE_VARCHAR:
			default:
				memcpy(data, el->data,el->size);
				printf(" %-*s|", col_len[i] + TBL_BOX_OFFSET, (const char *)data);	
		}
		el = el->next;
		i++;
	}
	printf("\n");
	free(data);
}

/**
 * @brief  Print table
 * @author Dino Laktašić and Mislav Čakarić (replaced old print table function by new one)
 * @param char* - table name
 * @return void
 */
void AK_print_table(char *tblName) {
	AK_header *head = AK_get_header(tblName);
    
	int i, j, k, l;
	int num_attr = AK_num_attr(tblName);
	int num_rows = AK_get_num_records(tblName);
	int len[num_attr];  //max length for each attribute in row
	int length = 0;		//length of spacer
	
	//store lengths of header attributes
	for (i = 0; i < num_attr; i++) {
		len[i] = strlen((head + i)->att_name);
	}
	
	//for each header attribute iterate through all table rows and check if 
	//there is longer element than previously longest and store it in array
	for (i = 0; i < num_attr; i++) {
		for (j = 0; j < num_rows; j++) {
			AK_list_elem el = AK_get_tuple(j, i, tblName);
			switch (el->type) {
				case TYPE_INT:
					length = AK_chars_num_from_number(*((int *)(el)->data), 10);
					if (len[i] < length) {
						len[i] = length;
					}
				break;
				case TYPE_FLOAT:
					length = AK_chars_num_from_number(*((float *)(el)->data), 10);
					if (len[i] < length) {
						len[i] = length;
					}
				break;
				case TYPE_VARCHAR:
				default:
					if (len[i] < el->size) {
						len[i] = el->size;
					}
			}
		}
    }
	//num_attr is number of char | + space in printf
	//set offset to change the box size
	length = 0;
	for (i = 0; i < num_attr; length += len[i++]);
	length += num_attr * TBL_BOX_OFFSET + 2 * num_attr + 1;
	
	//start measuring time
	time_t start = clock();
	printf( "Table: %s\n", tblName );
	
	if (num_attr < 0 || num_rows < 0) {
		printf("Table is empty.\n");
	} else {
		//print table header
		AK_print_row_spacer(len, length);
		printf("\n|");

		for (i = 0; i < num_attr; i++) {
			//print attributes center aligned inside box
			k = (len[i] - strlen((head + i)->att_name) + TBL_BOX_OFFSET + 1);
			if (k % 2 == 0) {
				k /= 2;
				printf("%-*s%-*s|", k, " ", k + strlen((head + i)->att_name), (head + i)->att_name);	
			} else {
				k /= 2;
				printf("%-*s%-*s|", k, " ", k + strlen((head + i)->att_name) + 1, (head + i)->att_name);					
			}
			
			//print attributes left aligned inside box
			//printf(" %-*s|", len[i] + MAX_TABLE_BOX_OFFSET, (head + i)->att_name);	
		}
		printf ("\n");
		AK_print_row_spacer(len, length);

		table_addresses *addresses = (table_addresses*) get_table_addresses(tblName);
		AK_list *row_root = (AK_list*) malloc(sizeof (AK_list));
		InitL(row_root);
		
		i=0;
		while (addresses->address_from[i] != 0) {
			for (j = addresses->address_from[i]; j < addresses->address_to[i]; j++) {
				AK_mem_block *temp = (AK_mem_block*) AK_get_block(j);
				if (temp->block->last_tuple_dict_id == 0)
					break;
				for (k = 0; k < DATA_BLOCK_SIZE; k += num_attr) {
					if( temp->block->tuple_dict[k].size > 0 ){
						for (l = 0; l < num_attr; l++) {
							int type = temp->block->tuple_dict[k + l].type;
							int size = temp->block->tuple_dict[k + l].size;
							int address = temp->block->tuple_dict[k + l].address;
							InsertAtEndL(type, &(temp->block->data[address]), size, row_root);
						}
						AK_print_row(len, row_root);
						AK_print_row_spacer(len, length);
						DeleteAllL(row_root);
					}
				}
			}
			i++;
		}
		printf("\n");
                
		/*
		//print table rows
		for (i = 0; i < num_rows; i++) {
			AK_list *row = AK_get_row(i, tblName);
			AK_print_row(len, row);
			AK_print_row_spacer(len, length);
			DeleteAllL(row);
		}
		printf("\n");*/
		//print table rows number and time spent to generate table
		time_t end = clock();
		printf( "%i records found, duration: %d μs\n\n", num_rows, end - start);
	}
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
void create_test_table() {
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
	memset( t_header + 5, '\0', MAX_ATTRIBUTES - 5);

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

/**
 * @brief  Gets obj_id of named table from AK_relation system table
 * @author Dejan Frankovic
 * @param char* - table name
 * @return obj_id of the table or EXIT_ERROR if there is no table with that name
 */
int AK_get_table_obj_id(char *table) {
    int i = 0, table_id=-1;
    AK_list *row;

    i = 0;
    while ((row = AK_get_row(i,"AK_relation")) != NULL ) {
        if (strcmp(row->next->next->data, table) == 0) {
            memcpy(&table_id, row->next->data, sizeof(int));
            break;
        }
        i++;
    }
    if (table_id == -1)
        return EXIT_ERROR;
    return table_id;
}

void table_test() {
  printf("table.c: Present!\n");

  create_test_table();

  printf( "\n********** TABLE ABSTRACTION TEST by Matija Šestak **********\n");

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
  //AK_print_row( row );
  printf("\n");

  printf("Table \"student\": AK_get_attr_name for index 3: ");
  printf( "%s\n", AK_get_attr_name( "student", 3 ));
  printf("\n");

  printf("Table \"student\": AK_get_attr_index of \"year\": ");
  printf( "%d\n", AK_get_attr_index( "student", "year" ));
  printf("\n");

  printf( "Table \"student\": AK_get_tuple for row=0, column=1:");
  printf( "%s\n", AK_tuple_to_string( AK_get_tuple(0,1,"student")));
}