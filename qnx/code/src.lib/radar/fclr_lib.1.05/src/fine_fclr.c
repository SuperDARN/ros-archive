/* fine_fclr.c
   ===========
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
 $Log: fine_fclr.c,v $
 Revision 1.6  2001/10/15 15:50:25  barnes
 Changed internals to floating point.

 Revision 1.5  2001/06/27 20:53:22  barnes
 Added license tag

 Revision 1.4  2001/01/29 18:11:49  barnes
 Added Author Name

 Revision 1.3  2000/04/17 21:15:04  barnes
 Added check for bad A/D transfer.

 Revision 1.2  1999/06/17 17:25:16  barnes
 Fixed bug in setting the attenuators.

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

#include "tsg.h"
#include "do_fclr.h"
#include "sum_product.h"

int fine_fclr(int tsg_id,int samples,int debug,int *lowest_freq,
	      int start_freq,int num_channel,float mxpwr,int max_atten,
	      int16 *buf_adr,
	      pid_t dio_id,pid_t ad_id,pid_t reset_id,
	      int *atten,float *noise,int *flg) {
  int chosen_freq=0;
  int num_words;
  int freq;
  int i;
  float max_power;
  float ave_power;
  int loop;
  int num_bad_atten;
  int status;
  unsigned char nflg=0;
  unsigned char seq;

  *flg=0;

  seq=tsg_id;
  num_words = samples * num_channel ;

  loop = 1;
  chosen_freq = lowest_freq[0];
  *atten = ATTEN_OFF;

  /* now do the second search */

  while (loop ) {

    loop = 0;
    num_bad_atten = 0;
    *noise = MAX_LONG;

    /* try all NUM_FCLR_FREQ freqs selected in the first search */
    for(i = 0; i < NUM_FCLR_FREQ ;i++) {
      freq = lowest_freq[i];
      if (freq == 0) freq = start_freq;

      /* set the freq */
      if ( (status=set_freq(dio_id,freq)) != 0) {
        /* report frequency error */
        *flg|=FCLR_FREQ_FAIL;
        return -1;
      }
		 
      /* start sampling */

      if (do_scan(ad_id,0,num_words,debug,num_channel) !=0) {
        /* report ad error */
        *flg|=FCLR_SCAN_FAIL;
        return -1;
      }

      if (send_tsg(dio_id,0,&seq,&nflg,&nflg,NULL,NULL) != 0) { 
        scan_reset(ad_id,reset_id);
        *flg|=FCLR_TSG_FAIL;
        return -1;
      }
		 
      status=(get_scan_status(ad_id)==SCAN_OK);
      if (status==1) {
        sum_product(buf_adr,&ave_power,samples,
			     &max_power);

        /*  count the number of freqs that exceed mxpwr  */
        if (max_power >= mxpwr)  num_bad_atten++;
         
        if (*noise > ave_power) {
          *noise = ave_power;
          chosen_freq = freq;
        }
      }


    } 

    /* check to see if it is necessary to increase attenuator */
    if ( (num_bad_atten >= NUM_FCLR_FREQ) && (*atten < max_atten) ) {
      (*atten)++;
      if (set_gain(dio_id,*atten) != 0) {
        /* report set gain error */
        *flg|=FCLR_GAIN_FAIL;
        return -1;
      }
      loop = 1;
    } 
  }
  return chosen_freq;
}

