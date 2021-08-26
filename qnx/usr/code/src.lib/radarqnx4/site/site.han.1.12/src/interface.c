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
#include "globals.h"

/*
 $Log: interface.c,v $
 Revision 1.3  2007/08/08 16:35:29  code
 Added missing backwards=1 term.

 Revision 1.2  2006/03/30 11:09:59  barnes
 Modifications for Stereo mode.

 Revision 1.1  2004/07/22 21:17:23  barnes
 Initial revision

*/

#define DAY_BAND_A 5
#define DAY_BAND_B 5
#define NIGHT_BAND_A 2
#define NIGHT_BAND_B 2

#define NIGHT_START_HOUR 18
#define DAY_START_HOUR 6

#define RSEP_A    45
#define RSEP_B    45
#define FRANG_A   180
#define FRANG_B   180



void SiteStart() {

  rsep=RSEP_A; 
  rsepA=RSEP_A;
  rsepB=RSEP_B;

  frang=FRANG_A;
  frangA=FRANG_A;
  frangB=FRANG_B;

  dfrq=DAY_BAND_A;
  nfrq=NIGHT_BAND_A;
  
  dfrqA=DAY_BAND_A;
  nfrqA=NIGHT_BAND_A;
 
  dfrqB=DAY_BAND_B;
  nfrqB=NIGHT_BAND_B;
   
  day=DAY_START_HOUR;
  night=NIGHT_START_HOUR;
  
  backward=1;   


}

void SiteEnd() {
  return;
}


int SiteStartScan() {
  return 1;
}
