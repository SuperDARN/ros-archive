/* print_schedule.c
   ================ */

/*
 $Log: print_schedule.c,v $
 Revision 1.1  1999/03/30 18:31:14  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "cnv_time.h"
#include "log_info.h"
#include "schedule.h"

void print_schedule(struct scd_blk *ptr) {/* prints out the schedule */
  int c;
  char interval[10];
  char txt[256];
  if (ptr->num==0) {
    log_info(1,"No schedule file loaded");
    return;
  }
  
  sprintf(txt,"Schedule file %s loaded",ptr->name);
  log_info(1,txt);
  sprintf(txt,"Command path -> %s",ptr->path);
  log_info(1,txt);
  if (ptr->entry[0].stime==-1) {
    sprintf(txt,"Default Program -> %s\n",ptr->entry[0].command);
    log_info(1,txt);
  }
  if ((ptr->cnt >0) && (ptr->cnt<ptr->num)) {
    int yr,mo,dy,hr,mt;
    double sc;
    log_info(1,"Pending programs :\n");    		 
    for (c=ptr->cnt;c<ptr->num;c++) {
      if (ptr->entry[c].stime==-1) continue;
      epoch_time(ptr->entry[c].stime,&yr,&mo,&dy,&hr,&mt,&sc);
      sprintf(txt,"%d : %d %d %d : %d %d -> %s",c,yr,mo,dy,hr,mt,
	          ptr->entry[c].command);
      log_info(1,txt);       
    } 
  } else log_info(1,"There are no pending programs");
  sprintf(txt,"\nCurrent program ->%s",ptr->command);
  log_info(1,txt);
  sprintf(txt,"Schedule reloaded every %d seconds",ptr->refresh);
  
  log_info(1,txt);
}  			
