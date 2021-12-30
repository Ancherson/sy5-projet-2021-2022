#include "run.h"

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