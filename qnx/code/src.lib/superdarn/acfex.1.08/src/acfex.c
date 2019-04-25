/* acfcalculate.c
   ==============
   Author: R.J.Barnes, R.A.Greenwald, K.Oksavik
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
#include <math.h>
#include "rtypes.h"
#include "limit.h"
#include "tsg.h"
#include "acfex.h"


/*
 $Log: acfex.c,v $
 Revision 1.8  2008/03/14 18:56:50  code
 Removed smpdelay, the propagation delay through the receiver.
 The propagation can only really be calculated in the driver, so
 those samples are now discarded by the driver automatically.

 Revision 1.7  2007/04/17 17:21:40  code
 Fixed typos.

 Revision 1.6  2007/04/17 17:20:37  code
 Ray's change to the algorithm.

 Revision 1.5  2007/03/15 14:00:37  code
 Fixed problem of underestimating powers due to transmitter pulses being
 masked out.

 Revision 1.4  2007/02/19 15:29:01  code
 Fixed acf calculation.

 Revision 1.3  2007/02/15 21:51:21  code
 Fixed imaginary component of the ACF.

 Revision 1.2  2007/02/13 16:33:44  code
 Fixed bugs.

 Revision 1.1  2007/02/12 18:04:29  code
 Initial revision

*/

int ACFexPwrSort(const void *a,const void *b) {
  float *x,*y;
  x=(float *) a;
  y=(float *) b;
  if (*x>*y) return 1;
  if (*x<*y) return -1;
  return 0;
}
 

