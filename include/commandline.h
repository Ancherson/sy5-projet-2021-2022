#ifndef COMMANDLINE
#define COMMANDLINE

#include <stdint.h>

#include "string2.h"
typedef struct string string;
typedef struct commandline {
  uint32_t argc;
  string *argv;
} commandline;

int create_commandline(commandline *dest, uint32_t argc, string *argv);

#endif // COMMANDLINE