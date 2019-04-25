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
#include <sys/proxy.h>
#endif
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
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
#include "fclrsgc214.h"
#include "integrategc214.h"
#include "integratesgc214.h"
#include "tcpipmsg.h"
#include "shmem.h"
#include "global.h"
#include "globals.h"

/*
 $Log: hdw.c,v $
 Revision 1.10  2008/03/21 16:47:04  code
 Added support for I&Q capture.

 Revision 1.9  2008/03/12 20:35:35  code
 Changed the clear frequency functions.

 Revision 1.8  2008/03/10 16:57:46  code
 Removed the extended integration code.

 Revision 1.7  2008/03/07 21:02:56  code
 Upgrades for Stereo and digital receivers.

 Revision 1.6  2007/02/08 16:43:20  code
 Added lagnum.

 Revision 1.5  2006/08/17 20:30:14  code
 Added support for extended lag tables.

 Revision 1.4  2006/04/12 16:42:38  barnes
 Adopted rawfeed.

 Revision 1.3  2006/02/08 15:56:10  barnes
 Modification to lag table.

 Revision 1.2  2004/07/06 17:07:08  barnes
 Passed attenuator stepping to intergration library.

 Revision 1.1  2004/06/15 20:33:18  barnes
 Initial revision

*/



#define DRNAME_A "gc214ts"
#define DRNAME_B "gc214tsB"
#define RAWFEEDNAME_A "echoraw"
#define RAWFEEDNAME_B "echorawB"
#define DIONAME "/dio_drive"

#define MSGSRVNAME "msgserver"

#define UCONT_NAME "ucont_moni"

#define MAXTSG 32

#define REAL_BUF_OFFSET 0
#define IMAG_BUF_OFFSET 1
//#define SAMPLE_DELAY 23	//changed by Todd Parris from 23 to 0
#define SAMPLE_DELAY 0



ipcid_t dioid;
ipcid_t drid[2];
ipcid_t rawid[2];

ipcid_t msgsrvid=-1;


struct TSGtable *tsgtable;

int bufnum[2];
int bufsze[2];
int bufoff=0;
void *bufadr[32];

int badrangeA=MAX_RANGE;
int dobadlagA=1;
int mpincoldA=0;
int frangoldA=0;
int rsepoldA=0;

int badrangeB=MAX_RANGE;
int dobadlagB=1;
int mpincoldB=0;
int frangoldB=0;
int rsepoldB=0;

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
  drid[0]=qnx_name_locate(0,DRNAME_A,0,NULL);
  drid[1]=qnx_name_locate(0,DRNAME_B,0,NULL);
  dioid=qnx_name_locate(0,DIONAME,0,NULL);
  rawid[0]=qnx_name_locate(0,RAWFEEDNAME_A,0,NULL);
  rawid[1]=qnx_name_locate(0,RAWFEEDNAME_B,0,NULL);

  if ((dioid==-1) || (drid[0]==-1)) {
    fprintf(stderr,"Failed to locate hardware.");
    exit(-1);
  }
#endif
  bufnum[0]=GC214GetBufNum(drid[0]);
  bufsze[0]=GC214GetBufSize(drid[0]);

  if (drid[1] !=-1) {
    bufnum[1]=GC214GetBufNum(drid[1]);
    bufsze[1]=GC214GetBufSize(drid[1]);
  } 
  bufoff=0;
  for (n=0;n<bufnum[0];n++) {
    bufadr[n]=GC214GetBufAdr(drid[0],n); 
  }

  
  if (drid[1] !=-1) {
    bufoff=bufnum[0];
    for (n=0;n<bufnum[1];n++) {
       bufadr[bufoff+n]=GC214GetBufAdr(drid[1],n); 
    }
  }  

  DIOReset(dioid);
  tsgtable=TSGMakeTable(MAXTSG);

  samples=(int16 *) 
  ShMemAlloc(sharedmemory,IQBUFSIZE,O_RDWR |O_CREAT,1,&shmemfd);
  samplesA=samples;

  /* Note: The tortured logic below is to make clear
     that we are jumping in bytes into the shared
     memory area - samples is cast to a byte array.
  */


  samplesB=(int16 *) ( (char *) samples+(IQBUFSIZE/2));
  
  return 0;
}

