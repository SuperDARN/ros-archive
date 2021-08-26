/* logger.c 
   ======== */

/* 
 $Log: logger.c,v $
 Revision 1.2  1998/06/03 19:10:24  root
 Modification to the version control system.

 Revision 1.1  1996/04/29 16:27:53  root
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int yday=-1;

FILE *open_log(void) {
  char log_name[12];
  time_t log_clock;
  struct tm *gmt; 

  /* open a log file */
 
  time(&log_clock);
  gmt=gmtime(&log_clock);

  if ((gmt->tm_yday+1) != yday) {   
     sprintf(log_name,"dio.log.%3.3d",gmt->tm_yday+1);
     yday=gmt->tm_yday+1;
  }
  return fopen(log_name,"a");    
}

