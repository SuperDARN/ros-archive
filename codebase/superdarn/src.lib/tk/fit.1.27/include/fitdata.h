/* fitdata.h
   ========== 
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/




#ifndef _FITDATA_H
#define _FITDATA_H


#ifndef _LIMIT_H
#include "limit.h"
#endif

#ifndef _FITBLK_H
#include "fitblk.h"
#endif

struct FitData {
  struct {
    int major;
    int minor;
  } revision;
  struct FitNoise noise;
  struct FitRange rng[MAX_RANGE];
  struct FitRange xrng[MAX_RANGE];
  struct FitElv  elv[MAX_RANGE];
};

#endif
