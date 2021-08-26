/* global.h
   ======== */

#define MAX_TSG 32

extern int max_atten;
extern int prot_atten;

extern int rx_wide;
extern int rx_narrow;
extern int rsep_switch;

extern int tsg_id;
extern int st_id;
extern int year;
extern int month;
extern int day;
extern int hour;
extern int minut;
extern int sec;
extern int msec;
extern int usec;
extern int txpow;
extern int nave;
extern int atten;
extern int lagfr;
extern int smsep;
extern int ercod;
extern int agc_stat;
extern int lopwr_stat;
extern int nbaud;
extern int noise;
extern int rxrise;
extern int intt;
extern int txpl;
extern int mpinc;
extern int mppul;
extern int mplgs;
extern int nrang;
extern int frang;
extern int rsep;
extern int bmnum;
extern int xcf;
extern int tfreq;
extern int scan;
extern int mxpwr;
extern int lvmax;

extern int noise_fac;
extern int noise_lim;

extern int noise_stat[4];

/* user variables */

extern int usr_resL1,
    usr_resL2;
extern int usr_resS1,
    usr_resS2,
    usr_resS3;



extern int cp;
extern char combf[128];

extern int pwr0[MAX_RANGE];
extern int acfd[MAX_RANGE*LAG_TAB_LEN*2];
extern int xcfd[MAX_RANGE*LAG_TAB_LEN*2];

extern int bufnum;
extern int bufsze;
extern void *bufadr[16];

extern pid_t dio_id;
extern pid_t ad_id;
extern pid_t reset_id;

extern struct tsg_table *tsg_table;

extern int start_beam,end_beam;
extern int scan_period;
extern int backward;
extern int discretion; 

extern int start_freq;
extern int freq_range;
extern int xcount,count;

extern struct rstable rstable;
extern struct optstr cmdlne;

extern int day_start_hr;
extern int night_start_hr;                                            
extern int day_start_freq; 
extern int night_start_freq;  
extern int day_frang; 
extern int night_frang;
extern int day_mpinc;
extern int night_mpinc;



void cmdlne_err(char *txt);
int day_time();
