/* File: kalashnikovDB.i */
%module kalashnikovDB


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

%typemap(freearg) (int attr_num, int* attr_type) {
   if ($2) free($2);
}

%typemap(out) unsigned char * {
  $1 = PyString_FromString((char *)$input);
}

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


%typemap(in) AK_block * block{

  AK_block *bl = malloc(sizeof(AK_block));
  memcpy(bl, $input, sizeof(AK_block));
  $1 = bl;
  //$1 = (AK_block*)$input;
}

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
%typemap(freearg) char ** {
  free((char *) $1);
}

//AK_create_table_parameters
%typemap(out) AK_create_table_parameter * {
  AK_create_table_parameter* parametar = malloc(sizeof(AK_create_table_parameter));
  memcpy(parametar, $1, sizeof(AK_create_table_parameter));
  $result = parametar;
  //$result = $1;
}

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

%typemap(freearg) AK_create_table_parameter ** {
  free($1);
}

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

#include "../transaction.h"
#include "../transaction.c"

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

%include "../transaction.h"
%include "../transaction.c"

%include "../function.h"
%include "../function.c"

%include "../sequence.h"
%include "../sequence.c"

%include "../redo_log.h"
%include "../redo_log.c"

