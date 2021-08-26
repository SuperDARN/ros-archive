/* fork.c
   ======
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
#include <sys/kernel.h>
#include <sys/name.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include "rtypes.h"
#include "taskid.h"
#include "errlog.h"
#include "socket.h"

/*
 $Log: fork.c,v $
 Revision 1.2  2004/06/22 13:27:55  barnes
 QNX4 Code audit.

 Revision 1.1  2004/05/10 15:38:47  barnes
 Initial revision

*/

  
extern char *taskname;
extern struct TaskID *tsk;

int forkinet(int port) {
 
  char errbuf[256];  
  int status;
  int sock;
  int pipeid[2];
  pid_t cpid; /* process id of the child */

  /* create the pipe */

  if (pipe(pipeid) <0) return -1;

  if ((status=fcntl(pipeid[1],F_GETFL))==-1) return -1;
  status|=O_NONBLOCK;
  if ((status=fcntl(pipeid[1],F_SETFL,status))==-1) return -1; 
 
  if ((cpid=fork()) !=0) {
    close(pipeid[0]);
    return pipeid[1];
  }

  close(pipeid[1]);

  ErrLog(tsk,taskname,"Child server process starting");
 
  sock=createsocket(&port);
  if (sock !=-1) {
    pid_t pid;
    sprintf(errbuf,"Listening on port %d.",port);
    ErrLog(tsk,taskname,errbuf);
    pid=getppid();
    sprintf(errbuf,"Parent PID %d.",(int) pid);
    ErrLog(tsk,taskname,errbuf);
    pid=getpid();
    sprintf(errbuf,"Child PID %d.",(int) pid);
    ErrLog(tsk,taskname,errbuf);
 

  } else ErrLog(tsk,taskname,"Failed to create Socket");

  if (sock !=-1) procsocket(sock,pipeid[0]);
	   
  close(sock);
  close(pipeid[0]);

  ErrLog(tsk,taskname,"Child Server process terminating");

  exit(0);
  
  return -1;
  
}    

