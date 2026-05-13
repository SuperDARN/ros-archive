/* echo_util.c
   ===========
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
#include <stdio.h>
#include <string.h>
#ifdef _QNX4
#include <sys/name.h>
#include <sys/kernel.h>
#endif

#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include "rtypes.h"
#include "echo.h"
#include "taskname.h"

/*
 $Log: echo.c,v $
 Revision 1.3  2004/06/16 22:02:16  barnes
 Added missing header.

 Revision 1.2  2004/05/05 23:35:27  barnes
 Defined type ipcid_t for the IPC calls.

 Revision 1.1  2003/10/31 19:00:31  barnes
 Initial revision
*/

int EchoRegister(char *name,char *echoname,char *pass,int plen) {
#ifdef _QNX4
  ipcid_t  ipcid;
  int nlen;
  char msg;
  struct _mxfer_entry send_mx[5];
  struct _mxfer_entry reply_mx[1]; 

  if (echoname !=NULL) { 
    if ( (ipcid=qnx_name_locate(0,echoname,0,NULL)) == -1) return -1;
  } else {
    if ( (ipcid=qnx_name_locate(0,ECHO_DATA,0,NULL)) == -1) return -1;
  }

  msg=ECHO_ADD;
  nlen=strlen(name)+1;
  _setmx(reply_mx,&msg,sizeof(char));
  _setmx(&send_mx[0],&msg,sizeof(char));
  _setmx(&send_mx[1],&nlen,sizeof(long int));
  _setmx(&send_mx[2],&plen,sizeof(long int));
  _setmx(&send_mx[3],name,nlen);
  _setmx(&send_mx[4],pass,plen);
  Sendmx(ipcid,5,1,send_mx,reply_mx);
  if (msg !=ECHO_OK) return -1;
#endif
  return 0;
}

