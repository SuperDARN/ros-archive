/* sumpower.c
   ==========
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




/* 
   dflg      turn sample delay on and off 
   rngoff    range scaling factor 2 or 4 depending on XCF.
   roffset   real offset into the A/D buffer
   ioffset   imaginary offset into the A/D buffer 
   badrng    bad range
   noise     noise value
   atten     attenuation level (0 = no attenuation)
*/

int ACFSumPower(struct TSGprm *prm,int mplgs,
                int (*lagtable)[2],float *acfpwr0,
		int16 *inbuf,int rngoff,int dflg,
		int roffset,int ioffset,
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
  
  if (dflg) sdelay=prm->smdelay; /* digital receiver delay term */
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
