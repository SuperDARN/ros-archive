/* build..h
   ========
   Author: R.J.Barnes
*/


/*
 $License$
*/

#ifndef _BUILD_H
#define _BUILD_H

void OpsBuildPrm(struct RadarParm *prm,
                 int *ptab,int (*lags)[2]);
void OpsBuildIQ(struct IQData *iq,unsigned int **badtr);
void OpsBuildRaw(struct RawData *raw);


#endif
