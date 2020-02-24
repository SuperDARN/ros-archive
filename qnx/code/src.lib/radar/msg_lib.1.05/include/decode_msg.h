/* decode_msg.h
   ============
   Author: R.J.Barnes
*/

#ifndef _DECODE_MSG
#define _DECODE_MSG
int open_poll(struct task_decode *ptr,int i);
int close_poll(struct task_decode *ptr,int i);
char decode_data(pid_t tid,struct task_block *blk,
			     unsigned char **store);
char decode_msg(pid_t tid,char msg,struct task_decode *ptr);
#endif 
