/* msgsnd.c
   ========
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
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "rtypes.h"
#include "rtime.h"
#include "rtimer.h"
#include "rmsg.h"
#include "taskid.h"

/*
 $Log: rmsgsnd.c,v $
 Revision 1.3  2004/05/11 13:52:19  barnes
 Removed redundant variables.

 Revision 1.2  2004/05/08 16:30:23  barnes
 Fixed bug in passing data message.

 Revision 1.1  2004/05/05 22:30:12  barnes
 Initial revision

*/

#define TIME_OUT 10.0

int RMsgSndOpen(struct TaskID *task,int buflen,unsigned char *bufadr) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 
  ipcid_t ipcid;

  struct _mxfer_entry send_mx[4];
  struct _mxfer_entry reply_mx[1]; 
 
  int s;

  char msg=TASK_OPEN;
  char status;
  int size; 

  if (task==NULL) return -1;
  if ((ipcid=TaskIDLocate(task))==-1) return -1;

  size=8*sizeof(int)+buflen*sizeof(char);
  
  _setmx(&send_mx[0],&msg,sizeof(char));
  _setmx(&send_mx[1],&size,sizeof(int));
  _setmx(&send_mx[2],&buflen,sizeof(int));
  _setmx(&send_mx[3],bufadr,sizeof(char)*buflen);
  _setmx(&reply_mx[0],&status,sizeof(char)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,4,1,send_mx,reply_mx);

  RTimerFree(tm);



  if (s !=0) return s;  
  if (status != TASK_OK) return -1;
#endif
  return 0;
}

int RMsgSndClose(struct TaskID *task) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 
  ipcid_t ipcid;

  struct _mxfer_entry send_mx[2];
  struct _mxfer_entry reply_mx[1]; 
  int s;

  char msg=TASK_CLOSE;
  char status;
  int size; 

  if (task==NULL) return -1;
  if ((ipcid=TaskIDLocate(task))==-1) return -1;

  size=0;
  
  _setmx(&send_mx[0],&msg,sizeof(char));
  _setmx(&send_mx[1],&size,sizeof(int));
  _setmx(&reply_mx[0],&status,sizeof(char)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,2,1,send_mx,reply_mx);

  RTimerFree(tm);

 
  if (s !=0) return s;  
  if (status != TASK_OK) return -1;
#endif
  return 0;
}


int RMsgSndQuit(struct TaskID *task) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 
  ipcid_t ipcid;

  struct _mxfer_entry send_mx[2];
  struct _mxfer_entry reply_mx[1]; 
  int s=0;

  char msg=TASK_QUIT;
  char status;
  int size=0;

  if (task==NULL) return -1;
  if ((ipcid=TaskIDLocate(task))==-1) return -1;
  
  _setmx(&send_mx[0],&msg,sizeof(char));
  _setmx(&send_mx[1],&size,sizeof(int));
  _setmx(&reply_mx[0],&status,sizeof(char)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,2,1,send_mx,reply_mx);
 
  RTimerFree(tm);

  if (s !=0) return s;  
  if (status != TASK_OK) return -1;
#endif 
 return 0;
}




int RMsgSndReset(struct TaskID *task) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 
  ipcid_t ipcid;

  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[1]; 
  int s=0;

  char msg=TASK_RESET;
  char status;
  int size=0;

  if (task==NULL) return -1;
  if ((ipcid=TaskIDLocate(task))==-1) return -1;

  _setmx(&send_mx[0],&msg,sizeof(char));
  _setmx(&send_mx[1],&size,sizeof(int));
  _setmx(&reply_mx[0],&status,sizeof(char)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,2,1,send_mx,reply_mx);
  RTimerFree(tm);

  if (s !=0) return s;  
  if (status != TASK_OK) return -1;
#endif
  return 0;
}

int RMsgSndSend(struct TaskID *task,struct RMsgBlock *ptr) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 
  ipcid_t ipcid;
 
  struct _mxfer_entry *send_mx;
  struct _mxfer_entry reply_mx[1]; 
  int s=0;

  int i;
  char msg=TASK_DATA;
  char status;
  int size; 

  if (ptr==NULL) return -1;
  if (ptr->num>=MAX_BUF) return -1;
  if (task==NULL) return -1;
  if ((ipcid=TaskIDLocate(task))==-1) return -1;
  
  size=sizeof(int)*2+sizeof(struct RMsgData)*ptr->num+ptr->tsize;

  send_mx=malloc(sizeof(struct _mxfer_entry)*(ptr->num+5));
  if (send_mx==NULL) return -1;

  _setmx(&send_mx[0],&msg,sizeof(char));
  _setmx(&send_mx[1],&size,sizeof(int));
  _setmx(&send_mx[2],&ptr->num,sizeof(int));
  _setmx(&send_mx[3],&ptr->tsize,sizeof(int));
  _setmx(&send_mx[4],&ptr->data,sizeof(struct RMsgData)*ptr->num);

  for (i=0;i<ptr->num;i++)
    _setmx(&send_mx[5+i],ptr->ptr[i],ptr->data[i].size);

  _setmx(&reply_mx[0],&status,sizeof(char)); 
 
  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return s=-1;

  s=Sendmx(ipcid,5+ptr->num,1,send_mx,reply_mx);
  if (tm !=NULL) RTimerFree(tm);

  free(send_mx);
  if (s !=0) return s;  
  if (status != TASK_OK) return -1;
#endif
  return 0;

}

int RMsgSndAdd(struct RMsgBlock *ptr, 
	      int buflen,unsigned char *buf,int type,int tag) {
   if (ptr->num>=MAX_BUF) return -1;  
   ptr->data[ptr->num].type=type;
   ptr->data[ptr->num].tag=tag;
   ptr->data[ptr->num].size=buflen;
   ptr->data[ptr->num].index=ptr->num;
   ptr->ptr[ptr->num]=buf;
   ptr->tsize+=buflen;
   ptr->num++;
   return 0;
}





