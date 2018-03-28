#ifndef COMMAND
#define COMMAND

#include "../auxi/test.h"
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
TestResult AK_test_command();

#endif

