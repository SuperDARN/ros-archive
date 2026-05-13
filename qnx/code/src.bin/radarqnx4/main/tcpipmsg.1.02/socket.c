/* socket.c
   ========
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
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

#include "socket.h"
#include "msgmem.h"
#include "connect.h"

/*
 $Log: socket.c,v $
 Revision 1.2  2004/06/22 13:36:17  barnes
 QNX4 Code audit.

 Revision 1.1  2004/05/10 15:40:04  barnes
 Initial revision

*/

struct client client[CLIENT_MAX];
int msgmax=0;
int runloop=1;

char tmpbuf[BUF_SIZE];
char mbuf[BUF_SIZE];

void trap_reset(int signal) {
   runloop=0;
}

void reset() {
  int i;
  for (i=0;i<CLIENT_MAX;i++) {
     client[i].sock=-1;
     client[i].in_sze=0;
     client[i].out_sze=0;
  }
}

int pollsock(int sock,struct timeval *tv,fd_set *fdset) {
  int i;
  FD_SET(sock,fdset);
  for (i=0;i<msgmax;i++) 
    if (client[i].sock !=-1) FD_SET(client[i].sock,fdset);

  if (select(FD_SETSIZE,fdset,0,0,tv) < 0) return -1;
  return 0;
}

int createsocket(int *port) {

  int length;
  int sock;
  struct sockaddr_in server;
 
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

int procsocket(int sock,int inpipe) {

  fd_set fdset; /* selected file descriptors */  
  int poll,i;

  struct timeval tv;

  signal(SIGPIPE,SIG_IGN);
  signal(SIGUSR1,trap_reset);
  listen(sock,5);
 
  tv.tv_sec=0;
  tv.tv_usec=0;

  poll=0;  
  runloop=1; 
  do {

    FD_ZERO(&fdset);
    FD_SET(inpipe,&fdset);  
    if (poll==0) {
      if (pollsock(sock,NULL,&fdset) !=0) continue;
    } else pollsock(sock,&tv,&fdset);

    /* open any new connections if possible */

    opensock(sock,&fdset);

    poll=0;

    /* check to see if the root server has sent any data */

    if (FD_ISSET(inpipe,&fdset)) {
      int size;
      size=read(inpipe,mbuf,BUF_SIZE);
      if (size==0) break;
      writeraw(mbuf,size);
    }
    
    /* send the data to the clients */

    if (writesock() !=0) poll=1;
   
    /* read back any data from the clients */

    readsock(&fdset,tmpbuf,BUF_SIZE); 
    
    /* decode the buffers here */

  } while(runloop);
  
  /* close all the clients down */
 
  for (i=0;i<msgmax;i++) {
    if (client[i].sock !=0) close(client[i].sock);
  }
  close(sock);
  return -1;
}



