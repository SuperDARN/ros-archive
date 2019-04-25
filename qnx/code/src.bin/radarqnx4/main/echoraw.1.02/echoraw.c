/* echoraw.c
   =========
   Author: R.J.Barnes
*/

/*
 $Log: echoraw.c,v $
 Revision 1.2  2006/08/17 15:33:19  code
 Sanity check for adding beam number and frequency.

 Revision 1.1  2006/04/07 16:56:31  barnes
 Initial revision
 
*/

#include <stdio.h>
#include <sys/name.h>
#include <stdlib.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include "rtypes.h"
#include "option.h"
#include "taskid.h"
#include "ad.h"
#include "dma-alloc.h"
#include "echoraw.h"

#include "echotask.h"

#include "version.h"
#include "taskname.h"
#include "hlpstr.h"

/*
 $Log: echoraw.c,v $
 Revision 1.2  2006/08/17 15:33:19  code
 Sanity check for adding beam number and frequency.

 Revision 1.1  2006/04/07 16:56:31  barnes
 Initial revision

 */

#define ECHO_MAX_TIME 5.00
#define ECHO_SINGLE_TIME 1.0

#define ECHO_SGL_TIMEOUT 2
#define ECHO_SGL_DEAD 1
#define ECHO_SGL_NULL 0


int arg=0;
struct OptionData opt;

struct task_list *list=NULL;
timer_t timer_i_id; /* individual task time */
timer_t timer_t_id; /* total task time */ 
struct timespec total_time; /* total time allowed for echo tasks */
struct timespec single_time; /* maximum time that a single task can claim */
int echo_signal;  /* the allocated time has elapsed */

char *taskname=NULL;
char *dname=NULL;

char taskdname[32]={ECHORAW};
char dnamed[32]={DNAME};
char errbuf[256];

pid_t did=0;

float floating_time(struct timespec time) {
  /* convert to floating point seconds */
  return (float)(time.tv_sec+(time.tv_nsec/1e9));
}

void int_time(struct timespec *output,float input) {

  /* convert from floating point seconds */
  if (input >0) {
    output->tv_sec=(int)  input;
    output->tv_nsec=(int) ((input-output->tv_sec)*1e9);
  }
}

void signal_handler(int sig_number) {
  /* do nothing */
  if (sig_number==SIGUSR2) echo_signal=ECHO_SGL_TIMEOUT;
  if (sig_number==SIGUSR1) echo_signal=ECHO_SGL_DEAD;
}

void set_handler() {
  /* grab the signals SIGUSR1 and SIGUSR2 */
  signal(SIGUSR1,signal_handler);
  signal(SIGUSR2,signal_handler);
}

void clear_handler() {
  /* free the signals */
  signal(SIGUSR1,SIG_DFL);
  signal(SIGUSR2,SIG_DFL);
}

void total_timer_arm() {
  struct itimerspec timer;
  timer.it_interval.tv_sec=0L;
  timer.it_interval.tv_nsec=0L;
  timer.it_value.tv_sec=total_time.tv_sec;
  timer.it_value.tv_nsec=total_time.tv_nsec;
  echo_signal=ECHO_SGL_NULL;
  timer_settime(timer_t_id,0,&timer,NULL); /* set total timer going */
}

void single_timer_arm() {
  struct itimerspec timer;
  timer.it_interval.tv_sec=0L;
  timer.it_interval.tv_nsec=0L;
  timer.it_value.tv_sec=single_time.tv_sec;
  timer.it_value.tv_nsec=single_time.tv_nsec;
  timer_settime(timer_i_id,0,&timer,NULL); /* set timer going */
}

void timer_disarm() {
  struct itimerspec timer;
  timer.it_value.tv_sec=0L;
  timer.it_value.tv_nsec=0L;
  timer_settime(timer_i_id,0,&timer,NULL);
  timer_settime(timer_t_id,0,&timer,NULL);
} 

