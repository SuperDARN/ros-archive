/* sum_power.h
   ===========
   Author: R.J.Barnes
*/

int sum_power(struct tsg_parms *prm,int mplgs,int *lag_table,
			  float *acf_pwr0,float mxpwr,
			  int16 *in_buf,int range_offset,
			  int change_atten,int bad_range,float noise,int thr,
                          int lmt,int *abort);
