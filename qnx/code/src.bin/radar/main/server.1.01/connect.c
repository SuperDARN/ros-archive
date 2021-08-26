/* connect.c 
   ========= */

/* 
 $Log: connect.c,v $
 Revision 2.3  1998/06/05 11:17:19  root
 Modifications to use the new task_id method of locating a process.

 Revision 2.2  1997/12/18 15:38:16  root
 Made the majority of variables local.

 Revision 2.1  1997/12/18 13:24:03  root
 New version of the server task using pipes and non-blocking sockets.

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
#include "message.h"
#include "task_msg.h"
#include "log_error.h"


extern char errstr[256];
extern struct task_id *tsk;
extern char prog_name[64];

extern struct client client[CLIENT_MAX];
extern int msgmax;

void close_sock(int i) {
  if ((i<msgmax) && (client[i].sock !=-1)) {
    sprintf(errstr,"%s : Close Connection.",client[i].host);
    log_error(tsk,prog_name,errstr);
    close(client[i].sock);
    client[i].sock=-1;
    if (i==msgmax-1) msgmax--;
  } 
}

int open_sock(int sock,struct fd_set *fdset) {
  int i,status;
  int temp,clength;
  struct hostent *cname;
  struct sockaddr_in caddr;
 
  if (FD_ISSET(sock,fdset)==0) return -1;
  for (i=0;(i<msgmax) && (client[i].sock !=-1);i++);
  if (i>=CLIENT_MAX) { 
    /* dequeue the request here */
    log_error(tsk,prog_name,
             "Too many clients attached - refusing connection.");

    temp=accept(sock,0,0);
    if (temp !=-1) close(temp);
    return -1;
  }

  clength=sizeof(caddr);
  if ((client[i].sock=accept(sock,
      (struct sockaddr *) &caddr,&clength))==-1) {
    log_error(tsk,prog_name,"Accept failed.");

     return -1;
  }
  /*
  cname=gethostbyaddr((char *) &caddr.sin_addr,
                       sizeof (caddr.sin_addr),AF_INET);

  
  if ((cname !=NULL) && (cname->h_name !=NULL)) sprintf(client[i].host,
     "[%s (%s)]",cname->h_name,inet_ntoa(caddr.sin_addr));
  */

  sprintf(client[i].host,"[%s]",inet_ntoa(caddr.sin_addr));

  if ((status=fcntl(client[i].sock,F_GETFL))==-1) {
    close(client[i].sock);
    client[i].sock=-1;
    log_error(tsk,prog_name,
             "Failed to read file control block.");
    return -1;
  }
  status|=O_NONBLOCK;
  if ((status=fcntl(client[i].sock,F_SETFL,status))==-1) {
    close(client[i].sock);
    client[i].sock=-1;
    log_error(tsk,prog_name,
             "Failed to write file control block.");
    return -1;
  } 

  sprintf(errstr,"%s : Open Connection.",client[i].host);
  log_error(tsk,prog_name,errstr);

  if (i==msgmax) msgmax++;
  return client[i].sock;
}





