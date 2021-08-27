/* base_fclr.c
   =========== */

/*
 $Log: base_fclr.c,v $
 Revision 1.4  2000/03/17 01:10:47  barnes
 Fixed bug in implicitly setting the timing sequence.

 Revision 1.3  2000/03/17 00:54:30  barnes
 Modified the clear frequency search to work in stereo mode.

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
#include "sum_product.h"

int base_fclr(int tsg_id,int samples,int debug,int start_freq,
			  int end_freq,
			  int step_freq,
              int16 *buf_adr,int buf_size,
			  struct freq_table *fptr,
              int *lowest_freq,int *statistic,int *flg,
			  int num_channel,pid_t dio_id,pid_t ad_id,pid_t reset_id) {

 
   int num_words;
   int input_word_size;

   
   int status;
     
   int lowest_pwr[NUM_FCLR_FREQ];  /* corresponding power of lowest_freq */
  
   char seq_id;
   int atten;
   unsigned int time_out;
   int freq;
   int max_power;	/* max power in the noise */
   int ave_power;	/* average power */
   int dterr;
   int i,j,f;
   
   int frq_num;
   int *frq_table=NULL;
   int *pwr_table=NULL;
   int pwr_sum=0;

   seq_id=tsg_id;
   *flg=0;
   
   /* build an array of frequencies */
   /* zeroes the lowest freq buffer */
   memset(lowest_freq,0,NUM_FCLR_FREQ*sizeof(int16));
   
   /* assign lowest_pwr with the maximum number */
   for(i = 0; i< NUM_FCLR_FREQ; i++)
     lowest_pwr[i] = MAX_LONG;

   /* claim memory to store the frequency table */
   if ((frq_table=malloc(sizeof(int)+
       sizeof(int)*(end_freq-start_freq)/step_freq)) ==NULL) {
      return -1;
   }

   frq_num=0;
 
   for (freq=start_freq;freq<=end_freq;freq+=step_freq) {
       if (test_freq(fptr,freq)==0) {
       frq_table[frq_num]=freq;
       frq_num++;
     }		 
   }


   if(frq_num == 0) {
      free(frq_table);
      return -1;
   }

   if ((pwr_table=malloc(sizeof(int)*frq_num))==NULL) {
      free(frq_table);
      return -1;
   }

   num_words = samples  * num_channel * frq_num;

   /* check to see if the number of words exceed the buffer limit */

   input_word_size = (int) (buf_size / sizeof(int16));
   if (num_words > input_word_size ) {
      frq_num = (input_word_size / (samples*num_channel)) - 1;
      num_words = samples*num_channel * frq_num;
      *flg|=FCLR_BUF_OVF;
   }
       
   /* turn off all attenuator */
   atten = ATTEN_OFF;
   if (set_gain(dio_id,atten) != 0) {
      /* failed to set gain */
      free(frq_table);
      *flg|=FCLR_GAIN_FAIL;
      return -1;
   }
 
   /* calculate how much time out to wait for response in send_fclr */

   time_out = (((MIN_SMSEP_STEP * samples) / 1000) * frq_num) / 50;
   time_out++;

   dterr = 0;     /* initialize dt_error count */

   if (do_scan(ad_id,0,num_words,debug,num_channel) !=0) {
     /* failed to set scan */
     free(frq_table);
     *flg|=FCLR_SCAN_FAIL;
     return -1;
   }

   if ((status=send_fclr(dio_id,0,&seq_id,NULL,&frq_num,&frq_table)) != 0) { 
     if (status !=FREQ_LOCAL) {
       free(frq_table);
       /* cannot do fclr */
       scan_reset(ad_id,reset_id);
       *flg|=FCLR_FCLR_FAIL;
	   return -1;
	 } else {
       /* frequency synthesizer in local mode */
       *flg|=FCLR_LOCAL_MODE;
     }
   }
   if (get_scan_status(ad_id) !=0) {
      /* scan failed to complete */
      free(frq_table);
      *flg|=FCLR_AD_FAIL;
      return -1;
   }


   for (f=0;f < frq_num;f++) {
     sum_product(buf_adr,&ave_power,samples,&max_power);

     pwr_table[f]=ave_power;
     pwr_sum+=ave_power;

     /* check if ave_power is one of the lowest power seen so far */
  
     if (ave_power < lowest_pwr[NUM_FCLR_FREQ-1]) {
       for(i = 0; i < NUM_FCLR_FREQ; i++) 
         if (ave_power < lowest_pwr[i]) break;
       j = NUM_FCLR_FREQ - 1;

       /* move everything down one notch */
       while(j > i ) {
         lowest_pwr[j] = lowest_pwr[j-1];
         lowest_freq[j] = lowest_freq[j-1];
         j--;
       }   
	   lowest_pwr[i] = ave_power; 
       lowest_freq[i] = frq_table[f];
      }
      buf_adr = buf_adr + (samples * num_channel);
   } 

   if (statistic !=NULL) {
     /* calculate the power statistics */

     statistic[0]=pwr_sum/frq_num; /* find the average power */
     
   }

   free(pwr_table);
   free(frq_table);
   return 0;
} 

