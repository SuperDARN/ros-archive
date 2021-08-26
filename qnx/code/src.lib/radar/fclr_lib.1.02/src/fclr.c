/* fclr.c
   ======= */

/*
 $Log: fclr.c,v $
 Revision 1.2  1999/10/08 17:24:00  barnes
 Added the ability to pass back statistics to the caller.

 Revision 1.1  1999/03/25 21:04:05  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#include "types.h"
#include "limit.h"
#include "do_radar.h"
#include "dio.h"
#include "a_d_drive.h" 
#include "freq_lib.h"

#include "tsg.h"
#include "do_fclr.h"
#include "base_fclr.h"
#include "fine_fclr.h"

int do_fclr(struct tsg_table *tab,int *tsg_id,int debug,int start_freq,
		    int end_freq,int step_freq,int16 *buf_adr,int buf_size,
			int mxpwr,int max_atten,
			struct freq_table *fptr,
			pid_t dio_id,pid_t ad_id,
	        pid_t reset_id,int *atten,int *noise,
            int *statistic,
            clock_t *tick,int *flg) {

   struct tsg_parms *prm;
   int id;   
   int num_channel;
   int lowest_freq[NUM_FCLR_FREQ]; 

   int freq;
   int tock;

   tock=clock();

   num_channel = 2;      /* use only 2 channels of the DT2828 */
   id=tsg_id[0];
   prm=&tab->buf[id];
   if  (base_fclr(id,prm->samples,debug,
				  start_freq,end_freq,step_freq,
				  buf_adr,buf_size,fptr,lowest_freq,statistic,flg,
                  num_channel,dio_id,ad_id,reset_id) !=0) {					
     return -1;				
   }

   id=tsg_id[1];

   prm=&tab->buf[id];
   freq=fine_fclr(id,prm->samples,debug,lowest_freq,start_freq,
			      num_channel,mxpwr,max_atten,buf_adr,
				  dio_id,ad_id,reset_id,atten,noise,flg);
  
  *tick=clock();
  *tick = (*tick < tock) ? (0xffffffffL - tock) + *tick : *tick - tock;


  return freq;
} 
