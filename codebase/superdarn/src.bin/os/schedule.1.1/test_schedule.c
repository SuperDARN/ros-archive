/* test_schedule.c
   ===============
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

int test_schedule(struct scd_blk *ptr) {
  int yr,mo,dy,hr,mt,sc,us;
  double stime;
  TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
  stime=TimeYMDHMSToEpoch(yr,mo,dy,hr,mt,sc);
  if (ptr->cnt==0) return 0;  
  if (ptr->cnt==ptr->num) return 0;
  if (stime>=ptr->entry[ptr->cnt].stime) return 1;
  return 0;
}
