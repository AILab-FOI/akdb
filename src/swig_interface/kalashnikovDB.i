/* File: kalashnikovDB.i */
/*
The interface file contains ANSI C function prototypes and variable declarations. 
The %module directive defines the name of the module that will be created by SWIG. 
The %{ %} block provides a location for inserting additional code, such as C header 
files or additional C declarations, into the generated C wrapper code.
*/
%module kalashnikovDB


/*
@brief used for conversion or marshalling of datatypes between programming languages. 
Specifically, for every C/C++ declaration, SWIG must somehow generate wrapper code that 
allows values to be passed back and forth between languages. Since every programming 
language represents data differently, this is not a simple of matter of simply linking code 
together with the C linker. Instead, SWIG has to know something about how data is 
represented in each language and how it can be manipulated. 

Documentation for typemaps:
  http://www.swig.org/Doc2.0/Typemaps.html#Typemaps

Convert from Python --> C 
  %typemap(in) 

Convert from C --> Python 
  %typemap(out) 

*/

/*
Convert python data type to C void* 
check if data is of type int, string, long or float, and then cast it to (void*)
*/
%typemap(in) void* {
  if (PyString_Check($input))
   {
    char* b =  PyString_AsString($input);
    $1 = (void*)b;
   }
  else if (PyInt_Check($input))
  {
    int a =  PyInt_AsLong($input);
    $1 = (void*)&a;
  }
  else if (PyFloat_Check($input))
  {
    float c =  PyFloat_AsDouble($input);
    $1 = (void*)&c;
  }
    
}

/*
Convert python list to C int and int *
given a python list, eg. ["a", "b", "c"], passed to C function that takes int (number of elements) and 
int* (elements in int array), list is split into int (representing list size) and int* (representing int array)
*/
%typemap(in) (int _num, int* _type) {
  int i;
  if (!PyList_Check($input)) {
    PyErr_SetString(PyExc_ValueError, "Expecting a list");
    return NULL;
  }
  $1 = PyList_Size($input);
  $2 = (int *) malloc(($1)*sizeof(int));
  for (i = 0; i < $1; i++) {
    PyObject *s = PyList_GetItem($input,i);
    if (!PyInt_Check(s)) {
        free($2);
        PyErr_SetString(PyExc_ValueError, "List items must be integers");
        return NULL;
    }
    $2[i] = PyInt_AsLong(s);
  }
}

/*
The "freearg" typemap is used to cleanup argument data. It is only used when an argument might have 
allocated resources that need to be cleaned up when the wrapper function exits. The "freearg" 
typemap usually cleans up argument resources allocated by the "in" typemap.
*/
%typemap(freearg) (int _num, int* _type) {
   if ($2) free($2);
}

/*
converts C char* into Python string
*/
%typemap(out) unsigned char * {
  $1 = PyString_FromString((char *)$input);
}

/*
converts Python datatypes (string, int or float) into C char* data type
*/
%typemap(in) char* data {
   
  if (PyString_Check($input))
   {
    char* b =  PyString_AsString($input);
    $1 = (char*)b;
   }
  else if (PyInt_Check($input))
  {
    int a =  PyInt_AsLong($input);
    $1 = (char*)&a;
  }
  else if (PyFloat_Check($input))
  {
    float c =  PyFloat_AsDouble($input);
    $1 = (char*)&c;
  }
}

/*
converts C int* datatype into Pythons list of integers
*/
%typemap(out) int * {
  PyObject *list = PyList_New(MAX_CONSTRAINTS);
  int i = 0;
  int *polje = $1;
  for (i = 0; i < MAX_CONSTRAINTS; i++) {
    PyObject *item = PyInt_FromLong(polje[i]);
    PyList_Append(list, item);
  }
  $result = list;
}

/*
handles C AK_block datatypes in Python
*/
%typemap(in) AK_block * block{
  AK_block *bl = malloc(sizeof(AK_block));
  memcpy(bl, $input, sizeof(AK_block));
  $1 = bl;
}

/*
converts Python list into char** type.
*/
%typemap(in) char ** {
  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    int i = 0;
    $1 = (char **) malloc((size+1)*sizeof(char *));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($input,i);
      if (PyString_Check(o))
  $1[i] = PyString_AsString(PyList_GetItem($input,i));
      else {
  PyErr_SetString(PyExc_TypeError,"list must contain strings");
  free($1);
  return NULL;
      }
    }
    $1[i] = 0;
  } else {
    PyErr_SetString(PyExc_TypeError,"not a list");
    return NULL;
  }
}

/*
frees char** allocated memory
*/
%typemap(freearg) char ** {
  free((char *) $1);
}

