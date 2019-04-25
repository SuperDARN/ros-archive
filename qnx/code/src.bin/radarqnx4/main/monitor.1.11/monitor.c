/* monitor.c
   =========
   Author: R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
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

#include <stdio.h>
#include <string.h>
#include <sys/name.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include "rtypes.h"
#include "rtime.h"
#include "option.h"
#include "limit.h"
#include "tsg.h"
#include "option.h"
#include "connex.h"
#include "echoraw.h"
#include "ad.h"
#include "taskname.h"
#include "hlpstr.h"
#include "fork.h"

/*
 $Log: monitor.c,v $
 Revision 1.11  2006/09/22 15:38:02  code
 Added alternative lag-zero power to the table.

 Revision 1.10  2006/08/23 19:45:56  code
 Fixed variable name.

 Revision 1.9  2006/08/23 19:44:28  code
 Added sample delay associated with the digital receiver.

 Revision 1.8  2006/08/18 15:41:08  code
 Fixed bug in decoding the lag table.

 Revision 1.7  2006/08/17 21:06:20  code
 Modification to retrieve the beam number and frequency.

 Revision 1.6  2006/04/07 20:21:15  barnes
 Adopted the echoraw transport mechanism.

 Revision 1.5  2005/08/04 15:09:25  barnes
 Fixed bug in handling XCF samples.

 Revision 1.4  2004/07/20 02:51:45  barnes
 Changed command line option to "-d" to specify driver.

 Revision 1.3  2004/06/22 12:56:20  barnes
 QNX4 Code audit.

 Revision 1.2  2004/06/08 20:43:42  barnes
 Increased the buffer size.

 Revision 1.1  2004/05/10 15:36:07  barnes
 Initial revision

*/

#define TRIGGER 't'
#define SCAN 's'
#define SET 'S'


int bufnum;
int bufsze;
char *bufadr[32];

int ptr=0;
int max=1024*1024;
char buf[1024*1024];

int num=0;

struct mark {
  struct timespec tval;
  int atten;
  int chn;
  int off;
  int sze;
};

struct mark mark[1024];

char *echoraw=NULL;
char echorawd[32]={ECHORAW};



struct packethdr {
  double time;
  int chn;
  int samp;
};

char *packet=NULL;


int arg=0;
struct OptionData opt;

int outpipe=-1;

void trap_pipe(int signal) {
  close(outpipe);
  outpipe=-1;
}

