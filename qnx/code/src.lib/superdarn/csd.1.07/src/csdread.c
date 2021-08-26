/* csdread.c
   =========
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


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "rtypes.h"
#include "rconvert.h"
#include "limit.h"
#include "csddata.h"
#include "csdread.h"

/*
 $Log: csdread.c,v $
 Revision 1.6  2006/01/24 16:43:19  barnes
 Fixed bug in calculating the size of a block.

 Revision 1.5  2006/01/09 16:46:38  barnes
 Changed microseconds to long integers.

 Revision 1.4  2004/06/22 17:38:17  barnes
 Changed references of Long to Int.

 Revision 1.3  2004/05/04 18:23:40  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 21:56:30  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2004/03/30 18:36:10  barnes
 Initial revision

*/


int CSDHighExpand(struct CSDfp *fptr,int32 *size,int32 *id) {
  if (fptr->fp !=-1) {
    if (ConvertReadInt(fptr->fp,id) !=0) return -1;
    if (ConvertReadInt(fptr->fp,size) !=0) return -1;
  } else {
    if ((fptr->fptr+8)>fptr->fsze) return -1;
    ConvertToInt(&fptr->fbuf[fptr->fptr],id);
    fptr->fptr+=4;
    ConvertToInt(&fptr->fbuf[fptr->fptr],size);
    fptr->fptr+=4;
  }
  return 0;
}

int CSDHeaderRead(struct CSDfp *fptr,struct CSDdata *ptr,int size) {

  /* the year, maximum value of the three parameters,
     and the station id are encoded in the header */


  if (fptr->fp !=-1) {   
    if (ConvertReadInt(fptr->fp,&ptr->version.major) !=0) return -1;
    if (ConvertReadInt(fptr->fp,&ptr->version.minor) !=0) return -1;
    if (ConvertReadDouble(fptr->fp,&ptr->pmax) !=0) return -1;
    if (ConvertReadDouble(fptr->fp,&ptr->vmax) !=0) return -1;
    if (ConvertReadDouble(fptr->fp,&ptr->wmax) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->stid) !=0) return -1;
    return (lseek(fptr->fp,size-(sizeof(double)*3+2*sizeof(int32)+
                  sizeof(int16)),SEEK_CUR)==-1);
  } else {
    if ((fptr->fptr+sizeof(double)*3+2*sizeof(int32)+
         sizeof(int16))>fptr->fsze) return -1;

    ConvertToInt(fptr->fbuf+fptr->fptr,&ptr->version.major);
    fptr->fptr+=sizeof(int32);
    ConvertToInt(fptr->fbuf+fptr->fptr,&ptr->version.minor);
    fptr->fptr+=sizeof(int32);;
    ConvertToDouble(fptr->fbuf+fptr->fptr,&ptr->pmax);
    fptr->fptr+=sizeof(double);
    ConvertToDouble(fptr->fbuf+fptr->fptr,&ptr->vmax);
    fptr->fptr+=sizeof(double);
    ConvertToDouble(fptr->fbuf+fptr->fptr,&ptr->wmax);
    fptr->fptr+=sizeof(double);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->stid);
    fptr->fptr+=sizeof(int16);
  }
  return 0;
}


