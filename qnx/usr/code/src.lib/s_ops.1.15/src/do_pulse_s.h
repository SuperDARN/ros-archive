/* do_pulse_s.h 
   ============ */


int do_pulse_s(int debug,int buffer,
 	       int num_words_A,int num_words_B,
               int num_channel_A,int num_channel_B,
	       int tsg_id_A,int tsg_id_B,
               int *delay,
	       pid_t dio_id,
               pid_t *ad_id,
               pid_t *reset_id);

