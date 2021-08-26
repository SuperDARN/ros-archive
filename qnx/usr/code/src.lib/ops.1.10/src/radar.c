/* radar.c
   ======= */

/*
 $Log: radar.c,v $
 Revision 1.3  2000/08/21 19:49:50  barnes
 Modification to allow noisy samples to be ignored.

 Revision 1.2  1999/08/02 17:23:42  barnes
 Fixed bug in passing the xcf flag.

 Revision 1.1  1999/03/31 19:58:35  barnes
 Initial revision

 Revision 1.2  1999/03/29 14:53:10  barnes
 Modified the syntax for the user call.

 Revision 1.1  1999/03/25 20:04:30  barnes
 Initial revision

 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cnv_time.h"
#include "types.h"
#include "limit.h"
#include "message.h"
#include "a_d_drive.h"
#include "dio.h"
#include "tsg.h"
#include "do_radar.h"

#include "default.h"

#include "radar.h"
#include "global.h"

int bad_range=MAX_RANGE;
int do_badlag=1;
int mpinc_old=DEFAULT_MPINC;
int frang_old=DEFAULT_FRANG;
int rsep_old=DEFAULT_FRANG;

clock_t tick;

extern pid_t dio_id;
extern pid_t ad_id;
extern pid_t reset_id;

int radar(int debug,int *lags) {

  int flg;
  
  if ((frang !=frang_old) || (rsep !=rsep_old) || (mpinc !=mpinc_old)) {
    do_badlag=1;
    frang_old=frang;
    rsep_old=rsep;
    mpinc_old=mpinc;
  }
  if (do_badlag==1) {
   bad_range=badlag_zero(&tsg_table->buf[tsg_id],mplgs,lags);
   do_badlag=0;
  }

  if (xcf==1) 
    return do_radar(&tsg_table->buf[tsg_id],tsg_id,mplgs,lags,intt,tick,
		  bad_range,debug,
		  dio_id,ad_id,reset_id,&bufadr,max_atten,mxpwr,&atten,&noise,
		  pwr0,acfd,xcfd,&flg,noise_fac,noise_lim,NULL,NULL);
  return do_radar(&tsg_table->buf[tsg_id],tsg_id,mplgs,lags,intt,tick,        
          bad_range,debug,                                                      
          dio_id,ad_id,reset_id,&bufadr,max_atten,mxpwr,&atten,&noise,          
          pwr0,acfd,NULL,&flg,noise_fac,noise_lim,NULL,NULL);                                       
  
} 

 
