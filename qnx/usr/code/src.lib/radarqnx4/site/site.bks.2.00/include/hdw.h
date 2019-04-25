/* hdw.h
   =====
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
 $Log: hdw.h,v $
 Revision 1.7  2008/03/14 19:23:06  code
 Removed the now redundant passing of usrdata for extended integration.

 Revision 1.6  2007/07/09 19:58:11  code
 Added a user parameter to SitIntegrateEx

 Revision 1.5  2007/02/22 15:57:40  code
 Added extended ACF calculation code.

 Revision 1.4  2006/08/18 14:24:53  code
 Removed tablen.

 Revision 1.3  2006/05/04 19:21:57  barnes
 Added tablen parameter to set the length of the lagtable.

 Revision 1.2  2006/02/07 20:53:16  barnes
 Simon Shepherd's modification to the lag table.

 Revision 1.1  2004/11/09 17:46:51  barnes
 Initial revision

*/

#define FREQ_LOCAL 2

extern pid_t dioid;
extern pid_t drid;

extern struct TSGtable *tsgtable;

extern int bufnum;
extern int bufsze;
extern void *bufadr[16];

extern int badrange;
extern struct timespec tock;

int SiteSetupHardware();
int SiteSetIntt(int sc,int us);
int SiteSetBeam(int bmnum);
int SiteFCLR(int sfrq,int efrq);
int SiteSetFreq(int tfreq);
int SiteIntegrate(int (*lags)[2]);
int SiteIntegrateex(int (*lags)[2]);
