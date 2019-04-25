/* interface.c
   ===========
   Author: R.J.Barnes
*/
 

/*
 $Log: interface.c,v $
 Revision 1.2  2008/03/21 18:41:36  code
 Added I&Q capture.

 Revision 1.1  2008/03/21 18:36:37  code
 Initial revision

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
#include "shmem.h"
#include "global.h"

/* 
 $Log: interface.c,v $
 Revision 1.2  2008/03/21 18:41:36  code
 Added I&Q capture.

 Revision 1.1  2008/03/21 18:36:37  code
 Initial revision

*/



void SiteStart() {

  /* insert stuff to be done when the program first starts here */
/* values for inv; DAndre 20070827 */
  backward=0;
  sbm=0;
  ebm=15;
  xcnt=1;
  day=12;
  night=22;
  dfrq=14400;
  nfrq=10200;
  frqrng=300;
}

void SiteEnd() {

  if (samples !=NULL)
  ShMemFree((unsigned char *) samples,sharedmemory,IQBUFSIZE,1,shmemfd);


  return;
}


int SiteStartScan() {
  return 1;
}
