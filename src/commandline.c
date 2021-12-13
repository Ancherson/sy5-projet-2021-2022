#include "commandline.h"

int create_commandline(struct commandline *dest, uint32_t argc, string *argv){
    dest->argc = argc;
    dest->argv = argv;
    return 0;
}

int write_commandline(char * buf, commandline cmd) {
    *((uint32_t *) buf) = htobe32(cmd.argc);
    int n = sizeof(uint32_t);
    for(unsigned int i = 0; i < cmd.argc; i++) {
        n += write_string(buf+n, cmd.argv[i]);
    }
    return n;
}

void alloc_commandline_incomplete(commandline *dest, uint32_t argc) {
    dest->argc = argc;
    dest->argv = malloc(argc * (sizeof(string)));
    if(dest->argv == NULL) {
        perror("error malloc alloc_commandline");
        exit(1);
    }
}

void alloc_commandline(commandline *dest, uint32_t argc, char **argv) {
    alloc_commandline_incomplete(dest, argc);
    for(int i = 0; i < argc; i++) {
        alloc_string(dest->argv + i, strlen(argv[i]), argv[i]);
    }
}

void free_commandline(commandline *c) {
    for(int i = 0; i < c->argc; i++) {
        free_string(c->argv + i);
    }
    free(c->argv);
}
