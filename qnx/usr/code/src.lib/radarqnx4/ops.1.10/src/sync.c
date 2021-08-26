/* sync.c
   ====== 
   Author: R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
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

#include <math.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <i86.h>
#include "rtime.h"
#include "rtypes.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"
#include "taskid.h"
#include "rmsg.h"
#include "radarshell.h"
#include "global.h"

/*
 $Log: sync.c,v $
 Revision 1.5  2007/03/26 17:35:08  code
 Removed hardcoding of the number of beams.

 Revision 1.4  2004/07/20 01:33:47  barnes
 Added missing header.

 Revision 1.3  2004/05/11 17:41:30  barnes
 Removed redundant code.

 Revision 1.2  2004/05/11 17:24:16  barnes
 Isolated platform specific code.

 Revision 1.1  2004/05/06 20:10:19  barnes
 Initial revision

*/

#define USEC 1000000.0


int OpsWaitBoundary(int bsc,int bus) {
#ifdef _QNX4
  struct timespec tock;
  struct timespec tick;
  double bnd;
  double tme;

  bnd=bsc+bus/USEC;

  if (clock_gettime(CLOCK_REALTIME,&tock)==-1) return -1;

  tme=tock.tv_sec+tock.tv_nsec/(1000.0*USEC);
  tme=bnd*floor(1.0+tme/bnd);
  tock.tv_sec=tme;
  tock.tv_nsec=(tme-floor(tme))*1000.0*USEC;

  clock_gettime(CLOCK_REALTIME,&tick);

  while (1) {
    if (tick.tv_sec>tock.tv_sec) break;
    if ((tick.tv_sec==tock.tv_sec) && (tick.tv_nsec>tock.tv_nsec)) break;
    delay(50);
    clock_gettime(CLOCK_REALTIME,&tick);
  }
#endif
  return 0;
}

int OpsDayNight() {
  if (day < night) {
    if ((day <= hr) && (hr < night)) return 1;
    else return 0;
  } else {
    if ((night <= hr) && (hr < day)) return 0;
  }
  return 1;
}
 
int OpsFindSkip(int bsc,int bus) {
  
  int tv;
  int bv;
  int iv;
  int skip;
  int nbm;
  nbm=fabs(ebm-sbm);
  TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
  iv=intsc*1000000+intus;
  bv=bsc*1000000+bus;
  tv=(mt*60+sc)*1000000+us+iv/2-100000; 
  skip=(tv % bv)/iv; 
  if (skip>nbm) skip=0;
  if (skip<0) skip=0;
  return skip;
}

 
