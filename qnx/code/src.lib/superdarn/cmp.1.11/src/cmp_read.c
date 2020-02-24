/* cmp_read.c
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

/*
 $Log: cmp_read.c,v $
 Revision 1.13  2002/02/06 17:11:17  barnes
 Added code to filter based on channel.

 Revision 1.12  2002/02/02 21:23:12  barnes
 Fixed bug in setting the channel bits.

 Revision 1.11  2002/01/29 21:42:58  barnes
 Added code to record the program ID in the data file.

 Revision 1.10  2001/06/27 20:46:56  barnes
 Added license tag

 Revision 1.9  2001/04/26 18:28:03  barnes
 Fixed minor bug that meant frang and rsep were not correctly set up.

 Revision 1.8  2001/01/29 18:11:51  barnes
 Added Author Name

 Revision 1.7  1999/07/26 16:50:51  barnes
 Modifications so that we don't tailspin on files with bad scan flags -
 The idea is to limit the number of beams in a single scan to 32, any more
 than that and we force a scan flag to be set.

 Revision 1.6  1999/07/26 14:09:20  barnes
 Fixed bug that meant the last record in a file was omitted.

 Revision 1.5  1999/07/25 00:46:52  barnes
 Fixed bug that would confuse old style files without revision information.

 Revision 1.4  1999/07/23 19:28:23  barnes
 Modified the memory allocation system so that it is much more efficient.

 Revision 1.3  1999/05/21 15:16:59  barnes
 Fixed bugs in reading records.

 Revision 1.2  1999/03/18 15:20:28  barnes
 Modified the internal format of the cmp file to use epoch time.

 Revision 1.1  1999/03/16 19:24:26  barnes
 Initial revision

 */


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "limit.h"
#include "cmpdata.h"
#include "cmp_read.h"
#include "cnv_data.h"
#include "cnv_time.h"

int high_expand(struct cmpfp *fptr,int32 *size,int32 *id) {
  if (fptr->fp !=-1) {
     if (read_long(fptr->fp,id) !=0) return -1;
     if (read_long(fptr->fp,size) !=0) return -1;
  } else {
    if ((fptr->fptr+8)>fptr->fsze) return -1;
    cnv_long(&fptr->fbuf[fptr->fptr],id);
    fptr->fptr+=4;
    cnv_long(&fptr->fbuf[fptr->fptr],size);
    fptr->fptr+=4;
  } 
  return 0;
}

int cmp_header_read(struct cmpfp *fptr,int size) {

    /* the year, maximum value of the three parameters,
       and the station id are encoded in the header */
 

  if (fptr->fp !=-1) {
    if (read(fptr->fp,&fptr->hdr.major_rev,1) !=1) return -1;
    if (read(fptr->fp,&fptr->hdr.minor_rev,1) !=1) return -1;
    if (read_double(fptr->fp,&fptr->hdr.pmax) !=0) return -1;
    if (read_double(fptr->fp,&fptr->hdr.vmax) !=0) return -1;
    if (read_double(fptr->fp,&fptr->hdr.wmax) !=0) return -1;

    if (read_short(fptr->fp,&fptr->hdr.st_id) !=0) return -1;
    if (read_short(fptr->fp,&fptr->hdr.frang) !=0) return -1;
    if (read_short(fptr->fp,&fptr->hdr.rsep) !=0) return -1;
    if (read_short(fptr->fp,&fptr->hdr.atten) !=0) return -1;
    if (read_short(fptr->fp,&fptr->hdr.cp) !=0) return -1;
    return (lseek(fptr->fp,size-(8*3+2*4+2),SEEK_CUR)==-1);
  } else {
    fptr->hdr.major_rev=*(fptr->fbuf+fptr->fptr);
    fptr->hdr.minor_rev=*(fptr->fbuf+fptr->fptr+1);
    fptr->fptr+=2;
    cnv_double(fptr->fbuf+fptr->fptr,&fptr->hdr.pmax);
    fptr->fptr+=8;
    cnv_double(fptr->fbuf+fptr->fptr,&fptr->hdr.vmax);
    fptr->fptr+=8;
    cnv_double(fptr->fbuf+fptr->fptr,&fptr->hdr.wmax);
    fptr->fptr+=8;

    cnv_short(fptr->fbuf+fptr->fptr,&fptr->hdr.st_id);
    fptr->fptr+=2;
    cnv_short(fptr->fbuf+fptr->fptr,&fptr->hdr.frang);
    fptr->fptr+=2;
    cnv_short(fptr->fbuf+fptr->fptr,&fptr->hdr.rsep);
    fptr->fptr+=2;
    cnv_short(fptr->fbuf+fptr->fptr,&fptr->hdr.atten);
    fptr->fptr+=2;
    cnv_short(fptr->fbuf+fptr->fptr,&fptr->hdr.cp);
    fptr->fptr+=2; 
  }
  return 0;
}


