/* radar.h
   ======= */

#define ATTEN_ERR 4
#define ATTEN_STEP 10
#define ACF_PART 0
#define XCF_PART 1
#define IMAG_BUF_OFFSET 1
#define MAX_AD_ERR 20

#define INT_PULSE_FAIL 0x01
#define INT_ATTEN_OVF 0x02
#define INT_AD_FAIL 0x04

int do_radar(struct tsg_parms *prm,int tsg_id,int mplgs,int *lags,int intt,
			 clock_t tick,int bad_range,int debug,
			 pid_t dio_id,pid_t ad_id,pid_t reset_id,
			 unsigned char **buf_adr,int max_atten,int mxpwr,int *atten,
			 int *noise,int *pwr0,int *acfd,int *xcfd,int *flg,
	                 int thr,int lmt,
			 void (*user)(struct tsg_parms *prm,
						  int intt,clock_t tick,int nave,
		                  unsigned char *buffer,int bufnum,
						  int num_words,int num_channel,
						  void *usr_data),void *usr_data);

