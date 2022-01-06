#include "write-data.h"

/* écrit l'opcode dans buf (sous la forme neccessaire pour la communication)
 * renvoie le nombre d'octets écrits dans buf */
int write_opcode(char * buf, uint16_t opcode){
    opcode = htobe16(opcode);
    *((uint16_t*)buf) = opcode;
    return sizeof(uint16_t);
}

/* écrit un timing dans buf qui était sous la forme de trois string
 * renvoie le nombre d'octets écrits dans buf */
int write_timing_from_strings(char * buf, char * minutes_str, char * hours_str, char * daysofweek_str){
    timing t;
    if(timing_from_strings(&t, minutes_str, hours_str, daysofweek_str) == -1) {
        printf("Erreur timing_from_strings\n");
        exit(EXIT_FAILURE);
    }
    return write_timing(buf, t);
}

/* écrit un timing dans buf qui était sous la forme d'un timing
 * renvoie le nombre d'octets écrits dans buf */
int write_timing(char * buf, timing t) {
    t.minutes = htobe64(t.minutes);
    t.hours = htobe32(t.hours);
    *((uint64_t*)buf) = t.minutes;
    buf += sizeof(uint64_t);
    *((uint32_t*)buf) = t.hours;
    buf += sizeof(uint32_t);
    *((uint8_t*)buf) = t.daysofweek;
    return TIMING_SIZE;
}   

/* écrit un taskid dans buf
 * renvoie le nombre d'octets écrits dans buf */
int write_taskid(char * buf, uint64_t taskid){
    taskid = htobe64(taskid);
    *((uint64_t*)buf) = taskid;
    return sizeof(uint64_t);
}

/* écrit la requete create dans buf à partir :
   - d'un timing (sous forme de trois string)
   - du nombre d'arguments de la commandline
   - du tableau d'arguments de la commandline
 * renvoie le nombre d'octets écrits dans buf */
int write_create(char * buf, char * minutes_str, char * hours_str, char * daysofweek_str, int argc, char **argv){
    int n = write_timing_from_strings(buf,minutes_str,hours_str,daysofweek_str);
    commandline cmd; 
    string tab[argc];

    for(int i = 0; i < argc; i++) {
        create_string(tab + i, strlen(argv[i]), argv[i]);
    }
    if (create_commandline(&cmd, argc, tab) != 0) return 1;
    n += write_commandline(buf+n, cmd);
    return n;
}
