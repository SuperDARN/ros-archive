/* fclrsgc214.c
   ============
   Author: R.Todd Parris, R.J.Barnes & D.Andre
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
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include "rtypes.h"
#include "gc214.h"
#include "dio.h"
#include "limit.h"
#include "freq.h"
#include "fft.h"
#include "fclrsgc214.h"

/*
 $Log: fclrsgc214.c,v $
 Revision 1.3  2008/03/12 21:10:38  code
 Fixed header.

 Revision 1.2  2008/03/12 20:22:17  code
 Changed name of function for consistency.

 Revision 1.1  2008/03/12 19:44:25  code
 Initial revision

 Revision 1.3  2008/03/12 16:11:48  code
 Added channel switching just in case.

 Revision 1.2  2008/03/04 20:44:15  code
 Fixed bug in selecting the memory buffer.

 Revision 1.1  2007/10/29 16:15:36  code
 Initial revision

*/


#define DELAY 5
#define LOOP 3
#define NN 512
#define MAXBW 1000000

int FCLRSGC214(ipcid_t dioid,ipcid_t *drid,struct FreqTable *fptr,
              int startA,int endA,int rsepA,
              int startB,int endB,int rsepB,
              int Fclk,int roff,int ioff,
	          void **bufadr,int bufoff,
		      int *tfreqA,float *noiseA,float *statisticA,
              int *flgA,void (*userA)(int start,int end,int rsep,
                                    int nn,float *pwr,void *usrdata),
              void *usrdataA,
              int *tfreqB,float *noiseB,float *statisticB,
              int *flgB,void (*userB)(int start,int end,int rsep,
                                    int nn,float *pwr,void *usrdata),
              void *usrdataB) {

  int16 *ptrA;
  int16 *ptrB;
  float fincA,fincB;

  int n,c,l;
  int status;
  int nnA=NN,nnB=NN,nave=15;
  int centreA,bandwidthA,bandtosearchA,samplesinwindowA;
  int centreB,bandwidthB,bandtosearchB,samplesinwindowB;
 
  int offsetA,offsetB,avglengthA,avglengthB;
  int imin;
  float fbwA=3333.3,fbwB=3333.3,remainderA,remainderB;
  double integralA,integralB;
  float minv,aval,fminA,fminB;


  float *fftA=NULL;
  float *pwrA,*pwr2A,*pwrtosearchA,*pwrtosearchfiltA;
  float *faxisA;

  float *fftB=NULL;
  float *pwrB,*pwr2B,*pwrtosearchB,*pwrtosearchfiltB;
  float *faxisB;

  ptrA=(int16 *) bufadr[0];
  ptrB=(int16 *) bufadr[bufoff];
  
  centreA=(startA+endA)/2;

  fbwA=300000/(rsepA*2); 
  DIOSetChannel(dioid,0); 
  DIOSetGain(dioid,0);
  DIOSetFreq(dioid,centreA);

  centreB=(startB+endB)/2;

  fbwB=300000/(rsepB*2); 
  DIOSetChannel(dioid,1); 
  DIOSetGain(dioid,0);
  DIOSetFreq(dioid,centreB);



  /* Routine to calculate nn and actual BW to optimize measurements.
     This routine finds the smallest bandwidth which is greater than
     1.25 times the band to search and is an integer power of 2kHz.
     This allows the output of the FFT to be on 1 kHz boundries in frequency.
     Since the transmitter can only transmit on 1 kHz boundries, 
     it is not necessary to look at frequencies that we cannot transmit on 
     (the power is later averaged over the receiver bandwidth)
  */

  bandtosearchA=endA-startA; 
  nnA=(int)pow(2,ceil(log10(1.25*(float)bandtosearchA)/log10(2))); 

  /* the 1.25 assures the bandwidth looked at is not near the filter 
     roll-off
  */

  if(nnA>1024){
	nnA=512;
	bandtosearchA=300;
	startA=(int)(centreA-bandtosearchA/2+0.49999);
	endB=(int)(centreA+bandtosearchA/2+0.49999);
  }
  bandwidthA=nnA*1000;		

  /* this makes the output of the ffts to be on 1 kHz boundries */

  pwrA=malloc(sizeof(float)*nnA);
  for (n=0;n<nnA;n++) pwrA[n]=0;
  fftA=malloc(sizeof(float)*2*nnA+1);
  pwr2A=malloc(sizeof(float)*nnA);
  

  bandtosearchB=endB-startB; 
  nnB=(int)pow(2,ceil(log10(1.25*(float)bandtosearchB)/log10(2))); 

  /* the 1.25 assures the bandwidth looked at is not near the filter 
     roll-off
  */

  if(nnB>1024){
	nnB=512;
	bandtosearchB=300;
	startB=(int)(centreB-bandtosearchB/2+0.49999);
	endB=(int)(centreB+bandtosearchB/2+0.49999);
  }
  bandwidthB=nnB*1000;		

  /* this makes the output of the ffts to be on 1 kHz boundries */

  pwrB=malloc(sizeof(float)*nnB);
  for (n=0;n<nnB;n++) pwrB[n]=0;
  fftB=malloc(sizeof(float)*2*nnB+1);
  pwr2B=malloc(sizeof(float)*nnB);
  


  if (Fclk==40625000) {
    GC214SetTxFreq( drid[0], centreA);
    GC214SetTxFreq( drid[1], centreB);
  }

  for (l=0;l<LOOP;l++) {

     DIOSetChannel(dioid,1);
     status=GC214DoFCLRScan(drid[0],0,nave*nnA,bandwidthA);
     DIOSetChannel(dioid,2);
     status=GC214DoFCLRScan(drid[1],0,nave*nnB,bandwidthB);
    
     if (status !=SCAN_OK) return -1;
     
     for (c=0;c<nave;c++) {
        for (n=0;n<nnA;n++){
		fftA[1+2*n]=(float) ptrA[2*n+roff+2*c*nnA];
		fftA[2+2*n]=(float) ptrA[2*n+ioff+2*c*nnA]; 
	  }
        for (n=0;n<nnB;n++){
		fftB[1+2*n]=(float) ptrB[2*n+roff+2*c*nnB];
		fftB[2+2*n]=(float) ptrB[2*n+ioff+2*c*nnB]; 
	  }

 
        fft1(fftA,nnA,-1);
        fft1(fftB,nnB,-1);
  
 	/* Calculate the power.  I added the /nn to each fft to nomalize the
	   fft values to actual voltages in.  The fft routine is from Numerical
 	   Recipes, and it does not normalize
 	*/

        for (n=0;n<nnA;n++) pwrA[n]+=(fftA[2*n+1]/nnA)*(fftA[2*n+1]/nnA)+
                                 (fftA[1+2*n+1]/nnA)*(fftA[1+2*n+1]/nnA);

        for (n=0;n<nnB;n++) pwrB[n]+=(fftB[2*n+1]/nnB)*(fftB[2*n+1]/nnB)+
                                 (fftB[1+2*n+1]/nnB)*(fftB[1+2*n+1]/nnB);





     }

  }
  free(fftA);
  free(fftB);

  /* Normalize the sum back to real voltages */
  for(n=0;n<nnA;n++) pwrA[n]=pwrA[n]/(nave*LOOP);
  for(n=0;n<nnB;n++) pwrB[n]=pwrB[n]/(nave*LOOP);


  if (userA !=NULL) (userA)(startA,endA,rsepA,nnA,pwrA,usrdataA);
  if (userB !=NULL) (userB)(startB,endB,rsepB,nnB,pwrB,usrdataB);

  imin=0;
  minv=1e30; 
  fminA=-1;
  fminB=-1;
  fincA=(float)bandwidthA/nnA;
  fincB=(float)bandwidthB/nnB;

  /* Re-arrange the output of the fft to go from start to end (fftshift).
     This centers the fft, so now the first element in pwr2 corresponds
     with the start freq, and goes up to the end freq
  */
  for(n=0;n<(nnA/2);n++){
	pwr2A[nnA/2+n]=pwrA[n];
	pwr2A[n]=pwrA[nnA/2+n];
  }
  free(pwrA);

  for(n=0;n<(nnB/2);n++){
	pwr2B[nnB/2+n]=pwrB[n];
	pwr2B[n]=pwrB[nnB/2+n];
  }
  free(pwrB);

  /* Window the spectrum to those freqs to be searched.
     We chose the bandwidth to be larger than the band to be searched.
     Now, we take out the data only in the band to be searched.
  */

  samplesinwindowA=(int)((endA-startA)*1000/fincA);
  pwrtosearchA=malloc(sizeof(float)*samplesinwindowA);
  offsetA=(int)(startA-(centreA-(bandwidthA/2000)));
  for(n=0;n<samplesinwindowA;n++){
	pwrtosearchA[n]=pwr2A[n+offsetA];
  }
  free(pwr2A);

  samplesinwindowB=(int)((endB-startB)*1000/fincB);
  pwrtosearchB=malloc(sizeof(float)*samplesinwindowB);
  offsetB=(int)(startB-(centreB-(bandwidthB/2000)));
  for(n=0;n<samplesinwindowB;n++){
	pwrtosearchB[n]=pwr2B[n+offsetB];
  }
  free(pwr2B);

  /* filter the data wid width=to fbw. 
     Now we run an averaging window of width=fbw to get the equivalent
     power over our RX window
  */
  remainderA=modf(fbwA/1000,&integralA);
  remainderA=remainderA/2;
  avglengthA=(int)integralA+2;
  pwrtosearchfiltA=malloc(sizeof(float)*(samplesinwindowA-avglengthA));
  
  for(n=0;n<(samplesinwindowA-avglengthA);n++){
        aval=(remainderA*pwrtosearchA[n]); 
  	for(l=1;l<=(avglengthA-2);l++){
		aval+=pwrtosearchA[n+l];
    }
  	aval+=(remainderA*pwrtosearchA[n+1]);
	pwrtosearchfiltA[n]=aval;
  }
  free(pwrtosearchA);
 
  samplesinwindowA=samplesinwindowA-avglengthA;

  remainderB=modf(fbwB/1000,&integralB);
  remainderB=remainderB/2;
  avglengthB=(int)integralB+2;
  pwrtosearchfiltB=malloc(sizeof(float)*(samplesinwindowB-avglengthB));
  
  for(n=0;n<(samplesinwindowB-avglengthB);n++){
        aval=(remainderB*pwrtosearchB[n]); 
  	for(l=1;l<=(avglengthB-2);l++){
		aval+=pwrtosearchB[n+l];
    }
  	aval+=(remainderB*pwrtosearchB[n+1]);
	pwrtosearchfiltB[n]=aval;
  }
  free(pwrtosearchB);
  samplesinwindowB=samplesinwindowB-avglengthB;

  /* Calculate the frequencies that correspond to the powers. 
     Now we calculate the frequencoes that correspond to out 
     noise power measurements
  */

  faxisA=malloc(sizeof(float)*samplesinwindowA);
  for(n=0;n<samplesinwindowA;n++){
	faxisA[n]=(float)(startA*1000)+(float)n*fincA+(avglengthA-1)*fincA/2;
  }

  /* Now we search for the lowest noise power */
  minv=1.08e10; /* this is the maw power that can be detected (2^15)^2 */

  for(n=0;n<samplesinwindowA;n++){
	if( pwrtosearchfiltA[n]<minv ){
		if(FreqTest(fptr,(int)(faxisA[n]/1000))==0){
			minv=pwrtosearchfiltA[n];
			fminA=faxisA[n]/1000;
		}
	}
  }
  *noiseA=minv;
  *tfreqA=fminA;


  faxisB=malloc(sizeof(float)*samplesinwindowB);
  for(n=0;n<samplesinwindowB;n++){
	faxisB[n]=(float)(startB*1000)+(float)n*fincB+(avglengthB-1)*fincB/2;
  }

  /* Now we search for the lowest noise power */
  minv=1.08e10; /* this is the maw power that can be detected (2^15)^2 */

  for(n=0;n<samplesinwindowB;n++){
	if( pwrtosearchfiltB[n]<minv ){
        if ((abs((faxisB[n]/1000)-fminA)>AB_FREQ_DIFF) &&
	        (FreqTest(fptr,(int)(faxisB[n]/1000))==0)){
			minv=pwrtosearchfiltB[n];
			fminB=faxisB[n]/1000;
		}
	}
  }
  *noiseB=minv;
  *tfreqB=fminB;

  free(faxisA);
  free(pwrtosearchfiltA);

  free(faxisB);
  free(pwrtosearchfiltB);

  return 0;
  
}



