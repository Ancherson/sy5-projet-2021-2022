#ifndef COMMANDLINE
#define COMMANDLINE

#include <stdint.h>
#include <endian.h>
#include "string2.h"

/* Structure représentant une ligne commande 
 * argc -> nombre d'arguments 
 * argv -> la ligne de commande
 */
typedef struct commandline {
  uint32_t argc;
  string *argv;
} commandline;

/* Ensemble de fonctions permettant de manipuler la structure string */
int create_commandline(commandline *dest, uint32_t argc, string *argv);
int write_commandline(char *buf, commandline str);
void alloc_commandline_incomplete(commandline *dest, uint32_t argc);
void alloc_commandline(commandline *dest, uint32_t argc, char **argv);
void free_commandline(commandline *c);
char **get_arg(char **tab, commandline cmd);

#endif // COMMANDLINE