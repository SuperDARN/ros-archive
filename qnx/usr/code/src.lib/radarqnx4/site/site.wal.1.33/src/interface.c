/* interface.c
   ===========
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "rtypes.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"
#include "taskid.h"
#include "rmsg.h"
#include "radarshell.h"
#include "global.h"
#include "shmem.h"

/*
 $Log: interface.c,v $
 Revision 1.9  2008/03/19 14:03:01  code
 Check to see if the I&Q buffer is claimed.

 Revision 1.8  2008/03/15 00:00:44  code
 Implemented shared memory for IQ sampling.

 Revision 1.7  2008/03/14 17:40:25  code
 Added skpnum and temporary allocation of the IQ buffer.

 Revision 1.6  2007/07/16 15:46:24  code
 Changed default parameters.

 Revision 1.5  2007/02/08 16:46:39  code
 Update as of January 2007.

 Revision 1.4  2006/03/13 19:31:31  barnes
 Modification for 24 beam operation.

 Revision 1.3  2005/07/29 14:22:25  barnes
 Changed the start of the day.

 Revision 1.2  2005/07/19 14:46:56  barnes
 Modification for extended ranges.

 Revision 1.1  2004/11/09 17:48:15  barnes
 Initial revision

 Revision 1.1  2004/05/11 15:31:56  barnes
 Initial revision

*/



void SiteStart() {

  /* insert stuff to be done when the program first starts here */

  backward=0;
  sbm=0;
  ebm=15;
  xcnt=0;
  day=11;
  night=22;
  dfrq=10800;
  nfrq=10200;
  frqrng=500;
  mpinc=2400;
  dmpinc=2400;
  nmpinc=2400;
  nrang=100;
  rsep=45;
  txpl=300;
  

}

void SiteEnd() {
  if (samples !=NULL)
  ShMemFree((unsigned char *) samples,sharedmemory,IQBUFSIZE,1,shmemfd);

  return;
}


int SiteStartScan() {
  return 1;
}
