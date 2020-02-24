/* message.h
   =========
   Author: R.J.Barnes
*/

#ifndef _MESSAGE
#ifndef __TYPES_H_INCLUDED
  #include <sys/types.h>
#endif

/* message errors */
     	 
#define MSGERR_NO_TASK 			0x0
#define MSGERR_SIGNAL_FAIL 		0x1
#define MSGERR_TIMER_FAIL 		0x2
#define MSGERR_MSG_FAIL 		0x3
#define MSGERR_NO_MEM           0x4
#define MSGERR_TME_OUT          0x5

struct task_id {
  char name[256];
  pid_t id;
};


/* Message passing primitive */

int message(char *,double,void *,void *,unsigned,unsigned);
int message_array(char *,double,void **,void **,unsigned *,unsigned *);
pid_t locate_task_id(char *);

int register_task(char *);
int release_task(int);

int message_pid(pid_t,double,void *,void *,unsigned,unsigned);
int message_pid_array(pid_t,double,void **,void **,unsigned *,unsigned *);
struct task_id *make_task_id(char *name);
int message_tsk(struct task_id *,double,void *,void *,unsigned,unsigned);
int message_tsk_array(struct task_id *,
					  double,void **,void **,unsigned *,unsigned *);


#endif
