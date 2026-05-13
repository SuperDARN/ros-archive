/* rawmfeed.c
   ==========
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
 

#include <sys/types.h>
#include <sys/time.h>

#ifdef _QNX4
#include <sys/kernel.h>
#include <sys/sendmx.h>
#endif

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "rtypes.h"
#include "rtimer.h"
#include "limit.h"
#include "tsg.h"
#include "rawfeed.h"

/*
 $Log: rawfeed.c,v $
 Revision 1.1  2006/04/07 14:36:56  barnes
 Initial revision

*/

#define TIME_OUT 5



int RawFeedTrigger(ipcid_t ipcid) {
#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[2];
  struct _mxfer_entry reply_mx[1]; 
  int s;
  int status;
  int size=0; 
  char code=TRIGGER;

  _setmx(&send_mx[0],&code,sizeof(char));
  _setmx(&send_mx[1],&size,sizeof(int));
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

int RawFeedScan(ipcid_t ipcid,int buffer,int sample,int channel,
                int atten,struct timespec *tval) {
#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[7];
  struct _mxfer_entry reply_mx[1]; 
  int s;
  int status; 
  int size;
  char code=SCAN;
  
  size=sizeof(struct timespec)+5*sizeof(int);

  _setmx(&send_mx[0],&code,sizeof(char));
  _setmx(&send_mx[1],&size,sizeof(int));
  _setmx(&send_mx[2],tval,sizeof(struct timespec));
  _setmx(&send_mx[3],&buffer,sizeof(int));
  _setmx(&send_mx[4],&sample,sizeof(int));
  _setmx(&send_mx[5],&channel,sizeof(int));
  _setmx(&send_mx[6],&atten,sizeof(int));

  _setmx(&reply_mx[0],&status,sizeof(int)); 
 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,7,1,send_mx,reply_mx);

  RTimerFree(tm);

  if (s != 0) return s;
  if (status !=0) return -1;
#endif
  return 0;
}



int RawFeedSet(ipcid_t ipcid,struct TSGprm *prm,int mplgs,int (*lag)[2],
               int atten,struct timespec *tval) {
#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[8];
  struct _mxfer_entry reply_mx[1]; 
  int s,n;
  int status; 
  char code=SET;
  int size=0;

  int pat[PULSE_SIZE];
  int lags[LAG_SIZE*2];
 
   for (n=0;n<prm->mppul;n++) pat[n]=prm->pat[n];
   for (n=0;n<mplgs;n++) {
     lags[n]=lag[n][0];
     lags[mplgs+n]=lag[n][1];
   }

  size=sizeof(struct timespec)+sizeof(struct TSGprm)+
             sizeof(int)*(2+prm->mppul+mplgs*2);
    
  _setmx(&send_mx[0],&code,sizeof(char));
  _setmx(&send_mx[1],&size,sizeof(int));
  _setmx(&send_mx[2],tval,sizeof(struct timespec));
  _setmx(&send_mx[3],prm,sizeof(struct TSGprm));
  _setmx(&send_mx[4],pat,sizeof(int)*prm->mppul);
  _setmx(&send_mx[5],&mplgs,sizeof(int));
  _setmx(&send_mx[6],lags,sizeof(int)*mplgs*2);
  _setmx(&send_mx[7],&atten,sizeof(int));
  

  _setmx(&reply_mx[0],&status,sizeof(int)); 
 
  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,8,1,send_mx,reply_mx);

  RTimerFree(tm);

  if (s != 0) return s;
  if (status !=0) return -1;
#endif
  return 0;
}






