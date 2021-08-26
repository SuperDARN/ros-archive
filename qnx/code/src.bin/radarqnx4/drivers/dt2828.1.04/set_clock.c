/* set_clock.c
   ===========
   Author: R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
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

/*  dt_set_clock sets the internal clock frequency
    the input parameter freq is the frequency to set the clock
    always return TRUE
*/
#include <stdio.h>
#include <math.h>
#include <sys/types.h>

#include "dma-alloc.h"
#include "dma.h"
#include "dt.h"
#include "dt_ext.h"
#include "dt2828.h"

/*
 $Log: set_clock.c,v $
 Revision 1.1  2004/05/10 14:34:00  barnes
 Initial revision

*/

extern int ad_port;

int dt_set_clock(long freq)  {
   int n;     /* prescale divisor */
   int x;     /* counter value */
   double temp;     /* temporary counter value */
   float temp_1;    /* another temporary counter value */

   /* find the appropriate counter value and prescale divisor 
      the prescale divisor goes from 0 to 15 ; so start from 0 and
      see if it fits
   */

   for(n = 0;n <= 15 ; ++n) {
      if(n == 1)
         ++n;
      temp = (4.e6l/ pow(2.0, (double) n)) / (double) freq ; 

      /* check to see if it fits */
 
     if(temp < 255.) {
         /* check to see if the freq is going to be exactly the same
            as the one requested, since the allowed freq is discrete
         */
         x = (int) ( temp + 0.5);  /* round */   
         temp_1 = (float) x;

         // if( temp_1 != temp) report a warning here ?

         x = 255 - x;                 /* do one complement */
         dt_tmrctr.bit.counter = x ;
         dt_tmrctr.bit.prs = n ;

         /* set register */
         dt_set_register(ad_port+TMRCTR_PORT,&dt_tmrctr);

         break;
      } 
   } 

   return 0;

}


