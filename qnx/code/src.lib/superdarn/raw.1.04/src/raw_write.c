/* raw_write.c
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
 $Log: raw_write.c,v $
 Revision 1.4  2001/10/15 15:00:15  barnes
 Changed storage type to floating point.

 Revision 1.3  2001/06/27 20:50:05  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:54  barnes
 Added Author Name

 Revision 1.1  1999/03/16 16:32:48  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include "cnv_data.h"
#include "types.h"
#include "limit.h"
#include "rawdata.h"

struct rawrechdr { /* this is the header for the output record */
    unsigned char size[2];
    unsigned char number[4];
    char stamp[8];
};


int raw_top_bit(int32 wrd,int bflg) {
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

void cmp_raw_bit(unsigned char *output,float *input,int num) {
  int i;
  int ptr=0;
  int sign=0;
  unsigned long int tmp=0;
  unsigned char *byte;
  int bit=0;
  int bflg;

  bflg=bit_order();  
  byte=(unsigned char *) &tmp;

  for (i=0;i<num;i++) { 
    if (input[i] < 0) {
      tmp=labs(input[i]);
      sign=1;
    } else {
      tmp=input[i];
      sign=0;
    } 
    bit=raw_top_bit(tmp,bflg);
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


int raw_write(int fp,char *name,struct rawdata *data,
		       int thresh,int32 recnum) {

  int radar_parms_pat[]={1,2,2,17,4,2,2,14,4,4,2,4,0,0};
  struct rawrechdr *buffer=NULL;
  unsigned char *buf=NULL;
  int ptr=0;
  int status;
  int threshold;
  int i,j;

  buffer=malloc(sizeof(struct rawrechdr)+sizeof(struct rawdata));
  
  strncpy(buffer->stamp,name,8);
  long_cnv(recnum,buffer->number);
  
  buf=(unsigned char *) buffer;
  
  ptr=sizeof(struct rawrechdr);   
  memcpy(buf+ptr,&data->PARMS,sizeof(struct radar_parms));
  cnv_block(buf+ptr,radar_parms_pat);
  ptr+=sizeof(struct radar_parms);
  
  for (i=0;i<data->PARMS.MPPUL;i++) {
    short_cnv(data->PULSE_PATTERN[i],buf+ptr);
    ptr+=sizeof(int16);
  }

  for (i=0;i<2;i++) {
    for (j=0;j<data->PARMS.MPLGS;j++) {
      short_cnv(data->LAG_TABLE[i][j],buf+ptr);
      ptr+=sizeof(int16);
    }
  }
  
  memcpy(buf+ptr,data->COMBF,COMBF_SIZE);
  ptr+=COMBF_SIZE; 
 
  cmp_raw_bit(buf+ptr, &data->pwr0[0], data->PARMS.NRANG);
  ptr += sizeof(int16)*data->PARMS.NRANG;
			
  threshold = (thresh*data->PARMS.NOISE)/2;

  for (i=0; i<data->PARMS.NRANG; i++) {
    if (data -> pwr0[i] < threshold) continue;
    short_cnv( (int16) (i+1),buf+ptr); 
    ptr+=sizeof(int16);
	 
	for (j=0; j< data->PARMS.MPLGS; j++) {
	  cmp_raw_bit(buf+ptr,&data->acfd[i][j][0],1);
	  ptr+=sizeof(int16);
	  cmp_raw_bit(buf+ptr,&data->acfd[i][j][1],1);
	  ptr+=sizeof(int16);
    }
  }
  		
  if (data->PARMS.XCF) {
    for (i=0; i<data->PARMS.NRANG; i++) {
      if (data -> pwr0[i] < threshold) continue;
      short_cnv( (int16) (i+1),buf+ptr); 
      ptr+=sizeof(int16);
	 
      for (j=0; j< data->PARMS.MPLGS; j++) {
        cmp_raw_bit(buf+ptr,&data->xcfd[i][j][0],1);
	ptr+=sizeof(int16);
	cmp_raw_bit(buf+ptr,&data->xcfd[i][j][1],1);
	ptr+=sizeof(int16);
      }
    }
  }
  
  short_cnv(ptr,buffer->size);
     
  status=(write(fp,buffer, ptr) !=ptr);
  free(buffer);
  return status;
}


int raw_header_write(char *name,char *version,int thresh,
		     char *text,int fp) {

  struct rawrechdr *buffer=NULL; 
  int16 ptr=0;
  int32 num=0;
  char *txt;

  int status;
  long current_time;

  buffer=malloc(sizeof(struct rawrechdr)+sizeof(struct rawdata));
  strncpy(buffer->stamp,name,8);
  long_cnv(num,buffer->number);
 
  txt=(char *) buffer;
  ptr=sizeof(struct rawrechdr);

  ptr+=sprintf(txt+ptr," version %s threshold %d ",version,thresh);
  ptr+=sprintf(txt+ptr,"%s\n",text);  
  
  current_time = time((long *) 0);
  ptr+=1+sprintf(txt+ptr,"%s",asctime(gmtime(&current_time)));

  short_cnv(ptr,buffer->size);
  status=(write(fp,buffer,ptr) !=ptr);
  free(buffer);
  return status;
}
   
int raw_fwrite(FILE *fp,char *name,struct rawdata *data,
		       int thresh,int32 recnum) { 
 return raw_write(fileno(fp),name,data,thresh,recnum);
}

int raw_header_fwrite(char *name,char *version,int thresh,
		     char *text,FILE *fp) {
  return raw_header_write(name,version,thresh,text,fileno(fp));
}













