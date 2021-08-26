/* global_s.h
   ========== */

#include "stereo.h"
#define MAX_TSG 32


extern pid_t dio_id_s;
extern pid_t ad_id_s[2];
extern pid_t reset_id_s[2];
extern int bufsze_s[2];
extern int bufnum_s[2];
extern void *bufadr_s[2*MAX_AD_BUF];
extern int delay_s[2];

extern int pwr0_A[MAX_RANGE];
extern int acfd_A[MAX_RANGE*LAG_TAB_LEN*2];
extern int xcfd_A[MAX_RANGE*LAG_TAB_LEN*2];

extern int pwr0_B[MAX_RANGE];
extern int acfd_B[MAX_RANGE*LAG_TAB_LEN*2];
extern int xcfd_B[MAX_RANGE*LAG_TAB_LEN*2];


extern int intt;
extern int st_id;
extern int year;
extern int month;
extern int day;
extern int hour;
extern int minut;
extern int sec;
extern int msec;
extern int usec;
extern int cp;

/* these are parameters per channel */

extern int max_atten_A;
extern int prot_atten_A;
extern int rx_wide_A;
extern int rx_narrow_A;
extern int rsep_switch_A;

extern int nave_A;
extern int tsg_id_A;
extern int txpow_A;
extern int atten_A;
extern int lagfr_A;
extern int smsep_A;
extern int ercod_A;
extern int agc_stat_A;
extern int lopwr_stat_A;
extern int nbaud_A;
extern int noise_A;
extern int rxrise_A;
extern int intt_A;
extern int txpl_A;
extern int mpinc_A;
extern int mppul_A;
extern int mplgs_A;
extern int nrang_A;
extern int frang_A;
extern int rsep_A;
extern int bmnum_A;
extern int xcf_A;
extern int tfreq_A;
extern int scan_A;
extern int mxpwr_A;
extern int lvmax_A;

extern int noise_stat_A[4];

/* user variables */

extern int usr_resL1_A,
    usr_resL2_A;
extern int usr_resS1_A,
    usr_resS2_A,
    usr_resS3_A;

extern char combf_A[128];

extern int max_atten_B;
extern int prot_atten_B;
extern int rx_wide_B;
extern int rx_narrow_B;
extern int rsep_switch_B;

extern int nave_B;
extern int tsg_id_B;
extern int txpow_B;
extern int atten_B;
extern int lagfr_B;
extern int smsep_B;
extern int ercod_B;
extern int agc_stat_B;
extern int lopwr_stat_B;
extern int nbaud_B;
extern int noise_B;
extern int rxrise_B;
extern int intt_B;
extern int txpl_B;
extern int mpinc_B;
extern int mppul_B;
extern int mplgs_B;
extern int nrang_B;
extern int frang_B;
extern int rsep_B;
extern int bmnum_B;
extern int xcf_B;
extern int tfreq_B;
extern int scan_B;
extern int mxpwr_B;
extern int lvmax_B;

extern int noise_stat_B[4];

/* user variables */

extern int usr_resL1_B,
    usr_resL2_B;
extern int usr_resS1_B,
    usr_resS2_B,
    usr_resS3_B;

extern char combf_B[128];


extern int start_beam,end_beam;
extern int scan_period;
extern int backward;
extern int discretion;


extern int start_freq_A;
extern int start_freq_B;

extern int freq_range_A;
extern int freq_range_B;


extern int xcount_A;
extern int count_A;
extern int xcount_B;
extern int count_B;


extern int day_start_hr;
extern int night_start_hr;                                            
extern int day_start_freq_A;
extern int day_start_freq_B;
 
extern int night_start_freq_A; 
extern int night_start_freq_B; 
 
extern int day_frang_A;
extern int day_frang_B;
 
extern int night_frang_A;
extern int night_frang_B;

extern int day_mpinc_A;     
extern int day_mpinc_A;     
                                              
extern int night_mpinc_B;
extern int night_mpinc_B;


extern int noise_fac;
extern int noise_lim;


extern struct rstable rstable;
extern struct optstr cmdlne;

extern struct tsg_table *tsg_table;

void cmdlne_err(char *txt);
int day_time(void);
