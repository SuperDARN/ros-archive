/* csdwrite.c
   ==========
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "limit.h"
#include "csddata.h"
#include "csdread.h"




int CSDHeaderWrite(int fp,struct CSDdata *ptr) {
  
  int32 magic=-1;
  int32 size;
  
  size=sizeof(double)*3+2*sizeof(int32)+1*sizeof(int16);
  if (ConvertWriteInt(fp,magic) !=0) return -1;
  if (ConvertWriteInt(fp,size) !=0) return -1;
  if (ConvertWriteInt(fp,ptr->version.major) !=0) return -1;
  if (ConvertWriteInt(fp,ptr->version.minor) !=0) return -1;
  if (ConvertWriteDouble(fp,ptr->pmax) !=0) return -1;
  if (ConvertWriteDouble(fp,ptr->vmax) !=0) return -1;
  if (ConvertWriteDouble(fp,ptr->wmax) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->stid) !=0) return -1;
  return 0;
}




int CSDWrite(int fp,struct CSDdata *ptr) {
  int32 magic='d',size;
  int loop,block=0,rng_num=0;
  int nrng,nbyte;
  unsigned char gsct[(MAX_RANGE+7)/8];
  unsigned char dflg[(MAX_RANGE+7)/8];
  unsigned char bm,chn; 

  unsigned char store[3*MAX_RANGE];
  memset(gsct,0,(MAX_RANGE+7)/8);
  memset(dflg,0,(MAX_RANGE+7)/8);
  memset(store,0,MAX_RANGE*3);

  nrng=ptr->nrang;
  nbyte=(nrng+7)/8;
  bm=ptr->bmnum & 0xff;
  chn=ptr->channel & 0xff;

  for (loop=0;loop<nrng;loop++) {
    if (ptr->store[MAX_RANGE+loop] !=0) set_bit(gsct,loop);
    if (ptr->store[loop] !=0) {
       set_bit(dflg,loop);
       rng_num++;
    }
  }
 
  if (rng_num>0) {
    int index=0; 
    for (loop=0;loop<nrng;loop++) {
      if (read_bit(dflg,loop) !=0) {
        block=0;
        if ((ptr->stored & 0x01) !=0) { 
          store[index]=ptr->store[2*MAX_RANGE+loop];
          block+=rng_num;
        }
        if ((ptr->stored & 0x02) !=0) {
          store[index+block]=ptr->store[3*MAX_RANGE+loop];
          block+=rng_num;
        }
        if ((ptr->stored & 0x04) !=0) 
          store[index+block]=ptr->store[4*MAX_RANGE+loop]; 
        index++; 
      } 
    }
  }
  block=0;
  if ((ptr->stored & 0x01) !=0) block+=rng_num;
  if ((ptr->stored & 0x02) !=0) block+=rng_num;
  if ((ptr->stored & 0x04) !=0) block+=rng_num;
    
  size=sizeof(double)+2*nbyte+3+block+sizeof(int16)*10+2*sizeof(int32);
  if (ConvertWriteInt(fp,magic) !=0) return -1;
  if (ConvertWriteInt(fp,size) !=0) return -1;
  if (ConvertWriteDouble(fp,ptr->time) !=0) return -1; 
  if (ConvertWriteShort(fp,ptr->scan) !=0) return -1; 
  if (ConvertWriteShort(fp,ptr->cp) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->intt.sc) !=0) return -1;
  if (ConvertWriteInt(fp,ptr->intt.us) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->frang) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->rsep) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->rxrise) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->tfreq) !=0) return -1;
  if (ConvertWriteInt(fp,ptr->noise) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->atten) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->nave) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->nrang) !=0) return -1;

  if (write(fp, (void *) gsct,nbyte) !=nbyte) return -1;
  if (write(fp, (void *) dflg,nbyte) !=nbyte) return -1;
  if (write(fp, (void *) &ptr->stored,1) !=1) return -1; 
  if (write(fp, (void *) &chn,1) !=1) return -1;
  if (write(fp, (void *) &bm,1) !=1) return -1;

  if ((block !=0) && (write(fp, (void *) store,block) !=block)) return -1;
  return 0;
}




unsigned char *CSDMakeBuffer(struct CSDdata *ptr,int *sze) {

  unsigned char *buf=NULL;
  int size,off;
  int loop,block=0,rng_num=0;
  int nrng,nbyte;
  unsigned char gsct[(MAX_RANGE+7)/8];
  unsigned char dflg[(MAX_RANGE+7)/8];
  unsigned char bm,chn; 

  unsigned char store[3*MAX_RANGE];
  memset(gsct,0,(MAX_RANGE+7)/8);
  memset(dflg,0,(MAX_RANGE+7)/8);
  memset(store,0,MAX_RANGE*3);

  nrng=ptr->nrang;
  nbyte=(nrng+7)/8;
  bm=ptr->bmnum & 0xff; 
  chn=ptr->channel & 0xff;

  for (loop=0;loop<nrng;loop++) {
    if (ptr->store[MAX_RANGE+loop] !=0) set_bit(gsct,loop);
    if (ptr->store[loop] !=0) {
       set_bit(dflg,loop);
       rng_num++;
    }
  }
 
  if (rng_num>0) {
    int index=0; 
    for (loop=0;loop<nrng;loop++) {
      if (read_bit(dflg,loop) !=0) {
        block=0;
        if ((ptr->stored & 0x01) !=0) { 
          store[index]=ptr->store[2*MAX_RANGE+loop];
          block+=rng_num;
        }
        if ((ptr->stored & 0x02) !=0) {
          store[index+block]=ptr->store[3*MAX_RANGE+loop];
          block+=rng_num;
        }
        if ((ptr->stored & 0x04) !=0) 
          store[index+block]=ptr->store[4*MAX_RANGE+loop]; 
        index++; 
      } 
    }
  }
  block=0;
  if ((ptr->stored & 0x01) !=0) block+=rng_num;
  if ((ptr->stored & 0x02) !=0) block+=rng_num;
  if ((ptr->stored & 0x04) !=0) block+=rng_num;
    
  size=sizeof(double)+2*nbyte+3+block+sizeof(int16)*10+2*sizeof(int32);

  buf=malloc(size);
  off=0;
  ConvertFromDouble(ptr->time,buf+off);
  off+=8;  
  ConvertFromShort(ptr->scan,buf+off);
  off+=2;
  ConvertFromShort(ptr->cp,buf+off);
  off+=2;
  ConvertFromShort(ptr->intt.sc,buf+off);
  off+=2;
  ConvertFromInt(ptr->intt.us,buf+off);
  off+=4;
  ConvertFromShort(ptr->frang,buf+off);
  off+=2;
  ConvertFromShort(ptr->rsep,buf+off);
  off+=2;
  ConvertFromShort(ptr->rxrise,buf+off);
  off+=2;
  ConvertFromShort(ptr->tfreq,buf+off);
  off+=2;
  ConvertFromInt(ptr->noise,buf+off);
  off+=4;
  ConvertFromShort(ptr->atten,buf+off);
  off+=2;
  ConvertFromShort(ptr->nave,buf+off);
  off+=2;
  ConvertFromShort(ptr->nrang,buf+off);
  off+=2;
  memcpy(buf+off,gsct,nbyte);
  off+=nbyte;
  memcpy(buf+off,dflg,nbyte);
  off+=nbyte;
  buf[off]=ptr->stored;
  off++;
  buf[off]=chn;
  off++;
  buf[off]=bm;
  off++;

  if (block !=0) memcpy(buf+off,store,block);
  if (sze !=NULL) *sze=size;
  return buf;
}




int CSDHeaderFwrite(FILE *fp,struct CSDdata *ptr) {
  return CSDHeaderWrite(fileno(fp),ptr);
}

int CSDFwrite(FILE *fp,struct CSDdata *ptr) {
  return CSDWrite(fileno(fp),ptr);
}
