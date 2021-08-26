/* global.c
   ======== */

/*
 $Log: global.c,v $
 Revision 1.12  2000/09/18 15:30:22  barnes
 Set the noise filtering to off by default.

 Revision 1.11  2000/08/21 19:49:50  barnes
 Modification to allow noisy samples to be ignored.

 Revision 1.10  1999/10/08 17:25:46  barnes
 Added the ability to calculate some noise statistics.

 Revision 1.9  1999/09/17 13:29:20  barnes
 Added day and night support.

 Revision 1.8  1999/09/17 03:24:39  barnes
 Added command line error routine.

 Revision 1.7  1999/09/17 02:43:13  barnes
 Added some extra global variables.

 Revision 1.6  1999/09/16 21:17:37  barnes
 Added command line to the globally defined variables.

 Revision 1.5  1999/09/16 18:37:01  barnes
 Added user variables.

 Revision 1.4  1999/09/10 00:19:29  barnes
 Moved the location of the shell structure.

 Revision 1.3  1999/09/10 00:08:57  barnes
 Changed the way the shell table is created.

 Revision 1.2  1999/08/02 17:23:42  barnes
 Fixed bug in passing the xcf flag.

 Revision 1.1  1999/03/31 19:57:19  barnes
 Initial revision

 Revision 1.2  1999/03/30 19:47:19  barnes
 Added the get_status function.

 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "limit.h"
#include "default.h"
#include "option.h"
#include "radar_shell.h"

struct rstable rstable;
struct optstr cmdlne;

int max_atten=DEFAULT_MAX_ATTEN;
int prot_atten=DEFAULT_PROT_ATTEN;

int rx_wide=DEFAULT_RXRISE;
int rx_narrow=DEFAULT_RXRISE;
int rsep_switch=DEFAULT_RSEP_SWITCH;

int tsg_id=0;
int st_id=0;
int year=0;
int month=0;
int day=0;
int hour=0;
int minut=0;
int sec=0;
int msec=0;
int usec=0;
int txpow=DEFAULT_TXPOW;
int nave=0;
int atten=0;
int lagfr=DEFAULT_LAGFR;
int smsep=DEFAULT_SMSEP;
int ercod=0;
int agc_stat=0;
int lopwr_stat=0;
int nbaud=0;
int noise=0;
int rxrise=DEFAULT_RXRISE;
int intt=DEFAULT_INTT;
int txpl=DEFAULT_TXPL;
int mpinc=DEFAULT_MPINC;
int mppul=DEFAULT_MPPUL;
int mplgs=DEFAULT_MPLGS;
int nrang=DEFAULT_NRANG;
int frang=DEFAULT_FRANG;
int rsep=DEFAULT_RSEP;
int bmnum=0;
int xcf=0;
int tfreq=DEFAULT_TFREQ;
int scan=DEFAULT_SCAN;
int mxpwr=DEFAULT_MXPWR;
int lvmax=DEFAULT_LVMAX;
int cp=50;

int noise_fac=0;
int noise_lim=0;

int noise_stat[4];

/* user variables */

int usr_resL1,
    usr_resL2;
int usr_resS1,
    usr_resS2,
    usr_resS3;

char combf[128]={"A SuperDARN"};

int pwr0[MAX_RANGE];
int acfd[MAX_RANGE*LAG_TAB_LEN*2];
int xcfd[MAX_RANGE*LAG_TAB_LEN*2];

int bufnum;
int bufsze;
void *bufadr[16];
 
pid_t dio_id;
pid_t ad_id;
pid_t reset_id;

struct tsg_table *tsg_table=NULL;

int start_beam=DEFAULT_START_BEAM,end_beam=DEFAULT_END_BEAM;
int scan_period=DEFAULT_SCAN_PERIOD;
int backward=DEFAULT_BACKWARD;
int discretion=DEFAULT_DISCRETION;

int start_freq=DEFAULT_START_FREQ;
int freq_range=DEFAULT_FREQ_RANGE;
int xcount=DEFAULT_XCOUNT;
int count=0;


int day_start_hr=DEFAULT_DAY_START_HR;
int night_start_hr=DEFAULT_NIGHT_START_HR;                                            
int day_start_freq=DEFAULT_START_FREQ; 
int night_start_freq=DEFAULT_START_FREQ;  
int day_frang=DEFAULT_FRANG; 
int night_frang=DEFAULT_FRANG;
int day_mpinc=DEFAULT_MPINC;                                                   
int night_mpinc=DEFAULT_MPINC;


   
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
                  



void cmdlne_err(char *txt) {
  fprintf(stderr,"Option not recognized:%s\n",txt);
}
