/* fine_fclr.c
   =========== */

int fine_fclr(int tsg_id,int samples,int debug,int *lowest_freq,
			  int start_freq,
			  int num_channel,int mxpwr,int max_atten,
			  int16 *buf_adr,
			  pid_t dio_id,pid_t ad_id,pid_t reset_id,
			  int *atten,int *noise,int *flg);
