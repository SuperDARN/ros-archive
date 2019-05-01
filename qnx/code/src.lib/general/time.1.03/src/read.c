/* read.c
   ======
   Author: R.J.Barnes
*/

/*
 Copyright © 2001 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 RST-ROS (hereinafter "the Program").
 
 The Program was developed at The Johns Hopkins University/Applied Physics
 Laboratory (JHU/APL) which is the author thereof under the "work made for
 hire" provisions of the copyright law.  
 
 JHU/APL assumes no obligation to provide support of any kind with regard to
 the Program.  This includes no obligation to provide assistance in using the
 Program or to provide updated versions of the Program.
 
 THE PROGRAM AND ITS DOCUMENTATION ARE PROVIDED AS IS AND WITHOUT ANY EXPRESS
 OR IMPLIED WARRANTIES WHATSOEVER.  ALL WARRANTIES INCLUDING, BUT NOT LIMITED
 TO, PERFORMANCE, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 HEREBY DISCLAIMED.  YOU ASSUME THE ENTIRE RISK AND LIABILITY OF USING THE
 PROGRAM TO INCLUDE USE IN COMPLIANCE WITH ANY THIRD PARTY RIGHTS.  YOU ARE
 ADVISED TO TEST THE PROGRAM THOROUGHLY BEFORE RELYING ON IT.  IN NO EVENT
 SHALL JHU/APL BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING, WITHOUT
 LIMITATION, ANY LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, ARISING OUT OF THE USE OR INABILITY TO USE THE
 PROGRAM."
 
 
 
 
 
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

/*
 $Log: read.c,v $
 Revision 1.2  2004/04/30 22:12:36  barnes
 Changed macro definition.

 Revision 1.1  2004/01/06 20:47:46  barnes
 Initial revision

*/

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
