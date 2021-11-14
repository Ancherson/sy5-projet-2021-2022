#include "write-request.h"

int write_opcode(int fd, uint16_t opcode){
    opcode = reverse_byte16(opcode);
    return write(fd, &opcode, sizeof(uint16_t));
}
