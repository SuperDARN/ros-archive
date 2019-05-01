/* errlog.c
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
#include <string.h>
#include <unistd.h>
#include "rtypes.h"
#include "rtimer.h"
#include "taskid.h"

/*
 $Log: errlog.c,v $
 Revision 1.2  2004/05/05 22:02:02  barnes
 Used the type ipcid_t for the IPC calls.

 Revision 1.1  2003/10/31 19:05:05  barnes
 Initial revision

*/


#define TIME_OUT 10.0
#define OK 'o'
#define ERROR 'e'

int flag=0;

char *ErrLogStrTime() {
  char *str;
  time_t clock;
  struct tm *gmt;

  time(&clock);
  gmt = gmtime(&clock); 
  str = asctime(gmt);
  str[strlen(str)-1] = 0; /* get rid of new line */
  return str;
}

int ErrLog(struct TaskID *task,char *name,char *buffer) {
  int s=0;
#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 
  struct _mxfer_entry send_mx[3];
  struct _mxfer_entry reply_mx[1]; 
  ipcid_t ipcid;
 
  char status=0;
  char tname[32];
  int size; 

  if (task==NULL) return -1;
  if (buffer == NULL) return -1;
  size=strlen(buffer)+1;

  if ((ipcid=TaskIDLocate(task))==-1) s=-1;

  if (flag==0) {
    setvbuf(stderr,NULL,_IOLBF,1024);
    flag=1;
  }

  if (name !=NULL) strncpy(tname,name,32);
  else sprintf(tname,"(unknown)");

  if (s==0) {  
    _setmx(&send_mx[0],tname,sizeof(char)*32);
    _setmx(&send_mx[1],&size,sizeof(int));
    _setmx(&send_mx[2],buffer,sizeof(char)*size);
    _setmx(&reply_mx[0],&status,sizeof(char));
 
    tmout.tv_sec=TIME_OUT;
    tmout.tv_usec=0;
    tm=RTimerMake(&tmout);
    if (tm==NULL) s=-1;
    if (s==0) s=Sendmx(ipcid,3,1,send_mx,reply_mx);
    if (tm !=NULL) RTimerFree(tm);  

  }
  if ((s !=0) || (status != OK))  
    fprintf(stderr,"WARNING: Error not logged\n");
  fprintf(stderr,"%s: %d : %s :%s\n",ErrLogStrTime(),getpid(),tname,buffer);
#endif
  return s;
}






