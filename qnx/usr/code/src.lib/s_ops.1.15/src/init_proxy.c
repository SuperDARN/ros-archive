/* init_proxy.c
   ============ */

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
 $Log: init_proxy.c,v $
 Revision 1.2  2001/06/27 21:10:46  barnes
 Added license tag

 Revision 1.1  2000/03/19 20:33:19  barnes
 Initial revision

 */


#include <sys/name.h>
#include <sys/proxy.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "ucont.h"


pid_t init_proxy(char *name){

	pid_t proxy_pid;
    pid_t ucont_proxy;  
    if (name==NULL) proxy_pid = qnx_name_locate(0,UCONT_NAME,0,NULL); 
    else proxy_pid=qnx_name_locate(0,name,0,NULL);
    if (proxy_pid==-1) return -1;

   	ucont_proxy = qnx_proxy_attach(proxy_pid,
                  "interp",strlen("interp"),-1); 
    return ucont_proxy;
}	

