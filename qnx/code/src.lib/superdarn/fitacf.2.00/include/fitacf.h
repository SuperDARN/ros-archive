/* fitacf.h
   ========
   Author: R.J.Barnes
*/

#ifndef _FITACF_H
#define _FITACF_H

void FitACFEnd(struct FitBlock *fptr); 
void FitACFStart(struct RadarSite *hd,int year,struct FitBlock *fptr);
void FitACF(struct RadarParm *prm,struct RawData *ptr,struct FitBlock *input,
	    struct FitData *fit);

#endif
