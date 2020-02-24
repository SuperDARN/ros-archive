/* phase_acf.h
   ===========
   Author: R.J.Barnes
*/

int lag_lookup(int *ptab,int mppul,int lag_val);
int phase_acf(struct tsg_parms *prm,float *real2,float *imag2,
	      float *real1,float *imag1,
	      int mplgs,int *lag_table,
	      float *acf_buf,
	      int change_atten);
