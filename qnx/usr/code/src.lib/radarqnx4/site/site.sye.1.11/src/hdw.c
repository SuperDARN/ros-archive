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
 Revision 1.7  2008/03/21 16:32:52  code
 Added support for I&Q sample capture.

 Revision 1.6  2007/02/08 16:42:28  code
 Added lagnum.

 Revision 1.5  2006/08/17 20:29:29  code
 Added support for extended lag tables.

 Revision 1.4  2006/04/12 16:41:09  barnes
 Adopted rawfeed.

 Revision 1.3  2006/02/08 15:56:10  barnes
 Modification to lag table.

 Revision 1.2  2004/07/06 17:07:08  barnes
 Passed attenuator stepping to intergration library.

 Revision 1.1  2004/06/15 20:33:18  barnes
 Initial revision

*/


#define ADNAME "dt2828"
#define RAWFEEDNAME "echoraw"
#define DIONAME "/dio_drive"

#define MAXTSG 32

ipcid_t dioid;
ipcid_t adid;
ipcid_t reid;
ipcid_t rawid;

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
  adid=qnx_name_locate(0,ADNAME,0,NULL);
  dioid=qnx_name_locate(0,DIONAME,0,NULL);
  rawid=qnx_name_locate(0,RAWFEEDNAME,0,NULL);

  if ((dioid==-1) || (adid==-1)) {
    fprintf(stderr,"Failed to locate hardware.");
    exit(-1);
  }
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


