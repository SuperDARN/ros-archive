/* radar.c
   ======= */

/*
 Copyright © 2001 The Johns Hopkins University/Applied Physics Laboratory.
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
 $Log: radar.c,v $
 Revision 1.6  2001/10/30 20:55:29  barnes
 Fixed bug in propagating noise.

 Revision 1.5  2001/10/15 16:04:38  barnes
 Moved to floating point internals.

 Revision 1.4  2001/06/27 21:10:39  barnes
 Added license tag

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
  int status;
  float fnoise;
 
  fnoise=noise; 
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
      status=do_radar(&tsg_table->buf[tsg_id],tsg_id,mplgs,lags,intt,tick,
		  bad_range,debug,dio_id,ad_id,reset_id,&bufadr,
                  max_atten,mxpwr,&atten,&fnoise,
		  pwr0,acfd,xcfd,&flg,noise_fac,noise_lim,NULL,NULL);
  else status=do_radar(&tsg_table->buf[tsg_id],tsg_id,mplgs,lags,intt,tick,
                       bad_range,debug,dio_id,ad_id,reset_id,&bufadr,
                       max_atten,mxpwr,&atten,&fnoise,pwr0,acfd,NULL,
                       &flg,noise_fac,noise_lim,NULL,NULL);                                       
  noise=fnoise;
  return status; 
} 

 
