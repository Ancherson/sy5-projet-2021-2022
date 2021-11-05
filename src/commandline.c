#include "commandline.h"

int create_commandline(struct commandline *dest, uint32_t argc, string *argv){
    dest->argc = argc;
    dest->argv = argv;
    return 0;
}