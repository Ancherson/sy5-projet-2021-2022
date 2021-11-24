#include "read-request.h"

int read_taskid(int fd){
    uint64_t taskid;
    if(read(fd, &taskid, sizeof(uint64_t)) < sizeof(uint64_t)){
        perror("Erreur read_taskid");
        return 1;
    }
    taskid = reverse_byte64(taskid);
    printf("%lu", taskid);
    return 0;
}

int read_timing(int fd){
    timing t;
    if(read(fd, &t, TIMING_SIZE) < TIMING_SIZE){
        perror("Erreur read_timing");
        return 1;
    }
    t.minutes = reverse_byte64(t.minutes);
    t.hours = reverse_byte32(t.hours);
    char s[TIMING_TEXT_MIN_BUFFERSIZE];
    if(timing_string_from_timing(s, &t) == 1) {
        printf("Erreur read_timing timing_string_from_timing\n");
        return 1;
    }
    printf("%s", s);
    return 0;
}

int read_string(int fd){
    uint32_t len;
    if(read(fd, &len, sizeof(uint32_t)) < sizeof(uint32_t)){
        perror("Erreur read_string lecture de la taille de la string");
        return 1;
    }
    len = reverse_byte32(len);

    char str[len + 1];
    if(read(fd, str, len) < len) {
        perror("read_string string");
        return 1;
    }
    str[len] = '\0';

    printf("%s", str);
    return 0;
}

int read_commandline(int fd) {
    uint32_t argc;
    if(read(fd, &argc, sizeof(uint32_t)) < sizeof(uint32_t)){
        perror("Erreur read_commandline lecture du nombre d'argument");
        return 1;
    }
    argc = reverse_byte32(argc);
    
    for(unsigned int i = 0; i < argc; i++) {
        if(read_string(fd)) {
            printf("Erreur read_commandline read_string\n");
            return 1;
        }
        printf(" ");
    }

    return 0;
}