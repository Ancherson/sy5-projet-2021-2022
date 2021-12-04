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
