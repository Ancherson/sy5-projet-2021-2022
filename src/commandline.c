#include "commandline.h"

int create_commandline(struct commandline *dest, uint32_t argc, string *argv){
    dest->argc = argc;
    dest->argv = argv;
    return 0;
}

int write_commandline(int fd, commandline cmd) {
    uint32_t argc = reverse_byte32(cmd.argc);
    if(write(fd, &argc, sizeof(uint32_t)) < sizeof(uint32_t)) return 1;
    for(unsigned int i = 0; i < cmd.argc; i++) {
        if(write_string(fd, cmd.argv[i])) return 1;
    }
    return 0;
}
