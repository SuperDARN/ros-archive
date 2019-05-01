/* gc214.c
   =======
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
 

#include <sys/types.h>
#include <sys/time.h>
#ifdef _QNX4
#include <sys/kernel.h>
#include <sys/sendmx.h>
#endif
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "rtypes.h"
#include "rtimer.h"
#include "gc214.h"
#include "dma-alloc.h"
#include "dma-addr.h"

#define TIME_OUT 5

/*
 $Log: gc214.c,v $
 Revision 1.5  2004/06/16 22:05:14  barnes
 Fixed return codes.

 Revision 1.4  2004/05/04 18:52:26  barnes
 Renamed the type header to avoid confusion.

 Revision 1.3  2004/05/03 20:06:18  barnes
 Changed type ipcid to ipcid_t.

 Revision 1.2  2004/05/03 16:04:37  barnes
 Added ipcid type for the IPC calls.

 Revision 1.1  2004/01/12 18:28:31  barnes
 Initial revision

*/

int GC214GetBufNum(ipcid_t ipcid) {
#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 
  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[2]; 
  int s;
  int status; 
  char code=GET_BUF_NUM;
  int buffer;

  _setmx(&send_mx[0],&code,sizeof(char));
  _setmx(&reply_mx[0],&status,sizeof(int)); 
  _setmx(&reply_mx[1],&buffer,sizeof(int)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,1,2,send_mx,reply_mx);

  RTimerFree(tm);
  
  if (s != 0) return s;
  if (status !=0) return -1;
  else return buffer;
#else
  return 0; 
#endif
}

int GC214GetBufSize(ipcid_t ipcid) {
#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 
  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[2]; 
  int s;
  int status; 
  char code=GET_BUF_SIZE;
  int size;

  _setmx(&send_mx[0],&code,sizeof(char));
  _setmx(&reply_mx[0],&status,sizeof(int)); 
  _setmx(&reply_mx[1],&size,sizeof(int)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,1,2,send_mx,reply_mx);

  RTimerFree(tm);
  
  if (s != 0) return s;
  if (status !=0) return -1;
  else return size;
#else
  return 0;
#endif
}

void *GC214GetBufAdr(ipcid_t ipcid,int buffer) {
#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 
  struct _mxfer_entry send_mx[2];
  struct _mxfer_entry reply_mx[2]; 
  int s;
  int status; 
  char code=GET_BUF_ADR;
  dma_t dmabuf;

  _setmx(&send_mx[0],&code,sizeof(char));
  _setmx(&send_mx[1],&buffer,sizeof(int));
  _setmx(&reply_mx[0],&status,sizeof(int)); 
  _setmx(&reply_mx[1],&dmabuf,sizeof(dma_t)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return (void *) 0;

  s=Sendmx(ipcid,2,2,send_mx,reply_mx);

  RTimerFree(tm);
  
  if (s != 0) return (void *) 0;
  if (status !=0) return (void *) 0;
  if (dmaaddr(&dmabuf,ipcid) !=0) return  (void *) 0; 
  else return (void *) V_ADDR(&dmabuf); 
#else
  return (void *) 0;
#endif
}


int GC214GetScanStatus(ipcid_t ipcid) {
#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 
  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[1]; 
  int s;
  char code=GET_SCAN_STATUS;
  int scanstatus;

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  _setmx(&send_mx[0],&code,sizeof(char)); 
  _setmx(&reply_mx[0],&scanstatus,sizeof(int)); 

  s=Sendmx(ipcid,1,1,send_mx,reply_mx);

  RTimerFree(tm);
  
  if (s != 0) return s;
  else return scanstatus;
#else
  return 0;
#endif
}

int GC214DoScan(ipcid_t ipcid,int buffer,int sample,int channel) {
#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 
  struct _mxfer_entry send_mx[4];
  struct _mxfer_entry reply_mx[1]; 
  int s;
  int status; 
  char code=DO_SCAN;

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  _setmx(&send_mx[0],&code,sizeof(char));
  _setmx(&send_mx[1],&buffer,sizeof(int));
  _setmx(&send_mx[2],&sample,sizeof(int));
  _setmx(&send_mx[3],&channel,sizeof(int));
  _setmx(&reply_mx[0],&status,sizeof(int)); 
 
  s=Sendmx(ipcid,4,1,send_mx,reply_mx);

  RTimerFree(tm);
  
  if (s != 0) return s;
  if (status !=0) return -1;
#endif
  return 0;
}


int GC214DoFCLRScan(ipcid_t ipcid,int buffer,int sample,int bandwidth) {
#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 
  struct _mxfer_entry send_mx[4];
  struct _mxfer_entry reply_mx[1]; 
  int s;
  int scanstatus=-1; 
  char code=DO_FCLR_SCAN;

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  _setmx(&send_mx[0],&code,sizeof(char));
  _setmx(&send_mx[1],&buffer,sizeof(int));
  _setmx(&send_mx[2],&sample,sizeof(int));
  _setmx(&send_mx[3],&bandwidth,sizeof(int));
  _setmx(&reply_mx[0],&scanstatus,sizeof(int)); 
 
  s=Sendmx(ipcid,4,1,send_mx,reply_mx);
 
  RTimerFree(tm);
  
  if (s != 0) return s;
  else return scanstatus;
#else
  return 0;
#endif
}






int GC214SetSRate(ipcid_t ipcid,int srate) {
#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 
  struct _mxfer_entry send_mx[2];
  struct _mxfer_entry reply_mx[1]; 
  int s;
  int status; 
  char code=SET_SRATE;

  _setmx(&send_mx[0],&code,sizeof(char));
  _setmx(&send_mx[1],&srate,sizeof(int)); 
 _setmx(&reply_mx[0],&status,sizeof(int)); 
 
  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,2,1,send_mx,reply_mx);

  RTimerFree(tm);
  
  if (s != 0) return s;
  if (status !=0) return -1;
#endif
  return 0;
}


