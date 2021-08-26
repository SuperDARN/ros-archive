/* test_key.c
   ========== */

/*
 $Log: test_key.c,v $
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
 
