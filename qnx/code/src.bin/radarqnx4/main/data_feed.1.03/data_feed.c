/* data_feed.c
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

#include <i86.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "rtypes.h"
#include "option.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"

#include "fitacf.h"
#include "rawread.h"
#include "fitread.h"
#include "fitwrite.h"

#include "oldrawread.h"
#include "oldfitread.h"

#include "taskid.h"
#include "rmsg.h"
#include "rmsgsnd.h"

#include "errstr.h"
#include "hlpstr.h"

/*
 $Log: data_feed.c,v $
 Revision 1.3  2004/06/22 12:41:54  barnes
 QNX4 Code audit.

 Revision 1.2  2004/06/04 14:44:37  barnes
 Adopted strings as radar identifiers.

 Revision 1.1  2004/05/10 15:08:48  barnes
 Initial revision

 */

struct RadarParm prm;
struct RawData raw;
struct FitData fit;
struct FitBlock fblk;

struct RadarNetwork *network;  
struct Radar *radar;
struct RadarSite *site;

struct TaskID *tsk[32];
struct RMsgBlock msg;

char *tasklist[32];

int arg=0;
struct OptionData opt;


long int strtime(char *text) {
  int hr,mn;
  int i;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  mn=atoi(text+i+1);
  return hr*3600L+mn*60L;
}   

void wait_boundary(float bnd) {
   int stime=0;
   int yr,mo,dy,hr,mt,sc,us;
   TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
   bnd=bnd*1000;
   stime=(mt*60+sc)*1000+us/1000.0;
   stime=(int) bnd - (stime % (int) bnd);
   if (stime<0) return;
   sleep(stime / 1000);
   delay(stime% 1000);
}   

