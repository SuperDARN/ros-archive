/* normalscan.c
   ============
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "limit.h"
#include "radar.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "fitacf.h"


#include "taskid.h"
#include "errlog.h"
#include "rmsg.h"
#include "radarshell.h"
#include "rmsgsnd.h"
#include "tsg.h"
#include "tsgtable.h"
#include "maketsg.h"
#include "global.h"
#include "setup.h"
#include "reopen.h"
#include "tmseq.h"
#include "raw.h"
#include "sync.h"
#include "interface.h"
#include "hdw.h"

/*
 $Log: normalscan.c,v $
 Revision 1.2  2004/07/23 15:48:27  barnes
 Fixed bug in beam swinging direction.

 Revision 1.1  2004/05/11 17:43:22  barnes
 Initial revision
 
*/

#define SCHEDULER "schedule"
#define ERRLOG "errlog"

#define CONTROL_NAME "control_program"
#define TASK_NAMES "echo_data","raw_write","fit_write","rawacfwrite","fitacfwrite"

char cmdlne[1024];
char progid[80]={"$Id: normalscan.c,v 1.2 2004/07/23 15:48:27 barnes Exp $"};
char progname[256];
struct TaskID *errlog;

char *tasklist[]=
 { TASK_NAMES,
  0};

int arg=0;
struct OptionData opt;
      
