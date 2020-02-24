/* echo_util.c
   ===========
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


/*
 $Log: echo_util.c,v $
 Revision 1.4  2001/06/27 20:53:16  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:11:49  barnes
 Added Author Name

 Revision 1.2  1999/03/30 19:30:48  barnes
 Moved task name into a header.

 Revision 1.1  1998/06/19 10:49:39  root
 Initial revision

 Revision 2.1  1998/05/28 18:47:17  root
 Modifications to allow multiple data types to be filtered out.

 */

/*
 $Revision: 1.4 $
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
