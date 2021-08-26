/* acf.c 
   ===== */

/*
$Log: acf.c,v $
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
		int *acf_buf,
		int xcf_flag,int range_offset,int change_atten,int bad_range) {

   int range;
   int sample_unit;
   int first_range_offset;
   int offset_1;
   int real;
   int imag;
   int lag;
   int sample_1;
   int sample_2;
   int nrang;
   int temp_1;
   int temp_2;
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
         
       temp_1 = (long) in_buf[sample_1] * (long) in_buf[sample_2];
       temp_2 = (long) in_buf[sample_1 + IMAG_BUF_OFFSET] * 
                (long) in_buf[sample_2 + IMAG_BUF_OFFSET];
       real = temp_1 + temp_2;

       temp_1 = (long) in_buf[sample_1] *
                (long) in_buf[sample_2 + IMAG_BUF_OFFSET];
       temp_2 = (long) in_buf[sample_2] * 
			    (long) in_buf[sample_1 + IMAG_BUF_OFFSET]; 
       imag = temp_1 - temp_2;

       if(change_atten) {
          real = real / ATTEN_STEP;
          imag = imag / ATTEN_STEP;
       }

       /* sum the real and imaginary acfs */
		 
       acf_buf[range*(2*mplgs)+2*lag]=real+acf_buf[range*(2*mplgs)+2*lag];
       acf_buf[range*(2*mplgs)+2*lag+1]=imag+
						                acf_buf[range*(2*mplgs)+2*lag+1];
     } 
   }

   return 0;
}  