int main(int argc,char *argv[]) {

  int ptab[8] = {0,14,22,24,27,31,42,43};
  int lags[2*24] ={
    0,42,22,24,27,22,24,14,  22,14,31,31,14, 0,27,27,14,  24,24,22,22, 0, 0,
    0,43,24,27,31,27,31,22,  31,24,42,43,27,14,42,43,31,  42,43,42,43,22,24};

  char *sname=NULL;
  char *ename=NULL;
  char *sdname={SCHEDULER};
  char *edname={ERRLOG};
  char logtxt[1024];

  int n;
  pid_t sid;
  int exitpoll=0;
 
  int scnsc=120;
  int scnus=0;
  int skip;
  int cnt=0;

  unsigned char fast=0;
  unsigned char discretion=0;

  strcpy(cmdlne,argv[0]);
  for (n=1;n<argc;n++) {
    strcat(cmdlne," ");
    strcat(cmdlne,argv[n]);
  } 

  strncpy(combf,progid,80);   

  OpsSetupRadar();
  OpsSetupShell();
   
  RadarShellParse(&rstable,"sbm l ebm l dfrq l nfrq l dfrang l nfrang l dmpinc l nmpinc l frqrng l xcnt l",                        
                  &sbm,&ebm,                              
                  &dfrq,&nfrq,                  
                  &dfrang,&nfrang,                            
                  &dmpinc,&nmpinc,                            
                  &frqrng,&xcnt);        

  cp=150;
  intsc=7;
  intus=0;
  mppul=8;
  mplgs=23;
  mpinc=1500;
  dmpinc=1500;
  nrang=75;
  rsep=45;
  txpl=300;

  SiteStart();

  /* ========= PROCESS COMMAND LINE ARGUMENTS ============= */


  OptionAdd(&opt,"dt",'x',&discretion);


  OptionAdd(&opt,"el",'t',&ename);
  OptionAdd(&opt,"sc",'t',&sname);

  OptionAdd(&opt,"frang",'i',&frang);
  OptionAdd(&opt,"rsep",'i',&rsep);
 
  OptionAdd(&opt,"fast",'x',&fast);
 
  
  arg=OptionProcess(1,argc,argv,&opt,NULL);  
 
  if (sname==NULL) sname=sdname;
  if (ename==NULL) ename=edname;

  sid=RShellRegister(sname,CONTROL_NAME);

  errlog=TaskIDMake(ename);  
  OpsLogStart(errlog,progname,argc,argv);  

  SiteSetupHardware();

  if (fast) {
     scnsc=60;
     scnus=0;
     intsc=3;
     intus=0;
  }

  txpl=(rsep*20)/3;

  if (fast) sprintf(progname,"normalscan (fast)");
  else sprintf(progname,"normalscan");

  OpsFitACFStart();

  OpsSetupTask(tasklist);
  for (n=0;n<tnum;n++) {
    RMsgSndReset(tlist[n]);
    RMsgSndOpen(tlist[n],strlen(cmdlne),cmdlne);     
  }

  do {

    if (SiteStartScan()==0) continue;

    if (OpsReOpen(2,0,0) !=0) {
      ErrLog(errlog,progname,"Opening new files.");
      for (n=0;n<tnum;n++) {
        RMsgSndClose(tlist[n]);
        RMsgSndOpen(tlist[n],strlen(cmdlne),cmdlne);     
      }
    }

    scan=1;

    ErrLog(errlog,progname,"Starting scan.");

    if (xcnt>0) {
      cnt++;
      if (cnt==xcnt) {
        xcf=1;
        cnt=0;
      } else xcf=0;
    } else xcf=0;

    skip=OpsFindSkip(scnsc,scnus);

    if (backward) {
      bmnum=sbm-skip;
      if (bmnum<ebm) bmnum=sbm;
    } else {
      bmnum=sbm+skip;
      if (bmnum>ebm) bmnum=sbm;
    }

    do {

      TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
 
      if (OpsDayNight()==1) {
        stfrq=dfrq;
        mpinc=dmpinc;
        frang=dfrang;
      } else {
        stfrq=nfrq;
        mpinc=nmpinc;
        frang=nfrang;
      }        

      sprintf(logtxt,"Integrating beam:%d intt:%ds.%dus (%d:%d:%d:%d)",bmnum,
                      intsc,intus,hr,mt,sc,us);
      ErrLog(errlog,progname,logtxt);

      ErrLog(errlog,progname,"Setting beam.");

      SiteSetIntt(intsc,intus);
      SiteSetBeam(bmnum);

      ErrLog(errlog,progname,"Doing clear frequency search."); 
 
      if (SiteFCLR(stfrq,stfrq+frqrng)==FREQ_LOCAL)
        ErrLog(errlog,progname,"Frequency Synthesizer in local mode.");

      SiteSetFreq(tfreq);

      sprintf(logtxt,"Transmitting on: %d (Noise=%g)",tfreq,noise);

      ErrLog(errlog,progname,logtxt);

      tsgid=SiteTimeSeq(ptab);

      nave=SiteIntegrate(lags);   
      if (nave==-1) {
        ErrLog(errlog,progname,"Integration error."); 
        continue;
      }
      sprintf(logtxt,"Number of sequences: %d",nave);
      ErrLog(errlog,progname,logtxt);


      OpsBuildRaw(&prm,&raw,ptab,lags);

      FitACF(&prm,&raw,&fblk,&fit);

      ErrLog(errlog,progname,"Sending messages."); 
  

      msg.num=0;
      msg.tsize=0;

      RMsgSndAdd(&msg,sizeof(struct RadarParm),(unsigned char *) &prm,
		PRM_TYPE,0); 
      RMsgSndAdd(&msg,sizeof(struct RawData),(unsigned char *) &raw,
		RAW_TYPE,0);    

      RMsgSndAdd(&msg,sizeof(struct FitData),(unsigned char *) &fit,
		FIT_TYPE,0);   
      RMsgSndAdd(&msg,strlen(progname)+1,progname,
		NME_TYPE,0);   
 

      for (n=0;n<tnum;n++) RMsgSndSend(tlist[n],&msg); 
  
      ErrLog(errlog,progname,"Polling for exit."); 
    
      exitpoll=RadarShell(sid,&rstable);
      if (exitpoll !=0) break;
      scan=0;
      if (bmnum==ebm) break;
      if (backward) bmnum--;
      else bmnum++;

    } while (1);
    ErrLog(errlog,progname,"Waiting for scan boundary."); 
  
    if (exitpoll==0) OpsWaitBoundary(scnsc,scnus);

  } while (exitpoll==0);
  SiteEnd();
  for (n=0;n<tnum;n++) RMsgSndClose(tlist[n]);
  ErrLog(errlog,progname,"Ending program.");
  RShellTerminate(sid);
  return 0;   
} 
 
