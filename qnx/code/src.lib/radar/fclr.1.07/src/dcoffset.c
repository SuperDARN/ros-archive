/* dcoffset.c
   ========== */

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
#include <sys/time.h>
#include <time.h>
#include "rtypes.h"
#include "limit.h"
#include "freq.h"
#include "dio.h"
#include "ad.h"
#include "tsg.h"
#include "tsgtable.h"
#include "fclr.h"

/*
 $Log: dcoffset.c,v $
 Revision 1.5  2004/05/11 15:38:18  barnes
 Changed buffer type.

 Revision 1.4  2004/05/07 22:28:44  barnes
 Reordered the calling sequence.

 Revision 1.3  2004/05/07 21:52:17  barnes
 Removed redundant code.

 Revision 1.2  2004/05/04 18:50:46  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/05/03 22:31:23  barnes
 Initial revision

*/


int FCLRReadSamples(int16 *buffer, int numsmp, 
                   int gain,int maxatten,int status, int freq,
		   float *dco) {

  int i,j,k;
  int sample;
  int ltemp;
  int on=0;
  static int first=1;
  time_t t;
  static time_t t0;

  static float dcosmp[4][10][200];
  static int smpoff[10]={0,0,0,0,0,0,0,0,0,0};

  float maxdco=600., mindco=-600.;
  int goodsmp=0;

  /* initialize the samples */
  if( first==1 ) {
    for( i=0; i<4; i++ )
      for( j=0; j<10; j++ )
        for( k=0; k<200; k++ ) dcosmp[i][j][k]=0;
  }

  time(&t);
  for( sample=0;sample<numsmp;sample++) {
    ltemp=*buffer;
    dcosmp[0][gain][sample+smpoff[gain]]=ltemp;
    buffer++;
    ltemp=*buffer;
    dcosmp[1][gain][sample+smpoff[gain]]=ltemp;
    buffer++;
    ltemp=*buffer;
    dcosmp[2][gain][sample+smpoff[gain]]=ltemp;
    buffer++;
    ltemp=*buffer;
    dcosmp[3][gain][sample+smpoff[gain]]=ltemp;
    buffer++;
  }
  smpoff[gain]+=numsmp;
  if( smpoff[gain] >= 200 ) smpoff[gain]=0;

  for( i=0; i<4; i++ )
    for( j=0; j<maxatten; j++ ) {
      dco[j*4+i]=0;
      goodsmp=0;	
      for( k=0; k<200; k++ ) {
        if(dcosmp[i][maxatten][k]>=mindco && 
           dcosmp[i][maxatten][k]<=maxdco ) {
	     goodsmp++;
	     dco[4*j+i]+=dcosmp[i][j][k];
	   }
      }
      if( goodsmp>0 ) dco[4*j+i]/=(float) goodsmp;
      else dco[4*j+i]=0;
      if( first==1 ) {
        first=0;
	t0=t;
	t0=1017709200;
      }
      on=((t-t0)/600)%2;
      on=1;
      if( on==0 ) dco[4*j+i]=0;
    }

  return 0;
}


int FCLRGetDCOffset(int tsgid,int samples,int cfreq,
		    int numchn,int maxatten,void **bufadr,
		    ipcid_t dioid,ipcid_t adid,ipcid_t resetid,
		    float *dco,int *flg) {

  int freq;
  int i;
  int status, gain;
  unsigned char nflg=0;
  unsigned char seq;
  int returnvalue;
 
  *flg=0;

  seq=tsgid;
 
  freq=cfreq;

  for(i=0;i<maxatten;i++) {
    gain=DIOSetGain(dioid,i);

    /* set the freq */
    if ((status=DIOSetFreq(dioid,freq)) != 0) {
      /* report frequency error */
      *flg|=FCLR_FREQ_FAIL;
      return -1;
    }
		 
    /* start sampling */
    if (ADDoScan(adid,0,samples*numchn,0,numchn) !=0) {
      *flg|=FCLR_SCAN_FAIL;
      return -1;
    }

    if (DIOSendTSG(dioid,0,&seq,&nflg,&nflg,NULL,NULL) != 0) { 
      ADScanReset(adid,resetid);
      *flg|=FCLR_TSG_FAIL;
      return -1;
    }
		 
    status=(ADGetScanStatus(adid)==SCAN_OK);
    if (status==1 ) 
      returnvalue=FCLRReadSamples((int16 *) bufadr[0],
				  samples,i,maxatten,status,
				  freq,dco);
  }
  return 0;
}