int ACFexCalculate(struct TSGprm *prm,int16 *iqptr,int iqnum,
		 int nave,int nsample,
		 int roffset,int ioffset,
                 int mplgs,int lagnum,int (*lagtable)[2],int *lagsum,
                 float *pwr0,float *acfbuf,float *noise) {


   float *smpwr=NULL;
   float *srtpwr=NULL;
   float *acfr=NULL;
   float *acfi=NULL;

   float minpwrlvl = 2.0;
   float p0,p1,p2,v1,v2;
   float sumnzlagpwr,lagpwr;
   int R,L,i0,rxlag,n,i,x,lag;
   int i1min,i2min,i1,i2,lastlag;
   int srtn=0, lagcnt=0, ncnt=0;

/* Storage allocation for acfr and acfi allows up to 10 pair 
   combinations per lag. */

   acfr=malloc(sizeof(float)*nave*10);
   acfi=malloc(sizeof(float)*nave*10);
   smpwr=malloc(sizeof(float)*nsample);
   srtpwr=malloc(sizeof(float)*prm->nrang);

/* The following code determines the median power at each of the *nsample
   complex samples of the data arrays from the digital rx over the integration
   interval. */

   for (n=0;n<nsample;n++) {
      for (i=0;i<nave;i++) {
         x  = i*nsample+n;
         v1 = iqptr[2*x];
         v2 = iqptr[2*x+1];
         acfr[i] = v1*v1 + v2*v2;
      }
      qsort(acfr,nave,sizeof(float),ACFexPwrSort);
      smpwr[n]=acfr[nave/2];
   }

/* Put -1's into all array elements prior to the first sample and into
   those array elements for which a Tx pulse is occurring. Assume bad
   data during the Tx pulse and during the following two samples. */

   rxlag = prm->smdelay - 1;
   for (n=0;n<rxlag;n++) smpwr[n] = -1.0;
   for (n=1;n<prm->mppul;n++) {
      smpwr[rxlag - prm->frang/prm->rsep + 
                    prm->pat[n]*prm->mpinc/prm->smsep + 0] = -1.0;
      smpwr[rxlag - prm->frang/prm->rsep + 
                    prm->pat[n]*prm->mpinc/prm->smsep + 1] = -1.0;
      smpwr[rxlag - prm->frang/prm->rsep + 
                    prm->pat[n]*prm->mpinc/prm->smsep + 2] = -1.0;
   }

/* Determine the power profile of the backscattered signal for all R ranges.
   If a transmitter pulse is encountered before the last range is reached, 
   use an alternative transmitter pulse for the final ranges. */

   for (R=0;R<prm->nrang;R++) {
      i0 = rxlag + lagtable[0][0]*prm->mpinc/prm->smsep + R; 
      if (((lagtable[lagnum][0]-lagtable[lagnum][1])==0) && 
         (i0>=(rxlag - prm->frang/prm->rsep + 
                       prm->pat[1]*prm->mpinc/prm->smsep + R))) {
         i0 = rxlag + lagtable[lagnum][0]*prm->mpinc/prm->smsep + R;
      }
      pwr0[R] = smpwr[i0];
      if (pwr0[R]>0.0) {
         srtpwr[srtn] = pwr0[R];
         srtn++;
      }
   }

/* Reorder the non -1.0 power values in terms of increasing backscatter power.
   Then average values 10-20 to get a noise power determination. */

   qsort(srtpwr,srtn,sizeof(float),ACFexPwrSort);
   *noise = 0.0;
   for (srtn=10;srtn<20;srtn++) *noise += srtpwr[srtn];
   *noise = *noise/10.0;
 
/* Calculate the autocorrelation function for each range for which the    
   lag0[R] power is greater than 10*10^(minpwrlvl/10)*noise. 
   The ACFs are stored sequentially as 
   series of 2*mplgs elements (real, imag, real, imag....).
   If acfbufreal = acfbufimag = 0.0, there is no value.  */

   lastlag = 0;
   for (n=0;n<lagnum;n++) {
      if (abs(lagtable[n][0]-lagtable[n][1])>lastlag) {
         lastlag = abs(lagtable[n][0]-lagtable[n][1]); 
      }
   }
   for (R=0;R<prm->nrang;R++) {
      p0 = pwr0[R];
      if (p0>(pow(10,minpwrlvl/10.0)*(*noise))) {
         p0 -= *noise;
         acfbuf[R*2*mplgs]   = p0;
         acfbuf[R*2*mplgs+1] = 0.0;
         sumnzlagpwr = 0.0;
         lagcnt = 0;
         L = 0;
         for (lag=1;lag<=lastlag;lag++) {
            lagpwr = 0.0;
            i1min  = -1;
            i2min  = -1;
            if (lagsum[lag]>0) {
               L++;
               ncnt = 0;
               for (n=0;n<lagnum;n++) {
                  if (abs(lagtable[n][0]-lagtable[n][1]) !=lag) continue;
                  i1 = rxlag + lagtable[n][0]*prm->mpinc/prm->smsep + R;
                  i2 = rxlag + lagtable[n][1]*prm->mpinc/prm->smsep + R;
                  p1 = smpwr[i1];
                  p2 = smpwr[i2];
                  if ((p1>0) && (p2>0)) {
                     if ((p1<(p0*sqrt(1.0*nave))) && 
                         (p2<(p0*sqrt(1.0*nave)))) {
                        i1min = i1;
                        i2min = i2;
                     }
                  }
                  if ((i1min>=0) && (i2min>=0)) {
                     for (i=0;i<nave;i++) {
                        acfr[i+ncnt]=
                           iqptr[2*(i*nsample+i1min)+ioffset]*
                           iqptr[2*(i*nsample+i2min)+ioffset]+
                           iqptr[2*(i*nsample+i1min)+roffset]*
                           iqptr[2*(i*nsample+i2min)+roffset];
                        acfi[i+ncnt]=
                           iqptr[2*(i*nsample+i1min)+roffset]*
                           iqptr[2*(i*nsample+i2min)+ioffset]-
                           iqptr[2*(i*nsample+i2min)+roffset]*
                           iqptr[2*(i*nsample+i1min)+ioffset];
                     }
                     ncnt += nave;
                     i1min  = -1;
                     i2min  = -1;
                  }
               }
               if (ncnt>0) {
                  qsort(acfr,ncnt,sizeof(float),ACFexPwrSort);
                  qsort(acfi,ncnt,sizeof(float),ACFexPwrSort);
                  lagpwr = sqrt(acfr[ncnt/2]*acfr[ncnt/2] + 
                                acfi[ncnt/2]*acfi[ncnt/2]);
                  if (lagpwr>=(p0/sqrt(1.0*nave))) {
                     acfbuf[R*2*mplgs+2*L]   = acfr[ncnt/2];
                     acfbuf[R*2*mplgs+2*L+1] = acfi[ncnt/2];
                     lagcnt++;
                     sumnzlagpwr += lagpwr;
                  }
               }
            }
         }
         if (sumnzlagpwr < (*noise*(1.0*lagcnt)/sqrt(1.0*nave))) {
            for (L=0;L<mplgs;L++) {
               acfbuf[R*2*mplgs+2*L]   = 0.0;
               acfbuf[R*2*mplgs+2*L+1] = 0.0;
            }
         }
      }
   }
   free(smpwr);
   free(srtpwr);
   free(acfr);
   free(acfi);
   return 0;
}
