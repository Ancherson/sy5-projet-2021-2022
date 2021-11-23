#ifndef READ_REQUEST_H
#define READ_REQUEST_H

#include <stdio.h>
#include <unistd.h>

#include "timing.h"
#include "timing-text-io.h"
#include "convert-uint.h"
#include "string2.h"
#include "commandline.h"

int read_taskid(int fd);
int read_timing(int fd);
int read_string(int fd);
int read_commandline(int fd);

#endif // READ_REQUEST_H