/* timer.h
   ======= */

#ifndef _TIMER_LIB
#ifndef _TIME_H_INCLUDED
#include <time.h>
#endif

#ifndef BILLION
#define BILLION 1e9
#endif

struct tblk {
  struct timespec start;
  struct timespec stop;
  int count;
  double resolution;
  double total;
};


void set_clock_res(double);
double get_clock_res(void);

struct tblk *make_clock(void);
void start_clock(struct tblk *ptr);
void stop_clock(struct tblk *ptr);

#define _TIMER_LIB
#endif






