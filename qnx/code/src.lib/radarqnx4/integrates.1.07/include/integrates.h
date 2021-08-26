/* integrates.h
   ============
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

/*
 $Log: integrates.h,v $
 Revision 1.4  2006/08/17 19:23:05  code
 Added support for extended lag tables.

 Revision 1.3  2006/07/24 16:44:40  code
 Passed in beam number and frequency.

 Revision 1.2  2006/04/07 15:03:43  barnes
 Added user functions.

 Revision 1.1  2006/03/29 13:27:01  barnes
 Initial revision

*/


#ifndef _INTEGRATES_H
#define _INTEGRATES_H

int IntegrateS(struct timespec *tock,int *delay,	      
               ipcid_t dioid,ipcid_t *adid,ipcid_t *rawid,
               ipcid_t *resetid,void **bufadr,int bufoff,int thr,int lmt,
               struct TSGprm *prmA,int tsgidA,int mplgsA,int lagnumA,
               int (*lagsA)[2],
 	           int badrngA,int bmnumA,int tfreqA,
	       int maxattenA,float mxpwrA,
               float atstpA,int *attenA,
	           float *noiseA,float *pwr0A,
               float *acfdA,float *xcfdA,int *flgA,
	           float *dcoA,int *naveA,
               struct TSGprm *prmB,int tsgidB,int mplgsB,int lagnumB,
               int (*lagsB)[2],
 	           int badrngB,int bmnumB,int tfreqB,
	       int maxattenB,float mxpwrB,
               float atstpB,int *attenB,
	           float *noiseB,float *pwr0B,
               float *acfdB,float *xcfdB,int *flgB,
	           float *dcoB,int *naveB,
               void (*userA)(struct TSGprm *prm,
			   struct timespec *tick,
			   struct timespec *tock,int nave,int atten,
		           unsigned char *buffer,int bufnum,
			   int sample,int channel,
			   void *usrdataA),void *usrdataA,
               void (*userB)(struct TSGprm *prm,
			   struct timespec *tick,
			   struct timespec *tock,int nave,int atten,
		           unsigned char *buffer,int bufnum,
			   int sample,int channel,
			   void *usrdata),void *usrdataB);
  	         
#endif
