#include "execute-request.h"


void create(task t) {
    if(mkdir("task", 0750) == -1 && errno != EEXIST) {
        perror("mkdir task");
        exit(1);
    }
    char buf[100];
    memset(buf, 0, 100);
    snprintf(buf, 100, "task/%lu", t.taskid);
    if(mkdir(buf, 0750) == -1) {
        dprintf(2, "Error mkdir %s\n", buf);
        exit(1);
    }

    snprintf(buf, 100,"task/%lu/data", t.taskid);
    int fd = open(buf, O_CREAT | O_TRUNC | O_WRONLY, 0750);

    if(fd == -1) {
        dprintf(2, "Error open %s\n", buf);
        exit(1);
    }
    char buf_write[BUFFER_SIZE];

    int n = write_commandline(buf_write, t.cmd);

    n += write_timing(buf_write + n, t.time);

    if(write(fd, buf_write, n) < n) {
        dprintf(2, "Error write %s\n", buf);
        exit(1);
    }
}

int list(char *buf, task *t, uint32_t nb_tasks){
    int n = 0;
    uint32_t nbtasks = htobe32(nb_tasks);
    *((uint32_t*)buf) = nbtasks;
    n += sizeof(uint32_t);

    for(int i = 0; i < nb_tasks; i++){
        n += write_taskid(buf+n, t[i].taskid);
        n += write_timing(buf+n, t[i].time);
        n += write_commandline(buf+n, t[i].cmd);
    }
    return n;
}