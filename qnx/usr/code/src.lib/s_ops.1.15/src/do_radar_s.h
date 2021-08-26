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

int do_radar_s(int intt,int *delay,clock_t tick,int debug,
               pid_t dio_id,pid_t *ad_id,pid_t *reset_id, 
               unsigned char **buf_adr,int thr,int lmt,           

               struct tsg_parms *prm_A,int tsg_id_A,int mplgs_A,
               int *lags_A,int bad_range_A,
 	           int max_atten_A,float mxpwr_A,
               int *atten_A,float *noise_A,
               float *pwr0_A,float *acfd_A,float *xcfd_A,
               int *flg_A,int *nave_A,

               struct tsg_parms *prm_B,int tsg_id_B,int mplgs_B,
               int *lags_B,int bad_range_B,
	           int max_atten_B,float mxpwr_B,
               int *atten_B,float *noise_B,
               float *pwr0_B,float *acfd_B,float *xcfd_B,
               int *flg_B,int *nave_B);


