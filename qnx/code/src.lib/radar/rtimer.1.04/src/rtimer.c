/* rtimer.c
   ========
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

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include "rtimer.h"

/*
 $Log: rtimer.c,v $
 Revision 1.4  2004/06/16 22:11:00  barnes
 Removed unreachable code.

 Revision 1.3  2004/05/06 14:53:42  barnes
 Fixed bug in cancelling the timer.

 Revision 1.2  2004/04/30 22:07:46  barnes
 Changed macro definition.

 Revision 1.1  2004/04/29 19:58:05  barnes
 Initial revision

*/


void RTimerSigHandler(int signum) {
  return;
}

struct RTimer *RTimerMake(struct timeval *tmout) {
  int s=0;
  struct RTimer *ptr=NULL;

  ptr=malloc(sizeof(struct RTimer));
  if (ptr==NULL) return NULL;

  ptr->sigfunc=signal(SIGALRM,RTimerSigHandler);
  if (ptr->sigfunc==SIG_ERR) {
    free(ptr);
    return NULL;
  }

#ifdef _QNX4
  ptr->timer.it_interval.tv_sec=0;
  ptr->timer.it_interval.tv_nsec=0;
  ptr->timer.it_value.tv_sec=tmout->tv_sec;
  ptr->timer.it_value.tv_nsec=tmout->tv_usec*1000;
  ptr->tid=timer_create(CLOCK_REALTIME,NULL);
  if (ptr->tid==-1) {
    signal(SIGALRM,ptr->sigfunc);
    free(ptr);
    return NULL;
  }
  s=timer_settime(ptr->tid,0,&ptr->timer,NULL);   
  if (s !=0) {
    signal(SIGALRM,ptr->sigfunc);
    timer_delete(ptr->tid);
    free(ptr);
    return NULL;
  }
#elif defined _QNX6
  ptr->timer.it_interval.tv_sec=0;
  ptr->timer.it_interval.tv_nsec=0;
  ptr->timer.it_value.tv_sec=tmout->tv_sec;
  ptr->timer.it_value.tv_nsec=tmout->tv_usec*1000;
  s=timer_create(CLOCK_REALTIME,NULL,&ptr->tid);
  if (s==-1) {
    signal(SIGALRM,ptr->sigfunc);
    free(ptr);
    return NULL;
  }
  s=timer_settime(ptr->tid,0,&ptr->timer,NULL);   
  if (s !=0) {
    signal(SIGALRM,ptr->sigfunc);
    timer_delete(ptr->tid);
    free(ptr);
    return NULL;
  }
#else
  ptr->itimerval.it_interval.tv_sec=0;
  ptr->itimerval.it_interval.tv_usec=0;
  ptr->itimerval.it_value.tv_sec=0;
  ptr->itimerval.it_value.tv_usec=0;
  if (tmout !=NULL) {
    ptr->itimerval.it_value.tv_sec=tmout->tv_sec;
    ptr->itimerval.it_value.tv_usec=tmout->tv_usec;
  }
  s=setitimer(ITIMER_REAL,&ptr->itimerval,NULL);
  if (s !=0) {
    free(ptr);
    return NULL;
  }
  
#endif
  return ptr;
}


int RTimerSet(struct RTimer *ptr,struct timeval *tmout) {
  if (ptr==NULL) return -1;
#ifdef _QNX4
  ptr->timer.it_interval.tv_sec=0;
  ptr->timer.it_interval.tv_nsec=0;
  ptr->timer.it_value.tv_sec=tmout->tv_sec;
  ptr->timer.it_value.tv_nsec=tmout->tv_usec*1000;
  return timer_settime(ptr->tid,0,&ptr->timer,NULL);   
#elif _QNX6
  ptr->timer.it_interval.tv_sec=0;
  ptr->timer.it_interval.tv_nsec=0;
  ptr->timer.it_value.tv_sec=tmout->tv_sec;
  ptr->timer.it_value.tv_nsec=tmout->tv_usec*1000;
  return timer_settime(ptr->tid,0,&ptr->timer,NULL);
#else
  ptr->itimerval.it_interval.tv_sec=0;
  ptr->itimerval.it_interval.tv_usec=0;
  ptr->itimerval.it_value.tv_sec=0;
  ptr->itimerval.it_value.tv_usec=0;
  if (tmout !=NULL) {
    ptr->itimerval.it_value.tv_sec=tmout->tv_sec;
    ptr->itimerval.it_value.tv_usec=tmout->tv_usec;
  }
  return setitimer(ITIMER_REAL,&ptr->itimerval,NULL);
#endif
}

void RTimerFree(struct RTimer *ptr) {
  if (ptr==NULL) return;
#ifdef _QNX4
   timer_delete(ptr->tid);
#elif _QNX6
  timer_delete(ptr->tid);
#else
  ptr->itimerval.it_interval.tv_sec=0;
  ptr->itimerval.it_interval.tv_usec=0;
  ptr->itimerval.it_value.tv_sec=0;
  ptr->itimerval.it_value.tv_usec=0;
  setitimer(ITIMER_REAL,&ptr->itimerval,NULL);
#endif
  signal(SIGALRM,ptr->sigfunc);
  free(ptr);
}
