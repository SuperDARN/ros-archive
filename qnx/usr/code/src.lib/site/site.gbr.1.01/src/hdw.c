/* hdw.c
   ===== */

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

#include <stdlib.h>
#include <stdio.h>

#ifdef _QNX4
#include <sys/kernel.h>
#include <sys/name.h>
#endif

#include <sys/types.h>
#include <time.h>
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
#include "tcpipmsg.h"
#include "global.h"


/*
 $Log: hdw.c,v $
 Revision 1.1  2004/05/11 15:31:41  barnes
 Initial revision

*/


#define DRNAME "gc214"
#define MONNAME "monitor"
#define DIONAME "/dio_drive"

#define MSGSRVNAME "msgserver"

#define MAXTSG 32

ipcid_t dioid;
ipcid_t drid;
ipcid_t monid;

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
  monid=qnx_name_locate(0,MONNAME,0,NULL);

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
  return 0;
}

int SiteSetIntt(int sc,int us) {
  IntegrateGC214SetTock(&tock,sc,us);     
  return 0;
}

int SiteSetBeam(int bmnum) {
  return DIOSetBeam(dioid,bmnum);
}

int SiteFCLR(int sfrq,int efrq) { 
  int flg;
  tfreq=FCLRGC214(dioid,drid,ftable,sfrq,efrq,rsep,bufadr,&noise,noisestat,
                  &flg,SiteFCLRMonitor,NULL);
  return tfreq;

}

int SiteSetFreq(int tfreq) {
  if (DIOSetFreq(dioid,tfreq)==FREQ_LOCAL) return FREQ_LOCAL;
  return 0;
}

int SiteIntegrate(int *lags) {

  int flg;
  int status;

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
      status=IntegrateGC214(&tsgtable->buf[tsgid],tsgid,mplgs,lags,&tock,
		  badrange,dioid,drid,monid,maxatten,bufadr,noise,
		  pwr0,acfd,xcfd,&flg,noisefac,noiselim,NULL,NULL);
  else status=IntegrateGC214(&tsgtable->buf[tsgid],tsgid,mplgs,lags,&tock,
                       badrange,dioid,drid,monid,maxatten,
		       bufadr,noise,pwr0,acfd,NULL,
                       &flg,noisefac,noiselim,NULL,NULL);
  DIOGetStatus(dioid,0,0,&lopwrstat,&agcstat);
  return status; 
}


