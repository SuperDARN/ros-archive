/* fork.c
   ======
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
 $Log: fork.c,v $
 Revision 2.5  2001/06/27 20:56:29  barnes
 Added license tag

 Revision 2.4  2001/01/29 18:12:08  barnes
 Added Author Name

 Revision 2.3  1998/06/05 11:17:19  root
 Modifications to use the new task_id method of locating a process.

 Revision 2.2  1997/12/18 15:38:16  root
 Tidied up start and stop of the TCP/IP handler process.

 Revision 2.1  1997/12/18 13:24:03  root
 New version of the server task using pipes and non-blocking sockets.

 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include "message.h"
#include "task_msg.h"
#include "log_error.h"

extern char errstr[256];
extern struct task_id *tsk;
extern char prog_name[64];

int create_socket(int *);
int process_socket(int,int);
  
int fork_inet(int port) {  
  int i,status;
  int sock;
  int pipeid[2];
  pid_t cpid; /* process id of the child */

  /* create the pipe */

  if (pipe(pipeid) <0) return -1;

  /* make the pipe unblocking 
   *
   * as we are dealing with a pipe a message is
   * either sent or discarded. So there is no possibility
   * that the task will hang - however if the inet processing
   * fork does not empty the pipe quickly enough, it is possible
   * for a record to be lost.
   */

  if ((status=fcntl(pipeid[1],F_GETFL))==-1) return -1;
  status|=O_NONBLOCK;
  if ((status=fcntl(pipeid[1],F_SETFL,status))==-1) return -1; 
 
  if ((cpid=fork()) !=0) {
    close(pipeid[0]);
    return pipeid[1];
  }
  close(pipeid[1]);

  log_error(tsk,prog_name,"Starting TCP/IP Handler");

  sock=create_socket(&port);
  if (sock !=-1) {
    sprintf(errstr,"Listening on port %d.",port);
    log_error(tsk,prog_name,errstr);
  } else log_error(tsk,prog_name,"Failed to create socket");

  if (sock !=-1) process_socket(sock,pipeid[0]);
  close(sock);
  close(pipeid[0]);
  log_error(tsk,prog_name,"Stopping TCP/IP Handler");
  exit(0);
  return -1;
}    
