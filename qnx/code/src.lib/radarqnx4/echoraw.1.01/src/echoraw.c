/* echoraw.c
   =========
   Author: R.J.Barnes
*/

/*
 $Log: echoraw.c,v $
 Revision 1.1  2006/04/07 16:33:31  barnes
 Initial revision
 
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
#include "echoraw.h"
#include "taskname.h"

/*
 $Log: echoraw.c,v $
 Revision 1.1  2006/04/07 16:33:31  barnes
 Initial revision

*/

ipcid_t EchoRawRegister(char *name,char *echoname) {
#ifdef _QNX4
  ipcid_t  ipcid;
  ipcid_t  dipcid;
  int nlen;
  char msg;
  struct _mxfer_entry send_mx[3];
  struct _mxfer_entry reply_mx[2]; 

  if (echoname !=NULL) { 
    if ( (ipcid=qnx_name_locate(0,echoname,0,NULL)) == -1) return -1;
  } else {
    if ( (ipcid=qnx_name_locate(0,ECHORAW,0,NULL)) == -1) return -1;
  }

  msg=ECHORAW_ADD;
  nlen=strlen(name)+1;
  _setmx(&reply_mx[0],&msg,sizeof(char));
  _setmx(&reply_mx[1],&dipcid,sizeof(ipcid_t));
  _setmx(&send_mx[0],&msg,sizeof(char));
  _setmx(&send_mx[1],&nlen,sizeof(long int));
  _setmx(&send_mx[2],name,nlen);
  Sendmx(ipcid,3,2,send_mx,reply_mx);
  if (msg !=ECHORAW_OK) return (ipcid_t) -1;
#endif
  return dipcid;
}

