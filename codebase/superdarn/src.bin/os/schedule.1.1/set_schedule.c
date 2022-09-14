/* set_schedule.c
   ==============
   Author: R.J.Barnes
*/

/*
 $License$
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "rtime.h"
#include "log_info.h"
#include "schedule.h"

int set_schedule(struct scd_blk *ptr) {
  int yr,mo,dy,hr,mt,sc,us;
  double stime;
  int c;
  TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
  stime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  
  for (c=0;(c<ptr->num) && (ptr->entry[c].stime<=stime);c++);
  return c;  
}
