/* echo_util.c
   =========== */


/*
 $Log: echo_util.c,v $
 Revision 1.2  1999/03/30 19:30:48  barnes
 Moved task name into a header.

 Revision 1.1  1998/06/19 10:49:39  root
 Initial revision

 Revision 2.1  1998/05/28 18:47:17  root
 Modifications to allow multiple data types to be filtered out.

 */

/*
 $Revision: 1.2 $
 */

#include <stdio.h>
#include <sys/name.h>
#include <stdlib.h>
#include <sys/kernel.h>
#include <errno.h>
#include <signal.h>

#include "echo_util.h"
#include "task_name.h"

int echo_register(char *name,char *echo_name,char *pass,int plen) {

 /* self contained procedure to signal start up to echo */

 pid_t echo_id;
 int nlen;
 char msg;
 struct _mxfer_entry send_mx[5];
 struct _mxfer_entry reply_mx[1]; 

 if (echo_name !=NULL) { 
   if ( (echo_id=qnx_name_locate(0,echo_name,0,NULL)) == -1) return -1;
 } else {
   if ( (echo_id=qnx_name_locate(0,ECHO_DATA,0,NULL)) == -1) return -1;
 }

 msg=ECHO_ADD;
 nlen=strlen(name)+1;
 _setmx(reply_mx,&msg,sizeof(char));
 _setmx(&send_mx[0],&msg,sizeof(char));
 _setmx(&send_mx[1],&nlen,sizeof(long int));
 _setmx(&send_mx[2],&plen,sizeof(long int));
 _setmx(&send_mx[3],name,nlen);
 _setmx(&send_mx[4],pass,plen);
 Sendmx(echo_id,5,1,send_mx,reply_mx);
 if (msg !=ECHO_OK) return -1;
 return 0;
}
