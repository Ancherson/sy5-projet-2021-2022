#include "execute-request.h"


void do_create(task t) {
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

    snprintf(buf, 100, "task/%lu/data", t.taskid);
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

    memset(buf, 0, 100);
    snprintf(buf, 100, "task/%lu/times_exitcodes", t.taskid);
    fd = open(buf, O_CREAT | O_TRUNC | O_WRONLY, 0750);
    if(fd == -1) {
        dprintf(2, "Error open %s\n", buf);
        exit(1);
    }
    uint32_t nb_runs = 0;
    if(write(fd, &nb_runs, sizeof(uint32_t)) < sizeof(uint32_t)) {
        dprintf(2, "Error write %s\n", buf);
        exit(1);
    }
}



uint64_t gen_taskid(task *t, int nb_tasks) {
    uint64_t taskid = 0;
    while(task_exist(t,nb_tasks,taskid)) {
        taskid++;
    }
    return taskid;
}

int create(int fd, char *buf, task **pt, int *len, int *nb_task) {
    timing time = read_timing(fd);
    commandline c = read_commandline(fd);
    uint64_t taskid = gen_taskid(*pt, *nb_task);

    *pt = add_task(*pt, len, nb_task,taskid, c, time);
    do_create((*pt)[*nb_task - 1]);

    int n = write_opcode(buf, SERVER_REPLY_OK);
    n += write_taskid(buf + n, taskid);
    return n;
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

int times_exitcodes(int fd, char *buf, task *t, int len){
    int n = 0;
    uint64_t taskid = read_taskid(fd);
    int i;
    for(i = 0; i < len; i++){
        if(t[i].taskid == taskid) break;
    }
    if(i == len){
        *((uint16_t*)buf) = htobe16(SERVER_REPLY_ERROR);
        n += sizeof(uint16_t);
        *((uint16_t*)(buf+n)) = htobe16(SERVER_REPLY_ERROR_NOT_FOUND);
        return n +sizeof(uint16_t);
    }
    *((uint16_t*)buf) = htobe16(SERVER_REPLY_OK);
    n += sizeof(uint16_t);

    char path_buf[100];
    snprintf(path_buf, 100,"task/%lu/times_exitcodes", taskid);
    int task_fd = open(path_buf, O_RDONLY);
    if(task_fd == -1){
        perror("Erreur open dans times_exitcodes");
        exit(EXIT_FAILURE);
    }

    uint32_t nb_runs;
    if(read(task_fd, &nb_runs, sizeof(uint32_t)) == -1){
        perror("Erreur read nb_runs dans times_exitcodes");
        exit(EXIT_FAILURE);
    }
    uint32_t be_nb_runs = htobe32(nb_runs);
    *((uint32_t*)(buf+n)) = be_nb_runs;
    n += sizeof(uint32_t);
    read(task_fd, buf+n, nb_runs*(sizeof(int64_t)+sizeof(uint16_t)));
    for(int i = 0; i < nb_runs; i++){
        *((int64_t*)(buf+n)) = htobe64(*((int64_t*)(buf+n)));
        n += sizeof(int64_t);
        *((uint16_t*)(buf+n)) = htobe16(*((uint16_t*)(buf+n)));
        n += sizeof(uint16_t);
    }
    return n;
}