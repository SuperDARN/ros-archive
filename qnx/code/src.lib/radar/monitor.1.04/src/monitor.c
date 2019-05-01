/* monitor.c
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
#include "monitor.h"

/*
 $Log: monitor.c,v $
 Revision 1.4  2004/05/04 18:56:22  barnes
 Renamed the type header to avoid confusion.

 Revision 1.3  2004/05/03 20:09:06  barnes
 Changed type ipcid to ipcid_t.

 Revision 1.2  2004/05/03 15:35:30  barnes
 Added the ipcid type for the IPC calls.

 Revision 1.1  2004/01/12 19:32:08  barnes
 Initial revision

*/

#define TIME_OUT 5

int MonitorTrigger(ipcid_t ipcid) {
#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[1]; 
  int s;
  int status; 
  char code=TRIGGER;

  _setmx(&send_mx[0],&code,sizeof(char));
  _setmx(&reply_mx[0],&status,sizeof(int)); 
 
  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,1,1,send_mx,reply_mx);

  RTimerFree(tm);

  if (s != 0) return s;
  if (status !=0) return -1;
#endif
  return 0;
}

int MonitorScan(ipcid_t ipcid,int buffer,int sample,int channel) {
#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[4];
  struct _mxfer_entry reply_mx[1]; 
  int s;
  int status; 
  char code=SCAN;

  _setmx(&send_mx[0],&code,sizeof(char));
  _setmx(&send_mx[1],&buffer,sizeof(int));
  _setmx(&send_mx[2],&sample,sizeof(int));
  _setmx(&send_mx[3],&channel,sizeof(int));
  _setmx(&reply_mx[0],&status,sizeof(int)); 
 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,4,1,send_mx,reply_mx);

  RTimerFree(tm);

  if (s != 0) return s;
  if (status !=0) return -1;
#endif
  return 0;
}


