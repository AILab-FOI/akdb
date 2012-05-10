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
