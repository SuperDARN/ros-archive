/* rtserver.c
   ==========
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
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include "rtypes.h"
#include "option.h"
#include "taskid.h"
#include "errlog.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "radar.h"
#include "connex.h"
#include "rmsg.h"
#include "rmsgrcv.h"
#include "fio.h"
#include "fitwrite.h"
#include "echo.h"

#include "version.h"
#include "taskname.h"
#include "hlpstr.h"
#include "fitpacket.h"
#include "fork.h"

/*
 $Log: rtserver.c,v $
 Revision 1.4  2004/06/22 13:27:55  barnes
 QNX4 Code audit.

 Revision 1.3  2004/05/13 22:20:45  barnes
 Added a reply to the quit message.

 Revision 1.2  2004/05/11 14:52:48  barnes
 Changed version handling.

 Revision 1.1  2004/05/10 15:38:47  barnes
 Initial revision

 */

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;

struct RMsgBlock rblk;
char *store=NULL; 

struct DMsg {
  int tag;
  struct RadarParm *prm;
  struct FitData *fdt;
};

int dnum=0;
int dptr=0;
struct DMsg dmsg[32];

int arg=0;
struct OptionData opt;

int outpipe=-1;

char *taskname=NULL;
struct TaskID *tsk=NULL;


void trap_pipe(int signal) {
  close(outpipe);
  outpipe=-1;
}

main (int argc,char *argv[]) {

  int status;
  int tid;
  int rid;
  char msg;
  char rmsg;
  int i;

  int help=0;
  int port=0;
  int channel=-1;

  char errbuf[256];
  char tasknamed[32]={TASK_NAME};
  char echodatad[32]={ECHO_DATA};
  char errlogd[32]={ERRLOG};
  char *errlog=NULL;
  char *echodata=NULL;
  char *chstr=NULL;
  

  char echopass[3]={PRM_TYPE,FIT_TYPE,0};

  unsigned char *bufadr=NULL;
  int bufsze;

  sigset_t set;
  struct sigaction act;
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"e",'t',&errlog);
  OptionAdd(&opt,"ec",'t',&echodata);
  OptionAdd(&opt,"n",'t',&taskname);
  OptionAdd(&opt,"c",'t',&chstr);
  
  arg=OptionProcess(1,argc,argv,&opt,NULL);  
  
  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }
  
  if (arg<argc) port=atoi(argv[arg]);
  if (errlog==NULL) errlog=errlogd;
  if (echodata==NULL) echodata=echodatad;
  if (taskname==NULL) taskname=tasknamed;
  if (chstr !=NULL) {
    if (tolower(chstr[0])=='a') channel=1;
    if (tolower(chstr[0])=='b') channel=2;
  }

  tsk=TaskIDMake(errlog);
  if (tsk==NULL) {
    perror("Unable to establish error handler.");
    exit(EXIT_FAILURE);
  }  

  if (EchoRegister(TASK_NAME,echodata,echopass,2) !=0) {
    fprintf(stderr,"Could not register with echo_data task.\n");
    exit(1);
  }

  if ( (tid=qnx_name_attach(0,taskname) ) == -1) { 
    perror("Unable to attach fitacfwrite task");
    ErrLog(tsk,taskname,"Unable to register name");
    exit(EXIT_FAILURE);
  }

  sprintf(errbuf,"Started (version %d.%.2d)",MAJOR_VERSION,MINOR_VERSION);
  ErrLog(tsk,taskname,errbuf);  

  signal(SIGCHLD,SIG_IGN); 

  sigemptyset(&set);
  sigaddset(&set,SIGPIPE);

  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=trap_pipe;
  sigaction(SIGPIPE,&act,NULL);

  outpipe=forkinet(port);
 
  while (1) {
   
    rid=Receive(0,&msg,sizeof(char)); 
    rmsg=TASK_OK;
    switch (msg) {
    case TASK_OPEN:
      break;
    case TASK_CLOSE:
      break;
    case TASK_QUIT:
      Reply(rid,&rmsg,sizeof(char));
      ErrLog(tsk,taskname,"Stopped.");
      exit(0); 
      break;
    case TASK_RESET:
      ErrLog(tsk,taskname,"Reset");
      break;
    case TASK_DATA:
      rmsg=RMsgRcvDecodeData(rid,&rblk,&store);
    default:
      break;
    }
    Reply(rid,&rmsg,sizeof(char));
      
    if (msg==TASK_DATA) {
      dnum=0;
      for (i=0;i<rblk.num;i++) {
        for (dptr=0;dptr<dnum;dptr++) 
          if (dmsg[dptr].tag==rblk.data[i].tag) break;
          if (dptr==dnum) {
            dmsg[dptr].tag=rblk.data[i].tag;
            dmsg[dptr].prm=NULL;
            dmsg[dptr].fdt=NULL;
            dnum++;
	  }
          switch (rblk.data[i].type) {
	  case PRM_TYPE:
            dmsg[dptr].prm=(struct RadarParm *) 
                (rblk.ptr[rblk.data[i].index]);
	    break;
          case FIT_TYPE:
            dmsg[dptr].fdt=(struct FitData *) 
                (rblk.ptr[rblk.data[i].index]);
	    break;
          default:
            break;
	  }
      }
      for (dptr=0;dptr<dnum;dptr++) {
        if ((channel !=-1) && (dmsg[dptr].prm->channel !=0)) {
          if ((channel==1) && (dmsg[dptr].prm->channel==2)) continue;
          if ((channel==2) && (dmsg[dptr].prm->channel!=2)) continue;
	}
        if (outpipe==-1) {
          sprintf(errbuf,"Child process died - Restarting.");
          ErrLog(tsk,taskname,errbuf);  
          outpipe=forkinet(port);
        }
        bufadr=fitpacket(dmsg[dptr].prm,dmsg[dptr].fdt,&bufsze);
        if (bufadr !=NULL) {
          if (outpipe !=-1) status=ConnexWriteIP(outpipe,bufadr,bufsze);
          free(bufadr);
	}       
      }
      if (store !=NULL) free(store);
      store=NULL;
    }  
  }
}
