/* execute.c
   ========= */

/*
 $Log: execute.c,v $
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
  pid_t pid;

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
  char command[512];
  char *name;
  char *argv[256];

  /* build the argument vector */

  sprintf(command,"%s/%s",path,prog);
  name=strtok(command," ");
  argv[0]=prog;
  for (i=1;(argv[i]=strtok(NULL," ")) !=NULL;i++);
  pid=spawnv(P_NOWAITO,name,argv);
  return pid;
}
  
  



