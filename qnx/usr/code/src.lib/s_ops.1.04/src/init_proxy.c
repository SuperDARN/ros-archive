/* init_proxy.c
   ============ */

/* 
 $Log: init_proxy.c,v $
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

