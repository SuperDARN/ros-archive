/* rmsgrcv.c
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#ifdef _QNX4
#include <sys/kernel.h>
#include <sys/sendmx.h>
#endif
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include "rtypes.h"
#include "rmsg.h"

/*
 $Log: rmsgrcv.c,v $
 Revision 1.1  2004/05/05 22:18:14  barnes
 Initial revision

*/

char RMsgRcvDecodeOpen(ipcid_t ipcid,int *size,unsigned char **store) {
#ifdef _QNX4
  int offset;
  int status;
  int buflen=0;
  char *bufadr=NULL;
  offset=sizeof(char)+sizeof(int);

  status=(Readmsg(ipcid,offset,&buflen,
          sizeof(int))!=sizeof(int));
  offset+=sizeof(int);

  if ((status ==0) && (buflen !=0)) 
     bufadr=malloc(buflen+1);
  if ((status ==0) && (bufadr !=NULL)) 
    status=(Readmsg(ipcid,offset,bufadr,buflen) !=buflen);
  if ((status==0) && (bufadr !=NULL)) bufadr[buflen]=0;
  if (status==0) {
    *store=bufadr;
    *size=buflen;
  } 
  if (status !=0) return TASK_ERR;
#endif
  return TASK_OK;
}

char RMsgRcvDecodeData(ipcid_t ipcid,struct RMsgBlock *blk,
		       unsigned char **store) {
#ifdef _QNX4
  int offset;
  int status;
  int i;
  unsigned char *dptr;

  offset=sizeof(char)+sizeof(int);
  blk->num=0;
  status=(Readmsg(ipcid,offset,&blk->num,
          sizeof(int)) !=sizeof(int));
  offset+=sizeof(int);
  if (status==0) status=(Readmsg(ipcid,offset,&blk->tsize,
  sizeof(int))!=sizeof(int));
  offset+=sizeof(int);
 
  if (status==0) status=(Readmsg(ipcid,offset,&blk->data,
  sizeof(struct RMsgData)*blk->num) != 
        (sizeof(struct RMsgData)*blk->num));
  offset+=sizeof(struct RMsgData)*blk->num;
  if (*store !=NULL) {
    free(*store);
    *store=NULL;
  }  
  if ((status==0) && (blk->tsize !=0))
    status=((*store=malloc(blk->tsize))==NULL);
  if ((status==0) && (*store !=NULL))
    status=(Readmsg(ipcid,offset,*store,blk->tsize)
                    != blk->tsize);
  dptr=*store;
  if (status==0) for (i=0;i<blk->num;i++) {
    blk->ptr[i]=dptr;
    dptr+=blk->data[i].size;
  }
  if (status!=0) return TASK_ERR;   
#endif
  return TASK_OK;
}

