/* read_clock.c 
   ============ */

/*
 $Log: read_clock.c,v $
 Revision 1.2  1998/06/09 00:00:07  barnes
 Removed redundant header.

 Revision 1.1  1998/06/03 09:05:50  root
 Initial revision

 */

#include <time.h>

void read_clock(int *yr,int *month,int *day,int *hour,int *min,int *sec,
			   int *msec,int *usec) {

  time_t sys_time;
  struct tm *gmt_time;
	 
  sys_time = time(NULL);
  gmt_time = gmtime(&sys_time);
  *yr = gmt_time->tm_year + 1900;
  *month = gmt_time->tm_mon + 1;
  *day = gmt_time->tm_mday; 
  *hour = gmt_time->tm_hour;
  *min = gmt_time->tm_min;
  *sec = gmt_time->tm_sec;
  *msec = 0;
  *usec = 0;
}  
