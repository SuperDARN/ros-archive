/* cmp_write.c
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

/*
 $Log: cmp_write.c,v $
 Revision 1.7  2002/02/02 21:23:12  barnes
 Fixed bug in setting the channel bits.

 Revision 1.6  2002/01/29 21:42:58  barnes
 Added code to record the program ID in the data file.

 Revision 1.5  2001/06/27 20:46:56  barnes
 Added license tag

 Revision 1.4  2001/01/29 18:11:52  barnes
 Added Author Name

 Revision 1.3  1999/07/24 18:22:04  barnes
 Added version numbers to the file header.

 Revision 1.2  1999/03/18 15:20:28  barnes
 Modified the internal format of the cmp file to use epoch time.

 Revision 1.1  1999/03/16 19:24:26  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "rawdata.h"
#include "fitdata.h"
#include "cmpdata.h"
#include "cnv_data.h"

int cmp_header_write(int fp,struct cmphdr *ptr) {
  
  int32 magic=-1;
  int32 size;
  
  size=sizeof(double)*3+4*sizeof(int16)+2;
  if (write_long(fp,magic) !=0) return -1;
  if (write_long(fp,size) !=0) return -1;
  if (write(fp,&ptr->major_rev,1) !=1) return -1;
  if (write(fp,&ptr->minor_rev,1) !=1) return -1;
  if (write_double(fp,ptr->pmax) !=0) return -1;
  if (write_double(fp,ptr->vmax) !=0) return -1;
  if (write_double(fp,ptr->wmax) !=0) return -1;
  if (write_short(fp,ptr->st_id) !=0) return -1;
  if (write_short(fp,ptr->frang) !=0) return -1;
  if (write_short(fp,ptr->rsep) !=0) return -1;
  if (write_short(fp,ptr->atten) !=0) return -1;
  if (write_short(fp,ptr->cp) !=0) return -1;
  return 0;
}


int cmp_write(int fp,struct cmpdata *ptr,int flg) {
  int32 magic,size;
  int i,loop,index=0,block=0,rng_num=0;
  unsigned char gsct[10];
  unsigned char dflg[10];
 
  unsigned char store[3*75];
  memset(gsct,0,10);
  memset(dflg,0,10);
  memset(store,0,75*3);

  if (ptr->scan !=0) set_bit(dflg,75);
  if (ptr->channel !=0) set_bit(gsct,75); /* mark this as a stereo file */
  set_top(gsct,(ptr->stored & 0x07) | ((ptr->channel>1)<<3) ); 
  set_top(dflg,ptr->bmnum);
  for (loop=0;loop<MAX_RANGE;loop++) {
    if (ptr->store[MAX_RANGE+loop] !=0) set_bit(gsct,loop);
    if (ptr->store[loop] !=0) {
       set_bit(dflg,loop);
       rng_num++;
    }
  }
 
  if (rng_num>0) {
    int index=0; 
    for (loop=0;loop<MAX_RANGE;loop++) {
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
    
  size=sizeof(double)+20+block;
  if (write_long(fp,ptr->magic) !=0) return -1;
  if (write_long(fp,size) !=0) return -1;
  if (write_double(fp,ptr->time) !=0) return -1; 
  
  if (write(fp, (void *) gsct,10) !=10) return -1;
  if (write(fp, (void *) dflg,10) !=10) return -1;

  if ((block !=0) && (write(fp, (void *) store,block) !=block)) return -1;

  if (flg & 0x01) {
    magic=0L;
    size=sizeof(int32)+sizeof(int16);
    if (write_long(fp,magic) !=0) return -1;
    if (write_long(fp,size) !=0) return -1;
    write_short(fp,ptr->tfreq);
    write_long(fp,ptr->noise);
  }
  
  if (flg & 0x02) {
    magic=1L;
    size=sizeof(int16)+sizeof(int16);
    if (write_long(fp,magic) !=0) return -1;
    if (write_long(fp,size) !=0) return -1;
    write_short(fp,ptr->frang);
    write_short(fp,ptr->rsep);
  }

  if (flg & 0x04) {
    magic=2L;
    size=sizeof(int16);
    if (write_long(fp,magic) !=0) return -1;
    if (write_long(fp,size) !=0) return -1;
    write_short(fp,ptr->cp);
  }

  if (flg & 0x08) {
    magic=4L;
    size=sizeof(int16);
    if (write_long(fp,magic) !=0) return -1;
    if (write_long(fp,size) !=0) return -1;
    write_short(fp,ptr->atten);
  }


  return 0;
}

int cmp_header_fwrite(FILE *fp,struct cmphdr *ptr) {
  return cmp_header_write(fileno(fp),ptr);
}

int cmp_fwrite(FILE *fp,struct cmpdata *ptr,int flg) {
  return cmp_write(fileno(fp),ptr,flg);
}














