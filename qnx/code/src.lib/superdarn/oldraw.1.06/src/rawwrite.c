/* rawwrite.c
   ==========
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
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include "rtypes.h"
#include "rconvert.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "lmt.h"
#include "raw.h"

/*
 $Log: rawwrite.c,v $
 Revision 1.6  2004/07/20 00:51:40  barnes
 Zeroed the header block.

 Revision 1.5  2004/06/22 17:47:16  barnes
 Changed references of Long to Int.

 Revision 1.4  2004/05/04 18:34:31  barnes
 Renamed the type header to avoid confusion.

 Revision 1.3  2004/04/28 18:27:46  barnes
 Fixed time_t.

 Revision 1.2  2004/04/26 22:26:50  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2004/01/15 23:22:17  barnes
 Initial revision

*/

struct rawrechdr { /* this is the header for the output record */
    unsigned char size[2];
    unsigned char number[4];
    char stamp[8];
};


int OldRawTopBit(int32 wrd,int bflg) {
  /* returns the topmost bit in a word that is equal to 1 */

  char *byte;
  unsigned char tmp;
  int i=0;
  int bit;
  if (wrd ==0) return 0; /* zero word */
  byte=(char *) &wrd; 
  if (bflg==1) {
   for (i=3;i>=0;i--) if (byte[i] !=0x00) break;
   bit=32-8*i;
  } else {
    for (i=0;i<4;i++) if (byte[i] !=0x00) break;
    bit=8+8*i;
  }

  tmp=byte[i];
  
  if (tmp & 0xf0) {
    bit-=4;
    tmp=tmp>>0x04;
  }
  
  if (tmp & 0x0c) {
    bit-=2;
    tmp=tmp>>0x02;
  }
  if (tmp & 0x02) bit-=1;
  return bit;
}

void OldRawCmpRawBit(unsigned char *output,float *input,int num) {
  int i;
  int ptr=0;
  int sign=0;
  unsigned long int tmp=0;
  unsigned char *byte;
  int bit=0;
  int bflg;

  bflg=ConvertBitOrder();  
  byte=(unsigned char *) &tmp;

  for (i=0;i<num;i++) { 
    if (input[i] < 0) {
      tmp=labs(input[i]);
      sign=1;
    } else {
      tmp=input[i];
      sign=0;
    } 
    bit=OldRawTopBit(tmp,bflg);
    if (bit==0) tmp=(sign<<15);
    else if (bit>16) tmp=((tmp>>1) & 0x00007ff0) | (sign<<15);
    else tmp=( (tmp >> (17-bit) ) & 0x00007ff0) | 
             ((17-bit) & 0x0f) | (sign <<15);
    if (bflg==1) {
      output[ptr]=byte[0]; 
      output[ptr+1]=byte[1];
    } else {
     output[ptr]=byte[3]; 
     output[ptr+1]=byte[2];
    }
    ptr+=2; 
  }
}


