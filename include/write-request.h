#ifndef WRITEREQUEST_H
#define WRITEREQUEST_H

#include <unistd.h>
#include <stdint.h>

#include "convert-uint.h"

int write_opcode(int fd, uint16_t opcode);

#endif // WRITEREQUEST_H