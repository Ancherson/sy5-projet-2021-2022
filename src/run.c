#include "run.h"

task *init_task(int *len, int *nb_task, uint64_t *max_id) {
    *len = 1;
    *nb_task = 0;
    *max_id = 0;
    task *t = create_task_array(*len);

    char *dirname = "task";
    char path[1024];

    char * strtoull_endp;

    DIR *dirp = opendir(dirname);
    if(dirp == NULL) return t;
    struct dirent *entry;
    while ((entry = readdir(dirp))) {
        if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            memset(path, 0, 1024);
            snprintf(path, 1024, "%s/%s/data", dirname, entry->d_name);
            //printf("%s\n", path);
            uint64_t taskid = strtoull(entry->d_name, &strtoull_endp, 10);
            if (strtoull_endp == entry->d_name || strtoull_endp[0] != '\0') {
                dprintf(2, "Error get taskid %ld\n", taskid);
                exit(EXIT_FAILURE);
            }
            if(taskid >= *max_id) *max_id = taskid + 1;
            int fd = open(path, O_RDONLY);
            if(fd == -1 && errno == ENOENT) {
                continue;    
            }
            if(fd == -1) {
                printf("%d\n", errno);
                dprintf(2, "Error open %s\n", path);
                exit(EXIT_FAILURE);
            }

            commandline cmd = read_commandline(fd);
            timing time = read_timing(fd);

            t = add_task(t, len, nb_task, taskid, cmd, time);

            if(close(fd) == -1) {
                dprintf(2, "Error close %s\n", path);
                exit(EXIT_FAILURE);
            }
        }
    }
    closedir(dirp);

    return t;
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

            if(execvp(arg[0], arg) == -1) exit(0xFFFF);
            
        } else {
            //Pere
            int wstatus;
            wait(&wstatus);
            uint16_t exit_code;
            if(WIFEXITED(wstatus)) exit_code = WEXITSTATUS(wstatus);
            else exit_code = 0xFFFF;
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

void launch_executable_tasks(task *t, int nb_tasks){
    time_t current_time;
    struct tm ts;
    time(&current_time);
    ts = *localtime(&current_time);
    timing current_timing;
    current_timing.minutes = 1 << ts.tm_min;
    current_timing.hours = 1 << ts.tm_hour;
    current_timing.daysofweek = 1 << ts.tm_wday;
    for(int i = 0; i < nb_tasks; i++){
        if((current_timing.minutes & t[i].time.minutes) && (current_timing.hours & t[i].time.hours) && (current_timing.daysofweek & t[i].time.daysofweek)){
            //launch execute_task
            execute_task(t[i]);
        }
    }
}