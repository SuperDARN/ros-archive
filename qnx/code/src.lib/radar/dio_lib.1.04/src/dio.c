/* dio.c
   =====
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
$Log: dio.c,v $
Revision 1.14  2001/06/27 20:53:09  barnes
Added license tag

Revision 1.13  2001/01/29 18:11:49  barnes
Added Author Name

Revision 1.12  2000/03/17 00:54:04  barnes
Modified the clear frequency search routine to work in stereo mode.

Revision 1.11  1999/04/14 15:04:40  barnes
Added routine for getting the status of the last timing sequence.

Revision 1.10  1999/04/02 16:19:22  barnes
Added routine for reading the frequency mode.

Revision 1.9  1999/04/01 22:31:55  barnes
Fixed problems with misaligned data in send_tsg.

Revision 1.8  1998/11/08 22:12:24  barnes
Changed calling sequence of send_tsg to be more consistent.

Revision 1.7  1998/11/08 17:22:49  barnes
Modified the calling sequence of send_tsg to support stereo cutlass.

Revision 1.6  1998/11/05 22:54:48  barnes
Added the channel switching for stereo cutlass.

Revision 1.5  1998/10/28 22:09:40  barnes
Changed the message specification so that it refers to DIO.

Revision 1.4  1998/10/23 21:41:08  barnes
Fixed bug in getting the status arrays from the radar.

Revision 1.3  1998/10/22 13:04:15  barnes
Changed all message types to use integer types.

Revision 1.2  1998/10/21 20:44:59  barnes
Added extra code for extended pulse sequences.

*/  

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "message.h"
#include "dio.h"
#define TIME_OUT 100.0   

int set_gain(pid_t task_id,unsigned char atten) {
  int status;
  struct {
    char op_type;
	char atten;
  } msg;
  
  msg.op_type=SET_GAIN;
  msg.atten=atten;
  status=message_pid(task_id,TIME_OUT,&msg,&msg.op_type,
  				 sizeof(msg),sizeof(msg.op_type) );
  if (status !=0) return status;
  else if (msg.op_type != DIO_OK) return -1;
  return 0;
}

int set_filter(pid_t task_id,unsigned char filter) {
  int status;
  struct {
    char op_type;
	char filter;
  } msg;
  
  msg.op_type=SEND_FILTER;
  msg.filter=filter;
  status=message_pid(task_id,TIME_OUT,&msg,&msg.op_type,
  				 sizeof(msg),sizeof(msg.op_type) );
  if (status !=0) return status;
  else if (msg.op_type != DIO_OK) return -1;
  return 0;
}


int set_channel(pid_t task_id,unsigned char channel) {
  int status;
  struct {
    char op_type;
	char channel;
  } msg;
  
  msg.op_type=SET_CHANNEL;
  msg.channel=channel;
  status=message_pid(task_id,TIME_OUT,&msg,&msg.op_type,
  				 sizeof(msg),sizeof(msg.op_type) );
  if (status !=0) return status;
  else if (msg.op_type != DIO_OK) return -1;
  return 0;
}

int set_beam(pid_t task_id,unsigned char beam) {
  int status;
  struct {
    char op_type;
	char bmnum;
  } msg;
  
  msg.op_type=SET_BEAM;
  msg.bmnum=beam;
  status=message_pid(task_id,TIME_OUT,&msg,&msg.op_type,
  				 sizeof(msg),sizeof(msg.op_type) );
  if (status !=0) return status;
  else if (msg.op_type != DIO_OK) return -1;
  return 0;
}

int set_freq(pid_t task_id,int freq) {
  int status;
  struct {
    char op_type;
    int freq;
  } msg;
  
  msg.op_type=SET_FREQ;
  msg.freq=freq;
  status=message_pid(task_id,TIME_OUT,&msg,&msg.op_type,
  				 sizeof(msg),sizeof(msg.op_type) );
  if (status !=0) return status;
  else if (msg.op_type != DIO_OK) {
    if (msg.op_type == FREQ_LOCAL) return FREQ_LOCAL;  
    return -1;
  }
  return 0;
} 

int read_fmode(pid_t task_id) {
  int status;
  struct {
    char op_type;
    int mode;
  } msg;
  
  msg.op_type=GET_FREQ_MODE;
  status=message_pid(task_id,TIME_OUT,&msg.op_type,&msg,
  				 sizeof(msg.op_type),sizeof(msg) );
  if (status !=0) return status;
  else if (msg.op_type != DIO_OK) return -1;
  if (msg.mode == FREQ_LOCAL) return FREQ_LOCAL;  
  return 0;
}


