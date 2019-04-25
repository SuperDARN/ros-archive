/* rawfeed.h
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
 $Log: rawfeed.h,v $
 Revision 1.6  2006/08/23 19:34:05  code
 Added sample delay for digital receiver.

 Revision 1.5  2006/08/17 19:09:48  code
 Added support for extended lag tables.

 Revision 1.4  2006/08/17 15:18:35  code
 Passed in beam, noise and frequency.

 Revision 1.3  2006/04/07 14:36:30  barnes
 Added missing message type.

 Revision 1.2  2006/04/07 14:31:18  barnes
 Removed redundant argument to the trigger.

 Revision 1.1  2006/04/07 14:29:14  barnes
 Initial revision

*/

#ifndef _RAWFEED_H
#define _RAWFEED_H

#define SCAN 's'
#define SET 'S'
#define TRIGGER 't'


int RawFeedScan(ipcid_t ipcid,int buffer,int sample,int channel,int atten,
                struct timespec *tval);
int RawFeedTrigger(ipcid_t ipcid);

int RawFeedSet(ipcid_t ipcid,struct TSGprm *prm,int mplgs,int lagnum,
	       int (*lag)[2],int smpdelay,
               int atten,float noise,int bmnum,int tfreq,
	       struct timespec *tval);




#endif

