/* sum_power.c
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
$Log: sum_power.c,v $
Revision 1.11  2001/10/15 15:25:38  barnes
Moved to floating point internal.

Revision 1.10  2001/06/27 20:53:45  barnes
Added license tag

Revision 1.9  2001/01/29 18:11:50  barnes
Added Author Name

Revision 1.8  2000/09/19 15:59:43  barnes
Fixed bug that meant the noise algorithm was testing the wrong power
value when the attenuators switch.

Revision 1.7  2000/09/18 15:29:36  barnes
Bug fixes to the noise filter algorithm.

Revision 1.6  2000/08/22 14:21:47  barnes
Added algorithm for determining noise contamination.

Revision 1.5  2000/08/21 19:51:45  barnes
Modification to cope with noisy data.

Revision 1.4  2000/02/10 17:51:44  barnes
Fixed bug in getting the correct value from the lag table after the bad range.

Revision 1.3  1999/03/29 14:28:23  barnes
Changed comments.

Revision 1.2  1999/03/25 21:06:16  barnes
Renames the rada.h header to do_radar.h.

Revision 1.1  1998/10/28 22:15:09  barnes
Initial revision

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include "limit.h"
#include "do_radar.h"
#include "tsg.h"

int sum_power(struct tsg_parms *prm,int mplgs,
              int *lag_table,float *acf_pwr0,float mxpwr,
			  int16 *in_buf,int range_offset,
			  int change_atten,int bad_range,float noise,
	                  int thr,int lmt,
                          int *abort) {

  int sample_unit;
  int range;
  unsigned in_buf_ind;
  int max_range;
  float pwr0[MAX_RANGE];
  float real_power;
  float imag_power;
  float max_pwr;
  int lag0_m_sample;
  int16 *in_buf_adr;
  int min_pwr;
  int selec_range;
  float temp_min_pwr;
  int new_lag0_m_sample;
  float l_temp;

  int cnt=0;

  *abort=0;
  
  sample_unit = prm->mpinc / prm->smsep ; 
  max_range = prm->nrang;

  lag0_m_sample = lag_table[0] * sample_unit;
  min_pwr = MAX_LONG;
  new_lag0_m_sample = 0;

  for(range=0; range < max_range; range++) {
    /* check to see if there is a need for changeing the lag */
    if((range >= bad_range)  && (new_lag0_m_sample == 0)) {
       lag0_m_sample = lag_table[mplgs-1]*sample_unit;
       new_lag0_m_sample = 1;
    }

    in_buf_ind = (lag0_m_sample + range ) * range_offset ;
    in_buf_adr = in_buf + in_buf_ind; 

    l_temp = (float) *in_buf_adr;
    real_power = l_temp * l_temp;
    in_buf_adr = in_buf_adr + IMAG_BUF_OFFSET;
    l_temp = (float) *in_buf_adr;
    imag_power = l_temp * l_temp;
   
    pwr0[range] = real_power + imag_power ; 
     
   

    if (change_atten) pwr0[range] = pwr0[range] / ATTEN_STEP;
    if (min_pwr > pwr0[range]) min_pwr = pwr0[range];
	
    if ((thr !=0) &&   
       (pwr0[range]<(thr*noise))) cnt++; 
 

  } 
 
  if ((lmt !=0) && (cnt<lmt)) {
    *abort=1;
    return -1;
  }

  /* accumulate the power in raw_data which is the output buffer */
 
  max_pwr = 0L;
  selec_range = 0;
  for(range=0; range < max_range;range++)  {
    acf_pwr0[range] = acf_pwr0[range] + pwr0[range];

    if(acf_pwr0[range] > max_pwr) {
      max_pwr = acf_pwr0[range];
      selec_range = range;
    }
  }

  /* set the attenuation flag to invalid range if 
     max_power is smaller than mxpwr */

  /* temp_min_pwr = 4 *   min_pwr;    */
 
  temp_min_pwr = 0;

  if((pwr0[selec_range] < mxpwr)  ||
     (pwr0[selec_range] < temp_min_pwr)) return -1;
  
  return selec_range;
}

