/* test_key.c
   ==========
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
 $Log: test_key.c,v $
 Revision 1.4  2001/06/27 20:54:11  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:11:50  barnes
 Added Author Name

 Revision 1.2  1999/03/31 17:06:01  barnes
 Added routine for reading input from the terminal.

 Revision 1.1  1998/06/19 10:44:53  root
 Initial revision

 */

#include <sys/types.h>
#include <sys/name.h>
#include <sys/dev.h>
#include <sys/proxy.h>
#include <sys/console.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "test_key.h"

struct key_str *register_key(int console) { 
  char str[16];
  struct key_str *ptr=NULL;
  struct _console_ctrl *cc;
  int fd; 
  ptr=malloc(sizeof(struct key_str));

  if (console==-1) {
    fd=open("/dev/con1",O_RDWR);
    cc=console_open(fd,O_RDWR);
    close(fd);
    console=console_active(cc,-1);
    console_close(cc);
  }

  sprintf(str,"/dev/con%d\n",console);
  ptr->kb_fd=open(str,O_RDWR);
  if ((ptr->kb_proxy=qnx_proxy_attach(0,0,0,-1)) != -1) {
    dev_mode(ptr->kb_fd,0,_DEV_MODES);
    dev_state(ptr->kb_fd,0,_DEV_EVENT_INPUT);
    dev_arm(ptr->kb_fd,ptr->kb_proxy,_DEV_EVENT_INPUT);
	return ptr;
  }
  close(ptr->kb_fd);
  free(ptr);
  return NULL;
}

void free_key(struct key_str *ptr) {
  close(ptr->kb_fd);
  qnx_proxy_detach(ptr->kb_proxy);
}  

int test_key(pid_t pid,struct key_str *ptr) {
  unsigned state;
  char buf[80];  
  int n;
  if (pid==ptr->kb_proxy) {
     state=dev_state(ptr->kb_fd,0,_DEV_EVENT_INPUT);
     if (state & _DEV_EVENT_INPUT) {
       n=read(ptr->kb_fd,buf,sizeof(buf));
       dev_arm(ptr->kb_fd,ptr->kb_proxy,_DEV_EVENT_INPUT);
       return 1; 
     }
  }
  return 0;
}   

int read_key(pid_t pid,struct key_str *ptr,char *buf,int blen) {
  unsigned state; 
  int n;
  if (pid==ptr->kb_proxy) {
     state=dev_state(ptr->kb_fd,0,_DEV_EVENT_INPUT);
     if (state & _DEV_EVENT_INPUT) {
       n=read(ptr->kb_fd,buf,blen);
       dev_arm(ptr->kb_fd,ptr->kb_proxy,_DEV_EVENT_INPUT);
       return n; 
     }
  }
  return 0;
}   






void key_wait(struct key_str *ptr) {
  int rectid;
  do {
    rectid = Receive(0,NULL,0);
  } while (test_key(rectid,ptr)==0); 
}
 
