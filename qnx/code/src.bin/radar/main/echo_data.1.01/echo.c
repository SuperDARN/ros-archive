 /* echo.c
   ====== */

/*
 $Log: echo.c,v $
 Revision 2.9  1999/04/10 20:41:39  barnes
 mAdded help message system.

 Revision 2.8  1999/03/30 19:24:59  barnes
 Moved task names into a header.

 Revision 2.7  1999/03/26 14:28:33  barnes
 Added support for the reset and quit messages.

 Revision 2.6  1998/06/11 15:35:35  root
 Added support for the TASK_RESET message.

 Revision 2.5  1998/06/11 15:09:49  root
 Moved usage to another file

 Revision 2.4  1998/05/29 13:32:32  root
 Fixed bug that caused the task list to become corrupted.

 Revision 2.3  1998/05/28 18:44:40  root
 Relocated echo_data.h.

 Revision 2.2  1998/05/28 17:50:48  root
 Added RCS log and revision number.

 */

#include <stdio.h>
#include <sys/name.h>
#include <stdlib.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

#include "print_info.h"
#include "log_error.h"
#include "message.h"
#include "task_msg.h"
#include "decode_msg.h"
#include "echo_util.h"
#include "echo.h"
#include "version.h"
#include "task_name.h"
#include "hlpstr.h"

#define ECHO_MAX_TIME 5.00
#define ECHO_SINGLE_TIME 1.0

#define ECHO_SGL_TIMEOUT 2
#define ECHO_SGL_DEAD 1
#define ECHO_SGL_NULL 0


struct task_list *list=NULL;
timer_t timer_i_id; /* individual task time */
timer_t timer_t_id; /* total task time */ 
struct timespec total_time; /* total time allowed for echo tasks */
struct timespec single_time; /* maximum time that a single task can claim */
int echo_signal;  /* the allocated time has elapsed */

char echo_name[32]={ECHO_DATA};
char errlog_name[32]={ERRLOG};
char errbuf[256];
struct task_id *tsk=NULL;

float floating_time(struct timespec time) {
  /* convert to floating point seconds */
  return (float)(time.tv_sec+(time.tv_nsec/1e9));
}

void int_time(struct timespec *output,float input) {

  /* convert from floating point seconds */
  if (input >0) {
    output->tv_sec=(long)  input;
    output->tv_nsec=(long) ((input-output->tv_sec)*1e9);
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
    log_error(tsk,echo_name,errbuf);
    remove_task(ptr->task_id);
  } else if (echo_signal== ECHO_SGL_DEAD) {
    sprintf(errbuf,"Task %s has timed out.",ptr->name);
    log_error(tsk,echo_name,errbuf);
    remove_task(ptr->task_id);
  } else if (echo_signal== ECHO_SGL_TIMEOUT) {
    log_error(tsk,echo_name,"Too many tasks - timed out");
  }
}

void disconnect(struct task_list *ptr) {
  sprintf(errbuf,"Task %s has disconnected.",ptr->name);
  log_error(tsk,echo_name,errbuf);
  remove_task(ptr->task_id);
}



void send_msg(char msg,int size,unsigned char *data) {

  char msg_in;
  int tid;
  struct task_list *list_ptr;

  struct _mxfer_entry send_mx[3];
  struct _mxfer_entry reply_mx[1];

  _setmx(&reply_mx[0],&msg_in,sizeof(char)); 
  _setmx(&send_mx[0],&msg,sizeof(char));

  if (size !=0) {
    _setmx(&send_mx[1],&size,sizeof(long int));
    _setmx(&send_mx[2],data,size);
  }

  /* send message to tasks */
  if (list !=NULL) {
     total_timer_arm();
   
     for (list_ptr=list;(list_ptr != NULL) && (echo_signal==ECHO_SGL_NULL);
		  list_ptr=list_ptr->next) {

       single_timer_arm();

       if (size !=0) tid=Sendmx(list_ptr->task_id,3,1,send_mx,reply_mx);
       else tid=Sendmx(list_ptr->task_id,1,1,send_mx,reply_mx);
       if (tid==-1) signal_decode(list_ptr);
       else if (msg_in==ECHO_REMOVE) disconnect(list_ptr);
    }
    timer_disarm();
  } 
}

