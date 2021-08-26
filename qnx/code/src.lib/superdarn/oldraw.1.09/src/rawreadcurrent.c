/* rawreadcurrent.c
   ================
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
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "lmt.h"
#include "raw.h"
#include "oldrawread.h"

/*
 $Log: rawreadcurrent.c,v $
 Revision 1.4  2004/06/22 17:47:16  barnes
 Changed references of Long to Int.

 Revision 1.3  2004/05/04 18:34:31  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 22:26:50  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2004/01/07 21:18:49  barnes
 Initial revision

*/

int32 OldRawDcmpr(unsigned char *word) {
  /* decompress value */
  int count;
  int32 value;
  
  
  count=word[0] & 0x0f;
  value=(word[1] & 0x7f)<<8 | (word[0] & 0xf0); 
  if (count==0) value=value<<1;
  else {
     value=value | 0x8000;
     value=value<<count;
  }
  if ((word[1] & 0x80) !=0) value=-value; 
  return (value);  
}

int OldRawReadCurrent(struct OldRawFp *fp,struct RadarParm *prm,
                      struct RawData *raw) { 

   /* read raw data block from file */


   int radar_parms_pat[]={1,2,2,17,4,2,2,14,4,4,2,4,0,0};
   int i;
   int16 range;
   int j;
   int16 prev_range;
   int xcf_data;
   unsigned int stat;
   int16 num_byte;
   int32 rec_num=0;
   unsigned char *inbuf;
   unsigned char *inbuf_ptr;
   struct rawdata oldraw;
   struct rawdata *rptr;
   int c,d;

   xcf_data = 0;
   prev_range = -1;
 
   rptr=&oldraw;
   inbuf=malloc(sizeof(struct rawdata));

   /* zero out the raw data buffer */
   memset(rptr,0,sizeof(struct rawdata));
   fp->rlen=0;
   do {
     if (ConvertReadShort(fp->rawfp,&num_byte) !=0) {
       free(inbuf);
       return -1;
     }
     fp->rlen+=num_byte;
     fp->ptr+=num_byte;
     num_byte = num_byte - 2;
     stat = read(fp->rawfp,inbuf,num_byte);
     if(stat != num_byte) {
       free(inbuf);
       return -1; 
     }
     inbuf_ptr=inbuf;
     ConvertToInt(inbuf_ptr,&rec_num);
   } while (rec_num==0); 
   inbuf_ptr = inbuf_ptr + 12;  /* skip rec_num + rawwrite */  
   num_byte = num_byte - 12;
  
   /* zero out the raw data buffer */

   /* copy radar_parms */   
   ConvertBlock(inbuf_ptr,radar_parms_pat);
   memcpy((void *) &(rptr->PARMS),inbuf_ptr,sizeof(struct radar_parms)); 
   inbuf_ptr = inbuf_ptr + sizeof(struct radar_parms);
   num_byte = num_byte - sizeof(struct radar_parms);

   /* copy the pulse pattern */
  
   for (i=0;i<rptr->PARMS.MPPUL;i++) {
      ConvertToShort(inbuf_ptr,&rptr->PULSE_PATTERN[i]);
      inbuf_ptr+=sizeof(int16);
      num_byte-=sizeof(int16);
   }

   /* copy the lag table */

    for(j=0;j < 2; ++j)
      for(i=0; i < rptr->PARMS.MPLGS; ++i) {
         ConvertToShort(inbuf_ptr,&rptr->LAG_TABLE[j][i]);
         inbuf_ptr = inbuf_ptr + sizeof(int16);
         num_byte = num_byte - sizeof(int16);
      } 
	  

   /* copy comment buffer */
   memcpy(rptr->COMBF,inbuf_ptr,ORIG_COMBF_SIZE);
   inbuf_ptr = inbuf_ptr + ORIG_COMBF_SIZE;
   num_byte = num_byte - ORIG_COMBF_SIZE;

   /* decompress and copy the lag-0 powers */
   for(i=0 ; i < rptr->PARMS.NRANG ; ++i) {
      rptr->pwr0[i] = OldRawDcmpr(inbuf_ptr);
      inbuf_ptr = inbuf_ptr + sizeof(int16);
      num_byte = num_byte - sizeof(int16);
   }

   /* decompress and copy acfs */
   while ( num_byte > 0 ) {
      ConvertToShort(inbuf_ptr,&range);
      --range;
      inbuf_ptr = inbuf_ptr + sizeof(int16);
      num_byte = num_byte - sizeof(int16);

      if((range <= prev_range) && (rptr->PARMS.XCF))
		   xcf_data = 1;

      for(i = 0; i < rptr->PARMS.MPLGS ; ++i) {   
         for(j=0 ; j < 2; ++j) {
	    if (xcf_data) rptr->xcfd[range][i][j] = OldRawDcmpr(inbuf_ptr);
	    else rptr->acfd[range][i][j] = OldRawDcmpr(inbuf_ptr);
            inbuf_ptr = inbuf_ptr + sizeof(int16);
            num_byte = num_byte - sizeof(int16);
         }
      }
		
      prev_range = range;

   } 
  fp->ctime=TimeYMDHMSToEpoch(rptr->PARMS.YEAR,
			rptr->PARMS.MONTH,
			rptr->PARMS.DAY,
			rptr->PARMS.HOUR,
			rptr->PARMS.MINUT,
			rptr->PARMS.SEC);
  free(inbuf);  


  prm->revision.major=oldraw.PARMS.REV.MAJOR;
  prm->revision.minor=oldraw.PARMS.REV.MINOR;
  prm->cp=oldraw.PARMS.CP;
  prm->stid=oldraw.PARMS.ST_ID;
  prm->time.yr=oldraw.PARMS.YEAR;
  prm->time.mo=oldraw.PARMS.MONTH;
  prm->time.dy=oldraw.PARMS.DAY;
  prm->time.hr=oldraw.PARMS.HOUR;
  prm->time.mt=oldraw.PARMS.MINUT;
  prm->time.sc=oldraw.PARMS.SEC;
  prm->time.us=0;

  prm->txpow=oldraw.PARMS.TXPOW;
  prm->nave=oldraw.PARMS.NAVE;
  prm->atten=oldraw.PARMS.ATTEN;
  prm->lagfr=oldraw.PARMS.LAGFR;
  prm->smsep=oldraw.PARMS.SMSEP;
  prm->ercod=oldraw.PARMS.ERCOD;
  prm->stat.agc=oldraw.PARMS.AGC_STAT;
  prm->stat.lopwr=oldraw.PARMS.LOPWR_STAT;
  prm->noise.search=oldraw.PARMS.NOISE;
  prm->noise.mean=oldraw.PARMS.NOISE_MEAN;
      
  prm->channel=oldraw.PARMS.CHANNEL;
  prm->bmnum=oldraw.PARMS.BMNUM;
  prm->scan=oldraw.PARMS.SCAN;
   
  prm->offset=oldraw.PARMS.usr_resL1; 
  prm->rxrise=oldraw.PARMS.RXRISE;
  prm->intt.sc=oldraw.PARMS.INTT;
  prm->intt.us=0;
  prm->txpl=oldraw.PARMS.TXPL;
    
  prm->mpinc=oldraw.PARMS.MPINC;
  prm->mppul=oldraw.PARMS.MPPUL;
  prm->mplgs=oldraw.PARMS.MPLGS;
  prm->nrang=oldraw.PARMS.NRANG;
  prm->frang=oldraw.PARMS.FRANG;
  prm->rsep=oldraw.PARMS.RSEP;
  prm->xcf=oldraw.PARMS.XCF; 
  prm->tfreq=oldraw.PARMS.TFREQ;
  prm->mxpwr=oldraw.PARMS.MXPWR;
  prm->lvmax=oldraw.PARMS.LVMAX;   

  for (c=0;c<prm->mppul;c++) prm->pulse[c]=oldraw.PULSE_PATTERN[c];
  for (c=0;c<prm->mplgs;c++) {
    prm->lag[c][0]=oldraw.LAG_TABLE[0][c];
    prm->lag[c][1]=oldraw.LAG_TABLE[1][c];
  } 
  strcpy(prm->combf,oldraw.COMBF);

  raw->revision.major=fp->major_rev;
  raw->revision.minor=fp->minor_rev;
   
  for (c=0;c<prm->nrang;c++) {
    raw->pwr0[c]=oldraw.pwr0[c];
    for (d=0;d<prm->mplgs;d++) {
      raw->acfd[c][d][0]=oldraw.acfd[c][d][0];
      raw->acfd[c][d][1]=oldraw.acfd[c][d][1];
    }
  }
   
  if (prm->xcf !=0) for (c=0;c<prm->nrang;c++) {
    for (d=0;d<prm->mplgs;d++) {
      raw->xcfd[c][d][0]=oldraw.xcfd[c][d][0];
      raw->xcfd[c][d][1]=oldraw.xcfd[c][d][1];
    }
  }

  return 0;
}

