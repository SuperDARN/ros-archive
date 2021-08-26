/* a_d_drive.c 
   ===========  */
/*
$Log: a_d_drive.c,v $
Revision 1.2  1998/09/16 19:26:28  barnes
Moved from 16 bit typing.

Revision 1.1  1998/09/08 15:47:29  barnes
Initial revision

*/  
 
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/sched.h>
#include <time.h>

#include "message.h"
#include "a_d_drive.h"
#include "dma-alloc.h"
#include "dma-addr.h"

#define TIME_OUT 100.0

int get_buf_num(pid_t task_id) {
  void *b_out[2];
  void *b_in[3];
  unsigned s_out[1];
  unsigned s_in[2];
  
  int status;									
  int drv_status; 
  char code=GET_BUF_NUM;
  int buffer;
  
  b_out[0]=&code;
  b_out[1]=NULL;
 
  s_out[0]=sizeof(code);
 
  b_in[0]=&drv_status;
  b_in[1]=&buffer;
  b_in[2]=NULL;
  
  s_in[0]=sizeof(drv_status);
  s_in[1]=sizeof(buffer);
 
  status = message_pid_array(task_id,TIME_OUT,b_out,b_in,s_out,s_in);
 
  if (status != 0) return status;
  if (drv_status !=0) return -1;
  return buffer;
}
 
int get_buf_size(pid_t task_id) {
  void *b_out[2];
  void *b_in[3];
  unsigned s_out[1];
  unsigned s_in[2];
  
  int status;									
  int drv_status; 
  char code=GET_BUF_SIZE;
  int size;
  
  b_out[0]=&code;
  b_out[1]=NULL;
 
  s_out[0]=sizeof(code);
 
  b_in[0]=&drv_status;
  b_in[1]=&size;
  b_in[2]=NULL;
  
  s_in[0]=sizeof(drv_status);
  s_in[1]=sizeof(size);
 
  status = message_pid_array(task_id,TIME_OUT,b_out,b_in,s_out,s_in);
 
  if (status != 0) return status;
  if (drv_status !=0) return -1;
  return size;
}

void *get_buf_adr(pid_t task_id,int buffer) {
  void *b_out[3];
  void *b_in[3];
  unsigned s_out[2];
  unsigned s_in[2];
  
  int status;									
  int drv_status; 
  char code=GET_BUF_ADR;
  dma_t dma_buf;
  
  b_out[0]=&code;
  b_out[1]=&buffer;
  b_out[2]=NULL;
 
  s_out[0]=sizeof(code);
  s_out[1]=sizeof(buffer);

  b_in[0]=&drv_status;
  b_in[1]=&dma_buf;
  b_in[2]=NULL;
  
  s_in[0]=sizeof(drv_status);
  s_in[1]=sizeof(dma_buf);
 
  status = message_pid_array(task_id,TIME_OUT,b_out,b_in,s_out,s_in);
      
  if (status !=0) return (void *) 0; 
  if (drv_status !=0) return (void *) 0;
  if (dmaaddr(&dma_buf,task_id) !=0) return  (void *) 0; 
  return (void *) V_ADDR(&dma_buf); 
}

int get_scan_status(pid_t task_id) {
  void *b_out[2];
  void *b_in[2];
  unsigned s_out[1];
  unsigned s_in[1];
  
  int status;									
  int scan_status; 
  char code=GET_SCAN_STATUS;
  
  b_out[0]=&code;
  b_out[1]=NULL;
 
  s_out[0]=sizeof(code);
 
  b_in[0]=&scan_status;
  b_in[1]=NULL;
  
  s_in[0]=sizeof(scan_status);
 
  status = message_pid_array(task_id,TIME_OUT,b_out,b_in,s_out,s_in);
 
  if (status != 0) return status;
  return scan_status;
}

int do_scan(pid_t task_id,int buffer,int bytes,
			int mode,int channels) {
  void *b_out[6];
  void *b_in[2];
  unsigned s_out[5];
  unsigned s_in[1];
  
  int status;									
  int drv_status; 
  char code=DO_SCAN;
  
  b_out[0]=&code;
  b_out[1]=&buffer;
  b_out[2]=&bytes;
  b_out[3]=&mode;
  b_out[4]=&channels;
  b_out[5]=NULL;
 
  s_out[0]=sizeof(code);
  s_out[1]=sizeof(buffer);
  s_out[2]=sizeof(bytes);
  s_out[3]=sizeof(mode);
  s_out[4]=sizeof(channels);

  b_in[0]=&drv_status;
  b_in[1]=NULL;
  
  s_in[0]=sizeof(drv_status);
 
  status = message_pid_array(task_id,TIME_OUT,b_out,b_in,s_out,s_in);
 
  if (status != 0) return status;
  if (drv_status !=0) return -1;
  return 0;
}

pid_t get_scan_reset(pid_t task_id) {
  void *b_out[2];
  void *b_in[3];
  unsigned s_out[1];
  unsigned s_in[2];
  
  int status;									
  int drv_status; 
  char code=GET_PROXY_PID;
  int proxy_id;
  
  b_out[0]=&code;
  b_out[1]=NULL;
 
  s_out[0]=sizeof(code);
 
  b_in[0]=&drv_status;
  b_in[1]=&proxy_id;
  b_in[2]=NULL;
  
  s_in[0]=sizeof(drv_status);
  s_in[1]=sizeof(proxy_id);
 
  status = message_pid_array(task_id,TIME_OUT,b_out,b_in,s_out,s_in);
 
  if (status != 0) return status;
  if (drv_status !=0) return -1;
  return (pid_t) proxy_id;
}

int scan_reset(pid_t task_id,pid_t proxy_id) {

  if (Trigger(proxy_id) !=task_id) return -1;
  return 0;
}