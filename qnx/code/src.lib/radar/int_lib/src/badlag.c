/* badlag.c 
   ======== */

/*
$Log: badlag.c,v $
Revision 1.2  1999/03/25 21:06:16  barnes
Renames the rada.h header to do_radar.h.

Revision 1.1  1998/10/28 22:14:09  barnes
Initial revision

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include "limit.h"
#include "do_radar.h"
#include "tsg.h"

int badlag_zero(struct tsg_parms *prm,int mplgs,int *lag_table) {

  int i;
  int bad_range=MAX_RANGE;
  int sample_unit;
  int num_range;
  int num_frange;
  int mpinc_diff;
  int lag0_sample;
  int pulse_rise_time;
  int half_txpl;

  /* search which pulse in the pulse-pattern that is used to compute lag 0
      power  */
 
  for(i=0; i < prm->mppul;i++)
    if(lag_table[0] == prm->pat[i]) break;
    if(i >= (prm->mppul - 1)) bad_range = MAX_RANGE;  
    else {   
      sample_unit = prm->mpinc / prm->smsep;
      mpinc_diff = prm->pat[i+1] - 
                   prm->pat[i];
      num_range = mpinc_diff * sample_unit;
   
      /* compute the pulse rise time effect */

      half_txpl = prm->txpl / 2;
      pulse_rise_time = half_txpl /  prm->smsep;
      if ((half_txpl % prm->smsep) != 0)
         pulse_rise_time++;   /* add one because of the int div */
      num_range = num_range - pulse_rise_time;  /* subtract pulse rise time */
      num_frange = prm->lagfr / prm->smsep;

      /* now compute the start of the bad range */

      bad_range = num_range - num_frange;
      if (bad_range < 0) bad_range = 0;
   }      
   return bad_range;
}
