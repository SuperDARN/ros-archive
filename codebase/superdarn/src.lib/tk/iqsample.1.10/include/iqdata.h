
/* rawdumpdata.h
   ============= 
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/





#ifndef _LIMIT_H
#include "limit.h"
#endif



#ifndef _IQDATA_H
#define _IQDATA_H


struct IQData {
  struct { 
    int major;
    int minor;
  } revision;
  int chnnum;
  int smpnum;
  int skpnum;
  int seqnum;
  int tbadtr;
  struct timespec tval[MAXNAVE];
  int atten[MAXNAVE];
  float noise[MAXNAVE];
  int offset[MAXNAVE];
  int size[MAXNAVE];
  int badtr[MAXNAVE];
};

#endif
