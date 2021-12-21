#include "saturnd.h"

void print_task(task *t) {
    printf("taskid = %lu\n", t->taskid);
    printf("commandline : ");
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

task *init_task(int *len, int *nb_task) {
    *len = 1;
    *nb_task = 0;
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
            int fd = open(path, O_RDONLY);
            if(fd == -1) {
                dprintf(2, "Error open %s\n", path);
                exit(EXIT_FAILURE);
            }

            commandline cmd = read_commandline(fd);
            timing time = read_timing(fd);
            uint64_t taskid = strtoull(entry->d_name, &strtoull_endp, 10);
            if (strtoull_endp == entry->d_name || strtoull_endp[0] != '\0') {
                dprintf(2, "Error get taskid %ld\n", taskid);
                exit(EXIT_FAILURE);
            }

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

void create_tmp() {
    char path[4096];
    memset(path, 0, 4096);
    snprintf(path, 4096, "/tmp/%s", getenv("USER"));
    if(mkdir(path, 0750) == -1 && errno != EEXIST) {
        dprintf(2, "Error mkdir %s\n", path);
        exit(EXIT_FAILURE);
    }
    strcat(path, "/saturnd");
    if(mkdir(path, 0750) == -1 && errno != EEXIST) {
        dprintf(2, "Error mkdir %s\n", path);
        exit(EXIT_FAILURE);
    }
    strcat(path, "/pipe");
    if(mkdir(path, 0750) == -1 && errno != EEXIST) {
        dprintf(2, "Error mkdir %s\n", path);
        exit(EXIT_FAILURE);
    }
}

int main(){
    create_tmp();

    char buf[BUFFER_SIZE];
    int nb_tasks;
    int len;

    task *t = init_task(&len, &nb_tasks);
    
    print_task_array(t, nb_tasks);

    int fd_request = open("run/pipes/saturnd-request-pipe", O_RDONLY|O_NONBLOCK|O_CREAT);
    if(fd_request == -1) {
        perror("open request");
        return EXIT_FAILURE;
    }
    int fd_gohst = open("run/pipes/saturnd-request-pipe", O_WRONLY);
    if(fd_gohst == -1) {
        perror("open request gohst");
        return EXIT_FAILURE;
    }

    int fd_reply_gohst = open("run/pipes/saturnd-reply-pipe", O_RDONLY | O_NONBLOCK | O_CREAT);
    if(fd_reply_gohst == -1) {
        perror("Error fd reply gohst");
        return EXIT_FAILURE;
    }
    int fd_reply = open("run/pipes/saturnd-reply-pipe", O_WRONLY);
    if(fd_reply == -1) {
        dprintf(2, "Error fd reply\n");
        return EXIT_FAILURE;
    }

    int nfds = fd_request+1;
    fd_set read_set;
    while(1){
        struct timeval timeV;

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
                    x += create(fd_request,buf,&t,&len,&nb_tasks);
                    break;

                case CLIENT_REQUEST_REMOVE_TASK :
                    x += remove_(fd_request, buf, t, len, &nb_tasks);
                    break;
        
                case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES :
                    return 0;
                    break;

                case CLIENT_REQUEST_TERMINATE :
                    return 0;
                    break;
                
                case CLIENT_REQUEST_GET_STDOUT :
                    return 0;
                    break;   
    
                case CLIENT_REQUEST_GET_STDERR :
                    return 0;
                    break;  
                
                default:
                    return 1;
                    break;
                
            }
            if(write(fd_reply,buf, x) < x) {
                perror("write reply");
                return EXIT_FAILURE;
            }
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