int read_tstatus(pid_t task_id) {
  int status;
  struct {
    char op_type;
    char status;
  } msg;
  
  msg.op_type=GET_TSTATUS;
  status=message_pid(task_id,TIME_OUT,&msg.op_type,&msg,
  				 sizeof(msg.op_type),sizeof(msg) );
  if (status !=0) return status;
  else if (msg.op_type != DIO_OK) return -1;
  return (msg.status != 0);
}




int reset_dio(pid_t task_id) {
 int status;
 char msg;
  
  msg=RESET_XT;
    
  status=message_pid(task_id,TIME_OUT,&msg,&msg,
  				 sizeof(msg),sizeof(msg) );
  if (status !=0) return status;
  else if (msg != DIO_OK) return -1;
  return 0;
}

int set_tsg(pid_t task_id,unsigned char id,int length,char *code_byte,
			   char *rep_byte, char *xtd_byte) {
  int status=0;
 
  struct {
    char op_type;
    char seq;
    char xtflg;
    int length;
  }msg;
 
  void *b_out[5];
  void *b_in[2];
  unsigned s_out[4];
  unsigned s_in[1];

  msg.op_type=SET_TSG;
  msg.seq=id;
  msg.length=length; 
  if (xtd_byte !=NULL) msg.xtflg=1;
  
  b_out[0]=&msg;
  b_out[1]=code_byte;
  b_out[2]=rep_byte;
  b_out[3]=xtd_byte;
  b_out[4]=NULL;
 
  s_out[0]=sizeof(msg);
  s_out[1]=length;
  s_out[2]=length;
  if (xtd_byte !=NULL) s_out[3]=length;
 
  b_in[0]=&msg.op_type;
  b_in[1]=NULL;
  
  s_in[0]=sizeof(msg.op_type);

  status = message_pid_array(task_id,TIME_OUT,b_out,b_in,s_out,s_in);
    
  if (status !=0) return status;
  else if (msg.op_type != DIO_OK) return -1;
  return 0;
}

int send_tsg(pid_t task_id,int stereo,unsigned char *id,
			 unsigned char *xtd_flag,unsigned char *st_flag,
			 int *delay,int **freq) {
 int status=0;
 
  struct {
   char op_type;
   char stereo;
   char seq[2];
   int delay[2];
   char xtd_flag[2];
   char st_flag[2];
   int f1,f2,f3,f4;
  } msg;
  
  msg.op_type=SEND_TSG;
  if (stereo !=0) msg.stereo=1;
  else msg.stereo=0;
  msg.seq[0]=id[0];
  if (stereo !=0) msg.seq[1]=id[1];
  else msg.seq[1]=0;
 
  if (delay !=NULL) {
    msg.delay[0]=delay[0];
    if (stereo !=0) msg.delay[1]=delay[1];
    else msg.delay[1]=0;
  } else {
    msg.delay[0]=0;
    msg.delay[1]=0;
  }
  msg.xtd_flag[0]=xtd_flag[0];
  if (stereo !=0) msg.xtd_flag[1]=xtd_flag[1];  
  else msg.xtd_flag[1]=0;

  if (freq !=NULL) {
    msg.xtd_flag[0]|=(freq[0] !=NULL)*2;
    if (stereo !=0) msg.xtd_flag[1]|=(freq[1] !=NULL)*2;
    else msg.xtd_flag[1]=NULL;
  }

  msg.st_flag[0]=st_flag[0]; 
  if ((freq !=NULL) && (freq[0] !=NULL)) msg.f1=(freq[0])[0];
  else msg.f1=0;
  if ((freq !=NULL) && (freq[0] !=NULL)) msg.f2=(freq[0])[1];
  else msg.f2=0;
  msg.st_flag[1]=st_flag[1];
  if (stereo !=0) { 
    if ((freq !=NULL) && (freq[1] !=NULL)) msg.f3=(freq[1])[0];
    else msg.f3=0;
    if ((freq !=NULL) && (freq[1] !=NULL)) msg.f4=(freq[1])[1];
    else msg.f4=0;
  } else {
    msg.f3=0;
    msg.f4=0;
  }
  status=message_pid(task_id,TIME_OUT,&msg,&msg.op_type,
    			 sizeof(msg),sizeof(msg.op_type) );		 				   
  if (status !=0) return status;
  else if (msg.op_type != DIO_OK) return -1;
  return 0;
}

