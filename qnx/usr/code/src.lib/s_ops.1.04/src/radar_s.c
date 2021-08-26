/* radar_s.c
   ========== */

/*
 $Log: radar_s.c,v $
 Revision 1.2  2000/09/18 16:10:28  barnes
 Added noise filtering algorithm.

 Revision 1.1  2000/03/19 20:33:19  barnes
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
#include "do_radar_s.h"

#include "default.h"

#include "radar_s.h"
#include "global_s.h"

int bad_range_A=MAX_RANGE;
int do_badlag_A=1;
int mpinc_old_A=DEFAULT_MPINC;
int frang_old_A=DEFAULT_FRANG;
int rsep_old_A=DEFAULT_FRANG;

int bad_range_B=MAX_RANGE;
int do_badlag_B=1;
int mpinc_old_B=DEFAULT_MPINC;
int frang_old_B=DEFAULT_FRANG;
int rsep_old_B=DEFAULT_FRANG;

clock_t tick;


int radar_s(int debug,int *lags_A,int *lags_B) {

  int flg_A,flg_B;
  
  if ((frang_A !=frang_old_A) || (rsep_A !=rsep_old_A) || 
       (mpinc_A !=mpinc_old_A)) {
    do_badlag_A=1;
    frang_old_A=frang_A;
    rsep_old_A=rsep_A;
    mpinc_old_A=mpinc_A;
  }

  if ((frang_B !=frang_old_B) || (rsep_B !=rsep_old_B) || 
       (mpinc_B !=mpinc_old_B)) {
    do_badlag_B=1;
    frang_old_B=frang_B;
    rsep_old_B=rsep_B;
    mpinc_old_B=mpinc_B;
  }


 

  if (do_badlag_A==1) {
   bad_range_A=badlag_zero(&tsg_table->buf[tsg_id_A],mplgs_A,lags_A);
   do_badlag_A=0;
  }

  if (do_badlag_B==1) {
   bad_range_B=badlag_zero(&tsg_table->buf[tsg_id_B],mplgs_B,lags_B);
   do_badlag_B=0;
  }


  tick=0;

  if (xcf_A==1) return do_radar_s(intt,delay_s,tick,debug,
			   dio_id_s,ad_id_s,reset_id_s,&bufadr_s,noise_fac,
                           noise_lim,

                     &tsg_table->buf[tsg_id_A],tsg_id_A,mplgs_A,lags_A,        
                     bad_range_A,max_atten_A,mxpwr_A,&atten_A,&noise_A,
	 			     pwr0_A,acfd_A,xcfd_A,&flg_A,&nave_A,
                     &tsg_table->buf[tsg_id_B],tsg_id_B,mplgs_B,lags_B,        
                     bad_range_B,max_atten_B,mxpwr_B,&atten_B,&noise_B,
                     pwr0_B,acfd_B,xcfd_B,&flg_B,&nave_B);

   return do_radar_s(intt,delay_s,tick,debug,
                     dio_id_s,ad_id_s,reset_id_s,&bufadr_s,
                     noise_fac,noise_lim,

                     &tsg_table->buf[tsg_id_A],tsg_id_A,mplgs_A,lags_A,        
                     bad_range_A,max_atten_A,mxpwr_A,&atten_A,&noise_A,
                     pwr0_A,acfd_A,NULL,&flg_A,&nave_A,
                     &tsg_table->buf[tsg_id_B],tsg_id_B,mplgs_B,lags_B,        
                     bad_range_B,max_atten_B,mxpwr_B,&atten_B,&noise_B,
                     pwr0_B,acfd_B,NULL,&flg_B,&nave_B);

                                        
} 

 





