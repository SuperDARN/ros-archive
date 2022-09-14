/* sync.c
   ====== 
   Author: R.J.Barnes
*/

/*
 $License$
*/

#include <math.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "rtime.h"
#include "rtypes.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"
#include "global.h"


#define USEC 1000000.0

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

 
