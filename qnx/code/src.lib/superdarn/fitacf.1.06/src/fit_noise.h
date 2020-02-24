/* fit_noise.h
   ===========
   Author: R.J.Barnes
*/

void fit_noise(struct complex *nacf,int *badlag,struct badsmp *badsmp,
			   double skynoise,struct fit_prm *prm,
			   struct range_data *ptr);
