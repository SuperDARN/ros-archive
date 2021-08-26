/* radar.c
   ======= */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include "a_d_drive.h"
#include "dio.h"
#include "types.h"
#include "limit.h"
#include "do_radar.h"

#include "tsg.h"

#include "normalize.h"
#include "average.h"
#include "sum_power.h"
#include "acf.h"

int do_radar(struct tsg_parms *prm,int tsg_id,int mplgs,int *lags,int intt,
			 clock_t tick,int bad_range,int debug,
			 pid_t dio_id,pid_t ad_id,pid_t reset_id,
			 unsigned char **buf_adr,int max_atten,int mxpwr,int *atten,
			 int *noise,int *pwr0,int *acfd,int *xcfd,int *flg,
	                 int thr,int lmt,
			 void (*user)(struct tsg_parms *prm,
						  int intt,clock_t tick,int nave,
		                  unsigned char *buffer,int bufnum,
						  int num_words,int num_channel,
						  void *usr_data),void *usr_data) {
 
  int abflg;
  int nave=0;
  int i; 
  int abuf,buf;
  int aflg=-1, oaflg=-1;

  int num_channel;
  int num_words;
  
  clock_t tock;  
  clock_t intt_tick;
  clock_t num_ticks=0;
  int catn=0; 
  int adcnt=0;
  int status=0;
     
  intt_tick = (clock_t) intt * CLOCKS_PER_SEC; 		
  intt_tick-=tick; 
  memset(pwr0,0,MAX_RANGE*sizeof(int));
  memset(acfd,0,MAX_RANGE*2*LAG_TAB_LEN*sizeof(int));

  if (xcfd !=NULL) memset(xcfd,0,MAX_RANGE*2*LAG_TAB_LEN*sizeof(int));

  if (xcfd !=NULL) num_channel = 4;
  else num_channel = 2;

  buf=0;
  *flg=0;

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
         return -1;
      }
      if (status==1) {
     
     
        /* call the user function */
        if (user !=NULL) (user)(prm,intt,tick,nave,
		                      buf_adr[abuf],abuf,num_words,num_channel,
						      usr_data);

        aflg=sum_power(prm,mplgs,lags,pwr0,mxpwr,
		       (int16 *) buf_adr[abuf],num_channel,catn,bad_range,
                       *noise,thr,lmt,&abflg);

        if (abflg==0) {

          nave++;
          acf(prm,(int16 *) buf_adr[abuf],mplgs,
	  	    lags,acfd,ACF_PART,num_channel,catn,bad_range);
          if (xcfd !=NULL) acf(prm,(int16 *) buf_adr[abuf],mplgs,
			     lags,xcfd,XCF_PART,num_channel,catn,bad_range);
          catn = 0;
 		 
          if  ((aflg != -1) && (oaflg == aflg))  {
            aflg = -1;

            if (*atten < max_atten) {
              normalize(pwr0,acfd,xcfd,prm->nrang,mplgs); 
              (*atten)++;
              *noise=*noise/ATTEN_STEP;
              if (*noise==0) *noise=1;
              get_scan_status(ad_id);
              set_gain(dio_id,*atten);
              catn = 1;   
            } else *flg|=INT_ATTEN_OVF;
       
          }
          oaflg = aflg;
	}
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
     aflg=sum_power(prm,mplgs,lags,pwr0,mxpwr,
	           (int16 *) buf_adr[buf],num_channel,catn,bad_range,*noise,
                   thr,lmt,&abflg);
     if (abflg==0) {
        acf(prm,(int16 *) buf_adr[buf],mplgs,
	   lags,acfd,ACF_PART,num_channel,catn,bad_range);
        if (xcfd !=NULL) acf(prm,(int16 *) buf_adr[buf],mplgs,
			  lags,xcfd,XCF_PART,num_channel,catn,bad_range);
        nave++;
     }
   }
   if (adcnt>MAX_AD_ERR) *flg|=INT_AD_FAIL;

   if (nave>0) average(pwr0,acfd,xcfd,nave,prm->nrang,mplgs);
   set_gain(dio_id,max_atten);
 
   return nave;
} 

