/* global.c
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
 $Log: global.c,v $
 Revision 1.8  2008/03/14 22:06:42  code
 Added file number for shared memory.

 Revision 1.7  2008/03/14 22:02:34  code
 Added name of the shared memory device.

 Revision 1.6  2008/03/14 16:11:54  code
 Added the skip number of samples variable.

 Revision 1.5  2008/03/13 20:56:18  code
 Added access to the raw I&Q samples.

 Revision 1.4  2007/07/03 19:32:50  code
 Added the network and radar as globals.

 Revision 1.3  2006/08/18 14:26:35  code
 Added lagnum.

 Revision 1.2  2006/07/11 21:13:38  code
 Added command string.

 Revision 1.1  2004/05/06 20:11:21  barnes
 Initial revision

*/

struct RadarParm prm;
struct IQData iq;
struct RawData raw;
struct FitData fit;
struct FitBlock fblk;
struct RadarSite *site;
struct FreqTable *ftable;
struct RShellTable rstable;
struct RadarNetwork *network;
struct Radar *radar;

int tnum;
struct TaskID **tlist;

struct RMsgBlock msg;

int maxatten=DEFAULT_MAX_ATTEN;
int protatten=DEFAULT_PROT_ATTEN;

char command[128];

int tsgid=0;
int stid=0;
int yr=0;
int mo=0;
int dy=0;
int hr=0;
int mt=0;
int sc=0;
int us=0;
int txpow=DEFAULT_TXPOW;
int nave=0;
int atten=0;
int lagfr=DEFAULT_LAGFR;
int smsep=DEFAULT_SMSEP;
int ercod=0;
int agcstat=0;
int lopwrstat=0;
int nbaud=0;
float noise=0;
int rxrise=DEFAULT_RXRISE;
int intsc=DEFAULT_INTSC;
int intus=DEFAULT_INTUS;
int txpl=DEFAULT_TXPL;
int mpinc=DEFAULT_MPINC;
int mppul=DEFAULT_MPPUL;
int mplgs=DEFAULT_MPLGS;
int nrang=DEFAULT_NRANG;
int frang=DEFAULT_FRANG;
int rsep=DEFAULT_RSEP;
int bmnum=0;
int xcf=0;
int tfreq=DEFAULT_TFREQ;
int scan=DEFAULT_SCAN;
int mxpwr=DEFAULT_MXPWR;
int lvmax=DEFAULT_LVMAX;
int cp=50;

int lagnum=0;

int noisefac=0;
int noiselim=0;

float noisestat[4];

char combf[128]={"A SuperDARN"};

float pwr0[MAX_RANGE];
float acfd[MAX_RANGE*LAG_SIZE*2];
float xcfd[MAX_RANGE*LAG_SIZE*2];


int backward=0;
int sbm=0;
int ebm=16;

int xcnt=0;

int day=10;
int night=20;

int dfrq=12000;
int nfrq=12000;
int dmpinc=1500;
int nmpinc=1500;
int dfrang=180;
int nfrang=180;

int stfrq=12300;
int frqrng=300;


/* raw I&Q sample buffer */

int rxchn;
int smpnum;
int skpnum;
struct timespec seqtval[MAXNAVE];
int seqatten[MAXNAVE];
float seqnoise[MAXNAVE];
int seqoff[MAXNAVE];
int seqsze[MAXNAVE];
int16 *samples;

int shmemfd;
char *sharedmemory="IQBuffer";








