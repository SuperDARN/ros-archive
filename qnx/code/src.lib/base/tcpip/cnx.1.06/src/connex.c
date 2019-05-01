/* connex.c
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#ifdef _QNX4 
#include <sys/select.h>
#endif
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "connex.h"

/*
 $Log: connex.c,v $
 Revision 1.4  2004/07/08 16:17:58  barnes
 Added test for exceptions.

 Revision 1.3  2004/04/30 22:05:59  barnes
 Changed macro definition

 Revision 1.2  2004/04/26 20:18:47  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/01 18:06:44  barnes
 Initial revision

*/

int ConnexOpen(char *host,int remote_port) {
  int msgin_sock;
  struct sockaddr_in master;
  struct hostent *hp,*gethostbyname();
 
  /* get the address and port of the master server */
  
  master.sin_family=AF_INET;
  hp=gethostbyname(host);
  if (hp==0) {
    /* unknown host error */
    ConnexErr=UNKNOWN_HOST;
    return -1;
  }

  memcpy(&master.sin_addr,hp->h_addr,hp->h_length);
  master.sin_port=htons(remote_port);
 
  msgin_sock=socket(AF_INET,SOCK_STREAM,0);
  if (msgin_sock<0) {
    /*opening stream connection to master*/
     ConnexErr=OPEN_FAIL;
     return -1;
  }
      
  if (connect(msgin_sock,(struct sockaddr *) &master,
	      sizeof(master))<0) {
    /* connecting to the master server */
    close(msgin_sock);
    ConnexErr=CONNECT_FAIL;
    return -1;
  }
  return msgin_sock;
}

void ConnexClose(int msgin_sock) {
 shutdown(msgin_sock,2); 
 close(msgin_sock);
}

int ConnexRead(int num,int *sock,unsigned char **buffer,int *size,int *flag,
                struct timeval *tmout) {
  fd_set read_set;
  fd_set excp_set;
  int status=0,i=0;
  
  FD_ZERO(&read_set);
  FD_ZERO(&excp_set);

  for (i=0;i<num;i++) {
    if (sock[i] !=-1) FD_SET(sock[i],&read_set);
    if (sock[i] !=-1) FD_SET(sock[i],&excp_set);

    flag[i]=0;
  }

  if ((status=select(FD_SETSIZE,&read_set,0,&excp_set,
		     tmout)) < 0) { 
     perror("while waiting client/master I/O");
     return -1;
  }

  if (status==0) return 0;

  for (i=0;i<num;i++) {
    if ((sock[i] !=-1) && (FD_ISSET(sock[i],&excp_set))) {
   
      flag[i]=-1;
      continue;
    }
    if ((sock[i] !=-1) && (FD_ISSET(sock[i],&read_set))) {
      size[i]=ConnexReadIP(sock[i],&buffer[i]);
      if (size[i]==-1) flag[i]=-1;
      else flag[i]=1;
    } 

  }    
  return status;
}
   

 















