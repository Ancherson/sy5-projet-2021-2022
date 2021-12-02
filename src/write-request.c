#include "write-request.h"


int write_opcode(char * buf, uint16_t opcode){
    opcode = htobe16(opcode);
    *((uint16_t*)buf) = opcode;
    return sizeof(uint16_t);
}

int write_timing(char * buf, char * minutes_str, char * hours_str, char * daysofweek_str){
    timing t;
    if(timing_from_strings(&t, minutes_str, hours_str, daysofweek_str) == -1) {
        printf("Erreur timing_from_strings\n");
        return 1;
    }
    t.minutes = htobe64(t.minutes);
    t.hours = htobe32(t.hours);
    *((uint64_t*)buf) = t.minutes;
    buf += sizeof(uint64_t);
    *((uint32_t*)buf) = t.hours;
    buf += sizeof(uint32_t);
    *((uint8_t*)buf) = t.daysofweek;
    return TIMING_SIZE;
}
    
int write_taskid(char * buf, uint64_t taskid){
    taskid = htobe64(taskid);
    *((uint64_t*)buf) = taskid;
    return sizeof(uint64_t);
}

int write_create(char * buf, char * minutes_str, char * hours_str, char * daysofweek_str, int argc, char **argv){
    int n = write_timing(buf,minutes_str,hours_str,daysofweek_str);
    commandline cmd; 
    string tab[argc];

    for(int i = 0; i < argc; i++) {
        create_string(tab + i, strlen(argv[i]), argv[i]);
    }
    if (create_commandline(&cmd, argc, tab) != 0) return 1;
    n += write_commandline(buf+n, cmd);
    return n;
}
