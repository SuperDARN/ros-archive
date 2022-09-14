/* fitread.h
   ========= 
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/




#ifndef _FITREAD_H
#define _FITREAD_H

int FitDecode(struct DataMap *ptr,struct RadarParm *,struct FitData *);
int FitFread(FILE *fp,struct RadarParm *,struct FitData *);
int FitRead(int fid,struct RadarParm *,struct FitData *);

#endif