/*
handles AK_create_table_parameter * datatype in Python.
*/
%typemap(out) AK_create_table_parameter * {
  AK_create_table_parameter* parametar = malloc(sizeof(AK_create_table_parameter));
  memcpy(parametar, $1, sizeof(AK_create_table_parameter));
  $result = parametar;
}

/*
Converts python list to AK_create_table_parameter* datatype
*/
%typemap(in) AK_create_table_parameter * parameters{
  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    int i = 0;
    AK_create_table_parameter parameters[ size ];
    AK_create_table_parameter* parametar;

    for (i = 0; i < size; i++) {
      parametar = (AK_create_table_parameter *) PyList_GetItem($input, i);
      memcpy(parameters + i, parametar, sizeof ( AK_create_table_parameter));
    }
    $1 = parameters;
  }
  else $1 = (AK_create_table_parameter*)$input;
}

/*
frees AK_create_table_parameter** datatype
*/
%typemap(freearg) AK_create_table_parameter ** {
  free($1);
}

/*
Everything in the %{ ... %} block is simply copied verbatim to the resulting wrapper file created by SWIG.
*/
%{
  #define SWIG_FILE_WITH_INIT


  #include "../debug.h"
  #include "../debug.c"

  #include "../constants.h"
  #include "../configuration.h"

  #include "../auxiliary.c"
  #include "../auxiliary.h"

  #include "../dbman.h"

  extern table_addresses *AK_get_segment_addresses(char * segmentName, int segmentType);

  #include "../dbman.c"

  #include "../memoman.c"
  #include "../memoman.h"

  #include "../table.c"
  #include "../table.h"

  #include "../index.h"

  extern element_ad Ak_Get_First_elementAd(list_ad *L);
  extern element_ad Ak_Get_Last_elementAd(list_ad *L);
  extern element_ad Ak_Get_Next_elementAd(element_ad Currentelement_op);
  extern void Ak_Insert_NewelementAd(int addBlock, int indexTd, char *attName, element_ad elementBefore);

  #include "../bitmap.c"

  #include "../hash.c"
  #include "../hash.h"

  #include "../btree.c"
  #include "../btree.h"
  #include "../index.c"

  #include "../filesort.c"
  #include "../filesort.h"
  #include "../filesearch.c"
  #include "../filesearch.h"
  #include "../fileio.c"
  #include "../fileio.h"
  #include "../files.c"
  #include "../files.h"

  #include "../theta_join.c"
  #include "../theta_join.h"
  #include "../product.c"
  #include "../product.h"
  #include "../aggregation.c"
  #include "../aggregation.h"
  #include "../union.c"
  #include "../union.h"
  #include "../selection.c"
  #include "../selection.h"
  #include "../drop.c"
  #include "../drop.h"
  #include "../view.c"
  #include "../view.h"

  #include "../rename.c"
  #include "../rename.h"
  #include "../projection.c"
  #include "../projection.h"

  #include "../nat_join.c"
  #include "../nat_join.h"
  #include "../intersect.c"
  #include "../intersect.h"
  #include "../difference.c"
  #include "../difference.h"

  extern AK_list *AK_rel_eq_assoc(AK_list *list_rel_eq);

  extern char *AK_rel_eq_get_atrributes_char(char *tblName);
  extern char *AK_rel_eq_cond_attributes(char *cond);
  extern int AK_rel_eq_share_attributes(char *set, char *subset);
  extern char *AK_rel_eq_commute_with_theta_join(char *cond, char *tblName);
  extern AK_list *AK_rel_eq_split_condition(char *cond) ;
  extern AK_list *AK_rel_eq_selection(AK_list *list_rel_eq);

  extern char *AK_rel_eq_projection_attributes(char *attribs, char *tblName);
  extern AK_list *AK_rel_eq_projection(AK_list *list_rel_eq);
  extern AK_list *AK_rel_eq_get_attributes(char *tblName);

  #include "../query_optimization.c"
  #include "../query_optimization.h"

  #include "../rel_eq_comut.c"
  #include "../rel_eq_comut.h"
  #include "../rel_eq_assoc.c"
  #include "../rel_eq_assoc.h"

  #include "../rel_eq_selection.c"
  #include "../rel_eq_selection.h"
  #include "../rel_eq_projection.c"
  #include "../rel_eq_projection.h"

  #include "../expression_check.c"
  #include "../expression_check.h"
  #include "../id.c"
  #include "../id.h"
  #include "../nnull.c"
  #include "../nnull.h"
  #include "../between.c"
  #include "../between.h"
  #include "../reference.c"
  #include "../reference.h"

  #include "../transaction.h"
  #include "../transaction.c"

  #include "../observable.c"
  #include "../observable.h"

  #include "../test.c"
  #include "../test.h"


  #include "../trigger.c"
  #include "../trigger.h"
  #include "../main.c"

  #include "../archive_log.c"
  #include "../archive_log.h"

  #include "../command.c"
  #include "../command.h"

  #include "../ini_parser/iniparser.c"
  #include "../ini_parser/iniparser.h"

  #include "../ini_parser/dictionary.c"
  #include "../ini_parser/dictionary.h"



  #include "../function.h"
  #include "../function.c"

  #include "../sequence.h"
  #include "../sequence.c"

  #include "../redo_log.h"
  #include "../redo_log.c"
%}