void signal_decode(struct task_list *ptr) {
  if (echo_signal == ECHO_SGL_NULL) {
    sprintf(errbuf,"Task %s has died.",ptr->name);
    fprintf(stderr,errbuf);
    remove_task(ptr->task_id);
  } else if (echo_signal== ECHO_SGL_DEAD) {
    sprintf(errbuf,"Task %s has timed out.",ptr->name);
    fprintf(stderr,errbuf);
    remove_task(ptr->task_id);
  } else if (echo_signal== ECHO_SGL_TIMEOUT) {
    fprintf(stderr,"Too many tasks - timed out");
  }
}

void disconnect(struct task_list *ptr) {
  sprintf(errbuf,"Task %s has disconnected.",ptr->name);
  fprintf(stderr,errbuf);
  remove_task(ptr->task_id);
}



void send_msg(char msg,int size,char *data) {

  int status;
  int tid;
  struct task_list *list_ptr;

  struct _mxfer_entry send_mx[3];
  struct _mxfer_entry reply_mx[1];

  _setmx(&reply_mx[0],&status,sizeof(int)); 
  _setmx(&send_mx[0],&msg,sizeof(char));
  _setmx(&send_mx[1],&size,sizeof(int));

  if (size !=0) _setmx(&send_mx[2],data,size);
 
  /* send message to tasks */
  if (list !=NULL) {
     total_timer_arm();
   
     for (list_ptr=list;(list_ptr != NULL) && (echo_signal==ECHO_SGL_NULL);
		  list_ptr=list_ptr->next) {

       single_timer_arm();

       if (size !=0) tid=Sendmx(list_ptr->task_id,3,1,send_mx,reply_mx);
       else tid=Sendmx(list_ptr->task_id,2,1,send_mx,reply_mx);
       if (tid==-1) signal_decode(list_ptr);
       else if (status==ECHORAW_REMOVE) disconnect(list_ptr);
    }
    timer_disarm();
  } 
}
   
void remove_task(pid_t task_id) {
  /* removes a dead task from the list */

  struct task_list **list_ptr;
  struct task_list *dummy_ptr;
  /* find the task list */

  for (list_ptr=&list;(*(list_ptr) != NULL) && 
					  ( (*list_ptr)->task_id != task_id);
                      list_ptr=&( (*list_ptr)->next));

  if (*(list_ptr) == NULL) {
     fprintf(stderr,"Could not find task to remove ! \n");
     return;
  }
  sprintf(errbuf,"Task %s, removed from list.",(*list_ptr)->name);
  fprintf(stderr,errbuf);
  dummy_ptr=*(list_ptr);
  free( (*list_ptr)->name); /* free the name space */
  *(list_ptr)=(*list_ptr)->next;
  free(dummy_ptr);
}

struct task_list *add_task(int nlen) {

  /* adds another task to the end of the linked list */
  /* reserves space for task but does not set up the header */

  struct task_list **list_ptr;
 
  /* find the end of the list */

  for (list_ptr=&list;*(list_ptr) != NULL;list_ptr=&( (*list_ptr)->next));
 
  /* claim space for task entry */ 
  if ( (*(list_ptr)=malloc(sizeof(struct task_list)) ) == NULL) return 0;
  /* claim space for task name */
  if ( ( (*list_ptr)->name=malloc(nlen*sizeof(char)) ) == NULL) {
    free(*(list_ptr));
    *(list_ptr)=NULL;
    return NULL;
  } 
 
  (*list_ptr)->next=NULL;
  return (*list_ptr);
}

