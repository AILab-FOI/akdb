#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct {
	int type;
	char att_name[ MAX_ATT_NAME ];
	int integrity[ MAX_CONSTRAINTS ];
	char constr_name[ MAX_CONSTRAINTS ][ MAX_CONSTR_NAME ];
	char constr_code[ MAX_CONSTRAINTS ][ MAX_CONSTR_CODE ];
} KK_header;

typedef struct {
	int type;
	int address;
	int size;
} KK_tuple_dict;

typedef struct {
	int address;
	int type;
	int chained_with;
	int free_space;
	KK_header header[ MAX_ATTRIBUTES ];
	KK_tuple_dict tuple_dict[ DATA_BLOCK_SIZE ];
	int data[ DATA_BLOCK_SIZE ][ DATA_ENTRY_SIZE ];
} KK_block;


FILE * db;
unsigned int db_file_size;
