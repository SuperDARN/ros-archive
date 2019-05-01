/* fit_write.c
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
#include "fitdata.h"
#include "radar.h"
#include "rmsg.h"
#include "rmsgrcv.h"
#include "fio.h"

#include "oldfitwrite.h"


#include "version.h"
#include "taskname.h"
#include "hlpstr.h"

/*
 $Log: fit_write.c,v $
 Revision 1.21  2004/06/04 14:51:27  barnes
 Adopted text strings as radar identifiers.

 Revision 1.20  2004/05/13 22:21:48  barnes
 Added a reply to the quit message.

 Revision 1.19  2004/05/10 15:15:53  barnes
 Modification to deal with the new libraries.

 Revision 1.18  2001/06/27 20:56:07  barnes
 Added license tag

 Revision 1.17  2001/01/29 18:12:09  barnes
 Added Author Name

 Revision 1.16  2000/10/30 16:17:33  barnes
 Changed the fit header to contain the correct version number and
 the name of the control program.

 Revision 1.15  2000/09/20 14:29:16  barnes
 Removed redundant code.

 Revision 1.14  2000/09/18 18:22:36  barnes
 Added missing header.

 Revision 1.13  1999/04/10 20:44:22  barnes
 Added help message system.

 Revision 1.12  1999/03/30 19:22:46  barnes
 Moved task names into a header.

 Revision 1.11  1999/03/19 18:37:44  barnes
 Modifications to support the new file libraries.

 Revision 1.10  1998/11/09 23:03:56  barnes
 Incorporated rawdata header.

 Revision 1.9  1998/08/20 17:10:45  barnes
 Changed file suffix to lower case.

 Revision 1.8  1998/06/19 14:39:27  root
 Use the new radar_name function.

 Revision 1.7  1998/06/11 15:53:20  root
 Fixed problem with the reset message.

 Revision 1.6  1998/06/11 15:46:13  root
 Added support for the TASK_RESET message.

 Revision 1.5  1998/06/11 14:53:37  root
 Moved usage into a different file.

 Revision 1.4  1998/06/04 18:59:09  root
 Modification to stop the files being opened and closed each time a
 data block is processed.

 Revision 1.3  1998/06/03 20:20:23  root
 Modification to the version control system.

 Revision 1.2  1998/06/02 12:21:34  root
 Fixed problems in formatting the output file.

 Revision 1.1  1998/05/29 11:59:35  root
 Initial revision

 */

#define ORIG_MAX_RANGE 75

struct RadarNetwork *network;  

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
struct RadarParm prm;

int arg=0;
struct OptionData opt;

