/* normalize.c 
   =========== */

/*
$Log: normalize.c,v $
Revision 1.2  1999/03/25 21:06:16  barnes
Renames the rada.h header to do_radar.h.

Revision 1.1  1998/10/28 22:15:09  barnes
Initial revision

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "limit.h"
#include "do_radar.h"

void normalize(int *pwr0,int *acfd,int *xcfd,
			   int nrang,int mplgs) {
  int range,lag;
  for (range = 0; range < nrang ;range++) {
    for (lag=0;lag < mplgs;lag++) {
      acfd[range*(2*mplgs)+2*lag] = 
      acfd[range*(2*mplgs)+2*lag]/ ATTEN_STEP;
      acfd[range*(2*mplgs)+2*lag+1] =  
      acfd[range*(2*mplgs)+2*lag+1]/ATTEN_STEP; 
      if(xcfd !=NULL) {
        xcfd[range*(2*mplgs)+2*lag] =  
        xcfd[range*(2*mplgs)+2*lag] / ATTEN_STEP;
        xcfd[range*(2*mplgs)+2*lag+1] =  
        xcfd[range*(2*mplgs)+2*lag+1] / ATTEN_STEP; 
      }
    } 
    pwr0[range] = pwr0[range]/ATTEN_STEP;
  }
}


