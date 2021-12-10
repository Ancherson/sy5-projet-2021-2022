#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include "commandline.h"
#include "timing.h"

typedef struct task {
    uint32_t taskid;
    commandline cmd;
    timing time;
    int alive;
} task;

void create_task(task *t, uint32_t taskid, commandline c, timing time);
void free_task(task *t);
task *create_task_array(int len);
void free_task_array(task *t, int len);
task *add_task(task *t, int *len, uint32_t taskid, commandline c, timing time);
int remove_task(task *t, int len,uint32_t taskid);

#endif // TASK_H