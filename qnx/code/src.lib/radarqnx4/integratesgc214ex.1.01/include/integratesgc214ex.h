/* integratesgc214ex.h
   ===================
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
 $Log: integratesgc214ex.h,v $
 Revision 1.1  2008/03/21 13:54:37  code
 Initial revision

*/


#ifndef _INTEGRATESGC214EX_H
#define _INTEGRATESGC214EX_H

#define MAXPWR 1.0e30
#define MAX_SAMPLE_ERR 20

#define INT_PULSE_FAIL 0x01
#define INT_SAMPLE_FAIL 0x04

int IntegrateSGC214exSetTock(struct timespec *tock,int sc,int us);


int IntegrateSGC214ex(struct timespec *tock,int *delay,
		    ipcid_t dioid,ipcid_t *drid,ipcid_t *rawid,
		    void **bufadr,int bufoff,int thr,int lmt,
		    int roff,int ioff,
		    struct TSGprm *prmA,int tsgidA,int mplgsA,int lagnumA,
		    int (*lagsA)[2],
		    int badrngA,int bmnumA,int tfreqA,
		    int maxattenA,
		    float *noiseA,float *pwr0A,
		    float *acfdA,float *xcfdA,int *flgA,
		    int *naveA,
                    int *rxchnA,
                    int *smpnumA,
                    int *skpnumA,
                    struct timespec *seqtvalA,
                    int *seqattenA,
                    float *seqnoiseA,
                    int *seqoffA,
                    int *seqszeA,
                    int16 *samplesA,
		    struct TSGprm *prmB,int tsgidB,int mplgsB,int lagnumB,
		    int (*lagsB)[2],
		    int badrngB,int bmnumB,int tfreqB,
		    int maxattenB,
		    float *noiseB,float *pwr0B,
		    float *acfdB,float *xcfdB,int *flgB,
		    int *naveB,
                    int *rxchnB,
                    int *smpnumB,
                    int *skpnumB,
                    struct timespec *seqtvalB,
                    int *seqattenB,
                    float *seqnoiseB,
                    int *seqoffB,
                    int *seqszeB,
                    int16 *samplesB,
		    void (*userA)(struct TSGprm *prm,
				  struct timespec *tick,
				  struct timespec *tock,int nave,int atten,
				  float *noise,
				  unsigned char *buffer,int bufnum,
				  int sample,int channel,
				  void *usrdata),void *usrdataA,
		    void (*userB)(struct TSGprm *prm,
				  struct timespec *tick,
				  struct timespec *tock,int nave,int atten,
				  float *noise,
				  unsigned char *buffer,int bufnum,
				  int sample,int channel,
				  void *usrdata),void *usrdataB);

#endif


