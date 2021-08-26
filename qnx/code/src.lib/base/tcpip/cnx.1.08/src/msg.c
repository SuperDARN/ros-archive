/* msg.c
   =====
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
#include <unistd.h>
#include <sys/time.h>
#ifdef _QNX4
#include <sys/select.h>
#endif
#include "connex.h"

/*
 $Log: msg.c,v $
 Revision 1.6  2004/08/25 19:05:11  barnes
 Fixes for MacOS 10 (Darwin)

 Revision 1.5  2004/04/30 22:05:59  barnes
 Changed macro definition

 Revision 1.4  2004/04/29 20:14:51  barnes
 Added missing header.

 Revision 1.3  2004/04/28 18:22:55  barnes
 Added missing header.

 Revision 1.2  2004/04/26 20:18:47  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/01 18:06:44  barnes
 Initial revision

*/

int ConnexWriteMem(unsigned char *outbuf,unsigned int outsize,
                   unsigned char *buffer,unsigned int size) {
 
  struct {
     unsigned char msg_id[4];
     unsigned char length[4];
  } msg={{0x49,0x08,0x1e,0x00}, {0,0,0,0}};
 
  /* force little endian alignment */
  msg.length[0]=size & 0xff;
  msg.length[1]=(size & 0xff00) >>8;
  msg.length[2]=(size & 0xff0000) >>16;
  msg.length[3]=(size & 0xff000000) >> 24;
 
  if (outsize < 8) return -1;
  memcpy(outbuf,((char *) &msg),8);
  outsize-=8;
  if (outsize<size) return -1;
  memcpy(outbuf+8,buffer,size); 
  return size+8;
}  

int ConnexReadMem(unsigned char *inbuf,unsigned int insize,
                  unsigned char **buffer) {

  int cnt=0;

  unsigned char msg_id[4]={0x49,0x08,0x1e,0x00};
  unsigned char length[4];
  unsigned long int size;

  ConnexErr=0;
 
  while (cnt<4) { 
    if (inbuf[cnt] != msg_id[cnt]) {
       ConnexErr=MSGERR_OUT_OF_SYNC;
       break;
    }
    cnt++;
  }
  if (cnt<4) return -1;
  memcpy((char *) length,inbuf+4,4);

  /* decode the little endian length information */
  size=length[0];
  size|=length[1]<<8;
  size|=length[2]<<16;
  size|=length[3]<<24;
  
  if (size==0) {
    *buffer=NULL;
    return 0;
  }

  if ((*buffer=malloc(size)) ==NULL) {
    ConnexErr=MSGERR_NOMEM;
    return -1;
  }
  memcpy((*buffer),inbuf+8,size);
  return size;
}  



int ConnexWriteIP(int fildes,unsigned char *buffer,unsigned int size) {
  int byte=0;  
  int cnt=0;
 

  struct {
     unsigned char msg_id[4];
     unsigned char length[4];
  } msg={{0x49,0x08,0x1e,0x00}, {0,0,0,0}};
 
  /* force little endian alignment */
  msg.length[0]=size & 0xff;
  msg.length[1]=(size & 0xff00) >>8;
  msg.length[2]=(size & 0xff0000) >>16;
  msg.length[3]=(size & 0xff000000) >> 24;
  
  while (cnt<8) { 
    byte=write(fildes,((char*) &msg)+cnt,8-cnt);
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }
  if (cnt<8) return -1;

  cnt=0;  /* write out the message */
  while (cnt<size) {
    byte=write(fildes,buffer+cnt,size-cnt); 
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }
  return size;
}  

/* reads a message of the specified fildes */

int ConnexReadIP(int fildes,unsigned char **buffer) {

  int byte;
  int cnt=0;

  unsigned char msg_id[4]={0x49,0x08,0x1e,0x00};
  unsigned char byte_buf;
  unsigned char length[4];
  unsigned long int size;

  ConnexErr=0;
 
  while (cnt<4) { 
    byte=read(fildes,&byte_buf,1);
    if (byte<=0) {
       ConnexErr=MSGERR_PIPE_CLOSED;
       break;
    }
    if (byte_buf != msg_id[cnt]) {
       ConnexErr=MSGERR_OUT_OF_SYNC;
       break;
    }
    cnt++;
  }
  if (cnt<4) return -1;

   cnt=0;
  while (cnt<4) {
    byte=read(fildes,((char *) length)+cnt,4-cnt); 
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }
  if (cnt < 4)  return -1;

  /* decode the little endian length information */
  size=length[0];
  size|=length[1]<<8;
  size|=length[2]<<16;
  size|=length[3]<<24;
  
  if (size==0) {
    if (*buffer !=NULL) free(*buffer);
    *buffer=NULL;
    return 0;
  }

  if (*buffer !=NULL) {  
    char *tmp;  
    if ((tmp=realloc(*buffer,size)) ==NULL) {
      ConnexErr=MSGERR_NOMEM;
      return -1;
    }
    *buffer=tmp;
  } else {
    if ((*buffer=malloc(size)) ==NULL) {
      ConnexErr=MSGERR_NOMEM;
     return -1;
    }
  }

  cnt=0;  /* read in the message */
  while (cnt<size) {
    byte=read(fildes,(*buffer)+cnt,size-cnt); 
    if (byte<=0) {
      ConnexErr=MSGERR_PIPE_CLOSED;
      break;
    }
    cnt+=byte;
  }
  if (cnt<size) {
     free(*buffer);
     *buffer=NULL;
     return -1;
  }
  return size;
}  
