/* rawdata.h
   ========== 

  Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/





#ifndef _RAWDATA_H
#define _RAWDATA_H

#ifndef _LIMIT_H
#include "limit.h"
#endif

struct RawData {
  struct {
    int major;
    int minor;
  } revision;
  float thr;
  float pwr0[MAX_RANGE];
  float acfd[MAX_RANGE][LAG_SIZE][2];
  float xcfd[MAX_RANGE][LAG_SIZE][2];
};

#endif
