/* globals.h
   ======== 
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

/*
 $Log: globals.h,v $
 Revision 1.2  2008/03/21 16:50:14  code
 Added I&Q capture.

 Revision 1.1  2007/10/29 17:50:25  code
 Initial revision

*/

#ifndef _GLOBALS_H
#define _GLOBALS_H


extern int delays[2];

extern struct RadarParm prmA;
extern struct RadarParm prmB;

extern struct IQData iqA;
extern struct IQData iqB;

extern struct RawData rawA;
extern struct RawData rawB;
extern struct FitData fitA;
extern struct FitData fitB;

extern int maxattenA;
extern int protattenA;
extern int rx_wideA;
extern int rx_narrowA;
extern int rsep_switchA;


extern int tsgidA;
extern int txpowA;
extern int naveA;
extern int attenA;
extern int lagfrA;
extern int smsepA;
extern int ercodA;
extern int agcstatA;
extern int lopwrstatA;
extern int nbaudA;
extern float noiseA;
extern int rxriseA;
extern int txplA;
extern int mpincA;
extern int mppulA;
extern int mplgsA;
extern int nrangA;
extern int frangA;
extern int rsepA;
extern int bmnumA;
extern int xcfA;
extern int tfreqA;
extern int scanA;
extern int mxpwrA;
extern int lvmaxA;
extern int cpA;

extern int lagnumA;

extern float noisestatA[4];
extern char combfA[128];

extern int rxchnA;
extern int smpnumA;
extern int skpnumA;
extern struct timespec seqtvalA[MAXNAVE];
extern int seqattenA[MAXNAVE];
extern float seqnoiseA[MAXNAVE];
extern int seqoffA[MAXNAVE];
extern int seqszeA[MAXNAVE];
extern int16 *samplesA;
extern int IQoffsetA;

extern float pwr0A[MAX_RANGE];
extern float acfdA[MAX_RANGE*LAG_SIZE*2];
extern float xcfdA[MAX_RANGE*LAG_SIZE*2];

extern int xcntA;

extern int dfrqA;
extern int nfrqA;

extern int dmpincA;
extern int nmpincA;

extern int dfrangA;
extern int nfrangA;

extern int stfrqA;
extern int frqrngA;

extern int maxattenB;
extern int protattenB;
extern int rx_wideB;
extern int rx_narrowB;
extern int rsep_switchB;

extern int tsgidB;
extern int txpowB;
extern int naveB;
extern int attenB;
extern int lagfrB;
extern int smsepB;
extern int ercodB;
extern int agcstatB;
extern int lopwrstatB;
extern int nbaudB;
extern float noiseB;
extern int rxriseB;
extern int txplB;
extern int mpincB;
extern int mppulB;
extern int mplgsB;
extern int nrangB;
extern int frangB;
extern int rsepB;
extern int bmnumB;
extern int xcfB;
extern int tfreqB;
extern int scanB;
extern int mxpwrB;
extern int lvmaxB;
extern int cpB;

extern int lagnumB;

extern float noisestatB[4];
extern char combfB[128];

extern int rxchnB;
extern int smpnumB;
extern int skpnumB;
extern struct timespec seqtvalB[MAXNAVE];
extern int seqattenB[MAXNAVE];
extern float seqnoiseB[MAXNAVE];
extern int seqoffB[MAXNAVE];
extern int seqszeB[MAXNAVE];
extern int16 *samplesB;
extern int IQoffsetB;


extern float pwr0B[MAX_RANGE];
extern float acfdB[MAX_RANGE*LAG_SIZE*2];
extern float xcfdB[MAX_RANGE*LAG_SIZE*2];

extern int xcntB;

extern int dfrqB;
extern int nfrqB;

extern int dmpincB;
extern int nmpincB;

extern int dfrangB;
extern int nfrangB;

extern int stfrqB;
extern int frqrngB;

#endif












