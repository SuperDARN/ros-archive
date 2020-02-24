/* a_d_drive.c
   ===========
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
$Log: a_d_drive.c,v $
Revision 1.4  2001/06/27 20:53:03  barnes
Added license tag

Revision 1.3  2001/01/29 18:11:49  barnes
Added Author Name

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