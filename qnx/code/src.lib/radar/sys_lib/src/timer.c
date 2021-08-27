/* timer.c 
   ======= */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "timer.h"

/*
 $Log: timer.c,v $
 Revision 1.2  1998/11/10 00:24:17  barnes
 Changed the structure to return the actual time in seconds rather than
 nano seconds.

 Revision 1.1  1998/06/19 10:44:53  root
 Initial revision

 */

#define MSEC 1000000

struct tblk *make_clock() {
  struct tblk *ptr;
  ptr=malloc(sizeof(struct tblk));
  ptr->count=0;
  ptr->total=0;
  return ptr;
}

void set_clock_res(double msec) {
  struct timespec res;
  res.tv_nsec=msec*MSEC;
  clock_setres(CLOCK_REALTIME,&res);
}

double get_clock_res(void) {
  struct timespec res;
  clock_getres(CLOCK_REALTIME,&res);
  return (double) res.tv_nsec/MSEC; 
}

void start_clock(struct tblk *ptr) {
  clock_gettime(CLOCK_REALTIME,&ptr->start);				
}

void stop_clock(struct tblk *ptr) {
  struct timespec resval;
  clock_gettime(CLOCK_REALTIME,&ptr->stop);
  clock_getres(CLOCK_REALTIME,&resval);
  ptr->resolution=resval.tv_nsec;
  ptr->count++;

  ptr->total+=(ptr->stop.tv_sec-ptr->start.tv_sec)+
	          (ptr->stop.tv_nsec-ptr->start.tv_nsec)/BILLION; 
				
}









