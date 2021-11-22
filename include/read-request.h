#ifndef READ_REQUEST_H
#define READ_REQUEST_H

#include <stdio.h>
#include <unistd.h>

#include "timing.h"
#include "convert-uint.h"
#include "string2.h"

int read_timing(int fd, timing *t);
string *read_string(int fd);

#endif // READ_REQUEST_H