/* integrategc214.c
   ================
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
#include <sys/types.h>
#include <time.h>
#include "rtypes.h"
#include "gc214.h"
#include "dio.h"
#include "limit.h"
#include "tsg.h"
#include "acf.h"
#include "monitor.h"
#include "integrategc214.h"


#include "pulse.h"

/*
 $Log: integrategc214.c,v $
 Revision 1.13  2006/02/07 19:48:51  barnes
 Simon Shepherd's lag table modification.

 Revision 1.12  2006/02/07 17:41:34  barnes
 Fixed missing tests of error conditions.

 Revision 1.11  2006/02/07 17:02:54  barnes
 Dieter Andre's modification to return error codes to the control program
 and his bug fix for calculating the lag zero power.

 Revision 1.10  2005/07/26 19:53:12  barnes
 Fixed XCF calculation.

 Revision 1.9  2005/07/19 18:19:17  barnes
 Modification to move the sample delay and I & Q sample ordering out
 of the library.

 Revision 1.8  2004/05/11 17:31:16  barnes
 Isolated platform specific code.

 Revision 1.7  2004/05/11 15:29:56  barnes
 Changed buffer type.

 Revision 1.6  2004/05/05 01:22:09  barnes
 Removed redundant code.

 Revision 1.5  2004/05/04 18:54:08  barnes
 Renamed the type header to avoid confusion.

 Revision 1.4  2004/05/03 20:07:07  barnes
 Changed type ipcid to ipcid_t.

 Revision 1.3  2004/05/03 18:02:02  barnes
 Reordered headers.

 Revision 1.2  2004/05/03 15:30:09  barnes
 Added ipcid type for the IPC calls.

 Revision 1.1  2004/03/13 19:54:01  barnes
 Initial revision

*/

int IntegrateGC214SetTock(struct timespec *tock,int sc,int us) {
#ifdef _QNX4
  if (tock==NULL) return -1;
  if (clock_gettime(CLOCK_REALTIME,tock)==-1) return -1;
  tock->tv_sec+=sc;
  tock->tv_nsec+=us*1000;
#endif
  return 0;
}
   
int IntegrateGC214(struct TSGprm *prm,int tsgid,int mplgs,int (*lags)[2],
                   struct timespec *tock,int badrng,
		   ipcid_t dioid,ipcid_t drid,ipcid_t monid,int maxatten,
                   int smpdelay,int roff,int ioff,
		   void **bufadr,float noise,float *pwr0,
                   float *acfd,float *xcfd,int *flg,
	           int thr,int lmt,
		   void (*user)(struct TSGprm *prm,
			        struct timespec *tick,struct timespec *tock,
			        int nave,
		                unsigned char *buffer,int bufnum,
			        int numwords,int numchannel,
			        void *usrdata),void *usrdata) {
 
  struct timespec tick;
  int abflg;
  int nave=0;
  int i,j; 
  int abuf,buf;
 
  int nchannel;
  int nsample;
 
  int smpcnt=0;
  int status=0;
  int srate=3333;
  int s;
    
  for (i=0;i<MAX_RANGE;i++) {
     pwr0[i]=0;
     for (j=0;j<LAG_SIZE*2;j++) {
       acfd[i*LAG_SIZE*2+j]=0;
       if (xcfd !=NULL) xcfd[i*LAG_SIZE*2+j]=0;
     }
  }
  

  if (xcfd !=NULL) nchannel = 2;
  else nchannel = 1;

  buf=0;
  *flg=0;

  nsample=prm->samples;
  nsample+=prm->smdelay-1;

  srate=300000/(2*prm->rsep); /* 1/smsep = sample frequency */

  DIOSetGain(dioid,0);

  if ((s=GC214SetSRate(drid,srate)) !=0) { 
    DIOSetGain(dioid,maxatten);
    return (-100+s);
  }
  
  if ((s=IntegrateGC214Pulse(buf,nsample+smpdelay,
                          nchannel,tsgid,dioid,drid)) != 0) {
    *flg|=INT_PULSE_FAIL;
    DIOSetGain(dioid,maxatten);
    return (-200+s);
  }
#ifdef _QNX4 
  clock_gettime(CLOCK_REALTIME,&tick);
#endif  
  while (1) {
      if (tick.tv_sec>tock->tv_sec) break;
      if ((tick.tv_sec==tock->tv_sec) && (tick.tv_nsec>tock->tv_nsec)) break;
      abuf = buf;
      buf = (buf + 1) % 2 ;

      status=GC214GetScanStatus(drid)==SCAN_OK;
      if (status==0) smpcnt++;
      if (status==1) MonitorScan(monid,abuf,nsample+smpdelay,nchannel);
     
      if ((s=IntegrateGC214Pulse(buf,nsample+smpdelay,
                              nchannel,tsgid,dioid,drid)) !=0)  {
         *flg|=INT_PULSE_FAIL;  
         DIOSetGain(dioid,maxatten);
         return (-300+s);
      }
      if (status==1) {
     
     
        /* call the user function */
        if (user !=NULL) (user)(prm,&tick,tock,nave,
		                bufadr[abuf],abuf,nsample+smpdelay,
                                nchannel,usrdata);

        ACFSumPower(prm,mplgs,lags,pwr0,
	            (int16 *) bufadr[abuf],2,1,smpdelay,roff,
		    ioff,badrng,noise,MAXPWR,0.0,
                    thr,lmt,&abflg);

        if (abflg==0) {

          nave++;

          ACFCalculate(prm,(int16 *) bufadr[abuf],2,1,smpdelay,
                       roff,ioff,mplgs,
	  	       lags,acfd,ACF_PART,nsample+smpdelay,badrng,0.0,NULL);
          if (xcfd !=NULL) ACFCalculate(prm,(int16 *) bufadr[abuf],2,1,
				        smpdelay,roff,
				        ioff,mplgs,
		                        lags,xcfd,XCF_PART,nsample+smpdelay,
                                        badrng,0.0,NULL);
     	 
	}
      }
#ifdef _QNX4
    clock_gettime(CLOCK_REALTIME,&tick);
#endif
  }
  status=GC214GetScanStatus(drid)==SCAN_OK;
  if (status==1) MonitorScan(monid,buf,nsample+smpdelay,nchannel);

  if (status==0) smpcnt++;
  if (status==1) { 
    if (user !=NULL) (user)(prm,&tick,tock,nave,
		            bufadr[buf],buf,nsample+smpdelay,
                            nchannel,usrdata);
    ACFSumPower(prm,mplgs,lags,pwr0,
	        (int16 *) bufadr[buf],2,1,smpdelay,roff,
		ioff,badrng,noise,MAXPWR,0.0,
                thr,lmt,&abflg);
    if (abflg==0) {
      ACFCalculate(prm,(int16 *) bufadr[buf],2,1,smpdelay,roff,
		   ioff,mplgs,
 	           lags,acfd,ACF_PART,nsample+smpdelay,badrng,0.0,NULL);
      if (xcfd !=NULL) ACFCalculate(prm,(int16 *) bufadr[buf],2,1,smpdelay,
				    roff,ioff,mplgs,
			            lags,xcfd,XCF_PART,nsample+smpdelay,
                                    badrng,0.0,NULL);
      nave++;
    }
  }
  if (smpcnt>MAX_SAMPLE_ERR) *flg|=INT_SAMPLE_FAIL;
  MonitorTrigger(monid);
  if (nave>0) ACFAverage(pwr0,acfd,xcfd,nave,prm->nrang,mplgs);
  DIOSetGain(dioid,maxatten);
  return nave;
} 





