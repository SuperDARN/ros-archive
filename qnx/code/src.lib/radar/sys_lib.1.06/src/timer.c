/* timer.c
   =======
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
#include <unistd.h>
#include <time.h>

#include "timer.h"

/*
 $Log: timer.c,v $
 Revision 1.4  2001/06/27 20:54:11  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:11:50  barnes
 Added Author Name

 Revision 1.2  1998/11/10 00:24:17  barnes
 Changed the structure to return the actual time in seconds rather than
 nano seconds.

 Revision 1.1  1998/06/19 10:44:53  root
 Initial revision

 */

#define MSEC 1000000

struct tblk *make_clock() {
  struct tblk *ptr;
  ptr=malloc(sizeof(struct tblk));
  ptr->count=0;
  ptr->total=0;
  return ptr;
}

void set_clock_res(double msec) {
  struct timespec res;
  res.tv_nsec=msec*MSEC;
  clock_setres(CLOCK_REALTIME,&res);
}

double get_clock_res(void) {
  struct timespec res;
  clock_getres(CLOCK_REALTIME,&res);
  return (double) res.tv_nsec/MSEC; 
}

void start_clock(struct tblk *ptr) {
  clock_gettime(CLOCK_REALTIME,&ptr->start);				
}

void stop_clock(struct tblk *ptr) {
  struct timespec resval;
  clock_gettime(CLOCK_REALTIME,&ptr->stop);
  clock_getres(CLOCK_REALTIME,&resval);
  ptr->resolution=resval.tv_nsec;
  ptr->count++;

  ptr->total+=(ptr->stop.tv_sec-ptr->start.tv_sec)+
	          (ptr->stop.tv_nsec-ptr->start.tv_nsec)/BILLION; 
				
}









