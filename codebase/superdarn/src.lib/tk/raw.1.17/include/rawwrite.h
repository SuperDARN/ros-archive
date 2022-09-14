/* rawwrite.h
   ========== 
   Author: R.J.Barnes
*/


/*
 (c) 2009 JHU/APL
 
*/




#ifndef _RAWWRITE_H
#define _RAWWRITE_H

int RawFwrite(FILE *fp,struct RadarParm *,struct RawData *);
int RawWrite(int,struct RadarParm *,struct RawData *);

#endif