void send_data(struct task_block *ptr) {

  struct task_block out;
  char msg=TASK_DATA;
  char msg_in;
  int snum,i,j,k;
  int tid;
  long int size;
  struct task_list *list_ptr;

  struct _mxfer_entry *send_mx;
  struct _mxfer_entry reply_mx[1];

  send_mx=malloc(sizeof(struct _mxfer_entry)*(5+ptr->num));
  size=sizeof(long int)*2+sizeof(struct data_rec)*ptr->num+ptr->tsize;

  _setmx(&reply_mx[0],&msg_in,sizeof(char)); 

  _setmx(&send_mx[0],&msg,sizeof(char));
  _setmx(&send_mx[1],&size,sizeof(long int));
  _setmx(&send_mx[2],&out.num,sizeof(long int));
  _setmx(&send_mx[3],&out.tsize,sizeof(long int));
   
  if (list !=NULL) {
     total_timer_arm();
   
     for (list_ptr=list;(list_ptr != NULL) && (echo_signal==ECHO_SGL_NULL);
		  list_ptr=list_ptr->next) {
       single_timer_arm();
       out.tsize=0;
       j=0;
       for (i=0;i<ptr->num;i++) {
         for (k=0; (k<list_ptr->plen) && 
                   (list_ptr->pass[k] !=ptr->data[i].type);k++);
         if (k==list_ptr->plen) continue;
         out.ptr[j]=ptr->ptr[i];
         out.data[j].size=ptr->data[i].size;
         out.data[j].type=ptr->data[i].type;
         out.data[j].time=ptr->data[i].time;
         out.data[j].index=j;
         out.tsize+=ptr->data[i].size;
         j++;
       }
       out.num=j;
       if (out.num==0) continue;
       size=sizeof(long int)*2+sizeof(struct data_rec)*out.num+out.tsize;
       snum=5+out.num;
       _setmx(&send_mx[4],out.data,sizeof(struct data_rec)*out.num);
       for (i=0;i<out.num;i++) _setmx(&send_mx[5+i],
                                       out.ptr[i],out.data[i].size);
       tid=Sendmx(list_ptr->task_id,snum,1,send_mx,reply_mx);
       if (tid==-1) signal_decode(list_ptr);
       else if (msg_in==ECHO_REMOVE) disconnect(list_ptr);
    }
    timer_disarm();
  } 
  free(send_mx);
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
     log_error(tsk,echo_name,"Could not find task to remove ! \n");
     return;
  }
  sprintf(errbuf,"Task %s, removed from list.",(*list_ptr)->name);
  log_error(tsk,echo_name,errbuf);
  dummy_ptr=*(list_ptr);
  free( (*list_ptr)->name); /* free the name space */
  free( (*list_ptr)->pass);
  *(list_ptr)=(*list_ptr)->next;
  free(dummy_ptr);
}

struct task_list *add_task(int name_len,int plen) {

  /* adds another task to the end of the linked list */
  /* reserves space for task but does not set up the header */

  struct task_list **list_ptr;
 
  /* find the end of the list */

  for (list_ptr=&list;*(list_ptr) != NULL;list_ptr=&( (*list_ptr)->next));
 
  /* claim space for task entry */ 
  if ( (*(list_ptr)=malloc(sizeof(struct task_list)) ) == NULL) return 0;
  /* claim space for task name */
  if ( ( (*list_ptr)->name=malloc(name_len*sizeof(char)) ) == NULL) {
    free(*(list_ptr));
    *(list_ptr)=NULL;
    return NULL;
  } 
 
