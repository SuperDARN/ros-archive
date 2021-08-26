/* raw_write.c
   ===========
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
#include "rawdata.h"
#include "radar.h"
#include "rmsg.h"
#include "rmsgrcv.h"
#include "fio.h"

#include "oldrawwrite.h"


#include "version.h"
#include "taskname.h"
#include "hlpstr.h"

/*
 $Log: raw_write.c,v $
 Revision 6.24  2004/06/04 14:53:44  barnes
 Adopted text strings as radar identifiers.

 Revision 6.23  2004/05/13 22:23:21  barnes
 Added a reply to the quit message.

 Revision 6.22  2004/05/11 14:50:27  barnes
 Changed version handling.

 Revision 6.21  2004/05/10 15:17:58  barnes
 Modification to deal with new libraries.

 Revision 6.20  2001/06/27 20:56:19  barnes
 Added license tag

 Revision 6.19  2001/01/29 18:12:08  barnes
 Added Author Name

 Revision 6.18  2000/09/18 18:21:47  barnes
 Added missing header.

 Revision 6.17  1999/04/10 20:49:26  barnes
 Added help message system.

 Revision 6.16  1999/03/30 19:23:41  barnes
 Moved task names into header.

 Revision 6.15  1999/03/19 18:37:57  barnes
 Modifications to support the new file libraries.

 Revision 6.14  1998/11/09 23:04:23  barnes
 Incorporated rawdata header.

 Revision 6.13  1998/09/08 14:38:23  barnes
 Fixed problem with a change in the environment variable that locates
 the directory in which files are written.

 Revision 6.12  1998/09/08 14:30:33  root
 Modification to allow the threshold value to be recorded in the file
 header.

 Revision 6.11  1998/06/19 14:48:36  root
 Added support for the new radar_name function.

 Revision 6.10  1998/06/11 15:46:44  root
 Changed the log message when the task is reset.

 Revision 6.9  1998/06/11 15:44:12  root
 Added support for the TASK_RESET message.

 Revision 6.8  1998/06/11 14:54:38  root
 Moved usage into a different file.

 Revision 6.7  1998/06/04 18:58:11  root
 Modification to stop the raw file being opened and closed every time
 a data block is processed.

 Revision 6.6  1998/06/03 20:19:00  root
 Modification to the revision control system.

 Revision 6.5  1998/05/29 13:33:50  root
 Fixed incorrect text message on termination.

 Revision 6.4  1998/05/29 11:58:28  root
 Added code to shut down properly on receipt of the TASK_QUIT message.

 Revision 6.3  1998/05/29 11:24:45  root
 modified calling sequence of write_raw.c

 Revision 6.2  1998/05/29 10:05:09  root
 Removed redundant line of code.

 Revision 6.1  1998/05/28 18:35:54  root
 New version of the task that uses multi-plexed data messages.

 Revision 5.7  1998/05/27 19:19:38  root
 Modifications for operation with the new libraries.

 Revision 5.6  1997/05/02 18:22:55  root
 Fixed memory leakage problem and bug in skipping data writes.

 Revision 5.5  1997/03/26 15:22:14  root
 Corrected bug that prevented version number being recorded in the file.

 * Revision 5.4  1997/03/26  12:50:43  root
 * Fixed bug that left files open if a beam was skipped.
 *
 * Revision 5.3  1997/03/26  12:44:19  root
 * Added flag to skip write operations.
 *
 * Revision 5.2  1997/03/25  18:24:57  root
 * Added record of version number in the error log.
 *
 * Revision 5.1  1997/03/25  18:20:54  root
 * New version using the file_io library.
 *
 * Revision 1.1  1997/03/25  18:19:19  root
 * Initial revision
 *
 */


#define ORIG_MAX_RANGE 75

struct RadarNetwork *network;  

struct RMsgBlock rblk;
char *store=NULL; 

struct DMsg {
  int tag;
  struct RadarParm *prm;
  struct RawData *rdt;
};

int dnum=0;
int dptr=0;
struct DMsg dmsg[32];

