#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXIT_SUCCESS 0
#define EXIT_ERROR 1

#define BLOCK_TYPE_FREE -1
#define BLOCK_TYPE_NORMAL 0
#define BLOCK_TYPE_CHAINED 1
#define NOT_CHAINED -1
#define BLOCK_DATA_BEGIN 0
#define FREE_INT -1
#define FREE_CHAR '\0'

#define DB_FILE "kalashnikov.db"

#define MAX_OBJECT_NAME 255
#define MAX_EXTENTS 20
#define MAX_ATTRIBUTES 10
#define MAX_ATT_NAME 255
#define MAX_CONSTRAINTS 5
#define MAX_CONSTR_NAME 255
#define MAX_CONSTR_CODE 255
#define DATA_BLOCK_SIZE 500

#define DB_FILE_SIZE 20

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
	KK_tuple_dict data[ DATA_BLOCK_SIZE ];
} KK_block;

typedef struct {
	int block_begin;
	int block_end;
} KK_extent;

typedef struct {
	char obj_name[ MAX_OBJECT_NAME ];
	KK_extent extent[ MAX_EXTENTS ];
} KK_segment;

FILE * db;
unsigned int db_file_size;
