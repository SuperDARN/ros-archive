/* task_write.c 
   ============ */

/*
 $Log: task_write.c,v $
 Revision 1.2  1998/10/22 13:08:40  barnes
 Changed to integer types.

 Revision 1.1  1998/06/19 10:48:42  root
 Initial revision

*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/timers.h>
#include <time.h>
#include "cnv_time.h"
#include "message.h"
#include "task_msg.h"

#define TIME_OUT 10.0

int task_open(struct task_id *tsk,int buflen,unsigned char *buf,
              int yr,int mn,int dy,
              int hr,int mt,int sc) { /* open file */
  int status;
  int size;
  double etime;
  char msg;
  void *b_out[6];
  void *b_in[2];
  
  unsigned s_out[5];
  unsigned s_in;

  msg=TASK_OPEN;
  etime=time_epoch(yr,mn,dy,hr,mt,(double) sc);  
  size=sizeof(double)+sizeof(int)+buflen;

  b_out[0]=&msg;
  b_out[1]=&size;
  b_out[2]=&etime;
  b_out[3]=&buflen;
  b_out[4]=buf;
  b_out[5]=NULL;
  
  b_in[0]=&msg;
  b_in[1]=NULL;
  
  s_out[0]=sizeof(char);
  s_out[1]=sizeof(int);
  s_out[2]=sizeof(double);
  s_out[3]=sizeof(int);
  s_out[4]=buflen;

  s_in=sizeof(char);
 
  status = message_tsk_array(tsk,TIME_OUT,b_out,b_in,s_out,&s_in);

  if (status !=0) return status;
  if (msg != TASK_OK) return -1;
  return 0;
}    

int task_close(struct task_id *tsk,
			   int yr,int mn,int dy,
			   int hr,int mt,int sc) {  /* close file */

  int status;
  int size;
  double etime;
  char msg;
  void *b_out[4];
  void *b_in[2];
  
  unsigned s_out[3];
  unsigned s_in;

  msg=TASK_CLOSE;
  etime=time_epoch(yr,mn,dy,hr,mt,(double) sc);  
  size=sizeof(double);

  b_out[0]=&msg;
  b_out[1]=&size;
  b_out[2]=&etime;
  b_out[3]=NULL;
  
  b_in[0]=&msg;
  b_in[1]=NULL;
  
  s_out[0]=sizeof(char);
  s_out[1]=sizeof(int);
  s_out[2]=sizeof(double);

  s_in=sizeof(char);

  status = message_tsk_array(tsk,TIME_OUT,b_out,b_in,s_out,&s_in);

  if (status !=0) return status;
  if (msg != TASK_OK) return -1;
  return 0;
}

int task_quit(struct task_id *tsk) { /*terminate program */
  int status;
  int size=0;
  char msg;
  void *b_out[3];
  void *b_in[2];
  
  unsigned s_out[2];
  unsigned s_in;

  msg=TASK_QUIT;

  size=sizeof(double);

  b_out[0]=&msg;
  b_out[1]=&size;
  b_out[2]=NULL;
  
  b_in[0]=&msg;
  b_in[1]=NULL;
  
  s_out[0]=sizeof(char);
  s_out[1]=sizeof(int);

  s_in=sizeof(char);

  status = message_tsk_array(tsk,TIME_OUT,b_out,b_in,s_out,&s_in);
  if (status !=0) return status;
  if (msg != TASK_OK) return -1;
  return 0;
}

int task_reset(struct task_id *tsk) { /*terminate program */
  int status;
  int size=0;
  char msg;
  void *b_out[3];
  void *b_in[2];
  
  unsigned s_out[2];
  unsigned s_in;

  msg=TASK_RESET;

  size=sizeof(double);

  b_out[0]=&msg;
  b_out[1]=&size;
  b_out[2]=NULL;
  
  b_in[0]=&msg;
  b_in[1]=NULL;
  
  s_out[0]=sizeof(char);
  s_out[1]=sizeof(int);

  s_in=sizeof(char);

  status = message_tsk_array(tsk,TIME_OUT,b_out,b_in,s_out,&s_in);
  if (status !=0) return status;
  if (msg != TASK_OK) return -1;
  return 0;
}




int task_send(struct task_id *tsk,struct task_block *ptr) { /* send  */

  int size=0;
  int status,i;
  char msg;
  void *b_in[2];
  void **b_out;
  unsigned int *s_out;
  unsigned int s_in;

  if (ptr->num==0) return -1;

  b_out=malloc( (ptr->num+6)*sizeof(void *));
  s_out=malloc( (ptr->num+5)*sizeof(unsigned int));

  msg=TASK_DATA;
  size=sizeof(int)*2+sizeof(struct data_rec)*ptr->num+ptr->tsize;

  b_out[0]=&msg;
  b_out[1]=&size;
  b_out[2]=&ptr->num; /* number of records */
  b_out[3]=&ptr->tsize;
  b_out[4]=ptr->data;
  
  b_in[0]=&msg;
  b_in[1]=NULL;
  
  s_out[0]=sizeof(char);
  s_out[1]=sizeof(int);
  s_out[2]=sizeof(int);
  s_out[3]=sizeof(int);
  s_out[4]=sizeof(struct data_rec)*ptr->num;
 
  s_in=sizeof(char);
 
 for (i=0;i<ptr->num;i++) {
    b_out[i+5]=ptr->ptr[i];
    s_out[i+5]=ptr->data[i].size;
  } 
  b_out[ptr->num+5]=NULL;
 
  status = message_tsk_array(tsk,TIME_OUT,b_out,b_in,s_out,&s_in);
  free(b_out);
  free(s_out);

  if (status !=0) return status;
  if (msg != TASK_OK) return -1;
  return 0;
}    

int task_add(struct task_block *ptr, 
             int buflen,unsigned char *buf,char type,
             int yr,int mn,int dy,
             int hr,int mt,int sc) {
   double etime;
   if (ptr->num>=MAX_BUF) return -1;
   etime=time_epoch(yr,mn,dy,hr,mt,(double) sc);  
   ptr->data[ptr->num].type=type;
   ptr->data[ptr->num].size=buflen;
   ptr->data[ptr->num].time=etime;
   ptr->data[ptr->num].index=ptr->num;
   ptr->ptr[ptr->num]=buf;
   ptr->tsize+=buflen;
   ptr->num++;
   return 0;
}




