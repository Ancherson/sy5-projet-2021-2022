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


int create(int fd, char *buf, task **pt, int *len, int *nb_task, uint64_t *max_id) {
    timing time = read_timing(fd);
    commandline c = read_commandline(fd);
    uint64_t taskid = *max_id;
    *max_id += 1;

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

void do_remove(task t) {
    char path[100];
    memset(path, 0, 100);
    snprintf(path, 100, "task/%lu/data", t.taskid);

    if(unlink(path) == -1) {
        dprintf(2, "Error unlink %s\n", path);
        exit(EXIT_FAILURE);
    }
}

int remove_(int fd, char *buf, task *t, int len, int *nb_task) {
    int n = 0;
    uint64_t taskid = read_taskid(fd);
    int index = get_index(t, *nb_task, taskid);
    if(index == -1) {
        n += write_opcode(buf + n, SERVER_REPLY_ERROR);
        n += write_opcode(buf + n, SERVER_REPLY_ERROR_NOT_FOUND);
        return n;
    }

    do_remove(t[index]);
    remove_task(t, nb_task, taskid);

    n += write_opcode(buf + n, SERVER_REPLY_OK);
    return n;
}

int times_exitcodes(int fd, char *buf, task *t, int nb_tasks, uint64_t max_id){
    int n = 0;
    uint64_t taskid = read_taskid(fd);
    if(taskid >= max_id){
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

int stdout_stderr(int fd, char *buf, task *t, int nb_tasks, uint16_t opcode, uint64_t max_id){
    int n = 0;
    uint64_t taskid = read_taskid(fd);
    if(taskid >= max_id){
        *((uint16_t*)buf) = htobe16(SERVER_REPLY_ERROR);
        n += sizeof(uint16_t);
        *((uint16_t*)(buf+n)) = htobe16(SERVER_REPLY_ERROR_NOT_FOUND);
        return n +sizeof(uint16_t);
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
            *((uint16_t*)buf) = htobe16(SERVER_REPLY_ERROR);
            n += sizeof(uint16_t);
            *((uint16_t*)(buf+n)) = htobe16(SERVER_REPLY_ERROR_NEVER_RUN);
            return n +sizeof(uint16_t);
        } else {
            perror("Erreur open dans stdout_stderr");
            exit(EXIT_FAILURE);
        }
    }

    *((uint16_t*)buf) = htobe16(SERVER_REPLY_OK);
    n += sizeof(uint16_t);

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
    n += write_string(buf+n, s);
    return n;
}

void execute_task(task t) {

    int rep = fork();

    if(rep == -1) {
        perror("fork execute task 1");
        exit(EXIT_FAILURE);
    }

    if(rep == 0) {
        char path[100];

        int r = fork();
        if(r == -1) {
            perror("fork execute task 2");
            exit(EXIT_FAILURE);
        }

        if(r == 0) {
            //Fils
            memset(path, 0, 100);
            snprintf(path, 100,"task/%lu/stdout", t.taskid);
            int fd_out = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0750);
            if(fd_out == -1) {
                dprintf(2, "Error execute_task, open %s\n", path);
                exit(EXIT_FAILURE);
            }

            memset(path, 0, 100);
            snprintf(path, 100,"task/%lu/stderr", t.taskid);
            int fd_err = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0750);
            if(fd_err == -1) {
                dprintf(2, "Error execute_task, open %s\n", path);
                exit(EXIT_FAILURE);
            }

            if(dup2(fd_out, 1) == -1) {
                perror("redirection fd_out");
                exit(EXIT_FAILURE);
            } 
            if(close(fd_out) == -1) {
                perror("close fd_out");
                exit(EXIT_FAILURE);
            }
            if(dup2(fd_err, 2) == -1) {
                perror("redirection fd_err");
                exit(EXIT_FAILURE);
            } 
            if(close(fd_err) == -1) {
                perror("close fd_err");
                exit(EXIT_FAILURE);
            }
            char *arg[t.cmd.argc + 1];
            get_arg(arg, t.cmd);
            arg[t.cmd.argc] = NULL;

            if(execvp(arg[0], arg) == -1) exit(EXIT_FAILURE);
            
        } else {
            //Pere
            int wstatus;
            wait(&wstatus);

            uint16_t exit_code = WEXITSTATUS(wstatus);
            time_t end_time;
            time(&end_time);

            memset(path, 0, 100);
            snprintf(path, 100,"task/%lu/times_exitcodes", t.taskid);
            int fd_time_exit = open(path, O_RDWR, 0750);

            if(fd_time_exit == -1) {
                dprintf(2, "Error open fd_time_exit : %s\n", path);
                exit(EXIT_FAILURE);
            }

            uint32_t nbruns;
            if(read(fd_time_exit, &nbruns, sizeof(uint32_t)) != sizeof(uint32_t)) {
                dprintf(2, "Error execute_task read nbruns !\n");
                exit(EXIT_FAILURE);
            }
            nbruns += 1;

            lseek(fd_time_exit, 0, SEEK_SET);
            if(write(fd_time_exit, &nbruns, sizeof(uint32_t)) != sizeof(uint32_t)) {
                dprintf(2, "Error execute_task write nbruns !\n");
                exit(EXIT_FAILURE);
            }

            lseek(fd_time_exit, 0, SEEK_END);
            if(write(fd_time_exit, &end_time, sizeof(uint64_t)) != sizeof(uint64_t)) {
                dprintf(2, "Error execute_task write time !\n");
                exit(EXIT_FAILURE);
            }

            if(write(fd_time_exit, &exit_code, sizeof(uint16_t)) != sizeof(uint16_t)) {
                dprintf(2, "Error execute_task write exitcode !\n");
                exit(EXIT_FAILURE);
            }
            if(close(fd_time_exit) == -1) {
                dprintf(2, "Error execute_task close !\n");
                exit(EXIT_FAILURE);
            }

        }
        exit(EXIT_SUCCESS);
    }
}