int OldRawWrite(int fid,char *name,struct RadarParm *prm,
                struct RawData *raw,
		int32 recnum,int *rtab) {

  int radar_parms_pat[]={1,2,2,17,4,2,2,14,4,4,2,4,0,0};
  struct rawrechdr *buffer=NULL;
  unsigned char *buf=NULL;
  struct rawdata oldraw;
  struct rawdata *rptr;
  int ptr=0;
  int status;
  int threshold;
  int i,j;
  int c,d,rv;

  memset(&oldraw.PARMS,0,sizeof(struct radar_parms));
  oldraw.PARMS.REV.MAJOR=prm->revision.major;
  oldraw.PARMS.REV.MINOR=prm->revision.minor;
  oldraw.PARMS.NPARM=48;
  oldraw.PARMS.CP=prm->cp;
  oldraw.PARMS.ST_ID=prm->stid;
  oldraw.PARMS.YEAR=prm->time.yr;
  oldraw.PARMS.MONTH=prm->time.mo;
  oldraw.PARMS.DAY=prm->time.dy;
  oldraw.PARMS.HOUR=prm->time.hr;
  oldraw.PARMS.MINUT=prm->time.mt;
  oldraw.PARMS.SEC=prm->time.sc;
    

  oldraw.PARMS.TXPOW=prm->txpow;
  oldraw.PARMS.NAVE=prm->nave;
  oldraw.PARMS.ATTEN=prm->atten;
  oldraw.PARMS.LAGFR=prm->lagfr;
  oldraw.PARMS.SMSEP=prm->smsep;
  oldraw.PARMS.ERCOD=prm->ercod;
  oldraw.PARMS.AGC_STAT=prm->stat.agc;
  oldraw.PARMS.LOPWR_STAT=prm->stat.lopwr;
  oldraw.PARMS.NOISE=prm->noise.search;
  oldraw.PARMS.NOISE_MEAN=prm->noise.mean;
      
  oldraw.PARMS.CHANNEL=prm->channel;
  oldraw.PARMS.BMNUM=prm->bmnum;
  oldraw.PARMS.SCAN=prm->scan;
 
  oldraw.PARMS.RXRISE=prm->rxrise;
  oldraw.PARMS.INTT=prm->intt.sc;
    
  oldraw.PARMS.TXPL=prm->txpl;
    
  oldraw.PARMS.MPINC=prm->mpinc;
  oldraw.PARMS.MPPUL=prm->mppul;
  oldraw.PARMS.MPLGS=prm->mplgs;
  oldraw.PARMS.NRANG=prm->nrang;
  oldraw.PARMS.FRANG=prm->frang;
  oldraw.PARMS.RSEP=prm->rsep;
  oldraw.PARMS.XCF=prm->xcf; 
  oldraw.PARMS.TFREQ=prm->tfreq;
  oldraw.PARMS.MXPWR=prm->mxpwr;
  oldraw.PARMS.LVMAX=prm->lvmax;   

  if (oldraw.PARMS.NRANG>ORIG_MAX_RANGE) oldraw.PARMS.NRANG=ORIG_MAX_RANGE;

  for (c=0;c<prm->mppul;c++) oldraw.PULSE_PATTERN[c]=prm->pulse[c];
  for (c=0;c<prm->mplgs;c++) {
    oldraw.LAG_TABLE[0][c]=prm->lag[c][0];
    oldraw.LAG_TABLE[1][c]=prm->lag[c][1];
  } 
  strcpy(oldraw.COMBF,prm->combf);
   
  for (c=0;c<oldraw.PARMS.NRANG;c++) {
    if (rtab !=NULL) rv=rtab[c];
    else rv=c;
    oldraw.pwr0[c]=raw->pwr0[rv];
    for (d=0;d<prm->mplgs;d++) {
      oldraw.acfd[c][d][0]=raw->acfd[rv][d][0];
      oldraw.acfd[c][d][1]=raw->acfd[rv][d][1];
    }
  }
   
  if (prm->xcf !=0) for (c=0;c<oldraw.PARMS.NRANG;c++) {
    if (rtab !=NULL) rv=rtab[c];
    else rv=c;
    for (d=0;d<prm->mplgs;d++) {
      oldraw.xcfd[c][d][0]=raw->xcfd[rv][d][0];
      oldraw.xcfd[c][d][1]=raw->xcfd[rv][d][1];
    }
  }
   
  rptr=&oldraw;
  buffer=malloc(sizeof(struct rawrechdr)+sizeof(struct rawdata));
  
  strncpy(buffer->stamp,name,8);
  ConvertFromInt(recnum,buffer->number);
  
  buf=(unsigned char *) buffer;
  
  ptr=sizeof(struct rawrechdr);   
  memcpy(buf+ptr,&rptr->PARMS,sizeof(struct radar_parms));
  ConvertBlock(buf+ptr,radar_parms_pat);
  ptr+=sizeof(struct radar_parms);
  
  for (i=0;i<rptr->PARMS.MPPUL;i++) {
    ConvertFromShort(rptr->PULSE_PATTERN[i],buf+ptr);
    ptr+=sizeof(int16);
  }

  for (i=0;i<2;i++) {
    for (j=0;j<rptr->PARMS.MPLGS;j++) {
      ConvertFromShort(rptr->LAG_TABLE[i][j],buf+ptr);
      ptr+=sizeof(int16);
    }
  }
  
  memcpy(buf+ptr,rptr->COMBF,ORIG_COMBF_SIZE);
  ptr+=ORIG_COMBF_SIZE; 
 
  OldRawCmpRawBit(buf+ptr, &rptr->pwr0[0], rptr->PARMS.NRANG);
  ptr += sizeof(int16)*rptr->PARMS.NRANG;
			
  threshold = (raw->thr*rptr->PARMS.NOISE)/2;

  for (i=0; i<rptr->PARMS.NRANG; i++) {
    if (rptr -> pwr0[i] < threshold) continue;
    ConvertFromShort( (int16) (i+1),buf+ptr); 
    ptr+=sizeof(int16);
	 
	for (j=0; j< rptr->PARMS.MPLGS; j++) {
	  OldRawCmpRawBit(buf+ptr,&rptr->acfd[i][j][0],1);
	  ptr+=sizeof(int16);
	  OldRawCmpRawBit(buf+ptr,&rptr->acfd[i][j][1],1);
	  ptr+=sizeof(int16);
    }
  }
  		
  if (rptr->PARMS.XCF) {
    for (i=0; i<rptr->PARMS.NRANG; i++) {
      if (rptr -> pwr0[i] < threshold) continue;
      ConvertFromShort( (int16) (i+1),buf+ptr); 
      ptr+=sizeof(int16);
	 
      for (j=0; j< rptr->PARMS.MPLGS; j++) {
        OldRawCmpRawBit(buf+ptr,&rptr->xcfd[i][j][0],1);
	ptr+=sizeof(int16);
	OldRawCmpRawBit(buf+ptr,&rptr->xcfd[i][j][1],1);
	ptr+=sizeof(int16);
      }
    }
  }
  
  ConvertFromShort(ptr,buffer->size);
     
  status=(write(fid,buffer, ptr) !=ptr);
  free(buffer);
  return status;
}


int OldRawHeaderWrite(int fid,char *name,char *version,int thresh,char *text) {

  struct rawrechdr *buffer=NULL; 
  int16 ptr=0;
  int32 num=0;
  char *txt;

  int status;
  time_t current_time;

  buffer=malloc(sizeof(struct rawrechdr)+sizeof(struct rawdata));
  strncpy(buffer->stamp,name,8);
  ConvertFromInt(num,buffer->number);
 
  txt=(char *) buffer;
  ptr=sizeof(struct rawrechdr);

  ptr+=sprintf(txt+ptr," version %s threshold %d ",version,thresh);
  ptr+=sprintf(txt+ptr,"%s\n",text);  
  
  current_time = time((time_t *) 0);
  ptr+=1+sprintf(txt+ptr,"%s",asctime(gmtime(&current_time)));

  ConvertFromShort(ptr,buffer->size);
  status=(write(fid,buffer,ptr) !=ptr);
  free(buffer);
  return status;
}
   


int OldRawFwrite(FILE *fp,char *name,struct RadarParm *prm,
                 struct RawData *raw,int32 recnum,int *rtab) { 
 return OldRawWrite(fileno(fp),name,prm,raw,recnum,rtab);
}

int OldRawHeaderFwrite(FILE *fp,char *name,char *version,int thresh,
		       char *text) {
  return OldRawHeaderWrite(fileno(fp),name,version,thresh,text);
}













