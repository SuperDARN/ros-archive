/* rawread.h
   ========= 
   Author: R.J.Barnes
*/


/* 
 (c) 2009 JHU/APL
 
*/




#ifndef _RAWREAD_H
#define _RAWREAD_H

int RawFread(FILE *fp,struct RadarParm *,struct RawData *);
int RawRead(int fid,struct RadarParm *,struct RawData *);

#endif
