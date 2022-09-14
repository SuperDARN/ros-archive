/* omega_guess.h
   =============
   Author: R.J.Barnes & K.Baker
*/

/*
 (c) 2009 JHU/APL
 
*/



double omega_guess(struct complex *acf,double *tau,
	           int *badlag,double *phi_res,
                   double *omega_err,int mpinc,int mplgs);