/* global.h
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

/*
 $Log: global.h,v $
 Revision 1.4  2007/07/09 19:48:12  code
 Added site information.

 Revision 1.3  2006/08/18 14:27:01  code
 Added lagnum.

 Revision 1.2  2006/07/11 21:02:41  code
 Added the command string for the command line string.

 Revision 1.1  2004/07/20 01:10:10  barnes
 Initial revision

*/

#ifndef _GLOBAL_H
#define _GLOBAL_H

extern struct RadarParm prm;
extern struct RawData raw;
extern struct FitData fit;
extern struct FitBlock fblk;
extern struct RadarSite *site;
extern struct FreqTable *ftable;
extern struct RShellTable rstable;
extern struct RadarNetwork *network;
extern struct Radar *radar;

extern int tnum;
extern struct TaskID **tlist;

extern struct RMsgBlock msg;

extern char command[128];

extern int maxatten;
extern int protatten;

extern int tsgid;
extern int stid;
extern int yr;
extern int mo;
extern int dy;
extern int hr;
extern int mt;
extern int sc;
extern int us;
extern int txpow;
extern int nave;
extern int atten;
extern int lagfr;
extern int smsep;
extern int ercod;
extern int agcstat;
extern int lopwrstat;
extern int nbaud;
extern float noise;
extern int rxrise;
extern int intsc;
extern int intus;
extern int txpl;
extern int mpinc;
extern int mppul;
extern int mplgs;
extern int nrang;
extern int frang;
extern int rsep;
extern int bmnum;
extern int xcf;
extern int tfreq;
extern int scan;
extern int mxpwr;
extern int lvmax;
extern int cp;

extern int lagnum;

extern int noisefac;
extern int noiselim;

extern float noisestat[4];
extern char combf[128];

extern float pwr0[MAX_RANGE];
extern float acfd[MAX_RANGE*LAG_SIZE*2];
extern float xcfd[MAX_RANGE*LAG_SIZE*2];


extern int backward;
extern int sbm;
extern int ebm;

extern int xcnt;

extern int day;
extern int night;

extern int dfrq;
extern int nfrq;

extern int dmpinc;
extern int nmpinc;

extern int dfrang;
extern int nfrang;

extern int stfrq;
extern int frqrng;

#endif












