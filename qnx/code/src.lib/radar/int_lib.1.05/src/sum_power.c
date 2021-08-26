/* sum_power.c 
   =========== */

/*
$Log: sum_power.c,v $
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
              int *lag_table,int *acf_pwr0,int mxpwr,
			  int16 *in_buf,int range_offset,
			  int change_atten,int bad_range,int noise,
	                  int thr,int lmt,
                          int *abort) {

  int sample_unit;
  int range;
  unsigned in_buf_ind;
  int max_range;
  int pwr0[MAX_RANGE];
  int real_power;
  int imag_power;
  int max_pwr;
  int lag0_m_sample;
  int16 *in_buf_adr;
  int min_pwr;
  int selec_range;
  int temp_min_pwr;
  int new_lag0_m_sample;
  int l_temp;

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

    l_temp = (long) *in_buf_adr;
    real_power = l_temp * l_temp;
    in_buf_adr = in_buf_adr + IMAG_BUF_OFFSET;
    l_temp = (long) *in_buf_adr;
    imag_power = l_temp * l_temp;
   
    pwr0[range] = real_power + imag_power ; 
    if ((thr !=0) &&   
       (pwr0[range]<(thr*noise))) cnt++; 
   

    if (change_atten) pwr0[range] = pwr0[range] / ATTEN_STEP;
    if (min_pwr > pwr0[range]) min_pwr = pwr0[range];
	
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

