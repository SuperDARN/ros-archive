/* noise_stat.c
   ============ */

/*
 $Log: noise_stat.c,v $
 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include <math.h>
#include "math_constant.h"
#include "fit_blk.h"

#include "badsmp.h"
#include "badlags.h"

#define PLIM (1.6)
#define ROOT_3 1.7


double lag_power(struct complex *a) {
  return sqrt(a->x*a->x + a->y*a->y);
}

double noise_stat(double mnpwr,struct fit_prm *ptr,struct badsmp *badsmp,
				  struct complex *acf[MAX_RANGE],
	              double *signal) {
  double plim;
  int i, j, np0, npt;
  int bdlag[LAG_TAB_LEN];
  double var, sigma, P, P2;
  double temp, fluct, low_lim, high_lim;

  plim = PLIM * mnpwr;

  P = 0.0;
  P2 = 0.0;
  var = 0.0;
  np0 = 0;
  npt = 0;

  for (i=0; i < ptr->nrang; ++i) { 
    if ((acf[i][0].x > plim) || (acf[i][0].x <= 0.0)) continue;
	ckrng((i+1), bdlag,badsmp, ptr);
	++np0;
	fluct = ((double) acf[i][0].x)/sqrt(ptr->nave);
	low_lim = acf[i][0].x - 2.0*fluct;
	if (low_lim < 0) low_lim = low_lim + fluct;
	high_lim = acf[i][0].x + fluct;

	for (j=1; j < ptr->mplgs; ++j) {
      if (bdlag[j]) continue;
	  temp = lag_power(&acf[i][j]);
	  if (temp < low_lim || temp > high_lim) continue;
	  ++npt;
	  P = P + temp;
	  P2 = P2 + temp*temp;
    }
  }

  if (npt < 2) {
    *signal = 0;
	return plim/sqrt((double) ptr->nave);
  }

  P = P/npt;
  var = (P2 - P*P*npt)/((double) (npt-1));

  sigma = (sigma > 0.0) ? sqrt(var) : 0.0;

  if ((P >= sigma * ROOT_3) && (sigma > 0.0)) *signal = P;
  else *signal = 0.0;
  return (P > sigma) ? P : sigma;
}
