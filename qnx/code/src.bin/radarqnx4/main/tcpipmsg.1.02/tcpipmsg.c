/* tcpipmsg.c
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
#include <sys/name.h>
#include <stdlib.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include "option.h"
#include "connex.h"
#include "taskname.h"
#include "hlpstr.h"
#include "fork.h"

/*
 $Log: tcpipmsg.c,v $
 Revision 1.2  2004/06/22 13:36:17  barnes
 QNX4 Code audit.

 Revision 1.1  2004/05/10 15:40:04  barnes
 Initial revision

*/

#define MESSAGE 'm'

char *buf=NULL;
int sze=0;

char *taskname=NULL;
char taskdname[32]={MSGSERVER};

int arg=0;
struct OptionData opt;

int outpipe=-1;

void trap_pipe(int signal) {
  close(outpipe);
  outpipe=-1;
}

int main (int argc,char *argv[]) {
  

  int help=0;
  int tid;
  pid_t rid;

  char rmsg;
  int status;

  struct _mxfer_entry msg[4];


  int offset=0;
  int port=0;
  sigset_t set;
  struct sigaction act;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"n",'t',&taskname);

  arg=OptionProcess(1,argc,argv,&opt,NULL);  
 

  if (arg<argc) port=atoi(argv[arg]);
  if (taskname==NULL) taskname=taskdname;


  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if ( (tid=qnx_name_attach(0,taskname) ) == -1) { 
    fprintf(stderr,"Unable to register name.\n");
    exit(EXIT_FAILURE);
  }

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
    case MESSAGE:

      offset=sizeof(char);
      status=(Readmsg(rid,offset,&sze,
             sizeof(int)) !=sizeof(int));
      offset+=sizeof(int);
      if (sze==0) {
        if (buf !=NULL) free(buf);
        buf=NULL;
      } else {
        if (buf==NULL) buf=malloc(sze);
        else {
          char *tmp;
          tmp=realloc(buf,sze);
          if (tmp==NULL) status=-1;
          else buf=tmp;
        }
        if (status==0) status=(Readmsg(rid,offset,buf,sze) !=sze); 
        
      }
      _setmx(&msg[0],&status,sizeof(int));   
      Replymx(rid,1,msg);


      if (outpipe==-1) outpipe=forkinet(port);
       
      if ((outpipe !=-1) && (status==0) && (sze>0)) 
         status=ConnexWriteIP(outpipe,buf,sze);
      break;
    default:
      break;
    }    
  }
  return 0;
} 
