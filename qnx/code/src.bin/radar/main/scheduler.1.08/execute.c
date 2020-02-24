/* execute.c
   =========
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
 $Log: execute.c,v $
 Revision 1.7  2001/06/27 20:56:24  barnes
 Added license tag

 Revision 1.6  2001/01/29 18:12:08  barnes
 Added Author Name

 Revision 1.5  2000/11/20 14:39:48  barnes
 Fixed minor bugs in the code.

 Revision 1.4  2000/10/30 17:25:51  barnes
 Changed setting of argv[0] to basename(name).

 Revision 1.3  2000/09/18 20:45:40  barnes
 Added code so that the schedule is reloaded the moment it is altered.

 Revision 1.2  1999/04/14 17:12:57  barnes
 Fixed bug in creating the child that meant it ended up as a zombie
 process.

 Revision 1.1  1999/03/30 18:29:38  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/sched.h>
#include <sys/wait.h>
#include <process.h>
#include "schedule.h"

#define EXIT 'E'
#define EXIT_TIME_OUT 120.0

void terminate_signal_handler(int sig_number) { /* null handler for time out */
}

void terminate(pid_t prog_id) {
  /* kill off a running program */

  double time=EXIT_TIME_OUT;
  char msg=EXIT; 
  timer_t timer_id=-1;
  struct itimerspec timer;
  struct sigevent event;
  void (*prev_handler)(int);
  int pid;

  prev_handler=signal(SIGUSR1,terminate_signal_handler);
  
  if (prev_handler==SIG_ERR) {
    kill(prog_id,SIGKILL);   
    return;
  }

  timer.it_interval.tv_sec=0L;
  timer.it_interval.tv_nsec=0L;
  timer.it_value.tv_sec=(long int) time;
  timer.it_value.tv_nsec=(long int)  ((time - (long int) time)*1e9);
  event.sigev_signo=SIGUSR1;

  if ( ((timer_id=timer_create(CLOCK_REALTIME,&event))==-1) ||
       (timer_settime(timer_id,0,&timer,NULL)==-1)) {
    signal(SIGUSR1,prev_handler);  
    kill(prog_id,SIGKILL);
    return;
  }     
 
  pid=Send(prog_id,&msg,NULL,sizeof(char),0);

  if (timer_id !=-1) {
    timer_delete(timer_id);
    signal(SIGUSR1,prev_handler);
  } 

  if (pid==-1) kill(prog_id,SIGKILL); 
  if (getprio(prog_id) !=-1) kill(prog_id,SIGKILL);
}
   


int execute(char *path,char *prog) {
  int pid,i;
  char command[2*SCHED_LINE_LENGTH+1];
  char *name;
  char *argv[256];

  /* build the argument vector */

  sprintf(command,"%s/%s",path,prog);
  name=strtok(command," ");
  argv[0]=basename(name);
  for (i=1;(argv[i]=strtok(NULL," ")) !=NULL;i++);
  pid=spawnv(P_NOWAITO,name,argv);
  return pid;
}
  
  



