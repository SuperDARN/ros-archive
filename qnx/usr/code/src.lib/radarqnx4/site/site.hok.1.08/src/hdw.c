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
#include "integrate.h"
#include "shmem.h"
#include "global.h"


/*
 $Log: hdw.c,v $
 Revision 1.7  2008/03/21 16:14:04  code
 Added I&Q sample capture.

 Revision 1.6  2007/02/08 16:31:03  code
 Added lagnum.

 Revision 1.5  2006/09/12 14:31:18  code
 Modification to implement the micro-controller code.

 Revision 1.4  2006/08/17 20:13:37  code
 Added support for extended lag tables.

 Revision 1.3  2006/08/16 15:45:18  code
 Added rtoxmin.

 Revision 1.2  2006/06/14 13:43:27  code
 Corrected name of A/D driver.

 Revision 1.1  2006/06/14 13:27:00  code
 Initial revision

*/


#define ADNAME "a_d_drive"
#define RAWFEEDNAME "echoraw"
#define DIONAME "/dio_drive"

#define UCONT_NAME "ucont_moni"


#define MAXTSG 32

ipcid_t dioid;
ipcid_t adid;
ipcid_t reid;
ipcid_t rawid;

ipcid_t ucont_proxy=-1;  


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

int fclrid[2];

int SiteSetupHardware() {
  int n;
#ifdef _QNX4 
  ipcid_t proxy_pid;

  adid=qnx_name_locate(0,ADNAME,0,NULL);
  dioid=qnx_name_locate(0,DIONAME,0,NULL);
  rawid=qnx_name_locate(0,RAWFEEDNAME,0,NULL);

  if ((dioid==-1) || (adid==-1)) {
    fprintf(stderr,"Failed to locate hardware.");
    exit(-1);
  }



  proxy_pid = qnx_name_locate(0,UCONT_NAME,0,NULL); 
   
  if (proxy_pid !=-1) ucont_proxy = qnx_proxy_attach(proxy_pid,
                                    "interp",strlen("interp"),-1); 


#endif
  bufnum=ADGetBufNum(adid);
  bufsze=ADGetBufSize(adid);

  for (n=0;n<bufnum;n++) {
    bufadr[n]=ADGetBufAdr(adid,n); 
  }
  reid=ADGetScanReset(adid); 
  

  DIOReset(dioid);
  tsgtable=TSGMakeTable(MAXTSG);

  samples=(int16 *) 
          ShMemAlloc(sharedmemory,IQBUFSIZE,O_RDWR |O_CREAT,1,&shmemfd);

  FCLRSet(dioid,tsgtable,fclrid);

  return 0;
}

int SiteSetIntt(int sc,int us) {
  IntegrateSetTock(&tock,sc,us);     
  return 0;
}

int SiteSetBeam(int bmnum) {
  return DIOSetBeam(dioid,bmnum);
}

int SiteFCLR(int sfrq,int efrq) { 
  int flg;
  
 
  tfreq=FCLR(dioid,adid,reid,tsgtable,fclrid,ftable,
             sfrq,efrq,5,bufadr,bufsze,mxpwr,maxatten,&atten,
	     &noise,noisestat,
             NULL,&flg);
  return tfreq;

}

int SiteSetFreq(int tfreq) {
  if (DIOSetFreq(dioid,tfreq)==FREQ_LOCAL) return FREQ_LOCAL;
  return 0;
}

int SiteIntegrate(int (*lags)[2]) {

  int flg;
  int status;

  if (ucont_proxy !=-1)  Trigger(ucont_proxy);

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
		  badrange,bmnum,tfreq,dioid,adid,rawid,reid,
		  maxatten,mxpwr,site->atten,bufadr,&atten,&noise,
		  pwr0,acfd,xcfd,&flg,noisefac,noiselim,
                  NULL,
		  &rxchn,&smpnum,&skpnum,
                  seqtval,seqatten,seqnoise,seqoff,seqsze,
		  samples,
                 NULL,NULL);
   else status=Integrate(&tsgtable->buf[tsgid],tsgid,mplgs,lagnum,lags,&tock,
		  badrange,bmnum,tfreq,dioid,adid,rawid,reid,
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


