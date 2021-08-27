/* refresh.c
   ========= */

/*
 $Log: refresh.c,v $
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

int then=-1;

int test_refresh(struct scd_blk *ptr) {
  int yr,mo,dy,hr,mt,sc,ms,us;
  int nowsec,now;
  read_clock(&yr,&mo,&dy,&hr,&mt,&sc,&ms,&us);
  
  /* okay check whether we're on a refresh boundary */
  
  nowsec=hr*3600+mt*60+sc;
  now=nowsec/ptr->refresh;
  if (then==-1) then=now;
  if (then !=now) {
    then=now;
    return 1;
  }     
  return 0;
}