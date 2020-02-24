/* fine_fclr.h
   ===========
   Author: R.J.Barnes
*/

int fine_fclr(int tsg_id,int samples,int debug,int *lowest_freq,
	      int start_freq,
	      int num_channel,float mxpwr,int max_atten,
	      int16 *buf_adr,
	      pid_t dio_id,pid_t ad_id,pid_t reset_id,
	      int *atten,float *noise,int *flg);
