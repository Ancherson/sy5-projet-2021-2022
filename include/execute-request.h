#ifndef EXECUTE_REQUEST_H
#define EXECUTE_REQUEST_H
#include "task.h"
#include <sys/stat.h>
#include "write-request.h"
#include <fcntl.h>
#include <errno.h>

void create(task t);

#endif // EXECUTE_REQUEST_H