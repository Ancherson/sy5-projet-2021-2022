#ifndef COMMANDLINE
#define COMMANDLINE

#include <stdint.h>
#include <endian.h>
#include "string2.h"

typedef struct string string;
typedef struct commandline {
  uint32_t argc;
  string *argv;
} commandline;

int create_commandline(commandline *dest, uint32_t argc, string *argv);
int write_commandline(char *buf, commandline str);
void alloc_commandline(commandline *dest, uint32_t argc, char **argv);
void free_commandline(commandline *c);

#endif // COMMANDLINE