main (int argc,char *argv[]) {

  char *envstr;
  int status;
  int fitid;
  int rid;
  char msg;
  char rmsg;
  int i; 

  int flg=0;
  char *fitname=NULL;
  char *inxname=NULL;
  char *cbufadr=NULL;
  int cbuflen;
  FILE *fp=NULL;
  FILE *fitfp=NULL;
  FILE *inxfp=NULL;

  int frec=0;
  int fnum=0;
  int irec=0;

  char errbuf[256];
  char taskname[32]={TASK_NAME};
  char errlogd[32]={ERRLOG};
  char *errlog=NULL;
  char vstr[256];

  struct TaskID *tsk=NULL;

  int rtab[ORIG_MAX_RANGE];
  float snr[MAX_RANGE];
  int inx,l,s,step;
  float maxval;  

  int help=0;

  sprintf(vstr,"%d.%.2d",MAJOR_VERSION,MINOR_VERSION);  

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"e",'t',&errlog);
  
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
   
  if (arg<argc) strcpy(taskname,argv[arg]);
  if (errlog==NULL) errlog=errlogd;
 
  tsk=TaskIDMake(errlog);
  if (tsk==NULL) {
    perror("Unable to establish error handler.");
    exit(EXIT_FAILURE);
  }  

  if ((fitid=qnx_name_attach(0,taskname) ) == -1) { 
    perror("Unable to attach fit_write task");
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
      
      if (inxname !=NULL) {
        status=((inxfp=fopen(inxname,"r+")) ==NULL);
        if (status==0) status=OldFitInxFclose(inxfp,&prm,irec-1);            
        if (inxfp !=NULL) fclose(inxfp);
        free(inxname);
        inxname=NULL;
      }
      
      if (fitname !=NULL) {
	free(fitname);
        fitname=NULL;
      }
      break;
    case TASK_QUIT:
      Reply(rid,&rmsg,sizeof(char));
      ErrLog(tsk,taskname,"Stopped.");
      
      if (inxname !=NULL) {
        status=((inxfp=fopen(inxname,"r+")) ==NULL);
        if (status==0) status=OldFitInxFclose(inxfp,&prm,irec-1);            
        if (inxfp !=NULL) fclose(inxfp);
        free(inxname);
        inxname=NULL;
      }
      
      if (fitname !=NULL) {
	free(fitname);
        fitname=NULL;
      }
      exit(0); 
      break;
    case TASK_RESET:
      ErrLog(tsk,taskname,"Reset");
      
      if (inxname !=NULL) {
        status=((inxfp=fopen(inxname,"r+")) ==NULL);
        if (status==0) status=OldFitInxFclose(inxfp,&prm,irec-1);            
        if (inxfp !=NULL) fclose(inxfp);
        free(inxname);
        inxname=NULL;
      }
      
      if (fitname !=NULL) {
	free(fitname);
        fitname=NULL;
      }
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
      if ((fitname==NULL) && (flg !=0)) {
        flg=0;
        if ((dnum>0) && (dmsg[0].prm !=NULL)) {
          
          fitname=FIOMakeFile(getenv("SD_FIT_PATH"),
			       dmsg[0].prm->time.yr,
                               dmsg[0].prm->time.mo,
                               dmsg[0].prm->time.dy,
                               dmsg[0].prm->time.hr,
                               dmsg[0].prm->time.mt,
                               dmsg[0].prm->time.sc,
			       RadarGetCode(network,dmsg[0].prm->stid,0),
			       "fit",0,0); 
          if (fitname==NULL) {
            ErrLog(tsk,taskname,"Could not create file.\n");
            continue;
	  } 
          inxname=FIOMakeFile(getenv("SD_FIT_PATH"),
			       dmsg[0].prm->time.yr,
                               dmsg[0].prm->time.mo,
                               dmsg[0].prm->time.dy,
                               dmsg[0].prm->time.hr,
                               dmsg[0].prm->time.mt,
                               dmsg[0].prm->time.sc,
			       RadarGetCode(network,dmsg[0].prm->stid,0),
			       "inx",0,0); 
          if (inxname==NULL) ErrLog(tsk,taskname,"Could not index file.");
	}

        fitfp=fopen(fitname,"w");
        if (fitfp==NULL) ErrLog(tsk,taskname,"Error opening file.");
        else {
          status=OldFitHeaderFwrite(fitfp,cbufadr,"fitacf",vstr);
          if (status==-1) ErrLog(tsk,taskname,"Error writing header.");
          fclose(fitfp);       
	}
        inxfp=fopen(inxname,"w");
        if (inxfp==NULL) ErrLog(tsk,taskname,"Error opening index.");
        else {
          status=OldFitInxHeaderFwrite(inxfp,dmsg[0].prm);
          if (status==-1) ErrLog(tsk,taskname,"Error writing index header.");
          fclose(inxfp);
	}       
        frec=2;
        irec=1;
      }
      if (fitname !=NULL) {
        fitfp=fopen(fitname,"a");
        if (inxname !=NULL) {
          inxfp=fopen(inxname,"a"); 
          if (inxfp==NULL) ErrLog(tsk,taskname,"Index not open.");
	}
        if (fitfp==NULL)  
           ErrLog(tsk,taskname,"File not open.");
	else {
          for (dptr=0;dptr<dnum;dptr++) {
             memcpy(&prm,dmsg[dptr].prm,sizeof(struct RadarParm));

             /* build output table here */
             for (l=0;l<prm.nrang;l++) {
             if (prm.noise.search>0) 
               snr[l]=dmsg[dptr].fdt->rng[l].p_0/prm.noise.search;
               else snr[l]=0;
             }
             if (prm.nrang>ORIG_MAX_RANGE) {
                step=prm.nrang/ORIG_MAX_RANGE; 
                for (l=0;l<ORIG_MAX_RANGE;l++) {
                  maxval=0.0;
                  inx=l*step;
                  for (s=0;s<step;s++) {
		    /*
                    if (fdt[dptr].fdt->rng[l*step+s].qflg==0) continue;
                    if (fdt[dptr].fdt->rng[l*step+s].gsct !=0) continue;
		    */
                    if (snr[l*step+s]>maxval) { 
                      maxval=snr[l*step+s];
                      inx=l*step+s;
                    }
		  }
                  rtab[l]=inx;
		}
                dmsg[dptr].prm->rsep=dmsg[dptr].prm->rsep*step;
                fnum=OldFitFwrite(fitfp,dmsg[dptr].prm,
                              dmsg[dptr].fdt,rtab);  
	     } else fnum=OldFitFwrite(fitfp,dmsg[dptr].prm,
                              dmsg[dptr].fdt,NULL);   
 
            if (dnum==-1) break;
            if (inxfp !=NULL) 
	      OldFitInxFwrite(inxfp,frec,dnum,dmsg[dptr].prm);
         
            frec+=fnum;
            irec++;
	  }
          if (dnum==-1) ErrLog(tsk,taskname,"Error writing record");
          fclose(fitfp);
          if (inxfp !=NULL) fclose(inxfp);
	}
      }
      if (store !=NULL) free(store);
      store=NULL;
    }  
  }

} 