%include "../debug.h"
%include "../debug.c"

%include "../constants.h"
%include "../configuration.h"

%include "../auxiliary.c"
%include "../auxiliary.h"

%include "../dbman.h"

extern table_addresses *AK_get_segment_addresses(char * segmentName, int segmentType);

%include "../dbman.c"

%include "../memoman.c"
%include "../memoman.h"

%include "../table.c"
%include "../table.h"

%include "../index.h"

extern element_ad Ak_Get_First_elementAd(list_ad *L);
extern element_ad Ak_Get_Last_elementAd(list_ad *L);
extern element_ad Ak_Get_Next_elementAd(element_ad Currentelement_op);
extern void Ak_Insert_NewelementAd(int addBlock, int indexTd, char *attName, element_ad elementBefore);

%include "../bitmap.c"

%include "../hash.c"
%include "../hash.h"

%include "../btree.c"
%include "../btree.h"
%include "../index.c"

%include "../filesort.c"
%include "../filesort.h"
%include "../filesearch.c"
%include "../filesearch.h"
%include "../fileio.c"
%include "../fileio.h"
%include "../files.c"
%include "../files.h"

%include "../theta_join.c"
%include "../theta_join.h"
%include "../product.c"
%include "../product.h"
%include "../aggregation.c"
%include "../aggregation.h"
%include "../union.c"
%include "../union.h"
%include "../selection.c"
%include "../selection.h"
%include "../drop.c"
%include "../drop.h"
%include "../view.c"
%include "../view.h"

%include "../rename.c"
%include "../rename.h"
%include "../projection.c"
%include "../projection.h"

%include "../nat_join.c"
%include "../nat_join.h"
%include "../intersect.c"
%include "../intersect.h"
%include "../difference.c"
%include "../difference.h"

extern AK_list *AK_rel_eq_assoc(AK_list *list_rel_eq);

extern char *AK_rel_eq_get_atrributes_char(char *tblName);
extern char *AK_rel_eq_cond_attributes(char *cond);
extern int AK_rel_eq_share_attributes(char *set, char *subset);
extern char *AK_rel_eq_commute_with_theta_join(char *cond, char *tblName);
extern AK_list *AK_rel_eq_split_condition(char *cond) ;
extern AK_list *AK_rel_eq_selection(AK_list *list_rel_eq);

extern char *AK_rel_eq_projection_attributes(char *attribs, char *tblName);
extern AK_list *AK_rel_eq_projection(AK_list *list_rel_eq);
extern AK_list *AK_rel_eq_get_attributes(char *tblName);

%include "../query_optimization.c"
%include "../query_optimization.h"

%include "../rel_eq_comut.c"
%include "../rel_eq_comut.h"
%include "../rel_eq_assoc.c"
%include "../rel_eq_assoc.h"

%include "../rel_eq_selection.c"
%include "../rel_eq_selection.h"
%include "../rel_eq_projection.c"
%include "../rel_eq_projection.h"

%include "../expression_check.c"
%include "../expression_check.h"
%include "../id.c"
%include "../id.h"
%include "../nnull.c"
%include "../nnull.h"
%include "../between.c"
%include "../between.h"
%include "../reference.c"
%include "../reference.h"

%include "../transaction.h"
%include "../transaction.c"

%include "../observable.c"
%include "../observable.h"

%include "../test.c"
%include "../test.h"
%include "../trigger.c"
%include "../trigger.h"
%include "../main.c"

%include "../archive_log.c"
%include "../archive_log.h"

%include "../command.c"
%include "../command.h"

%include "../ini_parser/iniparser.c"
%include "../ini_parser/iniparser.h"

%include "../ini_parser/dictionary.c"
%include "../ini_parser/dictionary.h"

%include "../function.h"
%include "../function.c"

%include "../sequence.h"
%include "../sequence.c"

%include "../redo_log.h"
%include "../redo_log.c"
