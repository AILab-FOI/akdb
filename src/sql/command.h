#ifndef COMMAND
#define COMMAND

#include "../file/table.h"
#include "../file/fileio.h"
#include "../rel/selection.h"

struct AK_command {
  int id_command;
  char *tblName;
  void *parameters;
};

typedef struct AK_command command;
#endif

int AK_command(command * komande, int brojkomandi);
void AK_test_command();