int SiteSetIntt(int sc,int us) {
  IntegrateSGC214SetTock(&tock,sc,us);     
  return 0;
}

int SiteSetChannel(int channel) {
  return DIOSetChannel(dioid,channel);
}

int SiteSetBeam(int bmnum) {
  return DIOSetBeam(dioid,bmnum);
}



int SiteFCLR(int sfrq,int efrq) { 
  int flg;

  tfreq=FCLRGC214(dioid,drid[0],ftable,sfrq,efrq,rsep,50625000,REAL_BUF_OFFSET,
                  IMAG_BUF_OFFSET,bufadr,&noise,noisestat,
                  &flg,SiteFCLRMonitor,NULL);

  return tfreq;

}

int SiteFCLRS(int sfrqA,int efrqA,int sfrqB,int efrqB) { 

  int flgA,flgB,s;
   
  s=FCLRSGC214(dioid,drid,ftable,
         sfrqA,efrqA,rsepA,sfrqB,efrqB,rsepB, 
         50625000,REAL_BUF_OFFSET,IMAG_BUF_OFFSET, 
         bufadr,bufoff,
	     &tfreqA,&noiseA,noisestatA,&flgA,NULL,NULL,
	     &tfreqB,&noiseB,noisestatB,&flgB,NULL,NULL);
 
 
  return s;
}


int SiteSetFreq(int tfreq) {
  int status;
  if (DIOSetFreq(dioid,tfreq)==FREQ_LOCAL) return FREQ_LOCAL;
  if ( (status=GC214SetTxFreq(drid[0],tfreq)) != 0) return status;

  return 0;
}

int SiteSetFreqS(int tfreqA,int tfreqB) {
  int status;
  if ((status=DIOSetChannel(dioid,0)) !=0) return status;
  if (DIOSetFreq(dioid,tfreqA)==FREQ_LOCAL) return FREQ_LOCAL;
  if ( (status=GC214SetTxFreq(drid[0],tfreqA)) != 0) return status;
  if ((status=DIOSetChannel(dioid,1)) !=0) return status;
  if (DIOSetFreq(dioid,tfreqB)==FREQ_LOCAL) return FREQ_LOCAL;
  if ( (status=GC214SetTxFreq(drid[1],tfreqB)) != 0) return status;


  return 0;
}




