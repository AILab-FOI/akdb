#ifndef COMMAND
#define COMMAND

#include "table.h"
struct AK_command {
  int id_command;
  char *tblName;
  void *parameters;
};

typedef struct AK_command command;
#endif

void AK_command(command * komande, int brojkomandi);
void AK_test_command();


