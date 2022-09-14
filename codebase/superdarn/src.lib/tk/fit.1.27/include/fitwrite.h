/* fitwrite.h
   ========== 
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/



#ifndef _FITWRITE_H
#define _FITWRITE_H

int FitFwrite(FILE *fp,struct RadarParm *,struct FitData *);
int FitWrite(int fid,struct RadarParm *,struct FitData *);

#endif
