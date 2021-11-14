#ifndef WRITEREQUEST_H
#define WRITEREQUEST_H

#include <unistd.h>
#include <stdint.h>

#include "convert-uint.h"
#include "timing-text-io.h"

int write_opcode(int fd, uint16_t opcode);

int write_timing(int fd, char * minutes_str, char * hours_str, char * daysofweek_str);

#endif // WRITEREQUEST_H