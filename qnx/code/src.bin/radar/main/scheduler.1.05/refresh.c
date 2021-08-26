/* refresh.c
   ========= */

/*
 $Log: refresh.c,v $
 Revision 1.3  2000/10/30 17:25:51  barnes
 Fixed possible fault if the schedule file is missing.

 Revision 1.2  2000/09/18 20:45:40  barnes
 Added code so that the schedule is reloaded the moment it is altered.

 Revision 1.1  1999/03/30 18:31:14  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "read_clock.h"
#include "cnv_time.h"
#include "log_info.h"
#include "schedule.h"

int then=-1;
time_t tval=-1;


int test_refresh(struct scd_blk *ptr) {

  struct stat buf;
  int yr,mo,dy,hr,mt,sc,ms,us;
  int nowsec,now,rc;
  read_clock(&yr,&mo,&dy,&hr,&mt,&sc,&ms,&us);
  
  nowsec=hr*3600+mt*60+sc;
  now=nowsec/ptr->refresh;

  /* test to see if the schedule has been altered */

  if ((rc=stat(ptr->name,&buf)) !=0) return -1;
  if (tval==-1) tval=buf.st_mtime;
  if (buf.st_mtime !=tval) {
    tval=buf.st_mtime;
    then=now;
    return 1;
  }


  /* okay check whether we're on a refresh boundary */
  
  if (then==-1) then=now;
  if (then !=now) {
    tval=buf.st_mtime;
    then=now;
    return 1;
  }   

 

  
  return 0;
}
