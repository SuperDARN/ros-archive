/* connect.c
   =========
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

#include <sys/uio.h>
#include <errno.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include "errlog.h"
#include "socket.h"

/*
 $Log: connect.c,v $
 Revision 1.3  2004/06/22 13:36:57  barnes
 Fixed log message.

*/

extern struct client client[CLIENT_MAX];
extern int msgmax;


void closesock(int i) {
  if ((i<msgmax) && (client[i].sock !=-1)) {
    
    fprintf(stderr,"%s : Close Connection.\n",client[i].host);
    
    close(client[i].sock);
    client[i].sock=-1;
    if (i==msgmax-1) msgmax--;
  } 
}

int opensock(int sock,fd_set *fdset) {
  int i,status;
 
  int temp,clength;
 
  struct sockaddr_in caddr;
 
  if (FD_ISSET(sock,fdset)==0) return -1;
  for (i=0;(i<msgmax) && (client[i].sock !=-1);i++);
  if (i>=CLIENT_MAX) { 
    /* dequeue the request here */
    
    fprintf(stderr,"Too many clients attached - refusing connection.\n");
    
    temp=accept(sock,0,0);
    if (temp !=-1) close(temp);
    return -1;
  }

  clength=sizeof(caddr);
  if ((client[i].sock=accept(sock,
      (struct sockaddr *) &caddr,&clength))==-1) {
    
     fprintf(stderr,"Accept failed.");
    
     return -1;
  }

  sprintf(client[i].host,"[%s]",inet_ntoa(caddr.sin_addr));

  if ((status=fcntl(client[i].sock,F_GETFL))==-1) {
    close(client[i].sock);
    client[i].sock=-1;
    
   fprintf(stderr,"Failed to read file control block.\n");
    
    return -1;
  }
  status|=O_NONBLOCK;
  if ((status=fcntl(client[i].sock,F_SETFL,status))==-1) {
    close(client[i].sock);
    client[i].sock=-1;
    
    fprintf(stderr,"Failed to write file control block.\n");
    
    return -1;
  } 
  
  fprintf(stderr,"%s : Open Connection.\n",client[i].host);

  
  if (i==msgmax) msgmax++;
  return client[i].sock;
}





