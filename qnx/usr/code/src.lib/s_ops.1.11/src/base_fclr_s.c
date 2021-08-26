/* base_fclr_s.c
   ============= */

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
 $Log: base_fclr_s.c,v $
 Revision 1.2  2001/06/27 21:10:46  barnes
 Added license tag

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
#include "sum_product.h"
#include "stereo.h"

int base_fclr_s(int tsg_id,int samples,int debug,
                int16 **buf_adr,int *buf_size,
                int num_channel,
                pid_t dio_id,pid_t *ad_id,pid_t *reset_id,
                struct freq_table *fptr,int *delay,
                int start_freqA,int end_freqA,int step_freqA,
                int *lowest_freqA,int *statisticA,int *flgA,
                int start_freqB,int end_freqB,int step_freqB,
                int *lowest_freqB,int *statisticB,int *flgB) {

 
   int num_wordsA,num_wordsB;
   int input_word_size;

   
   int status;
     
   int lowest_pwrA[NUM_FCLR_FREQ];  /* corresponding power of lowest_freq */
   int lowest_pwrB[NUM_FCLR_FREQ];  /* corresponding power of lowest_freq */

 
   char seq_id[2];
   int atten;
   unsigned int time_out;
   int freq;
   int max_power;	/* max power in the noise */
   int ave_power;	/* average power */
   int dterr;
   int i,j,f;

   int *fadr[2];
   int fnum[2];
   
   int frq_numA,frq_numB;
   int *frq_tableA=NULL;
   int *frq_tableB=NULL;

   int *pwr_tableA=NULL;
   int *pwr_tableB=NULL;
   int16 *buf;

   int pwr_sum=0;

   *flgA=0;
   *flgB=0;
 
   seq_id[0]=tsg_id;
   seq_id[1]=tsg_id;

 
   /* build an array of frequencies */
   /* zeroes the lowest freq buffer */

   memset(lowest_freqA,0,NUM_FCLR_FREQ*sizeof(int16));
   memset(lowest_freqB,0,NUM_FCLR_FREQ*sizeof(int16));
   

   /* assign lowest_pwr with the maximum number */
   for(i = 0; i< NUM_FCLR_FREQ; i++) {
     lowest_pwrA[i] = MAX_LONG;
     lowest_pwrB[i] = MAX_LONG;
   }

   /* claim memory to store the frequency table */
   if ((frq_tableA=malloc(sizeof(int)+
       sizeof(int)*(end_freqA-start_freqA)/step_freqA)) ==NULL) {
      return -1;
   }
   if ((frq_tableB=malloc(sizeof(int)+
       sizeof(int)*(end_freqB-start_freqB)/step_freqB)) ==NULL) {
      return -1;
   }

  

   frq_numA=0;
   frq_numB=0;
 
   for (freq=start_freqA;freq<=end_freqA;freq+=step_freqA) {
       
       if (test_freq(fptr,freq)==0) {
       frq_tableA[frq_numA]=freq;
       frq_numA++;
     }		 
   }

   

  if(frq_numA == 0) {
      free(frq_tableA);
      return -1;
   }


   for (freq=start_freqB;freq<=end_freqB;freq+=step_freqB) {
       if (test_freq(fptr,freq)==0) {
       frq_tableB[frq_numB]=freq;
       frq_numB++;
     }		 
   }


   if(frq_numB == 0) {
      free(frq_tableB);
      free(frq_tableA);
      return -1;
   }



   if ((pwr_tableA=malloc(sizeof(int)*frq_numA))==NULL) {
      free(frq_tableA);
      free(frq_tableB);
      return -1;
   }

  if ((pwr_tableB=malloc(sizeof(int)*frq_numB))==NULL) {
      free(frq_tableA);
      free(frq_tableB);
      free(pwr_tableB);
      return -1;
   }

   num_wordsA = samples  * num_channel * frq_numA;
   num_wordsB = samples  * num_channel * frq_numB;

   /* check to see if the number of words exceed the buffer limit */

   input_word_size = (int) (buf_size[0] / sizeof(int16));
   if (num_wordsA > input_word_size ) {
      frq_numA = (input_word_size / (samples*num_channel)) - 1;
      num_wordsA = samples*num_channel * frq_numA;
      *flgA|=FCLR_BUF_OVF;
   }

   input_word_size = (int) (buf_size[1] / sizeof(int16));
   if (num_wordsB > input_word_size ) {
      frq_numB = (input_word_size / (samples*num_channel)) - 1;
      num_wordsB = samples*num_channel * frq_numB;
      *flgB|=FCLR_BUF_OVF;
   }

       
   /* turn off all attenuator */

   atten = ATTEN_OFF;
   
   if ((set_channel(dio_id,0) !=0) || (set_gain(dio_id,atten) != 0)) {
      /* failed to set gain */
      free(frq_tableA);
      free(frq_tableB);
      free(pwr_tableA);
      free(pwr_tableB);
      *flgA|=FCLR_GAIN_FAIL;
      return -1;
   }

   if ((set_channel(dio_id,1) !=0) || (set_gain(dio_id,atten) != 0)) {
      /* failed to set gain */
      free(frq_tableA);
      free(frq_tableB);
      free(pwr_tableA);
      free(pwr_tableB);
      *flgB|=FCLR_GAIN_FAIL;
      return -1;
   }


 
   /* calculate how much time out to wait for response in send_fclr 

      NOTE: This code appears to be redundant?

      time_out = (((MIN_SMSEP_STEP * samples) / 1000) * frq_numA / 50;
      time_out++;

   */


   dterr = 0;     /* initialize dt_error count */

   if (do_scan(ad_id[0],0,num_wordsA,debug,num_channel) !=0) {
     /* failed to set scan */
     free(frq_tableA);
     free(frq_tableB);
     free(pwr_tableA);
     free(pwr_tableB);
     *flgA|=FCLR_SCAN_FAIL;
     return -1;
   }

   if (do_scan(ad_id[1],0,num_wordsB,debug,num_channel) !=0) {
     /* failed to set scan */
     free(frq_tableA);
     free(frq_tableB);
     free(pwr_tableA);
     free(pwr_tableB);
     *flgB|=FCLR_SCAN_FAIL;
     return -1;
   }



   /* this is the stereo version of the send_fclr routine: */


   fnum[0]=frq_numA;
   fnum[1]=frq_numB;
   fadr[0]=frq_tableA;
   fadr[1]=frq_tableB;
   
   if ((status=send_fclr(dio_id,1,seq_id,delay,fnum,fadr)) != 0) { 
     if (status !=FREQ_LOCAL) {
       free(frq_tableA);
       free(frq_tableB);
       free(pwr_tableA);
       free(pwr_tableB);
       
       scan_reset(ad_id[0],reset_id[0]);
       scan_reset(ad_id[1],reset_id[1]);

       *flgA|=FCLR_FCLR_FAIL;
       *flgB|=FCLR_FCLR_FAIL;
	   return -1;
	 } else {
       
       *flgA|=FCLR_LOCAL_MODE;
       *flgB|=FCLR_LOCAL_MODE;
     }
   }


   if (get_scan_status(ad_id[0]) !=0) {
      /* scan failed to complete */
      free(frq_tableA);
      free(frq_tableB);
      free(pwr_tableA);
      free(pwr_tableB);
      *flgA|=FCLR_AD_FAIL;
      return -1;
   }


   if (get_scan_status(ad_id[1]) !=0) {
      /* scan failed to complete */
      free(frq_tableA);
      free(frq_tableB);
      free(pwr_tableA);
      free(pwr_tableB);
      *flgB|=FCLR_AD_FAIL;
      return -1;
   }

 
   buf=buf_adr[0];
   for (f=0;f < frq_numA;f++) {
     sum_product(buf,&ave_power,samples,&max_power);

     pwr_tableA[f]=ave_power;
     pwr_sum+=ave_power;

     /* check if ave_power is one of the lowest power seen so far */
  
     if (ave_power < lowest_pwrA[NUM_FCLR_FREQ-1]) {
       for(i = 0; i < NUM_FCLR_FREQ; i++) 
         if (ave_power < lowest_pwrA[i]) break;
       j = NUM_FCLR_FREQ - 1;

       /* move everything down one notch */
       while(j > i ) {
         lowest_pwrA[j] = lowest_pwrA[j-1];
         lowest_freqA[j] = lowest_freqA[j-1];
         j--;
       }   
	   lowest_pwrA[i] = ave_power; 
       lowest_freqA[i] = frq_tableA[f];
      }
      buf = buf + (samples * num_channel);
   } 

  if (statisticA !=NULL) {
     /* calculate the power statistics */
     statisticA[0]=pwr_sum/frq_numA; /* find the average power */
     
   }

   pwr_sum=0;
   buf=buf_adr[MAX_AD_BUF];
   for (f=0;f < frq_numB;f++) {
     sum_product(buf,&ave_power,samples,&max_power);

     pwr_tableB[f]=ave_power;
     pwr_sum+=ave_power;

     /* check if ave_power is one of the lowest power seen so far */
  
     if (ave_power < lowest_pwrB[NUM_FCLR_FREQ-1]) {
       for(i = 0; i < NUM_FCLR_FREQ; i++) 
         if (ave_power < lowest_pwrB[i]) break;
       j = NUM_FCLR_FREQ - 1;

       /* move everything down one notch */
       while(j > i ) {
         lowest_pwrB[j] = lowest_pwrB[j-1];
         lowest_freqB[j] = lowest_freqB[j-1];
         j--;
       }   
	   lowest_pwrB[i] = ave_power; 
       lowest_freqB[i] = frq_tableB[f];
      }
      buf = buf + (samples * num_channel);
   } 



   if (statisticB !=NULL) {
     /* calculate the power statistics */
     statisticB[0]=pwr_sum/frq_numB; /* find the average power */
     
   }


   free(pwr_tableA);
   free(pwr_tableB);
   free(frq_tableA);
   free(frq_tableB);
   return 0;
} 