int arg=0;
struct OptionData opt;

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

  int recnum;
  int help=0;
 
  char errbuf[256];
  char taskname[32]={TASK_NAME};
  char errlogd[32]={ERRLOG};
  char *errlog=NULL;
  struct TaskID *tsk=NULL;
  int thresh=0;

  int rtab[ORIG_MAX_RANGE];
  float snr[MAX_RANGE];
  int inx,l,s,step;
  float maxval;  

  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"e",'t',&errlog);
  OptionAdd(&opt,"t",'i',&thresh);
 
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

  if ((rawid=qnx_name_attach(0,taskname) ) == -1) { 
    perror("Unable to attach raw_write task");
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
      if (filename !=NULL) free(filename);
      filename=NULL;
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
            dmsg[dptr].rdt=NULL;
            dnum++;
	  }
          switch (rblk.data[i].type) {
	  case PRM_TYPE:
            dmsg[dptr].prm=(struct RadarParm *) 
                (rblk.ptr[rblk.data[i].index]);
	    break;
          case RAW_TYPE:
            dmsg[dptr].rdt=(struct RawData *) 
                (rblk.ptr[rblk.data[i].index]);
	    break;
          default:
            break;
	  }
      }
      if ((filename==NULL) && (flg !=0)) {

        flg=0;
        recnum=0;
        if ((dnum>0) && (dmsg[0].prm !=NULL)) {
         
          filename=FIOMakeFile(getenv("SD_RAW_PATH"),
			       dmsg[0].prm->time.yr,
                               dmsg[0].prm->time.mo,
                               dmsg[0].prm->time.dy,
                               dmsg[0].prm->time.hr,
                               dmsg[0].prm->time.mt,
                               dmsg[0].prm->time.sc,
			       RadarGetCode(network,dmsg[0].prm->stid,0),
			       "dat",0,0);
	}
        if (filename==NULL) {
	  ErrLog(tsk,taskname,"Could not create file.");
          continue;
	} 
        fp=fopen(filename,"w");
        if (fp==NULL) ErrLog(tsk,taskname,"Error opening file.");
        else {
          char vstr[64];
          sprintf(vstr,"%d.%.2d",MAJOR_VERSION,MINOR_VERSION);
          status=OldRawHeaderFwrite(fp,"rawwrite",
                                    vstr,thresh,cbufadr);
          if (status==-1) ErrLog(tsk,taskname,"Error writing header.");
          fclose(fp);
	}  
        if (cbufadr !=NULL) free(cbufadr);
        cbufadr=NULL;     
      }
      if (filename !=NULL) {
        fp=fopen(filename,"a");
        if (fp==NULL) ErrLog(tsk,taskname,"Error opening file.");
	else {
          for (dptr=0;dptr<dnum;dptr++) {
          recnum++;


          for (l=0;l<dmsg[dptr].prm->nrang;l++) {
            if (dmsg[dptr].prm->noise.search>0) 
              snr[l]=dmsg[dptr].rdt->pwr0[l]/dmsg[dptr].prm->noise.search;
              else snr[l]=0;
            }
            if (dmsg[dptr].prm->nrang>ORIG_MAX_RANGE) {
              step=dmsg[dptr].prm->nrang/ORIG_MAX_RANGE; 
              for (l=0;l<ORIG_MAX_RANGE;l++) {
                maxval=0.0;
                inx=l*step;
                for (s=0;s<step;s++) {
                  if (snr[l*step+s]>maxval) { 
                    maxval=snr[l*step+s];
                    inx=l*step+s;
                  }
		}
                rtab[l]=inx;
	      }
              dmsg[dptr].prm->rsep=dmsg[dptr].prm->rsep*step;
              status=OldRawFwrite(fp,"rawwrite",dmsg[dptr].prm,
                                  dmsg[dptr].rdt,recnum,rtab);  
	    } else status=OldRawFwrite(fp,"rawwrite",dmsg[dptr].prm,
                              dmsg[dptr].rdt,recnum,NULL);  
          if (status !=0) break;
	  }
          if (status !=0) ErrLog(tsk,taskname,"Error writing record.");
          fclose(fp);
	}
      }
      if (store !=NULL) free(store);
      store=NULL;
    }  
  }
}
