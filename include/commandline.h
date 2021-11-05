#ifndef COMMANDLINE
#define COMMANDLINE

#include <stdint.h>

#include "string.h"

struct commandline {
  uint32_t argc;
  string *argv;
};

int create_commandline(struct commandline *dest, uint32_t argc, string *argv);

#endif // COMMANDLINE