/* set_schedule.c
   ============== */

/* 
 $Log: set_schedule.c,v $
 Revision 1.1  1999/03/30 18:31:14  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "read_clock.h"
#include "cnv_time.h"
#include "log_info.h"
#include "schedule.h"


int set_schedule(struct scd_blk *ptr) {
  int yr,mo,dy,hr,mt,sc,ms,us;
  double stime;
  int c;
  read_clock(&yr,&mo,&dy,&hr,&mt,&sc,&ms,&us);
  stime=time_epoch(yr,mo,dy,hr,mt,sc);
  
  for (c=0;(c<ptr->num) && (ptr->entry[c].stime<=stime);c++);
  return c;  
}