int CSDReadBlock(struct CSDfp *fptr,struct CSDdata *ptr) {

  unsigned char gsct[(MAX_RANGE+7)/8];
  unsigned char dflg[(MAX_RANGE+7)/8];
  unsigned char bnc;
  unsigned char store[3*MAX_RANGE];
  int nrng,nbyte;
 
  
  int block=0;
  int rng_num=0;
  int index=0;
  int loop;

  if (fptr->fp !=-1) {
    if (ConvertReadDouble(fptr->fp,&ptr->time) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->scan) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->cp) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->intt.sc) !=0) return -1;
    if (ConvertReadInt(fptr->fp,&ptr->intt.us) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->frang) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->rsep) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->rxrise) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->tfreq) !=0) return -1;
    if (ConvertReadInt(fptr->fp,&ptr->noise) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->atten) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->nave) !=0) return -1;
    if (ConvertReadShort(fptr->fp,&ptr->nrang) !=0) return -1;
  } else {
    if ((fptr->fptr+sizeof(double)+10*sizeof(int16)+
         2*sizeof(int32))>fptr->fsze) return -1;
    ConvertToDouble(&fptr->fbuf[fptr->fptr],&ptr->time);
    fptr->fptr+=sizeof(double);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->scan);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->cp);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->intt.sc);
    fptr->fptr+=sizeof(int16);
    ConvertToInt(fptr->fbuf+fptr->fptr,&ptr->intt.us);
    fptr->fptr+=sizeof(int32);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->frang);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->rsep);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->rxrise);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->tfreq);
    fptr->fptr+=sizeof(int16);
    ConvertToInt(fptr->fbuf+fptr->fptr,&ptr->noise);
    fptr->fptr+=sizeof(int32);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->atten);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->nave);
    fptr->fptr+=sizeof(int16);
    ConvertToShort(fptr->fbuf+fptr->fptr,&ptr->nrang);
    fptr->fptr+=sizeof(int16);
  }
  fptr->ctime=ptr->time;
  nrng=ptr->nrang;
  nbyte=(nrng+7)/8;

  /* read in the data flags */
  if (fptr->fp !=-1) {
    if (read(fptr->fp,gsct,nbyte) !=nbyte) return -1;
    if (read(fptr->fp,dflg,nbyte) !=nbyte) return -1;
    if (read(fptr->fp,&ptr->stored,1) !=1) return -1;  
    if (read(fptr->fp,&bnc,1) !=1) return -1;
   

  } else {
    if ((fptr->fptr+nbyte)>fptr->fsze) return -1;
    memcpy(gsct,&fptr->fbuf[fptr->fptr],nbyte);
    fptr->fptr+=nbyte;
    if ((fptr->fptr+nbyte)>fptr->fsze) return -1;
    memcpy(dflg,&fptr->fbuf[fptr->fptr],nbyte);
    fptr->fptr+=nbyte;
    ptr->stored=fptr->fbuf[fptr->fptr];
    fptr->fptr+=1;
    bnc=fptr->fbuf[fptr->fptr];
    fptr->fptr+=1;
  }

  ptr->bmnum=bnc & 0x0f; 
  ptr->channel=(bnc>>4) & 0x0f;
  
  memset(ptr->store,0,MAX_RANGE*5);

  for (loop=0;loop<nrng;loop++) if (read_bit(dflg,loop) !=0) rng_num++;

  block=0;
  if ((ptr->stored & 0x01) !=0) block+=rng_num;
  if ((ptr->stored & 0x02) !=0) block+=rng_num;
  if ((ptr->stored & 0x04) !=0) block+=rng_num;

  if (fptr->fp !=-1) {
    if (read(fptr->fp,store,block) != block) return -1;
  } else {
    if ((fptr->fptr+block)>fptr->fsze) return -1;
    memcpy(store,&fptr->fbuf[fptr->fptr],block);
    fptr->fptr+=block;
  }

  for (loop=0;loop<nrng;loop++) {
    if (read_bit(gsct,loop) !=0) ptr->store[MAX_RANGE+loop]=1;
    if (read_bit(dflg,loop) !=0) {
      ptr->store[loop]=1;
      block=0;

      if ((ptr->stored & 0x01) !=0) {
        ptr->store[2*MAX_RANGE+loop]=store[index];
        block+=rng_num;
      }
      if ((ptr->stored & 0x02) !=0) {
        ptr->store[3*MAX_RANGE+loop]=store[index+block];
        block+=rng_num;
      }
      if ((ptr->stored & 0x04) !=0) {
        ptr->store[4*MAX_RANGE+loop]=store[index+block];
      }
      index++;
    }
  }
  return 0;
}



int CSDRead(struct CSDfp *fptr,struct CSDdata *ptr) {
  int32 magic;
  int32 size;
  
  if (CSDHighExpand(fptr,&size,&magic) == -1) return -1;
 
  if (magic==-1) {
    if (CSDHeaderRead(fptr,ptr,size)==-1) return -1;
    fptr->blen+=size+2*sizeof(int32);
    return 1;
  } else if (magic=='d') {
    if (CSDReadBlock(fptr,ptr) == -1) return -1;
    fptr->blen=size+2*sizeof(int32);
  }
  return 0;
}






