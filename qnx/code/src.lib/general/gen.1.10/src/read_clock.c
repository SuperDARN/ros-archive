/* read_clock.c
   ============
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

/*
 $Log: read_clock.c,v $
 Revision 1.4  2001/06/27 20:38:16  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:11:48  barnes
 Added Author Name

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
