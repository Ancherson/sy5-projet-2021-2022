#ifndef WRITEREQUEST_H
#define WRITEREQUEST_H
#define BUFFER_SIZE 32768


#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include<endian.h>

#include "timing-text-io.h"
#include "commandline.h"

int write_opcode(char * buf, uint16_t opcode);
int write_timing_from_strings(char * buf, char * minutes_str, char * hours_str, char * daysofweek_str);
int write_timing(char * buf, timing t);
int write_taskid(char * buf, uint64_t taskid);
int write_create(char * buf, char * minutes_str, char * hours_str, char * daysofweek_str, int argc, char **argv);

#endif // WRITEREQUEST_H