/* do_fclr_s.c
   =========== */

/*
 $Log: do_fclr_s.c,v $
 Revision 1.1  2000/03/19 20:33:19  barnes
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
#include "do_fclr_s.h"
#include "base_fclr_s.h"
#include "fine_fclr.h"
#include "stereo.h"

int do_fclr_s(struct tsg_table *tab,
            int *tsg_id,int debug,
            int16 **buf_adr,int *buf_size,
            struct freq_table *fptr,
            pid_t dio_id,pid_t *ad_id,pid_t *reset_id,
            clock_t *tick,int *delay,

            int start_freqA,
	    int end_freqA,int step_freqA,
	    int mxpwrA,int max_attenA,		      
	    int *attenA,int *noiseA,
            int *statisticA,int *flgA,int *freqA,

            int start_freqB,
	    int end_freqB,int step_freqB,
	    int mxpwrB,int max_attenB,		      
	    int *attenB,int *noiseB,
            int *statisticB,int *flgB,int *freqB) {

   struct tsg_parms *prm;
   int id;   
   int num_channel;
   int lowest_freqA[NUM_FCLR_FREQ]; 
   int lowest_freqB[NUM_FCLR_FREQ];
   int tock;

   tock=clock();

   num_channel = 2;      /* use only 2 channels of the DT2828 */
   id=tsg_id[0];
   prm=&tab->buf[id];

   if  (base_fclr_s(id,prm->samples,debug,
                  buf_adr,buf_size,num_channel,
                  dio_id,ad_id,reset_id,fptr,delay,
                   
		  start_freqA,end_freqA,step_freqA,
		  lowest_freqA,statisticA,flgA,

                  start_freqB,end_freqB,step_freqB,
	 	  lowest_freqB,statisticB,flgB) !=0) {				    fprintf(stderr,"error...\n");	
     return -1;				
   }
   
   id=tsg_id[1];

   prm=&tab->buf[id];
   set_channel(dio_id,0);
   *freqA=fine_fclr(id,prm->samples,debug,lowest_freqA,start_freqA,
			      num_channel,mxpwrA,max_attenA,buf_adr[0],
				  dio_id,ad_id[0],reset_id[0],
                                  attenA,noiseA,flgA);
   set_channel(dio_id,1);
   *freqB=fine_fclr(id,prm->samples,debug,lowest_freqB,start_freqB,

		    num_channel,mxpwrB,max_attenB,
                    buf_adr[MAX_AD_BUF],
		    dio_id,ad_id[1],reset_id[1],
                    attenB,noiseB,flgB);

  *tick=clock();
  *tick = (*tick < tock) ? (0xffffffffL - tock) + *tick : *tick - tock;

  return 0;
} 



