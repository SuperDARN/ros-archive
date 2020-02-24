/* base_fclr.h
   ===========
   Author: R.J.Barnes
*/

int base_fclr(int tsg_id,int samples,int debug,int start_freq,
	      int end_freq,
	      int step_freq,
              int16 *buf_adr,int buf_size,
	      struct freq_table *fptr,
              int *lowest_freq,float *statistic,int *flg,
	      int num_channel,pid_t dio_id,pid_t ad_id,pid_t reset_id);
