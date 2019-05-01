/* dofit.h
   ========
   Author: R.J.Barnes
*/


/*
 $Log: dofit.h,v $
 Revision 1.1  2004/07/12 22:17:25  barnes
 Initial revision

*/

#ifndef _DOFIT_H
#define _DOFIT_H

int do_fit(struct FitBlock *iptr,int lag_lim,int goose,
	   struct FitRange *ptr,struct FitRange *xptr,
           struct FitElv *elv,
	   struct FitNoise *nptr);

#endif