int cmp_block_read(struct cmpfp *fptr,struct cmpdata *ptr) {

  unsigned char gsct[10];
  unsigned char dflg[10];
  unsigned char store[3*75];
  int size=0;
  int temp=0;
  int block=0;
  int rng_num=0;
  int index=0;
  int loop;
  
  /* read in the time stamp */
  if (fptr->fp !=-1) {
    if (read_double(fptr->fp,&ptr->time) !=0) return -1;
  } else {
    if ((fptr->fptr+8)>fptr->fsze) return -1;
    cnv_double(&fptr->fbuf[fptr->fptr],&ptr->time);
    fptr->fptr+=8;
  }
  fptr->ctime=ptr->time;
  /* read in the data flags */
  if (fptr->fp !=-1) {
    if (read(fptr->fp,gsct,10) !=10) return -1;
    if (read(fptr->fp,dflg,10) !=10) return -1;
  } else {
    if ((fptr->fptr+10)>fptr->fsze) return -1;
    memcpy(gsct,&fptr->fbuf[fptr->fptr],10);
    fptr->fptr+=10;
    if ((fptr->fptr+10)>fptr->fsze) return -1;
    memcpy(dflg,&fptr->fbuf[fptr->fptr],10);
    fptr->fptr+=10;
  }
  /* get the beam number and the scan number */
  ptr->scan=read_bit(dflg,75);
  ptr->stored=read_top(gsct);
  ptr->bmnum=read_top(dflg);

  ptr->channel=read_bit(gsct,75);
  if ((ptr->channel>0) && ((ptr->stored & 0x08) !=0)) {
    ptr->channel++;
    ptr->stored=ptr->stored & 0x07;
  }

  memset(ptr->store,0,75*5);
 
  for (loop=0;loop<MAX_RANGE;loop++) if (read_bit(dflg,loop) !=0) rng_num++;

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
  
  for (loop=0;loop<MAX_RANGE;loop++) {
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

int cmp_read(struct cmpfp *fptr,struct cmpdata *ptr) {
  int32 magic;
  int32 size;
  int status=0;

  if (high_expand(fptr,&size,&magic) == -1) return -1;
  ptr->magic=magic;
  if (magic==-1) {
      if (cmp_header_read(fptr,size) !=0) return -1;
      ptr->frang=fptr->hdr.frang;
      ptr->rsep=fptr->hdr.rsep;
      ptr->atten=fptr->hdr.atten;
      ptr->cp=fptr->hdr.cp;
      fptr->blen+=size+2*sizeof(int32);
      return 2;
  } else if (magic==0) {
    /* interleave record - with two words it is noise and freq. */
    if (fptr->fp !=-1) {
      if (read_short(fptr->fp,&ptr->tfreq) !=0) status= -1;
      if (read_long(fptr->fp,&ptr->noise) !=0) status= -1;
    } else { 
      if ((fptr->fptr+6)>fptr->fsze) status=-1;
      cnv_short(fptr->fbuf+fptr->fptr,&ptr->tfreq);
      fptr->fptr+=2;
      cnv_long(fptr->fbuf+fptr->fptr,&ptr->noise);
      fptr->fptr+=4;
    }      
    if (status == -1) return -1;
    fptr->blen+=size+2*sizeof(int32);
    return 1;
  } else if (magic==1) {
    /* interleave record - with magic word 1, must be frang and rsep */
    if (fptr->fp !=-1) {
      if (read_short(fptr->fp,&ptr->frang) !=0) status= -1;
      if (read_short(fptr->fp,&ptr->rsep) !=0) status= -1;
    } else { 
      if ((fptr->fptr+4)>fptr->fsze) status=-1;
      cnv_short(fptr->fbuf+fptr->fptr,&ptr->frang);
      fptr->fptr+=2;
      cnv_short(fptr->fbuf+fptr->fptr,&ptr->rsep);
      fptr->fptr+=2;   
    }      
    fptr->hdr.frang=ptr->frang;
    fptr->hdr.rsep=ptr->rsep;
    
    if (status == -1) return -1;
    fptr->blen+=size+2*sizeof(int32);
    return 3;
  } else if (magic==2) {

    /* interleave block with magic word 2, must be program ID */    
    if (fptr->fp !=-1) {
      if (read_short(fptr->fp,&ptr->cp) !=0) status= -1;
    } else { 
      if ((fptr->fptr+4)>fptr->fsze) status=-1;
      cnv_short(fptr->fbuf+fptr->fptr,&ptr->cp);
      fptr->fptr+=2;   
    }      
    fptr->hdr.cp=ptr->cp;
    if (status == -1) return -1;
    fptr->blen+=size+2*sizeof(int32);
  } else if (magic==4) {

    /* interleave block with magic word 4, must be attenuation */    
    if (fptr->fp !=-1) {
      if (read_short(fptr->fp,&ptr->atten) !=0) status= -1;
    } else { 
      if ((fptr->fptr+4)>fptr->fsze) status=-1;
      cnv_short(fptr->fbuf+fptr->fptr,&ptr->atten);
      fptr->fptr+=2;   
    }      
    fptr->hdr.atten=ptr->atten;
    if (status == -1) return -1;
    fptr->blen+=size+2*sizeof(int32);
  } else {
    if (cmp_block_read(fptr,ptr) == -1) 
      return -1;
    fptr->blen=size+2*sizeof(int32);
    ptr->frang=fptr->hdr.frang;
    ptr->rsep=fptr->hdr.rsep;

    return 0;  
  }
  return magic;
}


  
