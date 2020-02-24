/* average.c
   =========
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
$Log: average.c,v $
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
#include "limit.h"
#include "do_radar.h"

int average(float *pwr0,float *acfd,float *xcfd,int nave,int nrang,int mplgs) {
   int range;
   int lag;
   
   float *acfd_addr=NULL;
   float *xcfd_addr=NULL;
   float *pwr0_addr=NULL;

   pwr0_addr=pwr0;   
   for (range = 0; range < nrang ; range++) {
     acfd_addr = &acfd[range*(2*mplgs)];
     if (xcfd !=NULL) xcfd_addr = &xcfd[range*(2*mplgs)];
	      
	 for(lag=0;lag < mplgs ; lag++) {
       *acfd_addr = *acfd_addr / nave;   
	   acfd_addr++;
       *acfd_addr = *acfd_addr / nave;   
       acfd_addr++; 
       if(xcfd_addr !=NULL) {
         *xcfd_addr = *xcfd_addr / nave;  /* real */
         xcfd_addr++;
         *xcfd_addr = *xcfd_addr / nave;   /* imag */
         xcfd_addr++; 
        }
      } 
      *pwr0_addr = *pwr0_addr / nave;
      pwr0_addr++;
   } 
   return 0;
}



