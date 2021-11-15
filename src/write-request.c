#include "write-request.h"

int write_opcode(int fd, uint16_t opcode){
    opcode = reverse_byte16(opcode);
    return write(fd, &opcode, sizeof(uint16_t));
}

int write_taskid(int fd, uint64_t taskid){
    taskid = reverse_byte64(taskid);
    return write(fd, &taskid, sizeof(uint64_t));
}
