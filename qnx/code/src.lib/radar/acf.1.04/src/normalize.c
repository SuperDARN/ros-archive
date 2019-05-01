/* normalize.c
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "limit.h"


/*
 $Log: normalize.c,v $
 Revision 1.1  2004/01/14 22:27:53  barnes
 Initial revision

*/

void ACFNormalize(float *pwr0,float *acfd,float *xcfd,
		  int nrang,int mplgs,float atten) {
  int range,lag;
  if (atten==0) return;
  for (range = 0; range < nrang ;range++) {
    for (lag=0;lag < mplgs;lag++) {
      acfd[range*(2*mplgs)+2*lag] = 
      acfd[range*(2*mplgs)+2*lag]/ atten;
      acfd[range*(2*mplgs)+2*lag+1] =  
      acfd[range*(2*mplgs)+2*lag+1]/atten; 
      if(xcfd !=NULL) {
        xcfd[range*(2*mplgs)+2*lag] =  
        xcfd[range*(2*mplgs)+2*lag] / atten;
        xcfd[range*(2*mplgs)+2*lag+1] =  
        xcfd[range*(2*mplgs)+2*lag+1] / atten; 
      }
    } 
    pwr0[range] = pwr0[range]/atten;
  }
}


