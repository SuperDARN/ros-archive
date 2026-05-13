/* sumpower.c
   ==========
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

/*
 $Log: sumpower.c,v $
 Revision 1.3  2006/02/07 19:20:14  barnes
 Simon Shepherd's modifications to the lag table.

 Revision 1.2  2004/05/04 18:47:28  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/01/19 20:16:33  barnes
 Initial revision

*/


/* 
   dflg      turn sample delay on and off 
   rngoff    range scaling factor 2 or 4 depending on XCF.
   delay     number of extra samples to delay to deal with propagation
             through the digital receiver 
   roffset   real offset into the A/D buffer
   ioffset   imaginary offset into the A/D buffer 
   badrng    bad range
   noise     noise value
   atten     attenuation level (0 = no attenuation)
*/

int ACFSumPower(struct TSGprm *prm,int mplgs,
                int (*lagtable)[2],float *acfpwr0,
		int16 *inbuf,int rngoff,int dflg,
		int delay,int roffset,int ioffset,
		int badrng,float noise,float mxpwr,
		float atten,
	        int thr,int lmt,
                int *abort) {

  int sdelay=0;
  int sampleunit;
  int range;
  unsigned inbufind;
  int maxrange;
  float pwr0[MAX_RANGE];
  float rpwr;
  float ipwr;

  float tmpminpwr,minpwr,maxpwr;  
  int slcrng;
  int lag0msample;
  int16 *inbufadr;
  int newlag0msample;
  float ltemp;

  int cnt=0;

  *abort=0;
  
  if (dflg) sdelay=delay+prm->smdelay; /* digital receiver delay term */
  sampleunit = prm->mpinc / prm->smsep; 
  maxrange = prm->nrang;

  lag0msample = lagtable[0][0] * sampleunit;
  minpwr = 1e16;
  newlag0msample = 0;

  for(range=0; range < maxrange; range++) {
    /* check to see if there is a need for changeing the lag */
    if((range >= badrng)  && (newlag0msample == 0)) {
       lag0msample = lagtable[mplgs][0]*sampleunit;
       newlag0msample = 1;
    }

    inbufind = (lag0msample + range + sdelay ) * rngoff;
    inbufadr = inbuf + inbufind + roffset; 
    ltemp = (float) *inbufadr;
    rpwr = ltemp * ltemp;

    inbufadr = inbuf + inbufind + ioffset;
    ltemp = (float) *inbufadr;
    ipwr = ltemp * ltemp;
   
    pwr0[range] = rpwr + ipwr; 

    if (minpwr > pwr0[range]) minpwr = pwr0[range];
    if (atten !=0) pwr0[range] = pwr0[range] / atten;
 
    if ((thr !=0) &&   
       (pwr0[range]<(thr*noise))) cnt++; 
 

  } 
 
  if ((lmt !=0) && (cnt<lmt)) {
    *abort=1;
    return -1;
  }

  /* accumulate the power in rawdata which is the output buffer */

  maxpwr = 0L;
  slcrng = 0;
  for(range=0; range < maxrange;range++)  {
    acfpwr0[range] = acfpwr0[range] + pwr0[range];
    if(acfpwr0[range] > maxpwr) {
      maxpwr = acfpwr0[range];
      slcrng = range;
    }
  }

  if (mxpwr==0) return slcrng;
  
  tmpminpwr=0; /* tmpminpwr=4*minpwr */

  if((pwr0[slcrng] < mxpwr)  ||
     (pwr0[slcrng] < tmpminpwr)) return -1;

  return slcrng;
}

