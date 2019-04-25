/* hdw.c
   ===== */

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System - Site Files" (hereinafter "the Program").
 
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

#include <stdlib.h>
#include <stdio.h>

#ifdef _QNX4
#include <sys/kernel.h>
#include <sys/name.h>
#endif

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include "rmath.h"
#include "rtypes.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"
#include "taskid.h"
#include "rmsg.h"
#include "radarshell.h"
#include "rmsgsnd.h"
#include "tsg.h"
#include "tsgtable.h"
#include "maketsg.h"
#include "acf.h"
#include "dio.h"
#include "gc214.h"
#include "freq.h"
#include "fclrgc214.h"
#include "integrategc214.h"
#include "integrategc214ex.h"
#include "tcpipmsg.h"
#include "shmem.h"
#include "global.h"


/*
 $Log: hdw.c,v $
 Revision 1.26  2008/03/15 00:00:44  code
 Implemented shared memory for IQ sampling.

 Revision 1.25  2008/03/14 19:21:18  code
 Removed the now redundant passing of usrdata for extended integration.

 Revision 1.24  2008/03/14 19:18:41  code
 Modified the extended integration library to pass back I&Q samples.

 Revision 1.23  2008/03/14 17:40:25  code
 Added skpnum and temporary allocation of the IQ buffer.

 Revision 1.22  2008/03/14 13:55:27  code
 Added support for capturing I&Q samples.

 Revision 1.21  2008/03/12 20:45:10  code
 Changed SAMPLEDELAY to zero as this is now calculated in the driver.

 Revision 1.20  2007/07/09 20:04:44  code
 Passed through user variable pointer.

 Revision 1.19  2007/07/09 19:58:49  code
 Added a user parameter to SitIntegrateEx

 Revision 1.18  2007/02/28 21:36:29  code
 Fixed missing header.

 Revision 1.17  2007/02/22 15:50:28  code
 Added extended ACF calculation

 Revision 1.16  2006/08/18 14:25:38  code
 Removed tablen and replaced it with lagnum from the ops library.

 Revision 1.15  2006/08/17 19:51:17  code
 Added support for extended lag tables.

 Revision 1.14  2006/08/17 19:46:35  code
 Corrected code for extended lag tables.

 Revision 1.13  2006/05/04 19:21:20  barnes
 Fixed function call.

 Revision 1.12  2006/05/04 19:18:01  barnes
 Modification to deal with lag tables containing multiple instances of the same lag.

 Revision 1.11  2006/04/07 18:46:18  barnes
 Fixed error in variable name.

 Revision 1.10  2006/04/07 18:38:08  barnes
 Incorporated change to 24 beams.

 Revision 1.9  2006/02/07 20:53:40  barnes
 Simon Shepherd's modification to the lag table.

 Revision 1.8  2006/01/25 18:47:29  barnes
 Rotated array left by three beam widths.

 Revision 1.7  2006/01/05 18:57:22  barnes
 Modification to correct phase determination.

 Revision 1.6  2005/07/14 19:33:23  barnes
 Moved buffer offsets and the like to this library.

 Revision 1.5  2005/06/17 14:40:59  barnes
 Incorporated changes to the phasing and the passing of the clock.

 Revision 1.4  2005/03/03 15:35:06  barnes
 Added missing header.

 Revision 1.3  2005/03/03 15:33:21  barnes
 Added digital phasing.

 Revision 1.2  2004/12/01 14:35:26  barnes
 Added phase coding.

 Revision 1.1  2004/11/09 17:48:21  barnes
 Initial revision

*/


#define DRNAME "gc214"
#define RAWFEEDNAME "echoraw"
#define DIONAME "/dio_drive"

#define MSGSRVNAME "msgserver"

#define MAXTSG 32

#define REAL_BUF_OFFSET 0
#define IMAG_BUF_OFFSET 1

ipcid_t dioid;
ipcid_t drid;
ipcid_t rawid;

ipcid_t msgsrvid=-1;


struct TSGtable *tsgtable;

int bufnum;
int bufsze;
void *bufadr[16];

int badrange=MAX_RANGE;
int dobadlag=1;
int mpincold=0;
int frangold=0;
int rsepold=0;
struct timespec tock;

void SiteFCLRMonitor(int start,int end,int rsep,
                     int nn,float *pwr,void *data) {

#ifdef _QNX4
  if (msgsrvid==-1)  msgsrvid=qnx_name_locate(0,MSGSRVNAME,0,NULL);
  if (msgsrvid !=-1) TCPIPMsgSend(msgsrvid,(char *) pwr,sizeof(float)*nn);
#endif
}




