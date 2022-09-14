/* acfcalculate.c
   ==============
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rtypes.h"
#include "limit.h"
#include "tsg.h"
#include "acf.h"




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



