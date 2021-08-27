/* radar.c
   ======= */

/*
 $Log: do_radar_s.c,v $
 Revision 1.1  2000/03/19 20:33:19  barnes
 Initial revision

 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
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
               unsigned char **buf_adr,           

               struct tsg_parms *prm_A,int tsg_id_A,int mplgs_A,
               int *lags_A,int bad_range_A,
	       int max_atten_A,int mxpwr_A,
               int *atten_A,int *noise_A,
               int *pwr0_A,int *acfd_A,int *xcfd_A,
               int *flg_A,

               struct tsg_parms *prm_B,int tsg_id_B,int mplgs_B,
               int *lags_B,int bad_range_B,
	       int max_atten_B,int mxpwr_B,
               int *atten_B,int *noise_B,
               int *pwr0_B,int *acfd_B,int *xcfd_B,
               int *flg_B) {
 
  int nave=0;
  int i; 
  int abuf,buf;

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
 
  

      get_scan_status(ad_id[0]);
      get_scan_status(ad_id[1]);
     
      if (do_pulse_s(debug,buf,num_words_A,num_words_B,
                     num_channel_A,num_channel_B,
                     tsg_id_A,tsg_id_B,delay,dio_id,ad_id,reset_id) !=0)  {
         *flg_A|=INT_PULSE_FAIL;
         *flg_B|=INT_PULSE_FAIL;

         return -1;
      }

      nave++;

    
      aflg_A=sum_power(prm_A,mplgs_A,lags_A,pwr0_A,mxpwr_A,
			(int16 *) buf_adr[abuf],num_channel_A,catn_A,
                        bad_range_A);
      aflg_B=sum_power(prm_B,mplgs_B,lags_B,pwr0_B,mxpwr_B,
			(int16 *) buf_adr[MAX_AD_BUF+abuf],
                        num_channel_B,catn_B,bad_range_B);

     acf(prm_A,(int16 *) buf_adr[abuf],mplgs_A,
		  lags_A,acfd_A,ACF_PART,num_channel_A,catn_A,bad_range_A);
      acf(prm_B,(int16 *) buf_adr[MAX_AD_BUF+abuf],mplgs_B,
		  lags_B,acfd_B,ACF_PART,num_channel_B,catn_B,bad_range_B);


      if (xcfd_A !=NULL) acf(prm_A,(int16 *) buf_adr[abuf],mplgs_A,
			   lags_A,xcfd_A,XCF_PART,num_channel_A,catn_A,
                           bad_range_A);
      if (xcfd_B !=NULL) acf(prm_B,(int16 *) buf_adr[MAX_AD_BUF+abuf],mplgs_B,
			   lags_B,xcfd_B,XCF_PART,num_channel_B,catn_B,
                           bad_range_B);
 
  
      catn_A=0;
      catn_B=0;
  
      if  ((aflg_A != -1) && (oaflg_A == aflg_A))  {
        aflg_A = -1;

        if (*atten_A < max_atten_A) {
          normalize(pwr0_A,acfd_A,xcfd_A,prm_A->nrang,mplgs_A); 
          *atten_A++;
          *noise_A=*noise_A/ATTEN_STEP;
          if (*noise_A==0) *noise_A=1;
          get_scan_status(ad_id[0]);
          set_channel(dio_id,0);
          set_gain(dio_id,*atten_A);
          catn_A = 1;   
        } else *flg_A|=INT_ATTEN_OVF;
      }

      if  ((aflg_B != -1) && (oaflg_B == aflg_B))  {
        aflg_B = -1;

        if (*atten_B < max_atten_B) {
          normalize(pwr0_B,acfd_B,
                    xcfd_B,prm_B->nrang,mplgs_B); 
          *atten_B++;
          *noise_B=*noise_B/ATTEN_STEP;
          if (*noise_B==0) *noise_B=1;
          get_scan_status(ad_id[1]);
          set_channel(dio_id,1);
          set_gain(dio_id,*atten_B);
          catn_B = 1;   
        } else *flg_B|=INT_ATTEN_OVF;
      }


      oaflg_A = aflg_A;
      oaflg_B = aflg_B;
      tock = clock();
      num_ticks = (tock < tick) ? (0xffffffffL - tick) + tock :  
		                                                   tock - tick;																					 			   
   } 
   ++nave;

   get_scan_status(ad_id[0]);
   get_scan_status(ad_id[1]);


   aflg_A=sum_power(prm_A,mplgs_A,lags_A,pwr0_A,mxpwr_A,
	    (int16 *) buf_adr[buf],num_channel_A,catn_A,bad_range_A);
   aflg_B=sum_power(prm_B,mplgs_B,lags_B,pwr0_B,mxpwr_B,
	    (int16 *) buf_adr[MAX_AD_BUF+buf],num_channel_B,catn_B,bad_range_B);

   acf(prm_A,(int16 *) buf_adr[buf],mplgs_A,
	   lags_A,acfd_A,ACF_PART,num_channel_A,catn_A,bad_range_A);

   acf(prm_B,(int16 *) buf_adr[MAX_AD_BUF+buf],mplgs_B,
	   lags_B,acfd_B,ACF_PART,num_channel_B,catn_B,bad_range_B);


   if (xcfd_A !=NULL) acf(prm_A,(int16 *) buf_adr[MAX_AD_BUF+buf],mplgs_A,
			lags_A,xcfd_A,XCF_PART,num_channel_A,catn_A,
                        bad_range_A);
   if (xcfd_B !=NULL) acf(prm_B,(int16 *) buf_adr[MAX_AD_BUF+buf],mplgs_B,
			lags_B,xcfd_B,XCF_PART,num_channel_B,catn_B,
                        bad_range_B);
 


   average(pwr0_A,acfd_A,xcfd_A,nave,prm_A->nrang,mplgs_A);
   average(pwr0_B,acfd_B,xcfd_B,nave,prm_B->nrang,mplgs_B);


   set_channel(dio_id,0);
   set_gain(dio_id,max_atten_A);
   set_gain(dio_id,1);
   set_gain(dio_id,max_atten_B);
   
   return nave;
} 

