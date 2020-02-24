/* acf.c
   =====
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
$Log: acf.c,v $
Revision 1.7  2001/10/15 15:25:38  barnes
Moved to floating point internal.

Revision 1.6  2001/06/27 20:53:45  barnes
Added license tag

Revision 1.5  2001/01/29 18:11:50  barnes
Added Author Name

Revision 1.4  2000/02/10 16:10:33  barnes
Fixed problem with the lag table being over-run.

Revision 1.3  1999/10/13 22:15:04  barnes
Fixed bugs in the ACF code

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

int acf(struct tsg_parms *prm,
		int16 *in_buf,int mplgs,int *lag_table,
		float *acf_buf,
		int xcf_flag,int range_offset,int change_atten,int bad_range) {

   int range;
   int sample_unit;
   int first_range_offset;
   int offset_1;
   float real;
   float imag;
   int lag;
   int sample_1;
   int sample_2;
   int nrang;
   float temp_1;
   float temp_2;
   int offset_2;
   nrang = prm->nrang;
   sample_unit = (prm->mpinc / prm->smsep) *
                 range_offset;
				 
   for(range=0;range < nrang ; range++) {
     offset_1 = range * range_offset ;
	 if (xcf_flag == ACF_PART) offset_2 = offset_1;
	 else offset_2 = (range * range_offset) + 2;
      
	 for(lag=0;lag < mplgs ; lag++) {
         
       /* if the range is bad use the lag given lag_table[mplgs] */
         
       if ((range >= bad_range) && (lag == 0)) {
          
           sample_1 =lag_table[mplgs-1]*sample_unit + offset_1;        
           sample_2 =lag_table[2*mplgs-1]*sample_unit + offset_2;
       } else {  
           sample_1 =lag_table[lag]*sample_unit + offset_1;        
           sample_2 =lag_table[mplgs+lag]*sample_unit + offset_2;
       }
         
       temp_1 = (float) in_buf[sample_1] * (long) in_buf[sample_2];
       temp_2 = (float) in_buf[sample_1 + IMAG_BUF_OFFSET] * 
                (float) in_buf[sample_2 + IMAG_BUF_OFFSET];
       real = temp_1 + temp_2;

       temp_1 = (float) in_buf[sample_1] *
                (float) in_buf[sample_2 + IMAG_BUF_OFFSET];
       temp_2 = (float) in_buf[sample_2] * 
			    (long) in_buf[sample_1 + IMAG_BUF_OFFSET]; 
       imag = temp_1 - temp_2;

       if(change_atten) {
          real = real / ATTEN_STEP;
          imag = imag / ATTEN_STEP;
       }

       /* sum the real and imaginary acfs */
		 
       acf_buf[range*(2*mplgs)+2*lag]=real+acf_buf[range*(2*mplgs)+2*lag];
       acf_buf[range*(2*mplgs)+2*lag+1]=imag+acf_buf[range*(2*mplgs)+2*lag+1];
     } 
   }

   return 0;
}  

