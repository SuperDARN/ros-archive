/* cmp_write.c
   =========== */

/*
 $Log: cmp_write.c,v $
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
  int pat[]={8,3,2,3,0,0};
  struct cmphdr copy;
  int32 magic=-1;
  int32 size;

  memcpy(&copy,ptr,sizeof(struct cmphdr));
  size=sizeof(double)*3+3*sizeof(int16)+2;
  if (write_long(fp,magic) !=0) return -1;
  if (write_long(fp,size) !=0) return -1;
  cnv_block( (unsigned char *) &copy,pat);
  if (write(fp,&copy,size) !=size) return -1;
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
  set_top(gsct,ptr->stored);
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
  return 0;
}

int cmp_header_fwrite(FILE *fp,struct cmphdr *ptr) {
  return cmp_header_write(fileno(fp),ptr);
}

int cmp_fwrite(FILE *fp,struct cmpdata *ptr,int flg) {
  return cmp_write(fileno(fp),ptr,flg);
}














