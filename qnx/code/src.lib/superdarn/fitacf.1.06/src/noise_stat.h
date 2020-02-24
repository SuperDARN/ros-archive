/* noise_stat.h
   ============
   Author: R.J.Barnes
*/


double lag_power(struct complex *a);

double noise_stat(double mnpwr,struct fit_prm *ptr,struct badsmp *badsmp,
				  struct complex *acf[MAX_RANGE],
				  double *signal);