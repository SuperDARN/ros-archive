/* noise_acf.h
   =========== */

double noise_acf(double mnpwr,struct fit_prm *ptr,
	             double *pwr, struct badsmp *badsmp,
				 struct complex *raw[MAX_RANGE],
	             struct complex *n_acf);
