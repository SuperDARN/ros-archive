/* acfcalculate.c
   ==============
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rtypes.h"
#include "limit.h"
#include "tsg.h"
#include "acf.h"

/*
 $Log: acfcalculate.c,v $
 Revision 1.9  2008/03/13 22:34:20  code
 Removed "delay". This is the delay associated with propagation through
 the digital receiver, however it's value can only be calculated
 in the digital receiver driver. In the past we have "guessed"
 a value. It is much better to remove this term and account for the
 propagation in the driver.
 The "dflg" remains as this tells the routine to use "prm->stdelay", the
 number of samples to skip prior to the first TX pulse.

 Revision 1.8  2007/04/17 15:38:41  code
 Removed incorrect code for writing hte ACF array.
 The index into the ACF array is not based on the actual lag (tau)
 but are sequential - If a lag is missing, no gap appears in the array
 Missing lags are resolved by looking at the lag table.

 Revision 1.7  2006/08/17 19:02:02  code
 Removed redundant wrapper.

 Revision 1.6  2006/08/17 18:59:27  code
 Added support for extended lag tables.

 Revision 1.5  2006/02/07 19:20:14  barnes
 Simon Shepherd's modifications to the lag table.

 Revision 1.4  2005/07/26 17:49:36  barnes
 Added code to correctly find XCFs.

 Revision 1.3  2004/05/04 18:47:28  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/29 21:39:15  barnes
 Fixed redundant semi-colon.

 Revision 1.1  2004/01/19 20:16:33  barnes
 Initial revision

*/


 /* For an analogue receiver, samples are multiplexed,
  * eg. sample 1 & 2 are the I & Q for the main array,
  * end sample 3 & 4 are the I & Q for the interferometer array.
  * For a digital receiver samples are not multiplexed,
  * so for the APL and Saskatoon implmentation the first X
  * samples are the I & Q samples from the main array,
  * followed by the I & Q samples from the secondary array.
  * The Alaskan implementation is slightly different again
  * using two halves of the total DMA buffer. 
  *
  * The differences in implementation are handled by a combination
  * of the rngoff and xcfoff arguments.
  *
  * Analogue receiver:    rngoff        xcfoff
  *
  * No XCFs                 2             0
  * With XCFs               4             2
  *
  * Digital Receiver      rngoff        xcfoff
  * (APL & Saskatoon)     
  * No XCFs                 2             0
  * With XCFs               2             nsamp
  *
  * Digital Receiver      rngoff        xcfoff
  * (Alaska)
  * No XCFs                 2             0
  * With XCFs               2             8192 (half DMA buffer size)
  *
  *
  */

 

int ACFCalculate(struct TSGprm *prm,
 		 int16 *inbuf,int rngoff,int dflg,
		 int roffset,int ioffset,
		 int mplgs,int lagnum,int (*lagtable)[2],
  	         float *acfbuf,
	         int xcf,int xcfoff,
                 int badrange,float atten,float *dco) {

   int sdelay=0;
   int range;
   int sampleunit;
   int offset1;
   float real;
   float imag;
   int lag;
   int sample1;
   int sample2;
   int nrang;
   float temp1;
   float temp2;
   int offset2;

   float dcor1=0;
   float dcor2=0;
   float dcoi1=0;
   float dcoi2=0;

   int lagval;
   int lagmax;

   if (dco !=NULL) {
     if (xcf==ACF_PART) {
       dcor1=dco[0];
       dcor2=dco[0];
       dcoi1=dco[1];
       dcoi2=dco[1];       
     } else {
       dcor1=dco[0];
       dcor2=dco[2];
       dcoi1=dco[1];
       dcoi2=dco[3];
     }
   }

   lagmax=lagnum;
   if (lagmax==0) lagmax=mplgs;
 

   nrang = prm->nrang;
   if (dflg) sdelay=prm->smdelay; /* digital receiver delay term */
   sampleunit = (prm->mpinc / prm->smsep) *
                 rngoff;
				 
   for(range=0;range < nrang ; range++) {

         offset1 = (range+sdelay) * rngoff;

	 if (xcf == ACF_PART) offset2 = offset1;
	 else offset2 = ((range+sdelay) * rngoff) + xcfoff;

        
      
	 for(lag=0;lag < lagmax; lag++) {
         
       /* if the range is bad use the lag given lagtable[mplgs] */
         
       if ((range >= badrange) && (lag == 0)) {
           sample1 =lagtable[lagmax][0]*sampleunit + offset1;        
           sample2 =lagtable[lagmax][1]*sampleunit + offset2;
       } else { 
           sample1 =lagtable[lag][0]*sampleunit + offset1;        
           sample2 =lagtable[lag][1]*sampleunit + offset2;
       }
         
       temp1 = (float) (inbuf[sample1+ roffset]-dcor1) * 
                (float) (inbuf[sample2+ roffset]-dcor2);

       temp2 = (float) (inbuf[sample1 + ioffset]-dcoi1) * 
                (float) (inbuf[sample2 + ioffset]-dcoi2);
       real = temp1 + temp2;


       temp1 = (float) (inbuf[sample1 + roffset]-dcor1) *
                (float) (inbuf[sample2 + ioffset]-dcoi2);
       temp2 = (float) (inbuf[sample2 + roffset]-dcor2) * 
		(float) (inbuf[sample1 + ioffset]-dcoi1); 
       imag = temp1 - temp2;

       /* sum the real and imaginary acfs */
		
       if (atten !=0) {
         real=real/atten;
         imag=imag/atten;
       }

       /*
       if (lagnum != 0) lagval=abs(lagtable[lag][0]-lagtable[lag][1]);
       else lagval=lag;
       */

       lagval=lag;

       acfbuf[range*(2*mplgs)+2*lagval]=real+acfbuf[range*(2*mplgs)+2*lagval];
       acfbuf[range*(2*mplgs)+2*lagval+1]=imag+
	                                   acfbuf[range*(2*mplgs)+2*lagval+1];
     } 
   }

   return 0;
}  



