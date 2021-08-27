 /* message.c
   ========= */

/*
$Log: message.c,v $
Revision 1.1  1998/06/19 10:44:53  root
Initial revision

*/

/* Message passing primitive */

#include <time.h>
#include <signal.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "message.h"

int msgerr;

void message_signal_handler(int sig_number) { /* null handler for time out */
}

pid_t locate_task_id(char *task_name) {
 
  pid_t task_id;
  
  if ( (task_id=qnx_name_locate(0,task_name,0,NULL)) == -1) {
    msgerr=MSGERR_NO_TASK;
    return (pid_t) -1;
  }
  return task_id;
} 


int message_pid(pid_t task_id,double time,void *smsg,void *rmsg,
			unsigned snbytes,unsigned rnbytes) {

  /* General purpose message passing routine incorporating time-out.
   *
   * This version uses a task id to save time 
   *
   */ 
  
  int tid;
  timer_t timer_id=-1;
  struct itimerspec timer;
  struct sigevent event;

  void (*prev_handler)(int);

  struct _mxfer_entry send_mx;
  struct _mxfer_entry reply_mx; 

  _setmx(&reply_mx,rmsg,rnbytes);
  _setmx(&send_mx,smsg,snbytes);
 
  if (time >0) { /* build the time out */

    prev_handler=signal(SIGUSR1,message_signal_handler);
  
    if (prev_handler==SIG_ERR) {
      msgerr=MSGERR_SIGNAL_FAIL;
	  return -1;
    } 

    timer.it_interval.tv_sec=0L;
    timer.it_interval.tv_nsec=0L;
    timer.it_value.tv_sec=(long int) time;
    timer.it_value.tv_nsec=(long int)  ((time - (long int) time)*1e9);
    event.sigev_signo=SIGUSR1;

    if ( ((timer_id=timer_create(CLOCK_REALTIME,&event))==-1) ||
          (timer_settime(timer_id,0,&timer,NULL)==-1)) {
      signal(SIGUSR1,prev_handler);  
      msgerr=MSGERR_TIMER_FAIL;
      return -1;
    }     
  }
   
  tid=Sendmx(task_id,1,1,&send_mx,&reply_mx);
 
  if (timer_id !=-1) {
    timer_delete(timer_id);
    signal(SIGUSR1,prev_handler);
  } 

  if (tid==-1) { /* error occured */
    if (errno==EINTR) msgerr=MSGERR_TME_OUT;
	  else msgerr=MSGERR_MSG_FAIL;
	return -1;
  }  
  return 0;
}
 
int message_pid_array(pid_t task_id,double time,void **smsg,void **rmsg,
				  unsigned *snbytes,unsigned *rnbytes) {

  /* sends an array of messages to a task */
  
  unsigned snum,rnum,c;
  int tid;
  timer_t timer_id=-1;
  struct itimerspec timer;
  struct sigevent event;

  void (*prev_handler)(int);

  struct _mxfer_entry *send_mx;
  struct _mxfer_entry *reply_mx; 
    
  for (snum=0;smsg[snum] !=NULL;snum++);
  for (rnum=0;rmsg[rnum] !=NULL;rnum++);
  
  /* build mx stores */
  
  if ( (send_mx= (struct _mxfer_entry *)
	             malloc(sizeof(struct _mxfer_entry)*snum)) == NULL) {
     msgerr=MSGERR_NO_MEM;
     return -1; 
  }
  if ( (reply_mx= (struct _mxfer_entry *)
	             malloc(sizeof(struct _mxfer_entry)*rnum)) == NULL) {
    free(send_mx);
    msgerr=MSGERR_NO_MEM;
	return -1;
  } 
 
  for (c=0;c<snum;c++) _setmx(&send_mx[c],smsg[c],snbytes[c]);
  for (c=0;c<rnum;c++) _setmx(&reply_mx[c],rmsg[c],rnbytes[c]);
     
  if (time >0) { /* build the time out */

    prev_handler=signal(SIGUSR1,message_signal_handler);
  
    if (prev_handler==SIG_ERR) {
      free(send_mx);
   	  free(reply_mx);
      msgerr=MSGERR_SIGNAL_FAIL;
	  return -1;
    } 

    timer.it_interval.tv_sec=0L;
    timer.it_interval.tv_nsec=0L;
    timer.it_value.tv_sec=(long int) time;
    timer.it_value.tv_nsec=(long int)  ((time - (long int) time)*1e9);
    event.sigev_signo=SIGUSR1;

    if ( ((timer_id=timer_create(CLOCK_REALTIME,&event))==-1) ||
          (timer_settime(timer_id,0,&timer,NULL)==-1)) {
      signal(SIGUSR1,prev_handler);  
      free(send_mx);
   	  free(reply_mx);
      msgerr=MSGERR_TIMER_FAIL;
      return -1;
    }     
  }
 
  tid=Sendmx(task_id,snum,rnum,send_mx,reply_mx);

  if (timer_id !=-1) {
    free(send_mx);
   	free(reply_mx);
    timer_delete(timer_id);
    signal(SIGUSR1,prev_handler);
  } 

  free(send_mx);
  free(reply_mx);

  if (tid==-1) { /* error occured */
    if (errno==EINTR) msgerr=MSGERR_TME_OUT;
      else msgerr=MSGERR_MSG_FAIL;
    return -1;
  }   
  return 0;
}


int message_array(char *task_name,double time,void **smsg,void **rmsg,
				  unsigned *snbytes,unsigned *rnbytes) {

  pid_t task_id;
    
  if ( (task_id=qnx_name_locate(0,task_name,0,NULL)) == -1) {
    msgerr=MSGERR_NO_TASK;
    return -1;
  }
  return message_pid_array(task_id,time,smsg,rmsg,snbytes,rnbytes);  
}

int message(char *task_name,double time,void *smsg,void *rmsg,
			unsigned snbytes,unsigned rnbytes) {
  
  pid_t task_id;
  if ( (task_id=qnx_name_locate(0,task_name,0,NULL)) == -1) {
    msgerr=MSGERR_NO_TASK;
    return -1;
  }
  return message_pid(task_id,time,smsg,rmsg,snbytes,rnbytes); 
}

struct task_id *make_task_id(char *name) {
  struct task_id *ptr=NULL;
  if (name[0]==0) return NULL;
  ptr=malloc(sizeof(struct task_id));
  if (ptr==NULL) return NULL;
  strcpy(ptr->name,name);
  ptr->id=locate_task_id(ptr->name);
  return ptr;
}

int message_tsk(struct task_id *ptr,double time,void *smsg,void *rmsg,
			unsigned snbytes,unsigned rnbytes) {
  int status;
  if (ptr->id==-1) {
    if ( (ptr->id=qnx_name_locate(0,ptr->name,0,NULL)) == -1) {
      msgerr=MSGERR_NO_TASK;
      return -1;
    }
  }
  status=message_pid(ptr->id,time,smsg,rmsg,snbytes,rnbytes);
  if (status !=0) ptr->id=-1;
  return status;
}

int message_tsk_array(struct task_id *ptr,
					  double time,void **smsg,void **rmsg,
				      unsigned *snbytes,unsigned *rnbytes) {
  int status;
  if (ptr->id==-1) {
    if ( (ptr->id=qnx_name_locate(0,ptr->name,0,NULL)) == -1) {
      msgerr=MSGERR_NO_TASK;
      return -1;
    }
  }
  status=message_pid_array(ptr->id,time,smsg,rmsg,snbytes,rnbytes);  
  if (status !=0) ptr->id=-1;
  return status;
}




 