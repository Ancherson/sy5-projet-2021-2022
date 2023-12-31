#ifndef TIMING_H
#define TIMING_H

#define TIMING_SIZE 13

#include <stdint.h>

typedef struct timing {
  uint64_t minutes;
  uint32_t hours;
  uint8_t daysofweek;
} timing;

#endif // TIMING_H