int SiteSetupHardware() {
  int n;
#ifdef _QNX4   
  drid=qnx_name_locate(0,DRNAME,0,NULL);
  dioid=qnx_name_locate(0,DIONAME,0,NULL);
  rawid=qnx_name_locate(0,RAWFEEDNAME,0,NULL);

  if ((dioid==-1) || (drid==-1)) {
    fprintf(stderr,"Failed to locate hardware.");
    exit(-1);
  }
#endif
  bufnum=GC214GetBufNum(drid);
  bufsze=GC214GetBufSize(drid);

  for (n=0;n<bufnum;n++) {
    bufadr[n]=GC214GetBufAdr(drid,n); 
  }
  DIOReset(dioid);
  tsgtable=TSGMakeTable(MAXTSG);

  samples=(int16 *) 
          ShMemAlloc(sharedmemory,IQBUFSIZE,O_RDWR |O_CREAT,1,&shmemfd);

  return 0;
}

int SiteSetIntt(int sc,int us) {
  IntegrateGC214SetTock(&tock,sc,us);     
  return 0;
}

int SiteSetBeam(int bmnum) {
  /* insert phase code here 
  return DIOSetBeam(dioid,bmnum);
  */
  return 0;
}

int SiteSetPhase(int bmnum,int tfreq) {
  float thetai;
  float Di;
  float lambda,angle;
  int phi;
  char phic;
  float c=299792458.0;

  thetai=(bmnum-11.5)*3.24;
  lambda=c/(tfreq*1000.0);
  Di=12.80*sin(thetai*PI/180.0);
  angle=(Di/lambda)*360.0;
  phi=256*angle/360.0; 
  phic=-phi;
  return DIOSetPhase(dioid,phic,bmnum);
}







int SiteFCLR(int sfrq,int efrq) { 
  int flg;
  SiteSetPhase(bmnum,(sfrq+efrq)/2);


  tfreq=FCLRGC214(dioid,drid,ftable,sfrq,efrq,rsep,50625000,REAL_BUF_OFFSET,
                  IMAG_BUF_OFFSET,bufadr,&noise,noisestat,
                  &flg,SiteFCLRMonitor,NULL);
  return tfreq;

}

int SiteSetFreq(int tfreq) {
  if (DIOSetFreq(dioid,tfreq)==FREQ_LOCAL) return FREQ_LOCAL;
  return 0;
}

int SiteIntegrate(int (*lags)[2]) {

  int flg;
  int status;

  SiteSetPhase(bmnum,tfreq);

  if ((frang !=frangold) || (rsep !=rsepold) || (mpinc !=mpincold)) {
    dobadlag=1;
    frangold=frang;
    rsepold=rsep;
    mpincold=mpinc;
  }
  if (dobadlag==1) {
   badrange=ACFBadLagZero(&tsgtable->buf[tsgid],mplgs,lags);
   dobadlag=0;
  }

  if (xcf==1) 
      status=IntegrateGC214(&tsgtable->buf[tsgid],tsgid,mplgs,lagnum,
                  lags,&tock,
		  badrange,bmnum,tfreq,dioid,drid,rawid,maxatten,
                  REAL_BUF_OFFSET,IMAG_BUF_OFFSET,
                  bufadr,&noise,pwr0,acfd,xcfd,&flg,noisefac,noiselim,
                  &rxchn,&smpnum,&skpnum,
                  seqtval,seqatten,seqnoise,seqoff,seqsze,
		  samples,
                  NULL,NULL);
  else status=IntegrateGC214(&tsgtable->buf[tsgid],tsgid,mplgs,lagnum,
                       lags,&tock,
                       badrange,bmnum,tfreq,dioid,drid,rawid,maxatten,
		       REAL_BUF_OFFSET,IMAG_BUF_OFFSET,
		       bufadr,&noise,pwr0,acfd,NULL,
                       &flg,noisefac,noiselim,
                       &rxchn,&smpnum,&skpnum,
                       seqtval,seqatten,seqnoise,seqoff,seqsze,
		       samples,
                       NULL,NULL);
  DIOGetStatus(dioid,0,0,&lopwrstat,&agcstat);
  return status; 
}


int SiteIntegrateex(int (*lags)[2]) {

  int flg;
  int status;

  status=IntegrateGC214ex(&tsgtable->buf[tsgid],tsgid,mplgs,lagnum,
		       lags,&tock,
                       badrange,bmnum,tfreq,dioid,drid,rawid,maxatten,
		       REAL_BUF_OFFSET,IMAG_BUF_OFFSET,
		       bufadr,&noise,pwr0,acfd,NULL,
                       &flg,noisefac,noiselim,
                       &rxchn,&smpnum,&skpnum,
                       seqtval,seqatten,seqnoise,seqoff,seqsze,
		       samples,
                       NULL,NULL);


  DIOGetStatus(dioid,0,0,&lopwrstat,&agcstat);
  return status; 
}


