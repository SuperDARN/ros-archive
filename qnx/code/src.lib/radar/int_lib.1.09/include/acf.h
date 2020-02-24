/* acf.h
   =====
   Author: R.J.Barnes
*/

int acf(struct tsg_parms *prm,
		int16 *in_buf,int mplgs,int *lag_table,
		float *acf_buf,
		int xcf_flag,int range_offset,int change_atten,int bad_range);