int main (int argc,char *argv[]) {
  int i;

  int help=0;
 
  pid_t rid;

  pid_t did;

  char rmsg;
  int status=0;
  int size=0;
  int bufnum;
  int numchn;
  int numsmp;
  int atten=0;


  struct timespec tval;
  struct TSGprm prm;
  int mplgs;
  int lagnum;
  int rsmpdelay=0;
  int iatten=0;
  int pat[PULSE_SIZE];
  int lag[LAG_SIZE*2];
  float noise;
  int tfreq;
  int bmnum;

  struct timespec stval;

  struct _mxfer_entry msg[7];

  struct packethdr *hptr;
  int16 *sptr;
  float *fptr;
  int chn;
  int sze;
  int snum;
  int n,c;

  int port=0;
  sigset_t set;
  struct sigaction act;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"e",'t',&echoraw);

  arg=OptionProcess(1,argc,argv,&opt,NULL);  
 

  if (arg<argc) port=atoi(argv[arg]);
  if (echoraw==NULL) echoraw=echorawd;


  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if ((did=EchoRawRegister("monitor",echoraw))==-1) {
     fprintf(stderr,"Could not register with echoraw task.\n");
     exit(-1);
  }

  bufnum=ADGetBufNum(did);
  bufsze=ADGetBufSize(did);
  for (i=0;i<bufnum;i++) bufadr[i]=ADGetBufAdr(did,i);

  signal(SIGCHLD,SIG_IGN); 

  sigemptyset(&set);
  sigaddset(&set,SIGPIPE);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trap_pipe;
  sigaction(SIGPIPE,&act,NULL);

  outpipe=forkinet(port);


  while (1) {
    rid=Receive(0,&rmsg,sizeof(char));
    switch (rmsg) { 
    case TRIGGER:
      _setmx(&msg[0],&status,sizeof(int));   
      Replymx(rid,1,msg);

      if (outpipe==-1) outpipe=forkinet(port);

      chn=mark[0].chn;
      snum=mark[0].sze/(4*chn);

      sze=sizeof(struct packethdr)+mark[0].sze+sizeof(float)*snum;      

      if (packet==NULL) packet=malloc(sze);
      else {
	char *tmp=NULL;
        tmp=realloc(packet,sze);
        if (tmp==NULL) {
	  free(packet);
          packet=NULL;
          continue;
	}
        packet=tmp;
      }

      fptr=(float *) (packet+sizeof(struct packethdr));
      hptr=(struct packethdr *) packet;

      hptr->time=time(NULL);
      hptr->chn=chn;
      hptr->samp=snum;

      for (n=0;n<snum;n++) fptr[n]=0;
      for (n=0;n<num;n++) {
        sptr=(int16 *) (buf+mark[n].off);
        for (c=0;c<snum;c++) {
            fptr[c]+=sptr[2*chn*c]*sptr[2*chn*c]+
                    sptr[2*chn*c+1]*sptr[2*chn*c+1];
        }
      }        
      for (n=0;n<snum;n++) fptr[n]=fptr[n]/num;
      memcpy(packet+sizeof(struct packethdr)+
	     sizeof(float)*snum,buf+mark[0].off,
	                   mark[0].sze);

      if (outpipe !=-1) status=ConnexWriteIP(outpipe,packet,sze);
    
      num=0;
      ptr=0;
      break;
    case SCAN:
       _setmx(&msg[0],&size,sizeof(int));
      _setmx(&msg[1],&tval,sizeof(struct timespec));
      _setmx(&msg[2],&bufnum,sizeof(bufnum));
      _setmx(&msg[3],&numsmp,sizeof(numsmp));
      _setmx(&msg[4],&numchn,sizeof(numchn));
      _setmx(&msg[5],&atten,sizeof(atten));

      Readmsgmx(rid,sizeof(char),6,msg);
      status=0;
      _setmx(&msg[0],&status,sizeof(int));   
      Replymx(rid,1,msg);

    
      if (num>=1024) continue;
      if (numchn==1) {
	if (ptr+numsmp*4>max) continue;
        memcpy(buf+ptr,bufadr[bufnum],numsmp*4);
        mark[num].tval.tv_sec=tval.tv_sec;
        mark[num].tval.tv_nsec=tval.tv_nsec;
        mark[num].chn=numchn;
        mark[num].off=ptr;
        mark[num].sze=numsmp*4;
        mark[num].atten=atten;
        ptr+=numsmp*4;
      } else {
	if (ptr+numsmp*4*2>max) continue;
        memcpy(buf+ptr,bufadr[bufnum],numsmp*4*2);
        mark[num].tval.tv_sec=tval.tv_sec;
        mark[num].tval.tv_nsec=tval.tv_nsec;
        mark[num].chn=numchn;
        mark[num].off=ptr;
        mark[num].sze=numsmp*4*2;
        mark[num].atten=atten;
        ptr+=numsmp*4*2;
      
      }
      num++;
      break;
   case SET:
      _setmx(&msg[0],&size,sizeof(int));
      _setmx(&msg[1],&tval,sizeof(struct timespec));
      _setmx(&msg[2],&prm,sizeof(struct TSGprm));

      Readmsgmx(rid,sizeof(char),3,msg);
      _setmx(&msg[0],pat,sizeof(int)*prm.mppul);
      _setmx(&msg[1],&mplgs,sizeof(int));
      _setmx(&msg[2],&lagnum,sizeof(int));
 
      Readmsgmx(rid,sizeof(char)+sizeof(int)+
                sizeof(struct timespec)+sizeof(struct TSGprm),3,msg);

      _setmx(&msg[0],lag,sizeof(int)*(lagnum+1)*2);

      Readmsgmx(rid,sizeof(char)+sizeof(int)+
                sizeof(struct timespec)+sizeof(struct TSGprm)+
                sizeof(int)*(prm.mppul+2),1,msg);

      _setmx(&msg[0],&rsmpdelay,sizeof(int));
      _setmx(&msg[1],&iatten,sizeof(int));
      _setmx(&msg[2],&noise,sizeof(float));
      _setmx(&msg[3],&bmnum,sizeof(int));
      _setmx(&msg[4],&tfreq,sizeof(int));

      Readmsgmx(rid,sizeof(char)+sizeof(int)+
                sizeof(struct timespec)+sizeof(struct TSGprm)+
                sizeof(int)*(prm.mppul+2+2*(lagnum+1)),5,msg);


      _setmx(&msg[0],&status,sizeof(int));   

  
      status=0;
      Replymx(rid,1,msg);
      stval.tv_sec=tval.tv_sec;
      stval.tv_nsec=tval.tv_nsec;
    default:
      break;
    }    
  }
  return 0;
} 