int verify_id(pid_t task_id,unsigned char id) {

  int status; 

  struct {
    char op_type;
    char seq;
  } send_msg;
  
  struct {
     char op_type;
     int length;	 
  } reply_msg;
 
  send_msg.op_type=VALID_ID;
  send_msg.seq=id; 
   
  status=message_pid(task_id,TIME_OUT,&send_msg,&reply_msg,
    			 sizeof(send_msg),sizeof(reply_msg));		 				   
  if (status !=0) return status;
  else if (reply_msg.op_type != DIO_OK) return -1;
  return (reply_msg.length !=0);
}

int send_fclr(pid_t task_id,int stereo,unsigned char *id,
              int *delay,int *freq_num,int **freq_table) {

  int status=0;
  int i;
 
  struct {
    char op_type;
    char stereo;
    char seq[2];
    int delay[2];
    int freq_num[2];
  } msg;
  
  void *b_out[4];
  void *b_in[2];
  unsigned s_out[3];
  unsigned s_in[1];


  msg.op_type=SEND_FCLR;
  if (stereo !=0) msg.stereo=1;
  else msg.stereo=0;
  msg.seq[0]=id[0];
  if (stereo !=0) msg.seq[1]=id[1];
  else msg.seq[1]=0;
 
  if (delay !=NULL) {
    msg.delay[0]=delay[0];
    if (stereo !=0) msg.delay[1]=delay[1];
    else msg.delay[1]=0;
  } else {
    msg.delay[0]=0;
    msg.delay[1]=0;
  }

  msg.freq_num[0]=freq_num[0];
  if (stereo !=0) msg.freq_num[1]=freq_num[1];
 
  
  b_out[0]=&msg;
  b_out[1]=freq_table[0];
  if (stereo !=0) b_out[2]=freq_table[1];
  else b_out[2]=NULL;
  b_out[3]=NULL;
 
  s_out[0]=sizeof(msg);
  s_out[1]=freq_num[0]*sizeof(int);
  if (stereo !=0) s_out[2]=freq_num[1]*sizeof(int); 

  b_in[0]=&msg.op_type;
  b_in[1]=NULL;
  
  s_in[0]=sizeof(msg.op_type);
 
  status = message_pid_array(task_id,TIME_OUT,b_out,b_in,s_out,s_in);
    
  if (status !=0) return status;
  else if (msg.op_type != DIO_OK) {
    if (msg.op_type == FREQ_LOCAL) return FREQ_LOCAL;  
    return -1;
  }
  return 0;
}

int set_antenna(pid_t task_id,unsigned char mode,unsigned char bmnum) {
  int status;
  struct {
    char op_type;
	char mode;
    char bmnum;
  } msg;
  
  msg.op_type=SET_ANTENNA;
  msg.mode=mode;
  msg.bmnum=bmnum;
  status=message_pid(task_id,TIME_OUT,&msg,&msg.op_type,
  				 sizeof(msg),sizeof(msg.op_type) );
  if (status !=0) return status;
  else if (msg.op_type != DIO_OK) return -1;
  
  return 0;
}

int set_test_mode(pid_t task_id,unsigned char mode) {
  int status;
  struct {
    char op_type;
	char mode;
  } msg;
  
  msg.op_type=TEST_MODE;
  msg.mode=mode;
  status=message_pid(task_id,TIME_OUT,&msg,&msg.op_type,
  				 sizeof(msg),sizeof(msg.op_type) );
  if (status !=0) return status;
  else if (msg.op_type != DIO_OK) return -1;
  return 0;
}



int get_dio_status(pid_t task_id,int clear,int full,int *lstat,
				   int *astat) {

  int status;
  int i;
 
  struct {
    char op_type;
    int status[32];	
  } msg;

  if ((clear==1) && (full==0)) msg.op_type=GET_STATUS_NOFULL_NOCLR;
  else if ((clear==0) && (full==0)) msg.op_type=GET_STATUS_NOFULL_CLR;
  else if ((clear==1) && (full==1)) msg.op_type=GET_STATUS_FULL_NOCLR;
  else msg.op_type=GET_STATUS_FULL_CLR;

  if (full==1) status=message_pid(task_id,TIME_OUT,&msg.op_type,&msg,
  				 sizeof(msg.op_type),sizeof(msg) );
  else status=message_pid(task_id,TIME_OUT,&msg.op_type,&msg,
  				 sizeof(msg.op_type),sizeof(msg.op_type)+2*sizeof(int));
 
  if (status !=0) return status;
  else if (msg.op_type != DIO_OK) return -1;
  if (full==1) {
    for (i=0;i<16;i++) {
      astat[i]=msg.status[i];
      lstat[i]=msg.status[16+i];
    }
  } else {
    astat[0]=msg.status[0];
    lstat[0]=msg.status[1];
  }
  return 0;
}
