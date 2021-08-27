/* pulse_acf.h  
   =========== */

int lag_lookup(int *ptab,int mppul,int lag_val);
int pulse_acf(struct tsg_parms *prm,int *real2,int *imag2,
	          int *real1,int *imag1,
		      int mplgs,int *lag_table,
		      int *acf_buf,
		      int change_atten);
