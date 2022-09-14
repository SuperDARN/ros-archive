/* fit_noise.h
   ===========
   Author: R.J.Barnes & K.Baker
*/

/*
 (c) 2009 JHU/APL
 
*/



void fit_noise(struct complex *nacf,int *badlag,
               struct FitACFBadSample *badsmp,
	       double skynoise,struct FitPrm *prm,
	       struct FitRange *ptr);
