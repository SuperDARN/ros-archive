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

#include <sys/types.h>
#include <sys/time.h>

#ifdef _QNX4
#include <sys/kernel.h>
#include <sys/sendmx.h>
#endif
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "rtypes.h"
#include "rtimer.h"
#include "dio.h"

#define TIME_OUT 5

/*
 $Log: dio.c,v $
 Revision 1.7  2004/06/16 22:00:47  barnes
 Fixed return codes.

 Revision 1.6  2004/06/04 21:35:28  barnes
 Fixed bug in the clear frequency search.

 Revision 1.5  2004/05/07 20:53:38  barnes
  Fixed bug in sending set tsg message.

 Revision 1.4  2004/05/04 18:49:36  barnes
 Renamed the type header to avoid confusion.

 Revision 1.3  2004/05/03 20:45:39  barnes
 Changed type ipcid to ipcid_t.

 Revision 1.2  2004/05/03 15:51:03  barnes
 Added ipcid type for the IPC calls.

 Revision 1.1  2003/10/31 19:01:10  barnes
 Initial revision

*/



int DIOSetGain(ipcid_t ipcid,unsigned char atten) {
#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 
  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[1]; 
  int s;

  char status;
  struct {
    char op_type;
	char atten;
  } msg;
  
  msg.op_type=SET_GAIN;
  msg.atten=atten;

  _setmx(&send_mx[0],&msg,sizeof(msg));
  _setmx(&reply_mx[0],&status,sizeof(char)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,1,1,send_mx,reply_mx);

  RTimerFree(tm); 

  if (s != 0) return s;
  if (status !=DIO_OK) return -1;
#endif
  return 0;
}


int DIOSetFilter(ipcid_t ipcid,unsigned char filter) {
#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 
  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[1]; 
  int s;

  char status;
  struct {
    char op_type;
	char filter;
  } msg;
  
  msg.op_type=SET_FILTER;
  msg.filter=filter;

  _setmx(&send_mx[0],&msg,sizeof(msg));
  _setmx(&reply_mx[0],&status,sizeof(char)); 


  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,1,1,send_mx,reply_mx);

  RTimerFree(tm); 

  if (s != 0) return s;
  if (status !=DIO_OK) return -1;
#endif
  return 0;
}



int DIOSetChannel(ipcid_t ipcid,unsigned char channel) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[1]; 
  int s;

  char status;
  struct {
    char op_type;
	char channel;
  } msg;
  
  msg.op_type=SET_CHANNEL;
  msg.channel=channel;

  _setmx(&send_mx[0],&msg,sizeof(msg));
  _setmx(&reply_mx[0],&status,sizeof(char)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,1,1,send_mx,reply_mx);

  RTimerFree(tm); 

  if (s != 0) return s;
  if (status !=DIO_OK) return -1;
#endif
  return 0;
}

int DIOSetBeam(ipcid_t ipcid,unsigned char beam) {

#ifdef _QNX4 
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[1]; 
  int s;
  char status;
  struct {
    char op_type;
    char beam;
  } msg;
  
  msg.op_type=SET_BEAM;
  msg.beam=beam;

  _setmx(&send_mx[0],&msg,sizeof(msg));
  _setmx(&reply_mx[0],&status,sizeof(char)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,1,1,send_mx,reply_mx);

  RTimerFree(tm); 

  if (s != 0) return s;
  if (status !=DIO_OK) return -1;
#endif
  return 0;
}



int DIOSetAntenna(ipcid_t ipcid,unsigned char mode,unsigned char bmnum) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[1]; 
  int s;

  char status;
  struct {
    char op_type;
    char mode;
    char bmnum;
  } msg;
  
  msg.op_type=SET_ANTENNA;
  msg.mode=mode;
  msg.bmnum=bmnum;

  _setmx(&send_mx[0],&msg,sizeof(msg));
  _setmx(&reply_mx[0],&status,sizeof(char)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,1,1,send_mx,reply_mx);
  RTimerFree(tm); 

  if (s != 0) return s;
  if (status !=DIO_OK) return -1;
#endif
  return 0;
}

int DIOSetTestMode(ipcid_t ipcid,unsigned char mode) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[1]; 
  int s;

  char status;
  struct {
    char op_type;
    char mode;
  } msg;
  
  msg.op_type=SET_TEST_MODE;
  msg.mode=mode;

  _setmx(&send_mx[0],&msg,sizeof(msg));
  _setmx(&reply_mx[0],&status,sizeof(char)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,1,1,send_mx,reply_mx);
  RTimerFree(tm); 

  if (s != 0) return s;
  if (status !=DIO_OK) return -1;