int SiteIntegrate(int (*lags)[2]) {

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
      status=IntegrateGC214(&tsgtable->buf[tsgid],tsgid,mplgs,lagnum,
                  lags,&tock,
		  badrange,bmnum,tfreq,dioid,drid[0],rawid[0],maxatten,
                  REAL_BUF_OFFSET,IMAG_BUF_OFFSET,
                  bufadr,&noise,pwr0,acfd,xcfd,&flg,noisefac,noiselim,
                  &rxchn,&smpnum,&skpnum,
                  seqtval,seqatten,seqnoise,seqoff,seqsze,
		  samples,
                  NULL,NULL);
  else status=IntegrateGC214(&tsgtable->buf[tsgid],tsgid,mplgs,lagnum,
                       lags,&tock,
                       badrange,bmnum,tfreq,dioid,drid[0],rawid[0],maxatten,
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


int SiteIntegrateS(int (*lagsA)[2],int (*lagsB)[2]) {

  int flgA,flgB;
  int status;

  if ((frangA !=frangoldA) || (rsepA !=rsepoldA) || (mpincA !=mpincoldA)) {
    dobadlagA=1;
    frangoldA=frangB;
    rsepoldA=rsepB;
    mpincoldA=mpincB;
  }
  if (dobadlagA==1) {
   badrangeA=ACFBadLagZero(&tsgtable->buf[tsgidA],mplgsA,lagsA);
   dobadlagA=0;
  }

  if ((frangB !=frangoldB) || (rsepB !=rsepoldB) || (mpincB !=mpincoldB)) {
    dobadlagB=1;
    frangoldB=frangB;
    rsepoldB=rsepB;
    mpincoldB=mpincB;
  }
  if (dobadlagB==1) {
   badrangeB=ACFBadLagZero(&tsgtable->buf[tsgidB],mplgsB,lagsB);
   dobadlagB=0;
  }

 if (xcfA==1) 
      status=IntegrateSGC214(&tock,delays,dioid,drid,rawid,
                        bufadr,bufoff,noisefac,noiselim,
                        REAL_BUF_OFFSET,IMAG_BUF_OFFSET,
                        &tsgtable->buf[tsgidA],tsgidA,mplgsA,lagnumA,lagsA,
		                badrangeA,bmnumA,tfreqA,
			            maxattenA,mxpwrA,&noiseA,
		                pwr0A,acfdA,xcfdA,&flgA,&naveA,
	                &rxchnA,&smpnumA,&skpnumA,
                        seqtvalA,seqattenA,seqnoiseA,seqoffA,
                        seqszeA,
		        samplesA,
                        &tsgtable->buf[tsgidB],tsgidB,mplgsB,lagnumB,lagsB,
		                badrangeB,bmnumB,tfreqB,
   			            maxattenB,mxpwrB,&noiseB,
		                pwr0B,acfdB,xcfdB,&flgB,&naveB,
	                &rxchnB,&smpnumB,&skpnumB,
                        seqtvalB,seqattenB,seqnoiseB,seqoffB,
                        seqszeB,
		        samplesB,
                                NULL,NULL,NULL,NULL);


  else       status=IntegrateSGC214(&tock,delays,dioid,drid,rawid,
                        bufadr,bufoff,noisefac,noiselim,
                        REAL_BUF_OFFSET,IMAG_BUF_OFFSET,
                        &tsgtable->buf[tsgidA],tsgidA,mplgsA,lagnumA,lagsA,
		                badrangeA,bmnumA,tfreqA,
  			            maxattenA,mxpwrA,&noiseA,
		                pwr0A,acfdA,NULL,&flgA,&naveA,
	                &rxchnA,&smpnumA,&skpnumA,
                        seqtvalA,seqattenA,seqnoiseA,seqoffA,seqszeA,
		        samplesA,
                        &tsgtable->buf[tsgidB],tsgidB,mplgsB,lagnumB,lagsB,
		                badrangeB,bmnumB,tfreqB,
			            maxattenB,mxpwrB,&noiseB,
		                pwr0B,acfdB,NULL,&flgB,&naveB,
	                &rxchnB,&smpnumB,&skpnumB,
                        seqtvalB,seqattenB,seqnoiseB,seqoffB,
                        seqszeB,
		        samplesB,
                                NULL,NULL,NULL,NULL); 

  DIOSetChannel(dioid,0);
  DIOGetStatus(dioid,0,0,&lopwrstatA,&agcstatA);
  DIOSetChannel(dioid,1);
  DIOGetStatus(dioid,0,0,&lopwrstatB,&agcstatB);
  
  return status; 
}


pid_t SiteInitProxy(char *name){

	pid_t proxy_pid;
    pid_t ucont_proxy;  
    if (name==NULL) proxy_pid = qnx_name_locate(0,UCONT_NAME,0,NULL); 
    else proxy_pid=qnx_name_locate(0,name,0,NULL);
    if (proxy_pid==-1) return -1;

   	ucont_proxy = qnx_proxy_attach(proxy_pid,
                  "interp",strlen("interp"),-1); 
    return ucont_proxy;
}	




