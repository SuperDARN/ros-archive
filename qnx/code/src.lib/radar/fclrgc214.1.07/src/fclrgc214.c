/* fclrgc214.c
   ===========
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include "rtypes.h"
#include "gc214.h"
#include "dio.h"
#include "limit.h"
#include "freq.h"
#include "fft.h"

/*
 $Log: fclrgc214.c,v $
 Revision 1.7  2004/05/14 16:27:31  barnes
 Divided noise value by three to correct for signal abnormalities.

 Revision 1.6  2004/05/11 15:27:41  barnes
 Changed buffer pointer types.

 Revision 1.5  2004/05/04 18:51:24  barnes
 Renamed the type header to avoid confusion.

 Revision 1.4  2004/05/03 20:05:38  barnes
 Changed type ipcid to ipcid_t.

 Revision 1.3  2004/05/03 18:00:08  barnes
 Reordered headers.

 Revision 1.2  2004/05/03 15:43:59  barnes
 Added ipcid type for the IPC calls.

 Revision 1.1  2004/03/13 20:07:53  barnes
 Initial revision

*/

#define REAL_BUF_OFFSET 0
#define IMAG_BUF_OFFSET 1
#define DELAY 5
#define LOOP 3

int FCLRGC214(ipcid_t dioid,ipcid_t drid,struct FreqTable *fptr,
              int start,int end,int rsep,
	      void **bufadr,float *noise,float *statistic,
              int *flg,void (*user)(int start,int end,int rsep,
                                    int nn,float *pwr,void *usrdata),
              void *usrdata ) {

  int16 *ptr;
  float Fclk=50625000;
  float finc;
  int decimation;

  int n,c,l;
  int status;
  int nn=512,nave=20;
  int centre,bandwidth;
 
  int ind0,ind1;
  int imin,x;
  float fbw=3333.3;
  float minv,aval,fmin,freq;


  float *fft=NULL;
  float *pwr;

  pwr=malloc(sizeof(float)*nn);
  for (n=0;n<nn;n++) pwr[n]=0;

  fft=malloc(sizeof(float)*2*nn+1);

  ptr=(int16 *) bufadr[0];
  
  centre=(start+end)/2;
  bandwidth=1.0*1000*1000;
  fbw=300000/(rsep*2);  
  DIOSetGain(dioid,0);
  DIOSetFreq(dioid,centre);


  for (l=0;l<LOOP;l++) {
     status=GC214DoFCLRScan(drid,0,nave*nn,bandwidth);
    
     if (status !=SCAN_OK) return -1;
     
      for (c=0;c<nave;c++) {
           
      for (n=0;n<nn;n++) fft[1+2*n]=(float) ptr[2*n+REAL_BUF_OFFSET+2*c*nn];
      for (n=0;n<nn;n++) fft[1+2*n+1]=(float) ptr[2*n+IMAG_BUF_OFFSET+2*c*nn]; 
 
      fft1(fft,nn,1);
      
      pwr[0]+=fft[1]*fft[1]+fft[2]*fft[2];
      for (n=1;n<nn;n++) pwr[nn-n]+=fft[1+2*n]*fft[1+2*n]+
                                    fft[1+2*n+1]*fft[1+2*n+1];

    }
  }
  free(fft);

  if (user !=NULL) (user)(start,end,rsep,nn,pwr,usrdata);

  decimation=(int) Fclk/(4*bandwidth);
  finc=Fclk/(4*decimation)/nn;
  ind0=(int) 1000.0*(centre-start)/finc;
  ind1=nn-(int) 1000.0*(end-centre)/finc;

  imin=0;
  minv=1e30; 
  fmin=-1;
 
  for (n=0;n<ind0;n++) {
    if (statistic !=NULL) statistic[0]+=pwr[n];
    if (n>0) x=n-1;
    else x=nn-1;
    aval=pwr[x]+pwr[n]+pwr[n+1];
    x=(n>=(nn/2)) ? (n-nn) : n; 
    freq=(centre*1000.0+x*finc)/1000.0;
    if ((FreqTest(fptr,(int) freq)==0) && (aval<minv)) {
      minv=aval;
      imin=n;
      fmin=freq;
    }
  }
  for (n=ind1;n<nn;n++) {
    if (statistic !=NULL) statistic[0]+=pwr[n];
    if (n<(nn-1)) x=n+1;
    else x=0;
    aval=pwr[n-1]+pwr[n]+pwr[x];
    x=(n>=(nn/2)) ? (n-nn) : n;
    freq=(centre*1000.0+x*finc)/1000.0; 
    if ((FreqTest(fptr,(int) freq)==0) && (aval<minv)) {
      minv=aval;
      imin=n;
      fmin=freq;
    }
  }
  if (statistic !=NULL) statistic[0]=statistic[0]/(nn*nave);
  minv=minv/(LOOP*nave); 
  *noise=(minv/3/nn)*fbw/finc;


  free(pwr);
  return fmin;
  
}



