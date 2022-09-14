/* noise_acf.h
   ===========
   Author: R.J.Barnes & K.Baker
*/


/*
 (c) 2009 JHU/APL
 
*/



double noise_acf(double mnpwr,struct FitPrm *ptr,
	         double *pwr, struct FitACFBadSample *badsmp,
		 struct complex *raw[MAX_RANGE],
	         struct complex *n_acf);
