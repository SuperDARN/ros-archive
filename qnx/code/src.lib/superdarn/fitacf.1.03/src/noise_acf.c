/* noise_acf.c
   =========== */

/*
 $Log: noise_acf.c,v $
 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include <math.h>
#include "math_constant.h"
#include "badsmp.h"
#include "fit_blk.h"
#include "badlags.h"
#include "noise_stat.h"


#define PLIM 1.6

double noise_acf(double mnpwr,struct fit_prm *ptr,
	             double *pwr, struct badsmp *badsmp,
				 struct complex *raw[MAX_RANGE],
				 struct complex *n_acf) {
  int i, j;
  int np[LAG_TAB_LEN];
  int bad[LAG_TAB_LEN];
  double plim, P, temp, temp1;

  for (i=0; i< ptr->mplgs; i++) {
	n_acf[i].x = 0;
	n_acf[i].y= 0;
	np[i] = 0;
  }
  plim = PLIM * mnpwr;

  for (i=0; i< ptr->nrang; i++) {
    if ((pwr[i] < plim) && ((fabs(raw[i][0].x) + 
			fabs(raw[i][0].y)) > 0) &&
			(fabs(raw[i][0].x) < plim) &&
			(fabs(raw[i][0].y) < plim)) {
	  ckrng((i+1), bad,badsmp, ptr);

	  for (j=0; j< ptr->mplgs; j++) {
	    if ((fabs(raw[i][j].x) < plim) &&
			(fabs(raw[i][j].y) < plim) &&
			(bad[j] == 0)) {
		  n_acf[j].x = n_acf[j].x + raw[i][j].x;
		  n_acf[j].y = n_acf[j].y + raw[i][j].y;
		  ++(np[j]);
		}
	  }
	}
  }

  if (np[0] <= 2) {
	for (i=0; i < ptr->mplgs; ++i) {
	  n_acf[i].x = 0;
	  n_acf[i].y = 0;
	}
    return 0.0;
  }

  for (i=0; i< ptr->mplgs; i++) {
	if (np[i] > 2) {
	  n_acf[i].x = n_acf[i].x/np[i];
	  n_acf[i].y = n_acf[i].y/np[i];
	} else {
	  n_acf[i].x = 0;
	  n_acf[i].y= 0;
	}
  }

  /*  Now determine the average power in the non-zero lags of the noise acf */

  for (i=1, P=0; i < ptr->mplgs; ++i) {
	P = P + lag_power(&n_acf[i]);
  }
  P = P/(ptr->mplgs - 1);
  return P;
}
