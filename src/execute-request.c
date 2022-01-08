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


void create(int fd, int fd_reply, task **pt, int *len, int *nb_task, uint64_t *max_id) {
    timing time = read_timing(fd);
    commandline c = read_commandline(fd);
    uint64_t taskid = *max_id;
    *max_id += 1;

    *pt = add_task(*pt, len, nb_task,taskid, c, time);
    do_create((*pt)[*nb_task - 1]);

    int buf_len = sizeof(uint16_t)+sizeof(uint64_t);
    char buf[buf_len];
    int n = write_opcode(buf, SERVER_REPLY_OK);
    write_taskid(buf + n, taskid);
    write_pipebuf(fd_reply, buf, buf_len);
}

void list(int fd_reply, task *t, uint32_t nb_tasks){
    int len = sizeof(uint16_t)+sizeof(uint32_t)+nb_tasks*(sizeof(uint64_t)+TIMING_SIZE+sizeof(uint32_t));
    for(int i = 0; i < nb_tasks; i++){
        len += t[i].cmd.argc*sizeof(uint32_t);
        for(int j = 0; j < t[i].cmd.argc; j++){
            len += t[i].cmd.argv[j].len;
        }
    }
    char buf[len];
    int n = 0;
    n += write_opcode(buf, SERVER_REPLY_OK);
    *((uint32_t*)(buf+n)) = htobe32(nb_tasks);
    n += sizeof(uint32_t);
    for(int i = 0; i < nb_tasks; i++){
        n += write_taskid(buf+n, t[i].taskid);
        n += write_timing(buf+n, t[i].time);
        n += write_commandline(buf+n, t[i].cmd);
    }
    write_pipebuf(fd_reply, buf, len);
}

void do_remove(task t) {
    char path[100];
    memset(path, 0, 100);
    snprintf(path, 100, "task/%lu/data", t.taskid);

    if(unlink(path) == -1) {
        dprintf(2, "Error unlink %s\n", path);
        exit(EXIT_FAILURE);
    }
}

void remove_(int fd, int fd_reply, task *t, int len, int *nb_task) {
    int n = 0;
    int buf_len;
    uint64_t taskid = read_taskid(fd);
    int index = get_index(t, *nb_task, taskid);
    if(index == -1) {
        buf_len = 2*sizeof(uint16_t);
        char buf[buf_len];
        *((uint16_t*)buf) = htobe16(SERVER_REPLY_ERROR);
        n += sizeof(uint16_t);
        *((uint16_t*)(buf+n)) = htobe16(SERVER_REPLY_ERROR_NOT_FOUND);
        write_pipebuf(fd_reply, buf, buf_len);
        return;
    }

    do_remove(t[index]);
    remove_task(t, nb_task, taskid);

    buf_len = sizeof(uint16_t);
    char buf[buf_len];
    write_opcode(buf, SERVER_REPLY_OK);
    write_pipebuf(fd_reply, buf, buf_len);
}

void times_exitcodes(int fd, int fd_reply, task *t, int nb_tasks, uint64_t max_id){
    int len;
    int n = 0;
    uint64_t taskid = read_taskid(fd);
    if(taskid >= max_id){
        len = 2*sizeof(uint16_t);
        char buf[len];
        *((uint16_t*)buf) = htobe16(SERVER_REPLY_ERROR);
        n += sizeof(uint16_t);
        *((uint16_t*)(buf+n)) = htobe16(SERVER_REPLY_ERROR_NOT_FOUND);
        write_pipebuf(fd_reply, buf, len);
        return;
    }

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

    len = sizeof(uint16_t) + sizeof(uint32_t) + nb_runs*(sizeof(int64_t)+sizeof(uint16_t));
    char buf[len];

    n += write_opcode(buf, SERVER_REPLY_OK);
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
    write_pipebuf(fd_reply, buf, len);
}

void terminate(int fd_reply, int* running){
    *running = 0;
    char buf[sizeof(uint16_t)];
    write_opcode(buf,SERVER_REPLY_OK);
    write_pipebuf(fd_reply,buf,sizeof(uint16_t));
}

void stdout_stderr(int fd, int fd_reply, task *t, int nb_tasks, uint16_t opcode, uint64_t max_id){
    int len;
    int n = 0;
    uint64_t taskid = read_taskid(fd);
    if(taskid >= max_id){
        len = 2*sizeof(uint16_t);
        char buf[len];
        *((uint16_t*)buf) = htobe16(SERVER_REPLY_ERROR);
        n += sizeof(uint16_t);
        *((uint16_t*)(buf+n)) = htobe16(SERVER_REPLY_ERROR_NOT_FOUND);
        write_pipebuf(fd_reply, buf, len);
        return;
    }

    char path_buf[100];
    if(opcode == CLIENT_REQUEST_GET_STDOUT){
        snprintf(path_buf, 100,"task/%lu/stdout", taskid);
    } else {
        snprintf(path_buf, 100,"task/%lu/stderr", taskid);
    }
    int task_fd = open(path_buf, O_RDONLY);
    if(task_fd == -1){
        if(errno == ENOENT){
            len = 2*sizeof(uint16_t);
            char buf[len];
            *((uint16_t*)buf) = htobe16(SERVER_REPLY_ERROR);
            n += sizeof(uint16_t);
            *((uint16_t*)(buf+n)) = htobe16(SERVER_REPLY_ERROR_NEVER_RUN);
            write_pipebuf(fd_reply, buf, len);
            return;
        } else {
            perror("Erreur open dans stdout_stderr");
            exit(EXIT_FAILURE);
        }
    }

    int l = lseek(task_fd, 0, SEEK_END);
    lseek(task_fd, 0, SEEK_SET);
    if(l == -1){
        perror("Erreur lseek dans stdout_stderr");
        exit(EXIT_FAILURE);
    }
    string s;
    s.len = l;
    char b[l];
    s.str = b;
    int r = read(task_fd, b, l);
    if(r == -1){
        perror("Erreur read dans stdout_stderr");
        exit(EXIT_FAILURE);
    }
    
    len = sizeof(uint16_t) + sizeof(uint32_t) + l;
    char buf[len];

    n += write_opcode(buf, SERVER_REPLY_OK);
    write_string(buf+n, s);
    write_pipebuf(fd_reply, buf, len);
}
