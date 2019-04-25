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
#include <string.h>
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
#include "ad.h"
#include "freq.h"
#include "fclr.h"
#include "fclrs.h"
#include "integrate.h"
#include "integrates.h"
#include "shmem.h"
#include "global.h"
#include "globals.h"


/*
 $Log: hdw.c,v $
 Revision 1.9  2008/03/21 17:47:43  code
 Added missing function.

 Revision 1.8  2008/03/21 17:21:32  code
 Added I&Q capture.

 Revision 1.7  2007/02/08 16:36:49  code
 Added lagnum

 Revision 1.6  2006/08/17 20:24:09  code
 Added support for extended lag tables.

 Revision 1.5  2006/04/07 18:17:50  barnes
 Modified stereo calling sequence.

 Revision 1.4  2006/03/30 11:02:44  barnes
 Modifications for Stereo mode.
 ./

*/


#define ADNAME_A "a_d_drive"
#define ADNAME_B "a_d_drive_B"
#define RAWFEEDNAME_A "echoraw"
#define RAWFEEDNAME_B "echoraw_B"
#define DIONAME "/dio_drive"

#define UCONT_NAME "ucont_moni"

#define MAXTSG 32

ipcid_t dioid;
ipcid_t adid[2];
ipcid_t reid[2];
ipcid_t rawid[2];

struct TSGtable *tsgtable;

int bufnum[2];
int bufsze[2];
int bufoff;
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

int fclrid[2];

