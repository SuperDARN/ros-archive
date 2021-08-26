/* global_s.c 
   ========== */

/* 
 $Log: global_s.c,v $
 Revision 1.2  2000/09/18 16:10:28  barnes
 Added noise filtering algorithm.

 Revision 1.1  2000/03/19 20:33:19  barnes
 Initial revision

 */


#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "limit.h"
#include "default.h"
#include "stereo.h"
#include "option.h"
#include "radar_shell.h"

struct rstable rstable;
struct optstr cmdlne;

pid_t dio_id_s;
pid_t ad_id_s[2];
pid_t reset_id_s[2];
int bufsze_s[2];
int bufnum_s[2];
void *bufadr_s[MAX_AD_BUF*2];

int delay_s[2];

/* these are the buffers for the ACF's */

int pwr0_A[MAX_RANGE];
int acfd_A[MAX_RANGE*LAG_TAB_LEN*2];
int xcfd_A[MAX_RANGE*LAG_TAB_LEN*2];

int pwr0_B[MAX_RANGE];
int acfd_B[MAX_RANGE*LAG_TAB_LEN*2];
int xcfd_B[MAX_RANGE*LAG_TAB_LEN*2];

/* these are all global parameters */

int intt=3;
int st_id=0;
int year=0;
int month=0;
int day=0;
int hour=0;
int minut=0;
int sec=0;
int msec=0;
int usec=0;
int cp=50;

/* these are parameters per channel */

int max_atten_A=DEFAULT_MAX_ATTEN;
int prot_atten_A=DEFAULT_PROT_ATTEN;
int rx_wide_A=DEFAULT_RXRISE;
int rx_narrow_A=DEFAULT_RXRISE;
int rsep_switch_A=DEFAULT_RSEP_SWITCH;

int nave_A=0;
int tsg_id_A=0;
int txpow_A=DEFAULT_TXPOW;
int atten_A=0;
int lagfr_A=DEFAULT_LAGFR;
int smsep_A=DEFAULT_SMSEP;
int ercod_A=0;
int agc_stat_A=0;
int lopwr_stat_A=0;
int nbaud_A=0;
int noise_A=0;
int rxrise_A=DEFAULT_RXRISE;
int intt_A=DEFAULT_INTT;
int txpl_A=DEFAULT_TXPL;
int mpinc_A=DEFAULT_MPINC;
int mppul_A=DEFAULT_MPPUL;
int mplgs_A=DEFAULT_MPLGS;
int nrang_A=DEFAULT_NRANG;
int frang_A=DEFAULT_FRANG;
int rsep_A=DEFAULT_RSEP;
int bmnum_A=0;
int xcf_A=0;
int tfreq_A=DEFAULT_TFREQ;
int scan_A=DEFAULT_SCAN;
int mxpwr_A=DEFAULT_MXPWR;
int lvmax_A=DEFAULT_LVMAX;

int noise_stat_A[4];

/* user variables */

int usr_resL1_A,
    usr_resL2_A;
int usr_resS1_A,
    usr_resS2_A,
    usr_resS3_A;

char combf_A[128]={"A SuperDARN"};


int max_atten_B=DEFAULT_MAX_ATTEN;
int prot_atten_B=DEFAULT_PROT_ATTEN;
int rx_wide_B=DEFAULT_RXRISE;
int rx_narrow_B=DEFAULT_RXRISE;
int rsep_switch_B=DEFAULT_RSEP_SWITCH;

int nave_B=0;
int tsg_id_B=0;
int txpow_B=DEFAULT_TXPOW;
int atten_B=0;
int lagfr_B=DEFAULT_LAGFR;
int smsep_B=DEFAULT_SMSEP;
int ercod_B=0;
int agc_stat_B=0;
int lopwr_stat_B=0;
int nbaud_B=0;
int noise_B=0;
int rxrise_B=DEFAULT_RXRISE;
int intt_B=DEFAULT_INTT;
int txpl_B=DEFAULT_TXPL;
int mpinc_B=DEFAULT_MPINC;
int mppul_B=DEFAULT_MPPUL;
int mplgs_B=DEFAULT_MPLGS;
int nrang_B=DEFAULT_NRANG;
int frang_B=DEFAULT_FRANG;
int rsep_B=DEFAULT_RSEP;
int bmnum_B=0;
int xcf_B=0;
int tfreq_B=DEFAULT_TFREQ;
int scan_B=DEFAULT_SCAN;
int mxpwr_B=DEFAULT_MXPWR;
int lvmax_B=DEFAULT_LVMAX;


int start_beam=DEFAULT_START_BEAM,end_beam=DEFAULT_END_BEAM;
int scan_period=DEFAULT_SCAN_PERIOD;
int backward=DEFAULT_BACKWARD;
int discretion=DEFAULT_DISCRETION;


int start_freq_A=DEFAULT_START_FREQ;
int start_freq_B=DEFAULT_START_FREQ;

int freq_range_A=DEFAULT_FREQ_RANGE;
int freq_range_B=DEFAULT_FREQ_RANGE;


int xcount_A=DEFAULT_XCOUNT;
int count_A=0;

int xcount_B=DEFAULT_XCOUNT;
int count_B=0;



int day_start_hr=DEFAULT_DAY_START_HR;
int night_start_hr=DEFAULT_NIGHT_START_HR;                                            
int day_start_freq_A=DEFAULT_START_FREQ;
int day_start_freq_B=DEFAULT_START_FREQ;
 
int night_start_freq_A=DEFAULT_START_FREQ;  
int night_start_freq_B=DEFAULT_START_FREQ;  

int day_frang_A=DEFAULT_FRANG;
int day_frang_B=DEFAULT_FRANG;
 
int night_frang_A=DEFAULT_FRANG;
int night_frang_B=DEFAULT_FRANG;

int day_mpinc_A=DEFAULT_MPINC;     
int day_mpinc_B=DEFAULT_MPINC;     
                                              
int night_mpinc_A=DEFAULT_MPINC;
int night_mpinc_B=DEFAULT_MPINC;


int noise_stat_B[4];

/* user variables */

int usr_resL1_B,
    usr_resL2_B;
int usr_resS1_B,
    usr_resS2_B,
    usr_resS3_B;

int noise_fac=0;
int noise_lim=0;


char combf_B[128]={"A SuperDARN"};

struct tsg_table *tsg_table=NULL;

void cmdlne_err(char *txt) {
  fprintf(stderr,"Option not recognized:%s\n",txt);
}



int day_time() {
  if (day_start_hr < night_start_hr) {
    if ((day_start_hr <= hour) && (hour < night_start_hr)) return 1;
	else return 0;
  } else {
	if ((night_start_hr <= hour) && (hour < day_start_hr)) return 0;
	else return 1;
  }
  return 1;
}
                  

