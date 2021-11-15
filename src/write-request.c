#include "write-request.h"

int write_opcode(int fd, uint16_t opcode){
    opcode = reverse_byte16(opcode);
    if (write(fd, &opcode, sizeof(uint16_t)) < sizeof(uint16_t) return 1;
    return 0;
}

int write_timing(int fd, char * minutes_str, char * hours_str, char * daysofweek_str){
    struct timing t;
    timing_from_strings(&t, minutes_str, hours_str, daysofweek_str);
    t.minutes = reverse_byte64(t.minutes);
    t.hours = reverse_byte32(t.hours);
    if (write(fd, &t, sizeof(struct timing)) < sizeof(struct timing)) return 1;
    return 0;
}
    
int write_taskid(int fd, uint64_t taskid){
    taskid = reverse_byte64(taskid);
    if (write(fd, &taskid, sizeof(uint64_t)) < sizeof (unit64_t)) return 1;
    return 0;
}

int write_create(int fd, uint16_t opcode, char * minutes_str, char * hours_str, char * daysofweek_str, int argc, char **argv)
{return 1;}