main (int argc,char *argv[]) {

  int help=0;
  int echo_id;
  pid_t reply_tid;
  int nlen;
  struct sigevent event;
  struct task_list *new_task;
  int status;
  int loop=1;
  char code;
  int rnum=1;
  char rmsg; 
  float ttme,stme;

  struct _mxfer_entry msg[2];
  
 
  int size;
  char *data=NULL;

  int_time(&total_time,ECHO_MAX_TIME);
  int_time(&single_time,ECHO_SINGLE_TIME);


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"t",'f',&ttme);
  OptionAdd(&opt,"s",'f',&stme);
  OptionAdd(&opt,"n",'t',&taskname);
  OptionAdd(&opt,"d",'t',&dname);

  arg=OptionProcess(1,argc,argv,&opt,NULL);  
 
  
  if (ttme !=0)   int_time(&total_time,ttme);
  if (stme !=0)   int_time(&single_time,stme);

  if (taskname==NULL) taskname=taskdname;
  if (dname==NULL) dname=dnamed;

  if (floating_time(total_time)<floating_time(single_time)) {
     total_time.tv_sec=single_time.tv_sec;
     total_time.tv_nsec=single_time.tv_nsec;
  }


  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if ( (did=qnx_name_locate(0,dname,0,NULL))==-1) {
    fprintf(stderr,"Unable to find hardware.\n");
    exit(EXIT_FAILURE);
  }

  if ( (echo_id=qnx_name_attach(0,taskname) ) == -1) { 
    perror("Unable to attach echo task");
    fprintf(stderr,"Unable to register name");
    exit(EXIT_FAILURE);
  }

  sprintf(errbuf,"Started (version: %d.%.2d)",MAJOR_VERSION,MINOR_VERSION);
  fprintf(stderr,errbuf);

  sprintf(errbuf,"Total time allocated : %gs",
                  floating_time(total_time));
  fprintf(stderr,errbuf);
  sprintf(errbuf,"Time per task : %gs",
				  floating_time(single_time));
  fprintf(stderr,errbuf);
 
  /* zero linked list of tasks */
  
  list=NULL;
  
  /* set up the timer for the task list */
  set_handler();
  event.sigev_signo=SIGUSR1;
  timer_i_id=timer_create(CLOCK_REALTIME,&event);
  if (timer_i_id == -1) {
    fprintf(stderr,"Could not create timer");
    exit(EXIT_FAILURE);
  }

  event.sigev_signo=SIGUSR2;  
  timer_t_id=timer_create(CLOCK_REALTIME,&event);
  if (timer_t_id == -1) {
    fprintf(stderr,"Could not create timer");
    exit(EXIT_FAILURE);
  }  

  while (loop) {
    /* receive message from fit_dist */
   
   reply_tid=Receive(0,&code,sizeof(char));

   switch(code) {
     case ECHORAW_ADD :
        /* a task has started up */ 
        status=(Readmsg(reply_tid,sizeof(char),
         &nlen,sizeof(int)) != sizeof(int));
         _setmx(&msg[0],&rmsg,sizeof(rmsg));
         rnum=1;    
        if ((status==0) && ((new_task=add_task(nlen)) != NULL)) {
         /* get the task name */
         status=Readmsg(reply_tid,sizeof(char)+sizeof(int),
                        new_task->name,nlen);
         new_task->task_id=reply_tid;
         sprintf(errbuf,"Added %s.(%d)",new_task->name,reply_tid);
         fprintf(stderr,errbuf);
         rmsg=ECHORAW_OK;
         _setmx(&msg[1],&did,sizeof(ipcid_t));
         rnum=2;
       } else {
         rmsg=ECHORAW_QUIT;
         fprintf(stderr,"No room to add task");
       }
       Replymx(reply_tid,2,msg);       
       break;
     default:
       status=(Readmsg(reply_tid,sizeof(char),
       &size,sizeof(int)) != sizeof(int));
       if ((status==0) && (size>0)) status=((data=malloc(size))==NULL);
       if ((status==0) && (data !=NULL)) 
          status=(Readmsg(reply_tid,sizeof(char)+sizeof(int),
          data,size) != size);
       Reply(reply_tid,&status,sizeof(int));       
       
       send_msg(code,size,data);       
 
       free(data);
       data=NULL;
       break; 
    }
  }
  /* remove task timer */
  timer_delete(timer_i_id);
  timer_delete(timer_t_id);
  clear_handler();
} 