  if ( (plen !=0) && 
       ( ( (*list_ptr)->pass=malloc(plen*sizeof(char)) ) == NULL)) {
    free((*list_ptr)->name);
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
  pid_t task_id;
  pid_t reply_tid;
  long int nlen;
  long int plen;
  struct sigevent event;
  struct task_list *new_task;
  int status;
  int loop;
  char msg;
  char rmsg;
  unsigned char *data=NULL;
  struct task_block data_blk;
  int size; 
 
  int_time(&total_time,ECHO_MAX_TIME);
  int_time(&single_time,ECHO_SINGLE_TIME);

  if (argc>1) {
    int c;
    for (c=1;c<argc;c++) {
      if (strcmp(argv[c],"--help")==0) help=1;
      else if (strcmp(argv[c],"-t")==0) {
        int_time(&total_time,atof(argv[c+1]));
        c++;
      } else if (strcmp(argv[c],"-s")==0) {
        float time_val;
        int_time(&single_time,atof(argv[c+1]));
        c++;
      } else if (strcmp(argv[c],"-n")==0) {
        strcpy(echo_name,argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-e")==0) {
        strcpy(errlog_name,argv[c+1]);
        c++;
      }
    }
  }
  if (floating_time(total_time)<floating_time(single_time)) {
     total_time.tv_sec=single_time.tv_sec;
     total_time.tv_nsec=single_time.tv_nsec;
  }


  if (help==1) {
    print_info(stdout,hlpstr);
    exit(0);
  }


  tsk=make_task_id(errlog_name);
  if (tsk==NULL) {
    perror("Unable to establish error handler.");
    exit(EXIT_FAILURE);
  }

  if ( (echo_id=qnx_name_attach(0,echo_name) ) == -1) { 
    perror("Unable to attach echo task");
    log_error(tsk,echo_name,"Unable to register name");
    exit(EXIT_FAILURE);
  }

  sprintf(errbuf,"Started (version: %s)",VSTRING);
  log_error(tsk,echo_name,errbuf);

  sprintf(errbuf,"Total time allocated : %gs",
                  floating_time(total_time));
  log_error(tsk,echo_name,errbuf);
  sprintf(errbuf,"Time per task : %gs",
				  floating_time(single_time));
  log_error(tsk,echo_name,errbuf);
 
  /* zero linked list of tasks */
  
  list=NULL;
  
  /* set up the timer for the task list */
  set_handler();
  event.sigev_signo=SIGUSR1;
  timer_i_id=timer_create(CLOCK_REALTIME,&event);
  if (timer_i_id == -1) {
    log_error(tsk,echo_name,"Could not create timer");
    exit(EXIT_FAILURE);
  }

  event.sigev_signo=SIGUSR2;  
  timer_t_id=timer_create(CLOCK_REALTIME,&event);
  if (timer_t_id == -1) {
    log_error(tsk,echo_name,"Could not create timer");
    exit(EXIT_FAILURE);
  }  

  while (1) {
    /* receive message from fit_dist */
   
   reply_tid=Receive(0,&msg,sizeof(char));

   switch(msg) {
     case ECHO_ADD :
        /* a task has started up */ 
        status=(Readmsg(reply_tid,sizeof(char),
         &nlen,sizeof(int)) != sizeof(int));
        if (status==0) status=(Readmsg(reply_tid,sizeof(char)+
                       sizeof(int),
                       &plen,sizeof(int)) != sizeof(int));

        if ((status==0) && ((new_task=add_task(nlen,plen)) != NULL)) {
         /* get the task name */
         status=Readmsg(reply_tid,sizeof(char)+2*sizeof(long int),
                        new_task->name,nlen);
         status=Readmsg(reply_tid,sizeof(char)+2*sizeof(long int)+nlen,
                        new_task->pass,plen);
         new_task->plen=plen;
         new_task->task_id=reply_tid;
         sprintf(errbuf,"Added %s.(%d)",new_task->name,reply_tid);
         log_error(tsk,echo_name,errbuf);
         rmsg=ECHO_OK;
       } else {
         rmsg=ECHO_QUIT;
         log_error(tsk,echo_name,"No room to add task");
       }
       Reply(reply_tid,&rmsg,sizeof(char));       
       break;
     case TASK_DATA:
       rmsg=decode_data(reply_tid,&data_blk,&data);
       Reply(reply_tid,&rmsg,sizeof(char));       
       send_data(&data_blk);  
       if (data !=NULL) free(data);
       data=NULL;
       break;
     case TASK_OPEN:
     case TASK_CLOSE:
       status=(Readmsg(reply_tid,sizeof(char),
       &size,sizeof(long int)) != sizeof(long int));
       if ((status==0) && (size>0)) status=((data=malloc(size))==NULL);
       if ((status==0) && (data !=NULL)) 
          status=(Readmsg(reply_tid,sizeof(char)+sizeof(long int),
          data,size) != size);
       if (status==0) rmsg=TASK_OK;
       else rmsg=TASK_ERR;
       Reply(reply_tid,&rmsg,sizeof(char));       
       
       /* transmit the data to the other tasks */
       send_msg(msg,size,data);       
 
       free(data);
       data=NULL; 
       break;
     case TASK_RESET :
       send_msg(msg,0,NULL);
       rmsg=TASK_OK;
       Reply(reply_tid,&rmsg,sizeof(char));
       break;
     case TASK_QUIT:
       send_msg(msg,0,NULL);
       rmsg=TASK_OK;
       Reply(reply_tid,&rmsg,sizeof(char));
       exit(0);
       break;
     default :
       /* reply */
       rmsg= UNKNOWN_TYPE;
       Reply(reply_tid,&rmsg,sizeof(char));
       break; 
    }
  }
  /* remove task timer */
  timer_delete(timer_i_id);
  timer_delete(timer_t_id);
  clear_handler();
} 