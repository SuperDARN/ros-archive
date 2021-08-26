/* do_fclr_s.c
   =========== */

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
 $Log: do_fclr_s.c,v $
 Revision 1.6  2001/10/15 16:26:01  barnes
 Moved to floating point internals.

 Revision 1.5  2001/06/27 21:10:46  barnes
 Added license tag

 Revision 1.4  2000/10/24 16:36:23  barnes
 Modification to stop both channels hitting the same frequency.

 Revision 1.3  2000/09/18 16:14:24  barnes
 Removed debugging code.

 Revision 1.2  2000/09/18 16:10:28  barnes
 Changed error reporting.

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
	    float mxpwrA,int max_attenA,		      
	    int *attenA,float *noiseA,
            float *statisticA,int *flgA,int *freqA,

            int start_freqB,
	    int end_freqB,int step_freqB,
	    float mxpwrB,int max_attenB,		      
	    int *attenB,float *noiseB,
            float *statisticB,int *flgB,int *freqB) {

   struct tsg_parms *prm;
   int id,i;   
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
	 	  lowest_freqB,statisticB,flgB) !=0) {
     fprintf(stderr,"Frequency search error\n");			       
     return -1;				
   }
   
 

   id=tsg_id[1];

   prm=&tab->buf[id];
   set_channel(dio_id,0);
   *freqA=fine_fclr(id,prm->samples,debug,lowest_freqA,start_freqA,
			      num_channel,mxpwrA,max_attenA,buf_adr[0],
				  dio_id,ad_id[0],reset_id[0],
                                  attenA,noiseA,flgA);

   for (i=0;i<NUM_FCLR_FREQ;i++) {
     if (lowest_freqB[i]==*freqA) {
       if (i==0) lowest_freqB[i]=lowest_freqB[i+1];
        else lowest_freqB[i]=lowest_freqB[i-1];
     }
   }

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



