/* task_write.h
   ============
   Author: R.J.Barnes
*/


#ifndef _TASK_WRITE
#define _TASK_WRITE

int task_open(struct task_id *tsk,int buflen,unsigned char *buf,
              int yr,int mn,int dy,
              int hr,int mt,int sc);

int task_close(struct task_id *tsk,
			   int yr,int mn,int dy,
			   int hr,int mt,int sc);

int task_quit(struct task_id *tsk);
int task_send(struct task_id *tsk,struct task_block *ptr);
int task_add(struct task_block *ptr, 
             int buflen,unsigned char *buf,char type,
             int yr,int mn,int dy,
             int hr,int mt,int sc);
int task_reset(struct task_id *tsk);
#endif
