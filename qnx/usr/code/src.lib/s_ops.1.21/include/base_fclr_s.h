/* base_fclr_s.h
   ============= */



int base_fclr_s(int tsg_id,int samples,int debug,
                int16 **buf_adr,int *buf_size,
                int num_channel,
                pid_t dio_id,pid_t *ad_id,pid_t *reset_id,
                struct freq_table *fptr,int *delay,
                int start_freqA,int end_freqA,int step_freqA,
                int *lowest_freqA,float *statisticA,int *flgA,
                int start_freqB,int end_freqB,int step_freqB,
                int *lowest_freqB,float *statisticB,int *flgB);

 
 
