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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rtypes.h"
#include "limit.h"
#include "tsg.h"

/*
 $Log: badlag.c,v $
 Revision 1.3  2004/05/04 18:47:28  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/29 21:39:15  barnes
 Removed unused variable.

 Revision 1.1  2004/01/14 22:27:53  barnes
 Initial revision

*/

int ACFBadLagZero(struct TSGprm *prm,int mplgs,int *lagtable) {

  int i;
  int badrng=MAX_RANGE;
  int sampleunit;
  int numrng;
  int numfrng;
  int mpincdiff;
  int pulserisetime;
  int halftxpl;

  /* search which pulse in the pulse-pattern that is used to compute lag 0
      power  */
 
  for(i=0; i < prm->mppul;i++)
    if(lagtable[0] == prm->pat[i]) break;
    if(i >= (prm->mppul - 1)) badrng = MAX_RANGE;  
    else {   
      sampleunit = prm->mpinc / prm->smsep;
      mpincdiff = prm->pat[i+1] - 
                   prm->pat[i];
      numrng = mpincdiff * sampleunit;
   
      /* compute the pulse rise time effect */

      halftxpl = prm->txpl / 2;
      pulserisetime = halftxpl /  prm->smsep;
      if ((halftxpl % prm->smsep) != 0)
         pulserisetime++;   /* add one because of the int div */
      numrng = numrng - pulserisetime;  /* subtract pulse rise time */
      numfrng = prm->lagfr / prm->smsep;

      /* now compute the start of the bad range */

      badrng = numrng - numfrng;
      if (badrng < 0) badrng = 0;
   }      
   return badrng;
}

