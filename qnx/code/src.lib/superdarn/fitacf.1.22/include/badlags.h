/* badlags.h
   =========
   Author: R.J.Barnes
*/

/*
 $Log: badlags.h,v $
 Revision 1.1  2004/07/12 22:17:25  barnes
 Initial revision

*/

#ifndef _BADLAGS_H
#define _BADLAGS_H

void FitACFBadlags(struct FitPrm *ptr,struct FitACFBadSample *bptr);
void FitACFBadlagsStereo(struct FitPrm *ptr,struct FitACFBadSample *bptr);
void FitACFCkRng(int range,int *badlag,struct FitACFBadSample *bptr,
	         struct FitPrm *ptr);

#endif
