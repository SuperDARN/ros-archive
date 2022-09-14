/* read.c
   ======
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>



void TimeReadClock(int *yr,int *month,int *day,int *hour,int *min,int *sec,
		   int *usec) {

#ifdef _QNX4
 struct timespec tv;
#else
 struct timeval tv;
#endif


  time_t tm;
  struct tm *gmt_time;
  int s;

#ifdef _QNX4
  s=clock_gettime(CLOCK_REALTIME,&tv);
#else
  s=gettimeofday(&tv,NULL);
#endif
  tm=tv.tv_sec;
  gmt_time = gmtime(&tm);
  *yr = gmt_time->tm_year + 1900;
  *month = gmt_time->tm_mon + 1;
  *day = gmt_time->tm_mday; 
  *hour = gmt_time->tm_hour;
  *min = gmt_time->tm_min;
  *sec = gmt_time->tm_sec;
#ifdef _QNX4
  *usec = tv.tv_nsec/1000.0;
#else
  *usec = tv.tv_usec;
#endif


}  