#endif
  return 0;
}


int DIOSetFreq(ipcid_t ipcid,int freq) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[1]; 
  int s;

  char status;
  struct {
    char op_type;
    int freq;
  } msg;
  
  msg.op_type=SET_FREQ;
  msg.freq=freq;

  _setmx(&send_mx[0],&msg,sizeof(msg));
  _setmx(&reply_mx[0],&status,sizeof(char)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,1,1,send_mx,reply_mx);
  RTimerFree(tm); 

  if (s != 0) return s;
  if (status == FREQ_LOCAL) return FREQ_LOCAL;
  if (status !=DIO_OK) return -1;
#endif
  return 0;
}



int DIOReadFreqMode(ipcid_t ipcid) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[1]; 
  int s;

  struct {
    char op_type;
    int mode;
  } msg;
  
  msg.op_type=GET_FREQ_MODE;

  _setmx(&send_mx[0],&msg,sizeof(msg.op_type));
  _setmx(&reply_mx[0],&msg,sizeof(msg)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,1,1,send_mx,reply_mx);
  RTimerFree(tm); 

  if (s != 0) return s;
  if (msg.op_type !=DIO_OK) return -1;
  if (msg.mode == FREQ_LOCAL) return FREQ_LOCAL;  
#endif
  return 0;
}



int DIOReadTStatus(ipcid_t ipcid) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[1]; 
  int s;

  struct {
    char op_type;
    char status;
  } msg;
  
  msg.op_type=GET_TSTATUS;

  _setmx(&send_mx[0],&msg,sizeof(msg.op_type));
  _setmx(&reply_mx[0],&msg,sizeof(msg)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,1,1,send_mx,reply_mx);
  RTimerFree(tm); 

  if (s != 0) return s;
  if (msg.op_type !=DIO_OK) return -1;
  else return (msg.status !=0);
#else
  return 0;
#endif
}


int DIOReset(ipcid_t ipcid) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[1]; 
  int s;

  char status;
  char msg;
  
  msg=DIO_RESET;

  _setmx(&send_mx[0],&msg,sizeof(msg));
  _setmx(&reply_mx[0],&status,sizeof(status)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,1,1,send_mx,reply_mx);
  RTimerFree(tm); 

  if (s != 0) return s;
  if (status !=DIO_OK) return -1;
#endif
  return 0;
}


 
int DIOVerifyID(ipcid_t ipcid,unsigned char seq) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[1]; 
  int s;

  struct {
    char op_type;
    char seq;
  } smsg;

  struct {
     char status;
     int length;	 
  } rmsg;
  
  smsg.op_type=VERIFY_ID;
  smsg.seq=seq;

  _setmx(&send_mx[0],&smsg,sizeof(smsg));
  _setmx(&reply_mx[0],&rmsg,sizeof(rmsg)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,1,1,send_mx,reply_mx);
  RTimerFree(tm); 

  if (s != 0) return s;
  if (rmsg.status !=DIO_OK) return -1;
  else return (rmsg.length !=0);
#else
  return 0;
#endif
}



int DIOSetTSG(ipcid_t ipcid,unsigned char id,int length,char *codebyte,
	       char *repbyte, char *xtdbyte) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[4];
  struct _mxfer_entry reply_mx[1]; 
  int s;

  char status=0;
  struct {
    char op_type;
    char seq;
    char xtflg;
    int length;
  } msg;
  
  msg.op_type=SET_TSG;
  msg.seq=id;
  msg.length=length; 
  msg.xtflg=0;
  if (xtdbyte !=NULL) msg.xtflg=1;
  
  _setmx(&send_mx[0],&msg,sizeof(msg));
  _setmx(&send_mx[1],codebyte,length);
  _setmx(&send_mx[2],repbyte,length);
  if (xtdbyte !=NULL) _setmx(&send_mx[3],xtdbyte,length);

  _setmx(&reply_mx[0],&status,sizeof(char)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;
  s=Sendmx(ipcid,3+(xtdbyte !=NULL),1,send_mx,reply_mx);
  RTimerFree(tm); 

  if (s != 0) return s;
  if (status !=DIO_OK) return -1;
#endif
  return 0;
}

