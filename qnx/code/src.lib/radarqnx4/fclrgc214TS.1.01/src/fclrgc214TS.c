/* fclrgc214TS.c
   =============
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

/*
 $Log: fclrgc214TS.c,v $
 Revision 1.1  2007/05/18 14:56:52  code
 Initial revision

*/


#define DELAY 5
#define LOOP 3
#define NN 512
#define MAXBW 1000000

int FCLRGC214TS(ipcid_t dioid,ipcid_t drid,struct FreqTable *fptr,
              int start,int end,int rsep,int Fclk,int roff,int ioff,
	      void **bufadr,float *noise,float *statistic,
              int *flg,void (*user)(int start,int end,int rsep,
                                    int nn,float *pwr,void *usrdata),
              void *usrdata ) {

  int16 *ptr;
  float finc;

  int n,c,l;
  int status;
  int nn=NN,nave=15;
  int centre,bandwidth,bandtosearch,samplesinwindow;
 
  int offset,avglength;
  int imin;
  float fbw=3333.3,remainder;
  double integral;
  float minv,aval,fmin;


  float *fft=NULL;
  float *pwr,*pwr2,*pwrtosearch,*pwrtosearchfilt;
  float *faxis;





  ptr=(int16 *) bufadr[0];
  
  centre=(start+end)/2;

  fbw=300000/(rsep*2);  
  DIOSetGain(dioid,0);
  DIOSetFreq(dioid,centre);

  /* Routine to calculate nn and actual BW to optimize measurements.
     This routine finds the smallest bandwidth which is greater than
     1.25 times the band to search and is an integer power of 2kHz.
     This allows the output of the FFT to be on 1 kHz boundries in frequency.
     Since the transmitter can only transmit on 1 kHz boundries, 
     it is not necessary to look at frequencies that we cannot transmit on 
     (the power is later averaged over the receiver bandwidth)
  */

  bandtosearch=end-start; 
  nn=(int)pow(2,ceil(log10(1.25*(float)bandtosearch)/log10(2))); 

  /* the 1.25 assures the bandwidth looked at is not near the filter 
     roll-off
  */

  if(nn>1024){
	nn=512;
	bandtosearch=300;
	start=(int)(centre-bandtosearch/2+0.49999);
	end=(int)(centre+bandtosearch/2+0.49999);
  }
  bandwidth=nn*1000;		

  /* this makes the output of the ffts to be on 1 kHz boundries */

  pwr=malloc(sizeof(float)*nn);
  for (n=0;n<nn;n++) pwr[n]=0;
  fft=malloc(sizeof(float)*2*nn+1);
  pwr2=malloc(sizeof(float)*nn);
  

  if (Fclk==40625000) GC214SetTxFreq( drid, centre);
  
  for (l=0;l<LOOP;l++) {

  
     status=GC214DoFCLRScan(drid,0,nave*nn,bandwidth);
    
     if (status !=SCAN_OK) return -1;
     
     for (c=0;c<nave;c++) {
        for (n=0;n<nn;n++){
		fft[1+2*n]=(float) ptr[2*n+roff+2*c*nn];
		fft[2+2*n]=(float) ptr[2*n+ioff+2*c*nn]; 
	}
 
        fft1(fft,nn,-1);
        
 	/* Calculate the power.  I added the /nn to each fft to nomalize the
	   fft values to actual voltages in.  The fft routine is from Numerical
 	   Recipes, and it does not normalize
 	*/

        for (n=0;n<nn;n++) pwr[n]+=(fft[2*n+1]/nn)*(fft[2*n+1]/nn)+
                                     (fft[1+2*n+1]/nn)*(fft[1+2*n+1]/nn);
     }

  }
  free(fft);

  /* Normalize the sum back to real voltages */
  for(n=0;n<nn;n++) pwr[n]=pwr[n]/(nave*LOOP);


  if (user !=NULL) (user)(start,end,rsep,nn,pwr,usrdata);

  imin=0;
  minv=1e30; 
  fmin=-1;
  finc=(float)bandwidth/nn;


  /* Re-arrange the output of the fft to go from start to end (fftshift).
     This centers the fft, so now the first element in pwr2 corresponds
     with the start freq, and goes up to the end freq
  */
  for(n=0;n<(nn/2);n++){
	pwr2[nn/2+n]=pwr[n];
	pwr2[n]=pwr[nn/2+n];
  }
  free(pwr);

  /* Window the spectrum to those freqs to be searched.
     We chose the bandwidth to be larger than the band to be searched.
     Now, we take out the data only in the band to be searched.
  */

  samplesinwindow=(int)((end-start)*1000/finc);
  pwrtosearch=malloc(sizeof(float)*samplesinwindow);
  offset=(int)(start-(centre-(bandwidth/2000)));
  for(n=0;n<samplesinwindow;n++){
	pwrtosearch[n]=pwr2[n+offset];
  }
  free(pwr2);
  
  /* filter the data wid width=to fbw. 
     Now we run an averaging window of width=fbw to get the equivalent
     power over our RX window
  */
  remainder=modf(fbw/1000,&integral);
  remainder=remainder/2;
  avglength=(int)integral+2;
  pwrtosearchfilt=malloc(sizeof(float)*(samplesinwindow-avglength));
  
  for(n=0;n<(samplesinwindow-avglength);n++){
        aval=(remainder*pwrtosearch[n]); 
  	for(l=1;l<=(avglength-2);l++){
		aval+=pwrtosearch[n+l];
        }
  	aval+=(remainder*pwrtosearch[n+1]);

	pwrtosearchfilt[n]=aval;
  }
  free(pwrtosearch);
  samplesinwindow=samplesinwindow-avglength;


  /* Calculate the frequencies that correspond to the powers. 
     Now we calculate the frequencoes that correspond to out 
     noise power measurements
  */

  faxis=malloc(sizeof(float)*samplesinwindow);
  for(n=0;n<samplesinwindow;n++){
	faxis[n]=(float)(start*1000)+(float)n*finc+(avglength-1)*finc/2;
  }

  /* Now we search for the lowest noise power */
  minv=1.08e10; /* this is the maw power that can be detected (2^15)^2 */

  for(n=0;n<samplesinwindow;n++){
	if( pwrtosearchfilt[n]<minv ){
		if(FreqTest(fptr,(int)(faxis[n]/1000))==0){
			minv=pwrtosearchfilt[n];
			fmin=faxis[n]/1000;
		}
	}
  }

  /* Not sure what this does, so I left it in */
  if (statistic !=NULL){
 	for(n=0;n<nn;n++) statistic[0]+=pwr[n];
	statistic[0]=statistic[0]/(nn*nave);
  }
  *noise=minv;

  free(faxis);
  free(pwrtosearchfilt);
  return (int)fmin;
  
}



