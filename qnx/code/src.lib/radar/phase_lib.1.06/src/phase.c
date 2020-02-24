/* phase.c
   =======
   Author: R.J.Barnes
*/

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
 $Log: phase.c,v $
 Revision 1.7  2001/10/15 15:39:41  barnes
 Moved to floating point internals.

 Revision 1.6  2001/06/27 20:53:59  barnes
 Added license tag

 Revision 1.5  2001/01/29 18:11:50  barnes
 Added Author Name

 Revision 1.4  2000/04/17 22:19:41  barnes
 Fixed problem of trying to find average of zero ACF's.

 Revision 1.3  2000/04/17 21:18:47  barnes
 Added checks for bad A/D transfers.

 Revision 1.2  1999/06/17 17:27:29  barnes
 Fixed bug in setting attenuators.

 Revision 1.1  1999/03/30 23:55:58  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#include "types.h"
#include "limit.h"
#include "a_d_drive.h"
#include "dio.h"
#include "do_radar.h"

#include "tsg.h"

#include "normalize.h"
#include "average.h"
#include "phase_sum_power.h"
#include "phase_acf.h"

int do_phase(struct tsg_parms *prm,int tsg_id,int mplgs,int *lags,int intt,
	     clock_t tick,int bad_range,int debug,
	     pid_t dio_id,pid_t ad_id,pid_t reset_id,
	     unsigned char **buf_adr,int max_atten,float mxpwr,int *atten,
	     float *noise,float *pwr0,float *acfd,float *xcfd,int *flg,
	     void (*user)(struct tsg_parms *prm,
			  int intt,clock_t tick,int nave,
		          unsigned char *buffer,int bufnum,
			  int num_words,int num_channel,
			  void *usr_data),void *usr_data) {
 
  int nave=0;
  int i; 
  int abuf,buf;
  int aflg=-1, oaflg=-1;

  int num_channel;
  int num_words;
  int dsize;
  int *c_n=NULL;
  float *d_data=NULL;
  float *d_real,*d_imag,*x_real,*x_imag;  
  

  clock_t tock;  
  clock_t intt_tick;
  clock_t num_ticks=0;
  int catn=0; 
   
  int adcnt=0;
  int status=0;
  
  
  intt_tick = (clock_t) intt * CLOCKS_PER_SEC; 		
  intt_tick-=tick; 
  memset(pwr0,0,MAX_RANGE*sizeof(float));
  memset(acfd,0,MAX_RANGE*2*LAG_TAB_LEN*sizeof(float));

  if (xcfd !=NULL) memset(xcfd,0,MAX_RANGE*2*LAG_TAB_LEN*sizeof(float));

  if (xcfd !=NULL) num_channel = 4;
  else num_channel = 2;

  buf=0;
  *flg=0;

  /* generate the normalized code table */
  c_n=malloc(sizeof(int)*prm->mppul);
  if (c_n==0) return -1;
 
  normalize_baud(prm,c_n);

  /* generate decoding array d_data[nchan][np][MAX_RANGE] */

  dsize=num_channel*prm->mppul*MAX_RANGE*20*prm->rsep/3/prm->smsep; 
  d_data=malloc(sizeof(float)*dsize);
  if (d_data==NULL) {
    free(c_n);
    return -1;
  }

  d_real=d_data;
  d_imag=d_data+prm->mppul*MAX_RANGE*20*prm->rsep/3/prm->smsep;
  x_real=d_data+2*prm->mppul*MAX_RANGE*20*prm->rsep/3/prm->smsep;
  x_imag=d_data+2*prm->mppul*MAX_RANGE*20*prm->rsep/3/prm->smsep;

  num_words=prm->samples*num_channel;
  
  if (do_pulse(debug,buf,
               num_words,num_channel,
			   tsg_id,dio_id,ad_id,reset_id) != 0) {
    *flg|=INT_PULSE_FAIL;
    return -1;
  }

  tick=clock();  
  
  while (intt_tick > num_ticks) {
      abuf = buf;
      buf = (buf + 1) % 2 ;

      status=get_scan_status(ad_id)==SCAN_OK;
      if (status==0) adcnt++;
     
      if (do_pulse(debug,buf,num_words,
                 num_channel,tsg_id,dio_id,ad_id,reset_id) !=0)  {
         *flg|=INT_PULSE_FAIL;
         free(d_data);
         free(c_n);
         return -1;
      }
      if (status==1) {
    

        nave++;
    
        /* call the user function */
        if (user !=NULL) (user)(prm,intt,tick,nave,
		                buf_adr[abuf],abuf,num_words,num_channel,
				usr_data);

        phase_decode(prm,buf_adr[abuf],c_n,d_data);
       
        aflg=phase_sum_power(prm,mplgs,lags,pwr0,mxpwr,
			               d_data,num_channel,catn,bad_range);
        phase_acf(prm,d_real,d_imag,d_real,d_imag,mplgs,
		  lags,acfd,catn);
        if (xcfd !=NULL) phase_acf(prm,x_real,x_imag,d_real,d_imag,mplgs,
				   lags,xcfd,catn);
        catn = 0;
 		 
        if  ((aflg != -1) && (oaflg == aflg))  {
          aflg = -1;

          if (*atten < max_atten) {
            normalize(pwr0,acfd,xcfd,prm->nrang,mplgs); 
            (*atten)++;
            *noise=*noise/ATTEN_STEP;
            if (*noise==0) *noise=1;
            get_scan_status(ad_id);
            set_gain(*atten,dio_id);
            catn = 1;   
          } else *flg|=INT_ATTEN_OVF;
       
        }
        oaflg = aflg;
      }
      tock = clock();
      num_ticks = (tock < tick) ? (0xffffffffL - tick) + tock :  
		                                                   tock - tick;																					 			   
   } 
  

   status=get_scan_status(ad_id)==SCAN_OK;
   if (status==0) adcnt++;
   if (status==1) { 
  
   if (user !=NULL) (user)(prm,intt,tick,nave,
		                   buf_adr[buf],buf,num_words,num_channel,
						   usr_data);
     phase_decode(prm,buf_adr[abuf],c_n,d_data);
     aflg=phase_sum_power(prm,mplgs,lags,pwr0,mxpwr,
	                    d_data,num_channel,catn,bad_range);
     phase_acf(prm,d_real,d_imag,d_real,d_imag,mplgs,
	   lags,acfd,catn);
     if (xcfd !=NULL) phase_acf(prm,x_real,x_imag,d_real,d_imag,mplgs,
				        lags,xcfd,catn);
     nave++;
   }

   if (nave>0) average(pwr0,acfd,xcfd,nave,prm->nrang,mplgs);
   set_gain(max_atten,dio_id);
   free(d_data);
   free(c_n);   
   if (adcnt>MAX_AD_ERR) *flg|=INT_AD_FAIL;

   return nave;
} 

