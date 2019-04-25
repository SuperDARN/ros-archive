
/* rawdumpdata.h
   ============= 
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
 $Log: iqdata.h,v $
 Revision 1.6  2008/03/14 16:13:18  code
 Added a variable the tells you how many samples to skip before calculating an ACF.
 This is from the digital receivers that take samples immediately the pulse
 train is started.

 Revision 1.5  2008/03/13 19:15:10  code
 Removed redundant stereo channel array - stereo channels will be in
 different data structures.
 Added chnnum to indicate number of receiver channels stored
 so the total number of 16-bit words in the array is:
 2*chnnum*smpnum

 Revision 1.4  2008/03/13 16:56:29  code
 Added smpnum and seqnum.

 Revision 1.3  2008/03/13 16:25:58  code
 Added version number.

 Revision 1.2  2008/03/13 16:17:20  code
 Fixed typo.

 Revision 1.1  2008/03/13 16:16:11  code
 Initial revision

*/


#ifndef _LIMIT_H
#include "limit.h"
#endif



#ifndef _IQDATA_H
#define _IQDATA_H


struct IQData {
  struct { 
    int major;
    int minor;
  } revision;
  int chnnum;
  int smpnum;
  int skpnum;
  int seqnum;
  struct timespec tval[MAXNAVE];
  int atten[MAXNAVE];
  float noise[MAXNAVE];
  int offset[MAXNAVE];
  int size[MAXNAVE];
};

#endif