int DIOSendTSG(ipcid_t ipcid,int stereo,unsigned char *id,
	       unsigned char *xtdflag,unsigned char *stflag,
	       int *delay,int **freq) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[1]; 
  int s;

  char status;
  struct {
    char op_type;
    char stereo;
    char seq[2];
    int delay[2];
    char xtdflag[2];
    char stflag[2];
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
  msg.xtdflag[0]=xtdflag[0];
  if (stereo !=0) msg.xtdflag[1]=xtdflag[1];  
  else msg.xtdflag[1]=0;

  if (freq !=NULL) {
    msg.xtdflag[0]|=(freq[0] !=NULL)*2;
    if (stereo !=0) msg.xtdflag[1]|=(freq[1] !=NULL)*2;
    else msg.xtdflag[1]=NULL;
  }

  msg.stflag[0]=stflag[0]; 
  if ((freq !=NULL) && (freq[0] !=NULL)) msg.f1=(freq[0])[0];
  else msg.f1=0;
  if ((freq !=NULL) && (freq[0] !=NULL)) msg.f2=(freq[0])[1];
  else msg.f2=0;
  msg.stflag[1]=stflag[1];
  if (stereo !=0) { 
    if ((freq !=NULL) && (freq[1] !=NULL)) msg.f3=(freq[1])[0];
    else msg.f3=0;
    if ((freq !=NULL) && (freq[1] !=NULL)) msg.f4=(freq[1])[1];
    else msg.f4=0;
  } else {
    msg.f3=0;
    msg.f4=0;
  }
  
  _setmx(&send_mx[0],&msg,sizeof(msg));
  _setmx(&reply_mx[0],&status,sizeof(char)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,1,1,send_mx,reply_mx);
  RTimerFree(tm); 

  if (s != 0) return s;
  if (status !=DIO_OK) return -1;
#endif
  return 0;
}



int DIOSendFCLR(ipcid_t ipcid,int stereo,unsigned char *id,
              int *delay,int *freqnum,int **freqtable) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[4];
  struct _mxfer_entry reply_mx[1]; 
  int s;
  
  char status;
  struct {
    char op_type;
    char stereo;
    char seq[2];
    int delay[2];
    int freqnum[2];
  } msg;

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

  msg.freqnum[0]=freqnum[0];
  if (stereo !=0) msg.freqnum[1]=freqnum[1];
 
  _setmx(&send_mx[0],&msg,sizeof(msg));
  _setmx(&send_mx[1],freqtable[0],freqnum[0]*sizeof(int));
  if (stereo !=0) _setmx(&send_mx[2],freqtable[1],freqnum[1]*sizeof(int));
  _setmx(&reply_mx[0],&status,sizeof(char)); 

  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,2+(stereo !=0),1,send_mx,reply_mx);
  RTimerFree(tm); 

  if (s != 0) return s;
  if (status==FREQ_LOCAL) return FREQ_LOCAL;
  if (status !=DIO_OK) return -1;
#endif
  return 0;
}

int DIOGetStatus(ipcid_t ipcid,int clear,int full,int *lstat,int *astat) {

#ifdef _QNX4
  struct RTimer *tm;
  struct timeval tmout; 

  struct _mxfer_entry send_mx[1];
  struct _mxfer_entry reply_mx[1]; 
  int s;

  char smsg;
  struct {
     char status;
     int stat[32];	 
  } rmsg;
  int i;
  
  if ((clear==1) && (full==0)) smsg=GET_STATUS_NOFULL_NOCLR;
  else if ((clear==0) && (full==0)) smsg=GET_STATUS_NOFULL_CLR;
  else if ((clear==1) && (full==1)) smsg=GET_STATUS_FULL_NOCLR;
  else smsg=GET_STATUS_FULL_CLR;
 
  _setmx(&send_mx[0],&smsg,sizeof(smsg));
  _setmx(&reply_mx[0],&rmsg,sizeof(rmsg)); 


  tmout.tv_sec=TIME_OUT;
  tmout.tv_usec=0;
  tm=RTimerMake(&tmout);
  if (tm==NULL) return -1;

  s=Sendmx(ipcid,1,1,send_mx,reply_mx);

  RTimerFree(tm); 

  if (s != 0) return s;
  if (rmsg.status !=DIO_OK) return -1;
 
  if (full==1) {
    for (i=0;i<16;i++) {
      astat[i]=rmsg.stat[i];
      lstat[i]=rmsg.stat[16+i];
    }
  } else {
    astat[0]=rmsg.stat[0];
    lstat[0]=rmsg.stat[1];
  }
#endif
  return 0;
}
  
