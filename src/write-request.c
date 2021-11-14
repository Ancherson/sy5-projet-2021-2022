#include "write-request.h"

int write_opcode(int fd, uint16_t opcode){
    opcode = reverse_byte16(opcode);
    return write(fd, &opcode, sizeof(uint16_t));
}

int write_timing(int fd, char * minutes_str, char * hours_str, char * daysofweek_str){
    struct timing t;
    timing_from_strings(&t, minutes_str, hours_str, daysofweek_str);
    t.minutes = reverse_byte64(t.minutes);
    t.hours = reverse_byte32(t.hours);
    return write(fd, &t, sizeof(struct timing));
}
