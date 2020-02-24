/* fit_acf.h
   =========
   Author: R.J.Barnes
*/

int fit_acf (struct complex *acf,int range,int *badlag,
		     struct badsmp *badsmp,int lag_lim,
			 struct fit_prm *prm,
		     double noise_lev_in,char xflag,double xomega,
             struct range_data *ptr);
