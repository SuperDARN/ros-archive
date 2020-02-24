/* badlag.c
   ========
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
$Log: badlag.c,v $
Revision 1.5  2001/10/15 15:25:38  barnes
Moved to floating point internal.

Revision 1.4  2001/06/27 20:53:45  barnes
Added license tag

Revision 1.3  2001/01/29 18:11:50  barnes
Added Author Name

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

