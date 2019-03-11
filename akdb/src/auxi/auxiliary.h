/**
@file auxiliary.h Header file zhat provides data structures for the auxiliary functions
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


#ifndef AUXILIARY
#define AUXILIARY

#include "constants.h"
#include "configuration.h"
#include "test.h"

#include "assert.h"
#include "time.h"
#include "string.h"
#include "ctype.h"
#include "debug.h"
#include "mempro.h"

#define MAX_LOOP_ITERATIONS 1000
//#include "ini_parser/iniparser.h"

/**
 * @author Domagoj Šitum
 * @var testMode
 * @brief You can turn testMode on or off with TEST_MODE_ON and TEST_MODE_OFF.
 * To do this, simply enable or disable it in YOUR function (not in any other!)
 * Test mode can be used when you need some special cases in your functions
 * (i.e., when you are testing some functionality, which doesn't apply in normal conditions).
 * But don't forget to turn this mode off, after you are done (within test function for example)!
 */
int testMode;


/**
 * @author Aleksandra Polak
 * @brief Function that change type of argument from string to integer
 * @param *arg_type type of an argument
 * @return EXIT_SUCCESS of the function (return type of argument in value of integer) or EXIT_ERROR
*/
char* AK_convert_type(char* arg_type);


/**
 * @author Ljiljana Pintarić
 * @struct list_node
 * @brief Structure defines a list node.
 */
struct list_node { 
///TODO - type, attribute name, table staviti na početak polja data
		///data type
    int type;
    //data size in list element
    int size;
    ///loaded data
    char data[ MAX_VARCHAR_LENGTH ]; 
     //table name
    char table[ MAX_ATT_NAME ];
    //attribute name
    char attribute_name[ MAX_ATT_NAME ];
     int constraint; //if 0 then it is new data for insertion, if 1 then it is a constraint on which update and delete searches the data
    ///pointer to next element
    struct list_node *next;
};

typedef struct list_node AK_list;
typedef struct list_node *AK_list_elem;

#define TBL_BOX_OFFSET 1
//#define TBL_COL_DELIMITER '|'
//#define TBL_ROW_DELIMITER '-'

/*
enum tbl_valalign {
        ALIGN_LEFT  0,
        ALIGN_RIGHT 1;
};*/

int AK_strcmp(const void *a, const void *b);

/**
 * @author Frane Jakelić
 * @struct Vertex
 * @brief Structure defines a Vertex node element. Every Vertex has its VertexId, index, lowLink and pointer to next edge and vertex
 */
struct Vertex{
    int vertexId;
    int index;
    int lowLink;
    struct Succesor *nextSuccesor;
    struct Vertex *nextVertex;
};

/**
 * @author Frane Jakelić
 * @struct Succesor
 * @brief Structure defines a Succesor  element. Every Succesor has its Vertex pointer and pointer to next Succesor in the linked list
 */
struct Succesor{
	struct Vertex *link;
	struct Succesor *nextSuccesor;
};

/**
 * @author Frane Jakelić
 * @struct Stack
 * @brief Structure defines a Stack element. Every Stack has its Vertex pointer and pointer to next Stack in the linked list
 */
struct Stack{
	struct Vertex *link;
	struct Stack *nextElement;
};

typedef struct Vertex AK_graph;
typedef struct Succesor *AK_succesor;
typedef struct Vertex *AK_vertex;
typedef struct Stack *AK_stack;
typedef struct Stack AK_stackHead;

/**
 * @author Marko Sinko
 * @struct AK_synchronization_info
 * @brief Structure for managing the synchronization between multiple threads
 *        accessing the same resources (essentially a mutex).
 */
typedef struct {
    int init;
    int ready;
#ifdef _WIN32
    CRITICAL_SECTION critical_section;
#endif
#ifdef __linux__
    pthread_mutex_t mutex;
#endif
} AK_synchronization_info;

int AK_chars_num_from_number(int number, int base);
size_t AK_type_size(int iDB_type, char *szVarchar);
int AK_strcmp(const void *a, const void *b);
void Ak_Init_L3(struct list_node **L);
struct list_node *Ak_First_L2(struct list_node *L);
struct list_node *Ak_End_L2(struct list_node *L);
struct list_node *Ak_Next_L2(struct list_node *current);
struct list_node *Ak_Previous_L2(struct list_node *current, struct list_node *L);
unsigned int Ak_IsEmpty_L2(struct list_node *L);
void Ak_InsertBefore_L2(int type, char* data, int size, struct list_node **current, struct list_node **L);
void Ak_InsertAfter_L2(int type, char* data, int size, struct list_node **current,  struct list_node **L);
void Ak_InsertAtBegin_L3(int type, char* data, int size, struct list_node *L);
void Ak_InsertAtEnd_L3(int type, char* data, int size, struct list_node *L);
void Ak_Delete_L3(struct list_node **current, struct list_node **L);
void Ak_DeleteAll_L3(struct list_node **L);
int Ak_Size_L2(struct list_node *L);
char* Ak_Retrieve_L2(struct list_node *current, struct list_node *L);
//int Ak_GetType_L(AK_list_elem current, AK_list *L);
//int Ak_GetSize_L(AK_list_elem current, AK_list *L);
struct list_node * Ak_GetNth_L2(int pos, struct list_node *row);

//int Ak_Get_Position_Of_Element(AK_list_elem SearchedElement, AK_list *L);
char *AK_get_array_perms(char *arr) ;
AK_vertex AK_search_vertex(int id);
AK_vertex AK_search_empty_link() ;
AK_vertex AK_add_vertex(int id);
AK_succesor AK_add_succesor(int succesorId, int succesorOf);
AK_stack AK_search_empty_stack_link(AK_stack stackRoot);
AK_stack AK_push_to_stack(int id);
AK_stack AK_pop_from_stack();
AK_stack AK_search_in_stack(int id);
AK_are_values_unique(char* tableName, int numRows, char values[][MAX_VARCHAR_LENGTH], int numOfImpAttPos, int positionsOfAtts[], int performCleanup);
int MIN(int X, int Y);
void AK_tarjan(int id);
TestResult AK_tarjan_test();

AK_synchronization_info* AK_init_critical_section();
void AK_destroy_critical_section(AK_synchronization_info* info);
void AK_enter_critical_section(AK_synchronization_info* info);
void AK_leave_critical_section(AK_synchronization_info* info);

//void AK_copy_L_Ele(AK_list_elem srcElem, AK_list_elem destElem);
//void AK_copy_L(AK_list *src, AK_list *dest) ;
//int AK_compare_L(AK_list *srcInput, AK_list *srcOriginal);


#endif
