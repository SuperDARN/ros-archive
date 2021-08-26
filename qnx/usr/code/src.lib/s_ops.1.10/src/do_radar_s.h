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
 	           int max_atten_A,int mxpwr_A,
               int *atten_A,int *noise_A,
               int *pwr0_A,int *acfd_A,int *xcfd_A,
               int *flg_A,int *nave_A,

               struct tsg_parms *prm_B,int tsg_id_B,int mplgs_B,
               int *lags_B,int bad_range_B,
	           int max_atten_B,int mxpwr_B,
               int *atten_B,int *noise_B,
               int *pwr0_B,int *acfd_B,int *xcfd_B,
               int *flg_B,int *nave_B);


