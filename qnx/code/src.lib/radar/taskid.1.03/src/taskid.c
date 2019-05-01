/* taskid.c
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

#include <signal.h>
#include <string.h>
#include <sys/types.h>
#ifdef _QNX4
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "rtypes.h"
#include "taskid.h"

/* 
 $Log: taskid.c,v $
 Revision 1.3  2004/06/16 22:11:55  barnes
 Added missing header.

 Revision 1.2  2004/05/05 21:54:46  barnes
 Used the type ipcid_t for the IPC calls.

 Revision 1.1  2004/04/26 18:38:29  barnes
 Initial revision

*/

struct TaskID *TaskIDMake(char *name) {
  struct TaskID *ptr=NULL;
  if (name[0]==0) return NULL;
  ptr=malloc(sizeof(struct TaskID));
  if (ptr==NULL) return NULL;
  strcpy(ptr->name,name);
#ifdef _QNX4
  ptr->id=qnx_name_locate(0,name,0,NULL);
#endif
  return ptr;
}

pid_t TaskIDLocate(struct TaskID *ptr) {
  if (ptr->id==-1) {
#ifdef _QNX4
    ptr->id=qnx_name_locate(0,ptr->name,0,NULL);
#endif
    if (ptr->id==-1) return -1;
  }
  return ptr->id;
}
