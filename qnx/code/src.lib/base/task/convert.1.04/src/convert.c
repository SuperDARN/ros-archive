/* convert.c
   =========
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
#include <fcntl.h>
#include <unistd.h>
#include "rtypes.h"

/*
 $Log: convert.c,v $
 Revision 1.4  2004/06/22 17:11:55  barnes
 Changed function name of "Long" to "Int" for future 64-bit compatibility.

 Revision 1.3  2004/05/04 17:13:10  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 20:15:52  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/02/24 14:26:48  barnes
 Initial revision

*/

int ConvertBitOrder() {
  int32 test;
  test=1;
  return *((unsigned char *) &test);
}
  

void ConvertToInt(unsigned char *cnv,int32 *val) {
  unsigned char *out;
  int i;
  out=(unsigned char *) val;
  if (ConvertBitOrder()==0) for (i=0;i<4;i++) out[i]=cnv[3-i];
  else for (i=0;i<4;i++) out[i]=cnv[i];
}

void ConvertToShort(unsigned char *cnv,int16 *val) {
  short tmp;
  unsigned char *out;
  int i;
  out=(unsigned char *) &tmp;
  if (ConvertBitOrder()==0) for (i=0;i<2;i++) out[i]=cnv[1-i];
  else for (i=0;i<2;i++) out[i]=cnv[i];
  *val=tmp;
}

void ConvertToDouble(unsigned char *cnv,double *val) {
  unsigned char *out;
  int i;
  out=(unsigned char *) val; 
  if (ConvertBitOrder()==0) for (i=0;i<8;i++) out[i]=cnv[7-i];
  else for (i=0;i<8;i++) out[i]=cnv[i];  
}

void ConvertToFloat(unsigned char *cnv,float *val) {
  unsigned char *out;
  int i;
  out=(unsigned char *) val;
  if (ConvertBitOrder()==0) for (i=0;i<4;i++) out[i]=cnv[3-i];
  else for (i=0;i<4;i++) out[i]=cnv[i];
}

void ConvertFromFloat(float val,unsigned char *cnv) {
  unsigned char *in;
  int i;
  in=(unsigned char *) &val;
  if (ConvertBitOrder()==0) for (i=0;i<4;i++) cnv[3-i]=in[i];
  else for (i=0;i<4;i++) cnv[i]=in[i];
}

void ConvertFromDouble(double val,unsigned char *cnv) {
  unsigned char *in;
  int i;
  in=(unsigned char *) &val;
  if (ConvertBitOrder()==0) for (i=0;i<8;i++) cnv[7-i]=in[i];
  for (i=0;i<8;i++) cnv[i]=in[i];
}

void ConvertFromInt(int32 val,unsigned char *cnv) {
  unsigned char *in;
  int i;
  in=(unsigned char *) &val;
  if (ConvertBitOrder()==0) for (i=0;i<4;i++) cnv[3-i]=in[i];
  else for (i=0;i<4;i++) cnv[i]=in[i];
}

void ConvertFromShort(int16 val,unsigned char *cnv) {
  short tmp;
  unsigned char *in;
  int i;
  tmp=val;
  in=(unsigned char *) &tmp;
  if (ConvertBitOrder()==0) for (i=0;i<2;i++) cnv[1-i]=in[i];
  else for (i=0;i<2;i++) cnv[i]=in[i];
}

void ConvertBlock(unsigned char *cnv,int *pattern) {
  int i,j,k;
  int bflg;
  unsigned char *ptr;
  unsigned char tmp[8];
  bflg=ConvertBitOrder();
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

int ConvertFreadInt(FILE *fp,int32 *val) {
  unsigned char tmp[4];
  if (fread(tmp,4,1,fp) !=1) return -1;
  ConvertToInt(tmp,val);
  return 0;
}

int ConvertFreadShort(FILE *fp,int16 *val) {
  unsigned char tmp[2];
  if (fread(tmp,2,1,fp) !=1) return -1;
  ConvertToShort(tmp,val);
  return 0;
}

int ConvertFreadDouble(FILE *fp,double *val) {
  unsigned char tmp[8];
  if (fread(tmp,8,1,fp) !=1) return -1;
  ConvertToDouble(tmp,val);
  return 0;
}

int ConvertFreadFloat(FILE *fp,float *val) {
  unsigned char tmp[4];
  if (fread(tmp,4,1,fp) !=1) return -1;
  ConvertToFloat(tmp,val);
  return 0;
}

int ConvertFwriteFloat(FILE *fp,float val) {
  unsigned char tmp[4];
  ConvertFromFloat(val,tmp);
  if (fwrite(tmp,4,1,fp) !=1) return -1;
  return 0;
}

int ConvertFwriteDouble(FILE *fp,double val) {
  unsigned char tmp[8];
  ConvertFromDouble(val,tmp);
  if (fwrite(tmp,8,1,fp) !=1) return -1;
  return 0;
}

int ConvertFwriteInt(FILE *fp,int32 val) {
  unsigned char tmp[4];
  ConvertFromInt(val,tmp);
  if (fwrite(tmp,4,1,fp) !=1) return -1;
  return 0;
}

int ConvertFwriteShort(FILE *fp,int16 val) {
  unsigned char tmp[2];
  ConvertFromShort(val,tmp);
  if (fwrite(tmp,2,1,fp) !=1) return -1;
  return 0;
}

int ConvertReadInt(int fp,int32 *val) {
  unsigned char tmp[4];
  if (read(fp,tmp,4) !=4) return -1;
  ConvertToInt(tmp,val);
  return 0;
}

int ConvertReadShort(int fp,int16 *val) {
  unsigned char tmp[2];
  if (read(fp,tmp,2) !=2) return -1;
  ConvertToShort(tmp,val);
  return 0;
}

int ConvertReadDouble(int fp,double *val) {
  unsigned char tmp[8];
  if (read(fp,tmp,8) !=8) return -1;
  ConvertToDouble(tmp,val);
  return 0;
}

int ConvertReadFloat(int fp,float *val) {
  unsigned char tmp[4];
  if (read(fp,tmp,4) !=4) return -1;
  ConvertToFloat(tmp,val);
  return 0;
}


int ConvertWriteFloat(int fp,float val) {
  unsigned char tmp[4];
  ConvertFromFloat(val,tmp);
  if (write(fp,tmp,4) !=4) return -1;
  return 0;
}

int ConvertWriteDouble(int fp,double val) {
  unsigned char tmp[8];
  ConvertFromDouble(val,tmp);
  if (write(fp,tmp,8) !=8) return -1;
  return 0;
}

int ConvertWriteInt(int fp,int32 val) {
  unsigned char tmp[4];
  ConvertFromInt(val,tmp);
  if (write(fp,tmp,4) !=4) return -1;
  return 0;
}

int ConvertWriteShort(int fp,int16 val) {
  unsigned char tmp[2];
  ConvertFromShort(val,tmp);
  if (write(fp,tmp,2) !=2) return -1;
  return 0;
}


