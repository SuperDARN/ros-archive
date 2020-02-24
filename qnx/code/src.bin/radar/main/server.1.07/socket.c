/* socket.c
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

/* 
 $Log: socket.c,v $
 Revision 2.4  2001/06/27 20:56:29  barnes
 Added license tag

 Revision 2.3  2001/01/29 18:12:08  barnes
 Added Author Name

 Revision 2.2  1997/12/18 15:38:16  root
 Modifications so that the task can start and stop the TCP/IP handler
 and recover from TCP/IP crashes.

 Revision 2.1  1997/12/18 13:25:55  root
 New version of the Server task using pipes and non-blocking sockets.

 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <sys/uio.h>
#include <errno.h>

#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#include "socket.h"
#include "connect.h"
#include "msg_mem.h"

extern char errstr[256];
extern char errlog_name[64];
extern char prog_name[64];

struct client client[CLIENT_MAX];
int msgmax=0;
int runloop=1;

char tmpbuf[BUF_SIZE];
char mbuf[BUF_SIZE];

void trap_death(int signal) {
  /* trap the broken pipe signal when one of our clients dies */
  if (signal==SIGUSR1) runloop=0;
}

void reset() {
  int i;
  for (i=0;i<CLIENT_MAX;i++) {
     client[i].sock=-1;
     client[i].in_sze=0;
     client[i].out_sze=0;
  }
}

int poll_sock(int sock,struct timeval *tv,struct fd_set *fdset) {
  int i;
  FD_SET(sock,fdset);
  for (i=0;i<msgmax;i++) 
    if (client[i].sock !=-1) FD_SET(client[i].sock,fdset);

  if (select(FD_SETSIZE,fdset,0,0,tv) < 0) return -1;
  return 0;
}

int create_socket(int *port) {

  int length;
  int sock;
  struct sockaddr_in server;

  struct hostent *hp,*gethostbyname();
  struct sigaction act;
  struct timeval tv;
  sigset_t set;
 
  /* reset the client array */
  reset();
  
  sock=socket(AF_INET,SOCK_STREAM,0); /* create our listening socket */
  if (sock<0) return -1;
  
  /* name and bind socket to an address and port number */

  server.sin_family=AF_INET;
  server.sin_addr.s_addr=INADDR_ANY;
  if (*port !=0) server.sin_port=htons(*port); 
  else server.sin_port=0;
  
  if (bind(sock,(struct sockaddr *) &server,sizeof(server))) return -1;
   
  /* Find out assigned port number and print it out */

  length=sizeof(server);
  if (getsockname(sock,(struct sockaddr *) &server,&length)) return -1;
    
  *port=ntohs(server.sin_port);
  return sock;
}

int process_socket(int sock,int inpipe) {

  struct fd_set fdset; /* selected file descriptors */  
  int poll,i;
  struct sigaction act;
  struct timeval tv;
  sigset_t set;

  sigemptyset(&set);
  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=&trap_death;
  sigaction(SIGPIPE,&act,NULL);
  sigaction(SIGUSR1,&act,NULL);
  listen(sock,5);
 
  tv.tv_sec=0;
  tv.tv_usec=0;

  poll=0;  
  runloop=1; 
  do {

    FD_ZERO(&fdset);
    FD_SET(inpipe,&fdset);  
    if (poll==0) {
      if (poll_sock(sock,NULL,&fdset) !=0) continue;
    } else poll_sock(sock,&tv,&fdset);

    /* open any new connections if possible */

    open_sock(sock,&fdset);

    poll=0;

    /* check to see if the root server has sent any data */

    if (FD_ISSET(inpipe,&fdset)) {
      int size;
      size=read(inpipe,mbuf,BUF_SIZE);
      if (size==0) {
        log_error(errlog_name,prog_name,"PIPE broken");
        break;
      }
      write_raw(mbuf,size);
    }
    
    /* send the data to the clients */

    if (write_sock() !=0) poll=1;
   
    /* read back any data from the clients */

    read_sock(&fdset,tmpbuf,BUF_SIZE); 
    
    /* decode the buffers here */

  } while(runloop);
  
  /* close all the clients down */
 
  for (i=0;i<msgmax;i++) {
    if (client[i].sock !=0) close(client[i].sock);
  }
  close(sock);
  return -1;
}

