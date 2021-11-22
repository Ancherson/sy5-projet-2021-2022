#ifndef READ_REQUEST_H
#define READ_REQUEST_H

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "timing.h"
#include "convert-uint.h"

int read_timing(int fd, timing *t);

#endif // READ_REQUEST_H