void main(int argc,char *argv[]) {

 /* File format transistion
   * ------------------------
   * 
   * When we switch to the new file format remove any reference
   * to "new". Change the command line option "new" to "old" and
   * remove "old=!new".
   */


  unsigned char old=0;
  unsigned char new=0;

  int status;
  char *envstr;
  FILE *fp;
  struct OldRawFp *rawfp;
  struct OldFitFp *fitfp;
  int c;
  int tnum=0;
  char *str={"data_read"};
  int otime=0;
  double stime;
  double ctime;
  
  int i=0;
  float intt=7;
  float bnd=120;
  int st_id=-1;
  int rawflg=0;
  int repflg=0;
  int aflg=0; /* use the actual time rather than that from the file */
  char *ostr=NULL;
  char *bstr=NULL;
  char *ststr=NULL;
  
  int help=0; 

  int yr,mo,dy,hr,mt,sc,us;

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"new",'x',&new);
  OptionAdd(&opt,"r",'x',&rawflg);
  OptionAdd(&opt,"rp",'x',&repflg);
  OptionAdd(&opt,"a",'x',&aflg);
  OptionAdd(&opt,"o",'t',&ostr);
  OptionAdd(&opt,"b",'t',&bstr);
  OptionAdd(&opt,"s",'t',&ststr);
  OptionAdd(&opt,"i",'f',&intt);

  arg=OptionProcess(1,argc,argv,&opt,NULL);  

  old=!new;

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if ((argc-arg)<2) { 
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  envstr=getenv("SD_RADAR");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARNAME' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");

  if (fp==NULL) {
    fprintf(stderr,"Could not locate radar information file.\n");
    exit(-1);
  }

  network=RadarLoad(fp);
  fclose(fp); 
  if (network==NULL) {
    fprintf(stderr,"Failed to read radar information.\n");
    exit(-1);
  }

  envstr=getenv("SD_HDWPATH");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HDWPATH' must be defined.\n");
    exit(-1);
  }

  RadarLoadHardware(envstr,network);

  if (ostr !=NULL) otime=strtime(ostr);
  if (bstr !=NULL) bnd=strtime(bstr);
  if (ststr !=NULL) st_id=RadarGetID(network,ststr);



  for (c=arg;c<argc-1;c++) {  
    tasklist[tnum]=argv[c];
    tnum++;
  }
   

  if (tnum==0) {
    fprintf(stderr,"No tasks were specified.\n");
    exit(-1);
  }

  if (rawflg==1) {
    if (old) {
      rawfp=OldRawOpen(argv[argc-1],NULL);
      if (rawfp==NULL) {
        fprintf(stderr,"File not found.\n");
        exit(-1);
      } 
    } else {
      fp=fopen(argv[argc-1],"r");
      if (fp==NULL) {
        fprintf(stderr,"File not found.\n");
        exit(-1);
      }
    }
  } else {
    if (old) {
      fitfp=OldFitOpen(argv[argc-1],NULL);
      if (fitfp==NULL) {
        fprintf(stderr,"File not found.\n");
        exit(-1);
      }
    } else {
      fp=fopen(argv[argc-1],"r");
      if (fp==NULL) {
        fprintf(stderr,"File not found.\n");
        exit(-1);
      }
      status=FitFread(fp,&prm,&fit);
    }
  }

  for (i=0;i<tnum;i++) {
    fprintf(stderr,"%s\n",tasklist[i]);
    tsk[i]=TaskIDMake(tasklist[i]);
  }
  if (rawflg ==1) { /* using a dat file */
    if (old) status=OldRawRead(rawfp,&prm,&raw); 
    else status=RawFread(fp,&prm,&raw);
  } else {
    if (old) status=OldFitRead(fitfp,&prm,&fit);
    else status=FitFread(fp,&prm,&fit);
  }
  if (aflg==1) {
    if (prm.scan==1) {
      fprintf(stderr,"Start of new scan\n");
      wait_boundary(bnd);
    }
    TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
    prm.time.yr=yr;
    prm.time.mo=mo;
    prm.time.dy=dy;
    prm.time.hr=hr;
    prm.time.mt=mt;
    prm.time.sc=sc;
    prm.intt.sc=intt;
    prm.intt.us=0;
  }  
  if (st_id !=-1) prm.stid=st_id;

  stime=TimeYMDHMSToEpoch(prm.time.yr,prm.time.mo,
	                  prm.time.dy,prm.time.hr,prm.time.mt,prm.time.sc);

  for (i=0;i<tnum;i++) RMsgSndOpen(tsk[i],strlen(str),str);
  if (rawflg) {
    radar=RadarGetRadar(network,prm.stid);
    if (radar==NULL) {
      fprintf(stderr,"Failed to get radar information.\n");
      exit(-1);
    }

    site=RadarYMDHMSGetSite(radar,prm.time.yr,prm.time.mo,
		          prm.time.dy,prm.time.hr,prm.time.mt,
                          prm.time.sc);

    if (site==NULL) {
      fprintf(stderr,"Failed to get site information.\n");
      exit(-1);
    }
  
    FitACFStart(site,prm.time.yr,&fblk); 
  }
  do {
    msg.num=0;
    msg.tsize=0;
    fprintf(stderr,"%d-%d-%d %d:%d:%d\n",prm.time.yr,prm.time.mo,
	      prm.time.dy,prm.time.hr,prm.time.mt,prm.time.sc);
    RMsgSndAdd(&msg,sizeof(struct RadarParm),(unsigned char *) &prm,PRM_TYPE,
              0);        
      
    if (rawflg==1) {
      if (st_id !=-1) prm.stid=st_id;
      FitACF(&prm,&raw,&fblk,&fit);
      RMsgSndAdd(&msg,sizeof(struct RawData),(unsigned char *) &raw,RAW_TYPE,
              0);        


    } 
    RMsgSndAdd(&msg,sizeof(struct FitData),(unsigned char *) &fit,FIT_TYPE,
              0);        

    ctime=TimeYMDHMSToEpoch(prm.time.yr,prm.time.mo,
	                    prm.time.dy,prm.time.hr,prm.time.mt,prm.time.sc);
  
    if ((otime !=0) && ((ctime-stime)>=otime)) {
      fprintf(stderr,"Closing files.\n");
      for (i=0;i<tnum;i++) RMsgSndClose(tsk[i]);

      fprintf(stderr,"Opening files.\n");
      for (i=0;i<tnum;i++) RMsgSndOpen(tsk[i],strlen(str),str);

      stime=ctime;
    }
    for (i=0;i<tnum;i++) RMsgSndSend(tsk[i],&msg);
    delay(intt*1000.0);
    
    if (rawflg==1) {
      if (old) status=OldRawRead(rawfp,&prm,&raw); 
      else status=RawFread(fp,&prm,&raw);
    } else {
      if (old) status=OldFitRead(fitfp,&prm,&fit);
       else status=FitFread(fp,&prm,&fit);
    }

    if (aflg==1) {
      if (prm.scan==1) {
        fprintf(stderr,"Start of new scan\n");
        wait_boundary(bnd);

      }
      TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
      prm.time.yr=yr;
      prm.time.mo=mo;
      prm.time.dy=dy;
      prm.time.hr=hr;
      prm.time.mt=mt;
      prm.time.sc=sc;
      prm.intt.sc=intt;
      prm.intt.us=0;
    }
    if ((status !=0) && (repflg==1)) {
      status=0; 
      if (rawflg==1) {
        if (old) {
          OldRawClose(rawfp);
          rawfp=OldRawOpen(argv[argc-1],NULL);
          status=OldRawRead(rawfp,&prm,&raw); 
	} else {
          fclose(fp);
          fp=fopen(argv[argc-1],"r");
          status=RawFread(fp,&prm,&raw);
	}
      } else {
        if (old) {
          OldFitClose(fitfp);
          fitfp=OldFitOpen(argv[argc-1],NULL);
          status=OldFitRead(fitfp,&prm,&fit); 
	} else {
          fclose(fp);
          fp=fopen(argv[argc-1],"r");
          status=FitFread(fp,&prm,&fit);
	}
      }
      if (aflg==1) {
        if (prm.scan==1) {
          fprintf(stderr,"Start of new scan\n");
          wait_boundary(bnd);
        }
        TimeReadClock(&yr,&mo,&dy,&hr,&mt,&sc,&us);
        prm.time.yr=yr;
        prm.time.mo=mo;
        prm.time.dy=dy;
        prm.time.hr=hr;
        prm.time.mt=mt;
        prm.time.sc=sc;
        prm.intt.sc=intt;
        prm.intt.us=0;
      } 
      if (st_id !=-1) prm.stid=st_id;
      for (i=0;i<tnum;i++) RMsgSndClose(tsk[i]);
      for (i=0;i<tnum;i++) RMsgSndOpen(tsk[i],strlen(str),str);
      stime=TimeYMDHMSToEpoch(prm.time.dy,prm.time.mo,
	                   prm.time.yr,prm.time.hr,prm.time.mt,prm.time.sc);

    }
  } while (status==0);
  for (i=0;i<tnum;i++) RMsgSndQuit(tsk[i]);
  if (rawflg==1)  FitACFEnd(&fblk);
}
