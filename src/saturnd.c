#include "saturnd.h"

void print_task(task *t) {
    printf("ID : %lu, ", t->taskid);
    printf("CMD : ");
    for(int i = 0; i < t->cmd.argc; i++) {
        printf("%s ", t->cmd.argv[i].str);
    }
    printf("\n");
}

void print_task_array(task *t, int nb_tasks) {
    for(int i = 0; i < nb_tasks; i++) {
        print_task(t + i);
    }
}

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
            printf("%s\n", path);
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
            printf("It is time for task %lu to be executed\n", t[i].taskid);
        }
    }
}

int main(int argc, char **argv){
    char *pipes_directory = NULL;
    if(argc < 2) create_tmp();
    else pipes_directory = argv[1];

    char *pipe_request_file = NULL;
    char *pipe_reply_file = NULL;
    if(get_pipes_file(pipes_directory, &pipe_request_file, &pipe_reply_file)) {
        printf("Erreur construction chaine de caractere des fichiers pipes\n");
        exit(EXIT_FAILURE);
    }

    char buf[BUFFER_SIZE];
    int nb_tasks;
    int len;
    uint64_t max_id;

    task *t = init_task(&len, &nb_tasks, &max_id);
    launch_executable_tasks(t, nb_tasks);

    create_pipes(pipe_request_file, pipe_reply_file);

    int fd_request = open(pipe_request_file, O_RDONLY|O_NONBLOCK);
    if(fd_request == -1) {
        perror("open request");
        return EXIT_FAILURE;
    }
    int fd_gohst = open(pipe_request_file, O_WRONLY);
    if(fd_gohst == -1) {
        perror("open request gohst");
        return EXIT_FAILURE;
    }

    int fd_reply_gohst = open(pipe_reply_file, O_RDONLY | O_NONBLOCK);
    if(fd_reply_gohst == -1) {
        perror("Error fd reply gohst");
        return EXIT_FAILURE;
    }
    int fd_reply = open(pipe_reply_file, O_WRONLY);
    if(fd_reply == -1) {
        dprintf(2, "Error fd reply\n");
        return EXIT_FAILURE;
    }

    free(pipe_request_file);
    free(pipe_reply_file);

    int nfds = fd_request+1;
    fd_set read_set;
    struct timeval timeV;
    while(1){

        timeV.tv_sec = 10;
        timeV.tv_usec = 0;

        FD_ZERO(&read_set);
        FD_SET(fd_request,&read_set);

        int cond = select(nfds,&read_set,NULL,NULL,&timeV);
        if (cond == 0){
            printf("J'ai rien lu \n");
        }
        if (cond == -1) {
            perror("PB select saturnd");
            return 1;
        }
        if(FD_ISSET(fd_request, &read_set)){
            uint16_t op_code= read_uint16(fd_request);
            //TODO A ne plus hardcoder
            if(fd_reply == -1) {
                perror("open reply");
                return EXIT_FAILURE;
            }
            int x = 0;
            switch (op_code){
                case CLIENT_REQUEST_LIST_TASKS :
                    x += write_opcode(buf, SERVER_REPLY_OK);
                    x += list(buf + x, t, nb_tasks);
                    break;
                
                case CLIENT_REQUEST_CREATE_TASK :            
                    x += create(fd_request,buf,&t,&len,&nb_tasks,&max_id);
                    break;

                case CLIENT_REQUEST_REMOVE_TASK :
                    x += remove_(fd_request, buf, t, len, &nb_tasks);
                    break;
        
                case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES :
                    x += times_exitcodes(fd_request, buf, t, nb_tasks, max_id);
                    break;

                case CLIENT_REQUEST_TERMINATE :
                    return 0;
                    break;
                
                case CLIENT_REQUEST_GET_STDOUT :
                    x += stdout_stderr(fd_request, buf, t, nb_tasks, CLIENT_REQUEST_GET_STDOUT, max_id);
                    break;
    
                case CLIENT_REQUEST_GET_STDERR :
                    x += stdout_stderr(fd_request, buf, t, nb_tasks, CLIENT_REQUEST_GET_STDERR, max_id);
                    break;
                
                default:
                    return 1;
                    break;
                
            }
            if(write(fd_reply,buf, x) < x) {
                perror("write reply");
                return EXIT_FAILURE;
            }
        } else {
            launch_executable_tasks(t, nb_tasks);
        }
    }

    free_task_array(t, &nb_tasks);
    if(close(fd_reply) == -1) {
        perror("close reply");
        return EXIT_FAILURE;
    }
    if(close(fd_reply_gohst) == -1) {
        perror("close reply gohst");
        return EXIT_FAILURE;
    }
    if(close(fd_request) == -1) {
        perror("close request");
        return EXIT_FAILURE;
    }
    if(close(fd_gohst) == -1) {
        perror("close gohst");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
