#include "write-request.h"

int write_opcode(int fd, uint16_t opcode){
    opcode = (opcode<<8) | (opcode>>8);
    return write(fd, &opcode, sizeof(uint16_t));
}
