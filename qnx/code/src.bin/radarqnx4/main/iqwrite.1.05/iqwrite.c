/* iqwrite.c
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
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/stat.h>
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
#include "iqdata.h"
#include "radar.h"
#include "rmsg.h"
#include "rmsgrcv.h"
#include "fio.h"
#include "shmem.h"
#include "iqwrite.h"


#include "version.h"
#include "taskname.h"
#include "hlpstr.h"

/*
 $Log: iqwrite.c,v $
 Revision 1.5  2008/03/17 21:41:05  code
 Fixed offset code.

 Revision 1.4  2008/03/15 15:44:31  code
 Added an extra part of the data message to allow a byte offset into the IQ sample buffer.

 Revision 1.3  2008/03/14 23:40:51  code
 Code cleanup.

 Revision 1.2  2008/03/14 20:39:15  code
 Added the SD_IQ_PATH environment variable to get iqdat directory.

 Revision 1.1  2008/03/14 20:34:00  code
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
  struct IQData *iqdt;
  char *iqs;
  int *iqoff;
};

int dnum=0;
int dptr=0;
struct DMsg dmsg[32];

int arg=0;
struct OptionData opt;

char dummy[128*1024];

main (int argc,char *argv[]) {

  char *envstr;
  int status;
  int rawid;
  int rid;
  char msg;
  char rmsg;
  int i;

  int flg=0;
  char *filename=NULL;

  char *cbufadr=NULL;
  int cbuflen;

  FILE *fp;
  int help=0;
 
  char errbuf[256];
  char taskname[32]={TASK_NAME};
  char errlogd[32]={ERRLOG};
  char *errlog=NULL;
  struct TaskID *tsk=NULL;
  float thr=3.0; 


  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"e",'t',&errlog);
  OptionAdd(&opt,"t",'f',&thr);
 
  arg=OptionProcess(1,argc,argv,&opt,NULL);  
  
  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
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

 
  /* allocate memory to store the records */
 

  
  if (arg<argc) strcpy(taskname,argv[arg]);
  if (errlog==NULL) errlog=errlogd;
 
  tsk=TaskIDMake(errlog);
  if (tsk==NULL) {
    perror("Unable to establish error handler.");
    exit(EXIT_FAILURE);
  }  

  if ( (rawid=qnx_name_attach(0,taskname) ) == -1) { 
    perror("Unable to attach rawacfwrite task");
    ErrLog(tsk,taskname,"Unable to register name");
    exit(EXIT_FAILURE);
  }

  sprintf(errbuf,"Started (version %d.%.2d)",MAJOR_VERSION,MINOR_VERSION);
  ErrLog(tsk,taskname,errbuf);  

  while (1) {
   
    rid=Receive(0,&msg,sizeof(char)); 
    rmsg=TASK_OK;
    switch (msg) {
    case TASK_OPEN:
      ErrLog(tsk,taskname,"Opening file.");
      rmsg=RMsgRcvDecodeOpen(rid,&cbuflen,&cbufadr);
      if (rmsg==TASK_OK) flg=1;
      break;
    case TASK_CLOSE:
      ErrLog(tsk,taskname,"Closing file.");
      if (filename !=NULL) free(filename);
      filename=NULL;
      break;
    case TASK_QUIT:
      Reply(rid,&rmsg,sizeof(char));
      ErrLog(tsk,taskname,"Stopped.");
      exit(0); 
      break;
    case TASK_RESET:
      ErrLog(tsk,taskname,"Reset");
      if (filename !=NULL) free(filename);
      filename=NULL;
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
            dmsg[dptr].iqdt=NULL;
            dnum++;
	  }
          switch (rblk.data[i].type) {
	  case PRM_TYPE:
            dmsg[dptr].prm=(struct RadarParm *) 
                (rblk.ptr[rblk.data[i].index]);
	    break;
          case IQ_TYPE:
            dmsg[dptr].iqdt=(struct IQData *) 
                (rblk.ptr[rblk.data[i].index]);
	    break;
          case IQS_TYPE:
            dmsg[dptr].iqs=(char *) 
                (rblk.ptr[rblk.data[i].index]);
	    break;
          case IQO_TYPE:
            dmsg[dptr].iqoff=(int *)
	      (rblk.ptr[rblk.data[i].index]);
            break;          
          default:
            break;
	  }
      }
      if ((filename==NULL) && (flg !=0)) {

        flg=0;
        if ((dnum>0) && (dmsg[0].prm !=NULL)) {
         
          filename=FIOMakeFile(getenv("SD_IQ_PATH"),
			       dmsg[0].prm->time.yr,
                               dmsg[0].prm->time.mo,
                               dmsg[0].prm->time.dy,
                               dmsg[0].prm->time.hr,
                               dmsg[0].prm->time.mt,
                               dmsg[0].prm->time.sc,
			       RadarGetCode(network,dmsg[0].prm->stid,0),
			       "iqdat",0,0);
	}
        if (filename==NULL) ErrLog(tsk,taskname,"Could not create file.");
      }
      if (filename !=NULL) {
        fp=fopen(filename,"a");
        if (fp==NULL) ErrLog(tsk,taskname,"Error opening file.");
	else {
          int fd,s;
          unsigned char *p;
          int offset=0;
          for (dptr=0;dptr<dnum;dptr++) {
            if (dmsg[dptr].iqs==NULL) continue; /* no buffer */
            if (dmsg[dptr].iqoff !=NULL) offset=*dmsg[dptr].iqoff;
            p=ShMemAlloc(dmsg[dptr].iqs,IQBUFSIZE,O_RDWR,0,&fd);
     
            status=IQFwrite(fp,dmsg[dptr].prm,
                             dmsg[dptr].iqdt,(int16 *) (p+offset));  
            s=ShMemFree(p,dmsg[dptr].iqs,IQBUFSIZE,0,fd);
            
            if (status==-1) break;
	  }
          if (status==-1) ErrLog(tsk,taskname,"Error writing record.");
          fclose(fp);
	}
      }
      if (store !=NULL) free(store);
      store=NULL;
    }  
  }
}
