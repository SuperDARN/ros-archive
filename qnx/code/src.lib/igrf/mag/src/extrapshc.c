/* extrapshc.c
   =========== */

/* 
 $Log: extrapshc.c,v $
 Revision 1.1  1999/08/10 12:23:06  barnes
 Initial revision

 */

#include <math.h>

int extrapshc(double date, double dte1, int nmax1, 
               double *gh1, int nmax2, double *gh2, int *nmax, double *gh) {
   
    int i, k, l;
    double factor;

    --gh;
    --gh2;
    --gh1;

    factor = date - dte1;
    if (nmax1 == nmax2) {
	k = nmax1 * (nmax1 + 2);
	*nmax = nmax1;
    } else if (nmax1 > nmax2) {
	k = nmax2 * (nmax2 + 2);
	l = nmax1 * (nmax1 + 2);
     
	for (i = k + 1; i <= l; ++i) {
	    gh[i] = gh1[i];
	}
	*nmax = nmax1;
    } else {
	k = nmax1 * (nmax1 + 2);
	l = nmax2 * (nmax2 + 2);
       
	for (i = k + 1; i <= l; ++i) {
	    gh[i] = factor * gh2[i];
	}
	*nmax = nmax2;
    }
    
    for (i = 1; i <= k; ++i) {
	gh[i] = gh1[i] + factor * gh2[i];
    }
    return 0;
} 
