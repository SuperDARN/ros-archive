/* fclr.h
   ======= */

#define FCLR_BUF_OVF 0x01
#define FCLR_FREQ_FAIL 0x02
#define FCLR_GAIN_FAIL 0x04
#define FCLR_AD_FAIL 0x08
#define FCLR_TSG_FAIL 0x10
#define FCLR_FCLR_FAIL 0x20
#define FCLR_SCAN_FAIL 0x40
#define FCLR_LOCAL_MODE 0x80

#define FCLR_NRANG_BASE 50
#define FCLR_NRANG_FINE 200

#define MIN_SMSEP_STEP 60
#define NUM_FCLR_FREQ 5
#define ATTEN_OFF 0

int do_fclr(struct tsg_table *tab,int *tsg_id,int debug,int start_freq,
		    int end_freq,int step_freq,int16 *buf_adr,int buf_size,
            int mxpwr,int max_atten,
			struct freq_table *fptr,
			pid_t dio_id,pid_t ad_id,
	        pid_t reset_id,int *atten,int *noise,int *statistic,
            clock_t *tick,int *flg);

