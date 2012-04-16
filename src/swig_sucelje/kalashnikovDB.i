/* File: kalashnikovDB.i */
%module kalashnikovDB

%{
#define SWIG_FILE_WITH_INIT

extern char * AK_config_get(char * key, char * def);

#include "../debug.h"
#include "../debug.c"

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


#include "../test.c"
#include "../test.h"
#include "../trigger.c"
#include "../trigger.h"
#include "../main.c"

#include "../archive_log.c"
#include "../archive_log.h"

#include "../command.c"
#include "../command.h"


#include "../iniparser/iniparser.c"
#include "../iniparser/iniparser.h"

#include "../iniparser/dictionary.c"
#include "../iniparser/dictionary.h"

%}

extern char * AK_config_get(char * key, char * def);


%include "../debug.h"
%include "../debug.c"

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


%include "../test.c"
%include "../test.h"
%include "../trigger.c"
%include "../trigger.h"
%include "../main.c"

%include "../archive_log.c"
%include "../archive_log.h"

%include "../command.c"
%include "../command.h"

%include "../iniparser/iniparser.c"
%include "../iniparser/iniparser.h"

%include "../iniparser/dictionary.c"
%include "../iniparser/dictionary.h"