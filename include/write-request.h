#ifndef WRITEREQUEST_H
#define WRITEREQUEST_H

#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "convert-uint.h"
#include "timing-text-io.h"
#include "commandline.h"

int write_opcode(int fd, uint16_t opcode);
int write_timing(int fd, char * minutes_str, char * hours_str, char * daysofweek_str);
int write_taskid(int fd, uint64_t taskid);
int write_create(int fd, uint16_t opcode, char * minutes_str, char * hours_str, char * daysofweek_str, int argc, char **argv);

#endif // WRITEREQUEST_H