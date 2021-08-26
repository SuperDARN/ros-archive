/* average.c 
   ========= */

/*
$Log: average.c,v $
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

int average(int *pwr0,int *acfd,int *xcfd,int nave,int nrang,int mplgs) {
   int range;
   int lag;
   
   int *acfd_addr=NULL;
   int *xcfd_addr=NULL;
   int *pwr0_addr=NULL;

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



