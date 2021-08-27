/* fit_cmp.h
   ========= */


#ifndef _FIT_CMP_H
#define _FIT_CMP_H

void fit_cmphdr(double pmax,double vmax,double wmax,
				struct fitdata *fit,struct cmphdr *ptr);

int fit_cmp(int store,double min_pwr,
	    double pmax,double vmax,double wmax,struct fitdata *fit,
	    struct cmpdata *ptr,int scan);
 
#endif
