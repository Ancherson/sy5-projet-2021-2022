#ifndef WRITEREQUEST_H
#define WRITEREQUEST_H

#include <unistd.h>
#include <stdint.h>

#include "convert-uint.h"

int write_opcode(int fd, uint16_t opcode);

int write_taskid(int fd, uint64_t taskid);

#endif // WRITEREQUEST_H