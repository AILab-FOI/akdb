#ifndef COMMAND
#define COMMAND

#include "../file/table.h"
#include "../file/fileio.h"
#include "../rel/selection.h"
#include "../auxi/mempro.h"

struct AK_command_struct {
  int id_command;
  char *tblName;
  void *parameters;
};

typedef struct AK_command_struct command;

int AK_command(command * komande, int brojkomandi);
void AK_test_command();

#endif

