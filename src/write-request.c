#include "write-request.h"


int write_opcode(int fd, uint16_t opcode){
    opcode = reverse_byte16(opcode);
    if (write(fd, &opcode, sizeof(uint16_t)) < sizeof(uint16_t)) return 1;
    return 0;
}

int write_timing(int fd, char * minutes_str, char * hours_str, char * daysofweek_str){
    timing t;
    timing_from_strings(&t, minutes_str, hours_str, daysofweek_str);
    t.minutes = reverse_byte64(t.minutes);
    t.hours = reverse_byte32(t.hours);
    if (write(fd, &t, TIMING_SIZE) < TIMING_SIZE) return 1;
    return 0;
}
    
int write_taskid(int fd, uint64_t taskid){
    taskid = reverse_byte64(taskid);
    if (write(fd, &taskid, sizeof(uint64_t)) < sizeof (uint64_t)) return 1;
    return 0;
}

int write_create(int fd, char * minutes_str, char * hours_str, char * daysofweek_str, int argc, char **argv){
    if (write_timing(fd,minutes_str,hours_str,daysofweek_str) == 1) return 1;
    commandline *cmd = malloc(sizeof(commandline));
    string *tab = malloc(sizeof(string) * (argc));
    for(int i = 0; i < argc; i++) {
        create_string(tab + i, strlen(argv[i]), argv[i]);
    }
    if (create_commandline(cmd, argc, tab) != 0) return 1;
    if (write_commandline(fd,*cmd) == 1) return 1;
    free(cmd);
    free(tab);
    return 0;
}
