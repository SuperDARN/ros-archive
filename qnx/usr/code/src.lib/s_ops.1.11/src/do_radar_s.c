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
 $Log: do_radar_s.c,v $
 Revision 1.6  2001/06/27 21:10:46  barnes
 Added license tag

 Revision 1.5  2000/09/18 16:14:24  barnes
 Removed debugging code.

 Revision 1.4  2000/09/18 16:10:28  barnes
 Added noise filtering algorithm.

 Revision 1.3  2000/04/17 22:20:30  barnes
 Fixed problem of trying to calculate average of zero ACF's.

 Revision 1.2  2000/04/17 21:26:49  barnes
 Added tests for A/D errors.

 Revision 1.1  2000/03/19 20:33:19  barnes
 Initial revision

 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include "a_d_drive.h"
#include "dio.h"
#include "types.h"
#include "limit.h"
#include "do_radar_s.h"
#include "do_pulse_s.h"
#include "tsg.h"

#include "normalize.h"
#include "average.h"
#include "sum_power.h"
#include "acf.h"

#include "stereo.h"


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
               int *flg_B,int *nave_B
              ) {
 


  
  int i; 
  int abuf,buf;

  int abflg_A;
  int abflg_B;

  int aflg_A=-1;
  int aflg_B=-1;
  int oaflg_A=-1;
  int oaflg_B=-1;

  int num_channel_A,num_channel_B;
  int num_words_A,num_words_B;
  
  clock_t tock;  
  clock_t intt_tick;
  clock_t num_ticks=0;

  int catn_A=0;
  int catn_B=0;
  
  int adcnt_A=0;
  int adcnt_B=0;
  
  int status_A=0;
  int status_B=0;

  *nave_A=0;
  *nave_B=0;

 
 
  intt_tick = (clock_t) intt * CLOCKS_PER_SEC; 		
  intt_tick-=tick;
 
  memset(pwr0_A,0,MAX_RANGE*sizeof(int));
  memset(acfd_A,0,MAX_RANGE*2*LAG_TAB_LEN*sizeof(int));

  memset(pwr0_B,0,MAX_RANGE*sizeof(int));
  memset(acfd_B,0,MAX_RANGE*2*LAG_TAB_LEN*sizeof(int));

  if (xcfd_A !=NULL) memset(xcfd_A,0,MAX_RANGE*2*LAG_TAB_LEN*sizeof(int));
  if (xcfd_B !=NULL) memset(xcfd_B,0,MAX_RANGE*2*LAG_TAB_LEN*sizeof(int));
 
  if (xcfd_A !=NULL) num_channel_A = 4;
  else num_channel_A = 2;
  if (xcfd_B !=NULL) num_channel_B = 4;
  else num_channel_B = 2;


  buf=0;
  *flg_A=0;
  *flg_B=0;
  num_words_A=prm_A->samples*num_channel_A;
  num_words_B=prm_B->samples*num_channel_B;
  
  if (do_pulse_s(debug,buf,
                num_words_A,num_words_B,num_channel_A,num_channel_B,
	        tsg_id_A,tsg_id_B,delay,dio_id,ad_id,reset_id) != 0) {
    *flg_A|=INT_PULSE_FAIL;
    *flg_B|=INT_PULSE_FAIL;
  

    return -1;
  }

  tick=clock();  
  
  while (intt_tick > num_ticks) {
      abuf = buf;
      buf = (buf + 1) % 2 ;
 
  

      status_A=(get_scan_status(ad_id[0])==SCAN_OK);
      status_B=(get_scan_status(ad_id[1])==SCAN_OK);
     
      if (status_A==0) adcnt_A++;
      if (status_B==0) adcnt_B++;
    
      if (do_pulse_s(debug,buf,num_words_A,num_words_B,
                     num_channel_A,num_channel_B,
                     tsg_id_A,tsg_id_B,delay,dio_id,ad_id,reset_id) !=0)  {
         *flg_A|=INT_PULSE_FAIL;
         *flg_B|=INT_PULSE_FAIL;

         return -1;
      }


      if (status_A==1) {
        aflg_A=sum_power(prm_A,mplgs_A,lags_A,pwr0_A,mxpwr_A,
			(int16 *) buf_adr[abuf],num_channel_A,catn_A,
                        bad_range_A,*noise_A,thr,lmt,&abflg_A);
        if (abflg_A==0) {
          (*nave_A)++;
          acf(prm_A,(int16 *) buf_adr[abuf],mplgs_A,
	  	    lags_A,acfd_A,ACF_PART,num_channel_A,catn_A,bad_range_A);

          if (xcfd_A !=NULL) acf(prm_A,(int16 *) buf_adr[abuf],mplgs_A,
		  	     lags_A,xcfd_A,XCF_PART,num_channel_A,catn_A,
                             bad_range_A);
          catn_A=0;
          if  ((aflg_A != -1) && (oaflg_A == aflg_A))  {
            aflg_A = -1;

            if (*atten_A < max_atten_A) {
              normalize(pwr0_A,acfd_A,xcfd_A,prm_A->nrang,mplgs_A); 
              (*atten_A)++;
              *noise_A=*noise_A/ATTEN_STEP;
              if (*noise_A==0) *noise_A=1;
              get_scan_status(ad_id[0]);
              set_channel(dio_id,0);
              set_gain(dio_id,*atten_A);
              catn_A = 1;   
            } else *flg_A|=INT_ATTEN_OVF;
          }
          oaflg_A = aflg_A;
	}
      }

      if (status_B==1) { 
  
        aflg_B=sum_power(prm_B,mplgs_B,lags_B,pwr0_B,mxpwr_B,
			(int16 *) buf_adr[MAX_AD_BUF+abuf],
                        num_channel_B,catn_B,bad_range_B,
                        *noise_B,thr,lmt,&abflg_B);
        if (abflg_B==0) {
          (*nave_B)++;
          acf(prm_B,(int16 *) buf_adr[MAX_AD_BUF+abuf],mplgs_B,
	  	    lags_B,acfd_B,ACF_PART,num_channel_B,catn_B,bad_range_B);



          if (xcfd_B !=NULL) acf(prm_B,(int16 *) buf_adr[MAX_AD_BUF+abuf],
                                 mplgs_B,
		  	     lags_B,xcfd_B,XCF_PART,num_channel_B,catn_B,
                             bad_range_B);
 
  
   
          catn_B=0;
          if  ((aflg_B != -1) && (oaflg_B == aflg_B))  {
            aflg_B = -1;

            if (*atten_B < max_atten_B) {
              normalize(pwr0_B,acfd_B,
                      xcfd_B,prm_B->nrang,mplgs_B); 
              (*atten_B)++;
              *noise_B=*noise_B/ATTEN_STEP;
              if (*noise_B==0) *noise_B=1;
              get_scan_status(ad_id[1]);
              set_channel(dio_id,1);
              set_gain(dio_id,*atten_B);
              catn_B = 1;   
            } else *flg_B|=INT_ATTEN_OVF;
          }
          oaflg_B = aflg_B;
	}
      }
      tock = clock();
      num_ticks = (tock < tick) ? (0xffffffffL - tick) + tock : tock - tick;
 
   } 
  

  
   status_A=(get_scan_status(ad_id[0])==SCAN_OK);
   status_B=(get_scan_status(ad_id[1])==SCAN_OK);
     
   if (status_A==0) adcnt_A++;
   if (status_B==0) adcnt_B++;

  

   
   if (status_A ==1) {
     aflg_A=sum_power(prm_A,mplgs_A,lags_A,pwr0_A,mxpwr_A,
	    (int16 *) buf_adr[buf],num_channel_A,catn_A,bad_range_A,
            *noise_A,thr,lmt,&abflg_A);

     if (abflg_A==0) {
       acf(prm_A,(int16 *) buf_adr[buf],mplgs_A,
	   lags_A,acfd_A,ACF_PART,num_channel_A,catn_A,bad_range_A);

       if (xcfd_A !=NULL) acf(prm_A,(int16 *) buf_adr[MAX_AD_BUF+buf],mplgs_A,
			lags_A,xcfd_A,XCF_PART,num_channel_A,catn_A,
                        bad_range_A);
       (*nave_A)++;
     }
   }
 


   if (status_B==1) {
     aflg_B=sum_power(prm_B,mplgs_B,lags_B,pwr0_B,mxpwr_B,
	    (int16 *) buf_adr[MAX_AD_BUF+buf],num_channel_B,catn_B,
            bad_range_B,*noise_B,thr,lmt,&abflg_B);

     if (abflg_B==0) {

       acf(prm_B,(int16 *) buf_adr[MAX_AD_BUF+buf],mplgs_B,
	     lags_B,acfd_B,ACF_PART,num_channel_B,catn_B,bad_range_B);

       if (xcfd_B !=NULL) acf(prm_B,(int16 *) buf_adr[MAX_AD_BUF+buf],mplgs_B,
			lags_B,xcfd_B,XCF_PART,num_channel_B,catn_B,
                        bad_range_B);
       (*nave_B)++;
     }
   }
   

   if (*nave_A>0) average(pwr0_A,acfd_A,xcfd_A,*nave_A,prm_A->nrang,mplgs_A);
   if (*nave_B>0) average(pwr0_B,acfd_B,xcfd_B,*nave_B,prm_B->nrang,mplgs_B);
  
   if (adcnt_A>MAX_AD_ERR) *flg_A|=INT_AD_FAIL;
   if (adcnt_B>MAX_AD_ERR) *flg_B|=INT_AD_FAIL;


   set_channel(dio_id,0);
   set_gain(dio_id,max_atten_A);
   set_gain(dio_id,1);
   set_gain(dio_id,max_atten_B);


   return 0;
} 

