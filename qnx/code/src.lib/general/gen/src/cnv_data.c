/* cnv_data.c
   ========== */

/*
 $Log: cnv_data.c,v $
 Revision 1.3  1999/03/15 16:55:51  barnes
 Added POSIX I/O functions and changed naming convention.

 Revision 1.2  1998/06/05 23:58:06  barnes
 Changed cnv_block to make it more efficient.

 Revision 1.1  1998/06/05 16:25:13  barnes
 Initial revision

 */

/* 
 * Reads in standard ANSI C variable type from a file.
 *
 * This code will have to be changed for machines with non-standard
 * types and litte-endian machines.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "types.h"

int bit_order() {
  int32 test;
  test=1;
  return *((unsigned char *) &test);
}
  

void cnv_long(unsigned char *cnv,int32 *val) {
  unsigned char *out;
  int i,c;
  out=(unsigned char *) val;
  if (bit_order()==0) for (i=0;i<4;i++) out[i]=cnv[3-i];
  else for (i=0;i<4;i++) out[i]=cnv[i];
}

void cnv_short(unsigned char *cnv,int16 *val) {
  short tmp;
  unsigned char *out;
  int i,c;
  out=(unsigned char *) &tmp;
  if (bit_order()==0) for (i=0;i<2;i++) out[i]=cnv[1-i];
  else for (i=0;i<2;i++) out[i]=cnv[i];
  *val=tmp;
}

void cnv_double(unsigned char *cnv,double *val) {
  unsigned char *out;
  int i,c;
  out=(unsigned char *) val; 
  if (bit_order()==0) for (i=0;i<8;i++) out[i]=cnv[7-i];
  else for (i=0;i<8;i++) out[i]=cnv[i];  
}

void cnv_float(unsigned char *cnv,float *val) {
  unsigned char *out;
  int i,c;
  out=(unsigned char *) val;
  if (bit_order()==0) for (i=0;i<4;i++) out[i]=cnv[3-i];
  else for (i=0;i<4;i++) out[i]=cnv[i];
}

void float_cnv(float val,unsigned char *cnv) {
  unsigned char *in;
  int i,c;
  in=(unsigned char *) &val;
  if (bit_order()==0) for (i=0;i<4;i++) cnv[3-i]=in[i];
  else for (i=0;i<4;i++) cnv[i]=in[i];
}

void double_cnv(double val,unsigned char *cnv) {
  unsigned char *in;
  int i,c;
  in=(unsigned char *) &val;
  if (bit_order()==0) for (i=0;i<8;i++) cnv[7-i]=in[i];
  for (i=0;i<8;i++) cnv[i]=in[i];
}

void long_cnv(int32 val,unsigned char *cnv) {
  unsigned char *in;
  int i,c;
  in=(unsigned char *) &val;
  if (bit_order()==0) for (i=0;i<4;i++) cnv[3-i]=in[i];
  else for (i=0;i<4;i++) cnv[i]=in[i];
}

void short_cnv(int16 val,unsigned char *cnv) {
  short tmp;
  unsigned char *in;
  int i,c;
  tmp=val;
  in=(unsigned char *) &tmp;
  if (bit_order()==0) for (i=0;i<2;i++) cnv[1-i]=in[i];
  else for (i=0;i<2;i++) cnv[i]=in[i];
}

void cnv_block(unsigned char *cnv,int *pattern) {
  int i,j,k;
  int bflg;
  unsigned char *ptr;
  unsigned char tmp[8];
  bflg=bit_order();
  if (bflg==1) return;
  ptr=cnv;
  for (i=0;pattern[i] !=0;i+=2) {
    if (pattern[i]==1) ptr+=pattern[i+1]; /* character array so ignore */
    else for (j=0;j<pattern[i+1];j++) {
      for (k=0;k<pattern[i];k++) tmp[pattern[i]-1-k]=ptr[k];
      for (k=0;k<pattern[i];k++) ptr[k]=tmp[k];
      ptr+=pattern[i];
    }   
  }
}

int fread_long(FILE *fp,int32 *val) {
  unsigned char tmp[4];
  if (fread(tmp,4,1,fp) !=1) return -1;
  cnv_long(tmp,val);
  return 0;
}

int fread_short(FILE *fp,int16 *val) {
  unsigned char tmp[2];
  if (fread(tmp,2,1,fp) !=1) return -1;
  cnv_short(tmp,val);
  return 0;
}

int fread_double(FILE *fp,double *val) {
  unsigned char tmp[8];
  if (fread(tmp,8,1,fp) !=1) return -1;
  cnv_double(tmp,val);
  return 0;
}

int fread_float(FILE *fp,float *val) {
  unsigned char tmp[4];
  if (fread(tmp,4,1,fp) !=1) return -1;
  cnv_float(tmp,val);
  return 0;
}

int fwrite_float(FILE *fp,float val) {
  unsigned char tmp[4];
  float_cnv(val,tmp);
  if (fwrite(tmp,4,1,fp) !=1) return -1;
  return 0;
}

int fwrite_double(FILE *fp,double val) {
  unsigned char tmp[8];
  double_cnv(val,tmp);
  if (fwrite(tmp,8,1,fp) !=1) return -1;
  return 0;
}

int fwrite_long(FILE *fp,int32 val) {
  unsigned char tmp[4];
  long_cnv(val,tmp);
  if (fwrite(tmp,4,1,fp) !=1) return -1;
  return 0;
}

int fwrite_short(FILE *fp,int16 val) {
  unsigned char tmp[2];
  short_cnv(val,tmp);
  if (fwrite(tmp,2,1,fp) !=1) return -1;
  return 0;
}

int read_long(int fp,int32 *val) {
  unsigned char tmp[4];
  if (read(fp,tmp,4) !=4) return -1;
  cnv_long(tmp,val);
  return 0;
}

int read_short(int fp,int16 *val) {
  unsigned char tmp[2];
  if (read(fp,tmp,2) !=2) return -1;
  cnv_short(tmp,val);
  return 0;
}

int read_double(int fp,double *val) {
  unsigned char tmp[8];
  if (read(fp,tmp,8) !=8) return -1;
  cnv_double(tmp,val);
  return 0;
}

int read_float(int fp,float *val) {
  unsigned char tmp[4];
  if (read(fp,tmp,4) !=4) return -1;
  cnv_float(tmp,val);
  return 0;
}


int write_float(int fp,float val) {
  unsigned char tmp[4];
  float_cnv(val,tmp);
  if (write(fp,tmp,4) !=4) return -1;
  return 0;
}

int write_double(int fp,double val) {
  unsigned char tmp[8];
  double_cnv(val,tmp);
  if (write(fp,tmp,8) !=8) return -1;
  return 0;
}

int write_long(int fp,int32 val) {
  unsigned char tmp[4];
  long_cnv(val,tmp);
  if (write(fp,tmp,4) !=4) return -1;
  return 0;
}

int write_short(int fp,int16 val) {
  unsigned char tmp[2];
  short_cnv(val,tmp);
  if (write(fp,tmp,2) !=2) return -1;
  return 0;
}

