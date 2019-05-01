/* fitcfit.c
   =========
   Author: R.J.Barnes
*/

/*
 Copyright © 2001 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 RST-ROS (hereinafter "the Program").
 
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


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "cfitdata.h"

/*
 $Log: fitcfit.c,v $
 Revision 1.2  2004/05/04 18:26:11  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/03/11 22:09:15  barnes
 Initial revision

*/

int FitToCFit(double min_pwr,struct CFitdata *ptr,
              struct RadarParm *prm,
              struct FitData *fit) {
  
  int i,num=0,rng;
  ptr->version.major=CFIT_MAJOR_REVISION;
  ptr->version.minor=CFIT_MINOR_REVISION;

  /* time stamp the record */
  ptr->time=TimeYMDHMSToEpoch(prm->time.yr,prm->time.mo,
			      prm->time.dy,
			      prm->time.hr,prm->time.mt,
			      prm->time.sc+prm->time.us/1.0e6); 

 
   
   ptr->stid=prm->stid;
   ptr->scan=prm->scan;  
   ptr->cp=prm->cp;
   ptr->bmnum=prm->bmnum;
   ptr->channel=prm->channel; 
   ptr->intt.sc=prm->intt.sc;
   ptr->intt.us=prm->intt.us;
   ptr->frang=prm->frang;
   ptr->rsep=prm->rsep;
   ptr->rxrise=prm->rxrise;
   ptr->tfreq=prm->tfreq;
   ptr->noise=prm->noise.search;
   ptr->atten=prm->atten;
   ptr->nave=prm->nave;
   ptr->nrang=prm->nrang;

  for (i=0;i<prm->nrang;i++) {
    if ((fit->rng[i].qflg!=1) || 
        (fit->rng[i].p_0 <= min_pwr)) continue;
    ptr->rng[num]=(unsigned char) i;
    num++;
  }
  ptr->num=num;

  if (num>0) {
    for (i=0;i<num;i++) {
      rng=ptr->rng[i];
      ptr->data[i].gsct=fit->rng[rng].gsct;
      ptr->data[i].p_0=fit->rng[rng].p_0;
      ptr->data[i].p_0_e=0;
      ptr->data[i].v=fit->rng[rng].v;
      ptr->data[i].v_e=fit->rng[rng].v_err;
      ptr->data[i].p_l=fit->rng[rng].p_l;
      ptr->data[i].p_l_e=fit->rng[rng].p_l_err;
      ptr->data[i].w_l=fit->rng[rng].w_l;
      ptr->data[i].w_l_e=fit->rng[rng].w_l_err;
    }
  }
  return 0;
}
 






