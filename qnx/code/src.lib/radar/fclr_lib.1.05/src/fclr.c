/* fclr.c
   ======
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
 $Log: fclr.c,v $
 Revision 1.5  2001/10/15 15:50:25  barnes
 Changed internals to floating point.

 Revision 1.4  2001/06/27 20:53:22  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:11:49  barnes
 Added Author Name

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
	    float mxpwr,int max_atten,
	    struct freq_table *fptr,
	    pid_t dio_id,pid_t ad_id,
	    pid_t reset_id,int *atten,float *noise,
            float *statistic,
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
