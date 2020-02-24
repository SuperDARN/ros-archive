/* phase_sum_power.h
   =================
   Author: R.J.Barnes
*/

int phase_sum_power(struct tsg_parms *prm,int mplgs,int *lag_table,
		    float *acf_pwr0,float mxpwr,
		    float *d_data,int range_offset,
		    int change_atten,int bad_range);
