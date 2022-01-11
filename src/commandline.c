#include "commandline.h"

/* Initialise une commandline dest avec argc comme nombre d'arguments et argv comme tableau d'arguments */
int create_commandline(struct commandline *dest, uint32_t argc, string *argv){
    dest->argc = argc;
    dest->argv = argv;
    return 0;
}

/* Ecrit la commandline cmd dans le buffer buf (avec l'attribut argc converti en big endian)
 * Retourne le nombre d'octets écrits */ 
int write_commandline(char * buf, commandline cmd) {
    *((uint32_t *) buf) = htobe32(cmd.argc);
    int n = sizeof(uint32_t);
    for(unsigned int i = 0; i < cmd.argc; i++) {
        n += write_string(buf+n, cmd.argv[i]);
    }
    return n;
}

/* Permet l'alloction en mémoire d'une commandline
   n'alloue que le tableau d'argument, pas les éléments du tableau */
void alloc_commandline_incomplete(commandline *dest, uint32_t argc) {
    dest->argc = argc;
    dest->argv = malloc(argc * (sizeof(string)));
    if(dest->argv == NULL) {
        perror("error malloc alloc_commandline");
        exit(1);
    }
}

/* Permet l'allocation totale d'une commandline, en allouant à la fois le tableau d'arguments
   et à la fois chacun des éléments du tableau d'arguments */
void alloc_commandline(commandline *dest, uint32_t argc, char **argv) {
    alloc_commandline_incomplete(dest, argc);
    for(int i = 0; i < argc; i++) {
        alloc_string(dest->argv + i, strlen(argv[i]), argv[i]);
    }
}

/* Permet la libération de mémoire d'une commandline */
void free_commandline(commandline *c) {
    for(int i = 0; i < c->argc; i++) {
        free_string(c->argv + i);
    }
    free(c->argv);
}

/* Convertit une commandline en un char** (utile pour execvp) */
char **get_arg(char **tab, commandline cmd) {
    for(uint32_t i = 0; i < cmd.argc; i++) {
        tab[i] = cmd.argv[i].str;
    }
    return tab;
}