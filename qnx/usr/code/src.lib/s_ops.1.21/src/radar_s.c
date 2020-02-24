/* radar_s.c
   ========== */

/*
 Copyright � 2001 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 RST-ROS (hereinafter "the Program").
 
 The Program was developed at The Johns Hopkins University/Applied Physics
 Laboratory (JHU/APL) which is the author thereof under the "work made for
 hire" provisions of the copyright law.  
 
 JHU/APL assumes no obligation to provide support of any kind with regard to
 the Program.  This includes no obligation to provide assistance in using the
 Program or to provide updated versions of the Program.
 
 THE PROGRAM AND ITS DOCUMENTATION ARE PROVIDED AS IS AND WITHOUT ANY EXPRESS
 OR IMPLIED WARRANTIES WHATSOEVER.  ALL WARRANTIES INCLUDING, BUT NOT LIMITED
 TO, PERFORMANCE, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 HEREBY DISCLAIMED.  YOU ASSUME THE ENTIRE RISK AND LIABILITY OF USING THE
 PROGRAM TO INCLUDE USE IN COMPLIANCE WITH ANY THIRD PARTY RIGHTS.  YOU ARE
 ADVISED TO TEST THE PROGRAM THOROUGHLY BEFORE RELYING ON IT.  IN NO EVENT
 SHALL JHU/APL BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING, WITHOUT
 LIMITATION, ANY LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, ARISING OUT OF THE USE OR INABILITY TO USE THE
 PROGRAM."
 
 
 
 
 
 
*/

/*
 $Log: radar_s.c,v $
 Revision 1.6  2002/03/25 14:30:02  barnes
 Fixed bad return statement.

 Revision 1.5  2002/02/05 14:37:48  barnes
 Bug fix from Paul Hamadyk to remove tick=0.

 Revision 1.4  2001/10/15 16:26:01  barnes
 Moved to floating point internals.

 Revision 1.3  2001/06/27 21:10:46  barnes
 Added license tag

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

  int status;
  float fnoise_A,fnoise_B;
  int flg_A,flg_B;

	fnoise_A = (float) noise_A;
	fnoise_B = (float) noise_B;

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

  if (xcf_A==1) status=do_radar_s(intt,delay_s,tick,debug,
			   dio_id_s,ad_id_s,reset_id_s,&bufadr_s,noise_fac,
                           noise_lim,

                     &tsg_table->buf[tsg_id_A],tsg_id_A,mplgs_A,lags_A,        
                     bad_range_A,max_atten_A,mxpwr_A,&atten_A,&fnoise_A,
	 			     pwr0_A,acfd_A,xcfd_A,&flg_A,&nave_A,
                     &tsg_table->buf[tsg_id_B],tsg_id_B,mplgs_B,lags_B,        
                     bad_range_B,max_atten_B,mxpwr_B,&atten_B,&fnoise_B,
                     pwr0_B,acfd_B,xcfd_B,&flg_B,&nave_B);

  else status=do_radar_s(intt,delay_s,tick,debug,
                     dio_id_s,ad_id_s,reset_id_s,&bufadr_s,
                     noise_fac,noise_lim,

                     &tsg_table->buf[tsg_id_A],tsg_id_A,mplgs_A,lags_A,        
                     bad_range_A,max_atten_A,mxpwr_A,&atten_A,&fnoise_A,
                     pwr0_A,acfd_A,NULL,&flg_A,&nave_A,
                     &tsg_table->buf[tsg_id_B],tsg_id_B,mplgs_B,lags_B,        
                     bad_range_B,max_atten_B,mxpwr_B,&atten_B,&fnoise_B,
                     pwr0_B,acfd_B,NULL,&flg_B,&nave_B);

  noise_A=fnoise_A;
  noise_B=fnoise_B;

  return status;                                        
} 

 