int SiteSetupHardware() {
  int n;
#ifdef _QNX4 
  adid[0]=qnx_name_locate(0,ADNAME_A,0,NULL);
  adid[1]=qnx_name_locate(0,ADNAME_B,0,NULL);
  dioid=qnx_name_locate(0,DIONAME,0,NULL);
  rawid[0]=qnx_name_locate(0,RAWFEEDNAME_A,0,NULL);
  rawid[1]=qnx_name_locate(0,RAWFEEDNAME_B,0,NULL);

  if ((dioid==-1) || (adid[0]==-1)) {
    fprintf(stderr,"Failed to locate hardware.");
    exit(-1);
  }
#endif
  bufnum[0]=ADGetBufNum(adid[0]);
  bufsze[0]=ADGetBufSize(adid[0]);

  if (adid[1] !=-1) {
    bufnum[1]=ADGetBufNum(adid[1]);
    bufsze[1]=ADGetBufSize(adid[1]);
  } 
  bufoff=0;
  for (n=0;n<bufnum[0];n++) {
    bufadr[n]=ADGetBufAdr(adid[0],n); 
  }
  reid[0]=ADGetScanReset(adid[0]); 
  
  if (adid[1] !=-1) {
    bufoff=bufnum[0];
    for (n=0;n<bufnum[1];n++) {
       bufadr[bufoff+n]=ADGetBufAdr(adid[1],n); 
    }
    reid[1]=ADGetScanReset(adid[1]); 
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


  FCLRSet(dioid,tsgtable,fclrid);

  return 0;
}

int SiteSetIntt(int sc,int us) {
  IntegrateSetTock(&tock,sc,us);     
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
  
 
  tfreq=FCLR(dioid,adid[0],reid[0],tsgtable,fclrid,ftable,
             sfrq,efrq,5,bufadr,bufsze[0],mxpwr,maxatten,&atten,
	         &noise,noisestat,
             NULL,&flg);
  return tfreq;

}

int SiteFCLRS(int sfrqA,int efrqA,int sfrqB,int efrqB) { 

  int flgA,flgB,s;
   
  s=FCLRS(dioid,adid,reid,tsgtable,fclrid,ftable,delays,
         sfrqA,efrqA,5,sfrqB,efrqB,5,  
         bufadr,bufsze,bufoff,
         mxpwrA,maxattenA,&attenA,
	     &noiseA,noisestatA,
         NULL,&flgA,&tfreqA,
         mxpwrB,maxattenB,&attenB,
	     &noiseB,noisestatB,
         NULL,&flgB,&tfreqB);
  
  return s;
}

int SiteSetFreq(int tfreq) {
  if (DIOSetFreq(dioid,tfreq)==FREQ_LOCAL) return FREQ_LOCAL;
  return 0;
}


int SiteSetFreqS(int tfreqA,int tfreqB) {
  int status;
  if ((status=DIOSetChannel(dioid,0)) !=0) return status;
  if (DIOSetFreq(dioid,tfreqA)==FREQ_LOCAL) return FREQ_LOCAL;
  if ((status=DIOSetChannel(dioid,1)) !=0) return status;
  if (DIOSetFreq(dioid,tfreqB)==FREQ_LOCAL) return FREQ_LOCAL;
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
      status=Integrate(&tsgtable->buf[tsgid],tsgid,mplgs,lagnum,lags,&tock,
		  badrange,bmnum,tfreq,dioid,adid[0],rawid[0],reid[0],
		  maxatten,mxpwr,site->atten,bufadr,&atten,&noise,
		  pwr0,acfd,xcfd,&flg,noisefac,noiselim,
                  NULL,
		  &rxchn,&smpnum,&skpnum,
                  seqtval,seqatten,seqnoise,seqoff,seqsze,
		  samples,

                  NULL,NULL);
  else status=Integrate(&tsgtable->buf[tsgid],tsgid,mplgs,lagnum,lags,&tock,
		  badrange,bmnum,tfreq,dioid,adid[0],rawid[0],reid[0],
		  maxatten,mxpwr,site->atten,bufadr,&atten,&noise,
		  pwr0,acfd,NULL,&flg,noisefac,noiselim,
                  NULL,
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
      status=IntegrateS(&tock,delays,dioid,adid,rawid,reid,
                        bufadr,bufoff,noisefac,noiselim,
                        &tsgtable->buf[tsgidA],tsgidA,mplgsA,lagnumA,lagsA,
		        badrangeA, bmnumA,tfreqA,maxattenA,mxpwrA,site->atten,
                        &attenA,&noiseA,
		        pwr0A,acfdA,xcfdA,&flgA,NULL,&naveA,
	                &rxchnA,&smpnumA,&skpnum,
                        seqtvalA,seqattenA,seqnoiseA,seqoffA,
                        seqszeA,
		        samplesA,
                        &tsgtable->buf[tsgidB],tsgidB,mplgsB,lagnumB,lagsB,
		                badrangeB, bmnumB,tfreqB,
			        maxattenB,mxpwrB,site->atten,
                        &attenB,&noiseB,
		                pwr0B,acfdB,xcfdB,&flgB,NULL,&naveB,
	                &rxchnB,&smpnumB,&skpnum,
                        seqtvalB,seqattenB,seqnoiseB,seqoffB,
                        seqszeB,
		        samplesB,
                                NULL,NULL,NULL,NULL);


  else       status=IntegrateS(&tock,delays,dioid,adid,rawid,reid,
                        bufadr,bufoff,noisefac,noiselim,
                        &tsgtable->buf[tsgidA],tsgidA,mplgsA,lagnumA,lagsA,
		                badrangeA,bmnumA,tfreqA,
                                maxattenA,mxpwrA,site->atten,
                        &attenA,&noiseA,
		                pwr0A,acfdA,NULL,&flgA,NULL,&naveA,

	                &rxchnA,&smpnumA,&skpnum,
                        seqtvalA,seqattenA,seqnoiseA,seqoffA,
                        seqszeA,
		        samplesA,
                        &tsgtable->buf[tsgidB],tsgidB,mplgsB,lagnumB,lagsB,
		                badrangeB,bmnumB,tfreqB,
                                maxattenB,mxpwrB,site->atten,
                        &attenB,&noiseB,
		                pwr0B,acfdB,NULL,&flgB,NULL,&naveB,
	                &rxchnB,&smpnumB,&skpnum,
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













