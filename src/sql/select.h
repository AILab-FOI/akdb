#include "../file/table.h"
#include "../file/fileio.h"
#include "../rel/selection.h"
#include "../auxi/auxiliary.h"
#include "../auxi/mempro.h"

//int AK_select(char *srcTable,char *destTable,AK_list *atributi,AK_list *uvjet);
int AK_select(char *srcTable,char *destTable,list_node *atributi,list_node *uvjet);
void AK_select_test();
