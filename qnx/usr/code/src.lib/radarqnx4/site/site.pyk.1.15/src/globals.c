/* globals.c
   ========= 
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include "rtypes.h"
#include "limit.h"
#include "rprm.h"
#include "iqdata.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"
#include "taskid.h"
#include "freq.h"
#include "rmsg.h"
#include "radarshell.h"
#include "default.h"

/*
 $Log: globals.c,v $
 Revision 1.3  2008/03/21 17:21:32  code
 Added I&Q capture.

 Revision 1.2  2007/02/08 16:36:49  code
 Added lagnum

 Revision 1.1  2006/03/30 11:02:44  barnes
 Initial revision

*/



int delays[2];

struct RadarParm prmA;
struct RadarParm prmB;
struct IQData iqA;
struct IQData iqB;
struct RawData rawA;
struct RawData rawB;
struct FitData fitA;
struct FitData fitB;

int maxattenA=DEFAULT_MAX_ATTEN;
int protattenA=DEFAULT_PROT_ATTEN;
int rx_wideA=DEFAULT_RXRISE;
int rx_narrowA=DEFAULT_RXRISE;
int rsep_switchA=DEFAULT_RSEP_SWITCH;

int tsgidA=0;
int txpowA=DEFAULT_TXPOW;
int naveA=0;
int attenA=0;
int lagfrA=DEFAULT_LAGFR;
int smsepA=DEFAULT_SMSEP;
int ercodA=0;
int agcstatA=0;
int lopwrstatA=0;
int nbaudA=0;
float noiseA=0;
int rxriseA=DEFAULT_RXRISE;
int intscA=DEFAULT_INTSC;
int intusA=DEFAULT_INTUS;
int txplA=DEFAULT_TXPL;
int mpincA=DEFAULT_MPINC;
int mppulA=DEFAULT_MPPUL;
int mplgsA=DEFAULT_MPLGS;
int nrangA=DEFAULT_NRANG;
int frangA=DEFAULT_FRANG;
int rsepA=DEFAULT_RSEP;
int bmnumA=0;
int xcfA=0;
int tfreqA=DEFAULT_TFREQ;
int scanA=DEFAULT_SCAN;
int mxpwrA=DEFAULT_MXPWR;
int lvmaxA=DEFAULT_LVMAX;
int cpA=50;

int lagnumA=0;

float noisestatA[4];

char combfA[128]={"A SuperDARN"};

float pwr0A[MAX_RANGE];
float acfdA[MAX_RANGE*LAG_SIZE*2];
float xcfdA[MAX_RANGE*LAG_SIZE*2];

int xcntA=DEFAULT_XCOUNT;

int dfrqA=10800;
int nfrqA=10200;
int dmpincA=1500;
int nmpincA=1500;
int dfrangA=180;
int nfrangA=180;

int stfrqA=13000;
int frqrngA=400;

/* raw I&Q sample buffer */

int rxchnA;
int smpnumA;
int skpnumA;
struct timespec seqtvalA[MAXNAVE];
int seqattenA[MAXNAVE];
float seqnoiseA[MAXNAVE];
int seqoffA[MAXNAVE];
int seqszeA[MAXNAVE];
int16 *samplesA;
int IQoffsetA=0;


int maxattenB=DEFAULT_MAX_ATTEN;
int protattenB=DEFAULT_PROT_ATTEN;
int rx_wideB=DEFAULT_RXRISE;
int rx_narrowB=DEFAULT_RXRISE;
int rsep_switchB=DEFAULT_RSEP_SWITCH;

int tsgidB=0;
int txpowB=DEFAULT_TXPOW;
int naveB=0;
int attenB=0;
int lagfrB=DEFAULT_LAGFR;
int smsepB=DEFAULT_SMSEP;
int ercodB=0;
int agcstatB=0;
int lopwrstatB=0;
int nbaudB=0;
float noiseB=0;
int rxriseB=DEFAULT_RXRISE;
int intscB=DEFAULT_INTSC;
int intusB=DEFAULT_INTUS;
int txplB=DEFAULT_TXPL;
int mpincB=DEFAULT_MPINC;
int mppulB=DEFAULT_MPPUL;
int mplgsB=DEFAULT_MPLGS;
int nrangB=DEFAULT_NRANG;
int frangB=DEFAULT_FRANG;
int rsepB=DEFAULT_RSEP;
int bmnumB=0;
int xcfB=0;
int tfreqB=DEFAULT_TFREQ;
int scanB=DEFAULT_SCAN;
int mxpwrB=DEFAULT_MXPWR;
int lvmaxB=DEFAULT_LVMAX;
int cpB=50;

int lagnumB=0;

float noisestatB[4];

char combfB[128]={"A SuperDARN"};

float pwr0B[MAX_RANGE];
float acfdB[MAX_RANGE*LAG_SIZE*2];
float xcfdB[MAX_RANGE*LAG_SIZE*2];

int xcntB=DEFAULT_XCOUNT;

int dfrqB=11300;
int nfrqB=10700;
int dmpincB=1500;
int nmpincB=1500;
int dfrangB=180;
int nfrangB=180;

int stfrqB=13000;
int frqrngB=400;


/* raw I&Q sample buffer */

int rxchnB;
int smpnumB;
int skpnumB;
struct timespec seqtvalB[MAXNAVE];
int seqattenB[MAXNAVE];
float seqnoiseB[MAXNAVE];
int seqoffB[MAXNAVE];
int seqszeB[MAXNAVE];
int16 *samplesB;
int IQoffsetB=IQBUFSIZE/2;


