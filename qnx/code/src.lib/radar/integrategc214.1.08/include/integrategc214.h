/* integrategc214.h
   ================
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

/*
 $Log: integrategc214.h,v $
 Revision 1.1  2004/07/13 22:29:41  barnes
 Initial revision

*/


#ifndef _INTEGRATEGC214_H
#define _INTEGRATEGC214_H

#define REAL_BUF_OFFSET 0
#define IMAG_BUF_OFFSET 1
#define SAMPLE_DELAY 22

#define MAXPWR 1.0e30


#define MAX_SAMPLE_ERR 20

#define INT_PULSE_FAIL 0x01
#define INT_SAMPLE_FAIL 0x04

int IntegrateGC214SetTock(struct timespec *tock,int sc,int us);
int IntegrateGC214(struct TSGprm *prm,int tsgid,int mplgs,int *lags,
                  struct timespec *tock,int badrng,
		  ipcid_t dioid,ipcid_t drid,ipcid_t monid,int maxatten,
		  void **bufadr,float noise,float *pwr0,
                  float *acfd,float *xcfd,int *flg,
	          int thr,int lmt,
		  void (*user)(struct TSGprm *prm,
			       struct timespec *tick,struct timespec *tock,
			       int nave,
		               unsigned char *buffer,int bufnum,
			       int numwords,int numchannel,
			       void *usrdata),void *usrdata);


#endif
