#include "read-request.h"

int read_timing(int fd, timing *t){
    if(read(fd, t, TIMING_SIZE) < TIMING_SIZE){
        perror("Erreur read_timing");
        return 1;
    }
    t->minutes = reverse_byte64(t->minutes);
    t->hours = reverse_byte32(t->hours);
    return 0;
}