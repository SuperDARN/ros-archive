/* fitacf.c
   ========
   Author: R.J.Barnes & K.Baker
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
 $Log: fitacf.c,v $
 Revision 1.16  2006/03/14 18:00:39  barnes
 Allowed the number of beams to be varied.

 Revision 1.15  2004/07/20 02:28:11  barnes
 Explicitly definied the FitACF version number in a header.

 Revision 1.14  2004/06/16 22:43:14  barnes
 Incorporated Sessai's modifications.

 Revision 1.13  2004/05/04 22:54:21  barnes
 Changed math header name.

 Revision 1.12  2004/05/04 18:27:07  barnes
 Renamed the type header to avoid confusion.

 Revision 1.11  2004/04/26 22:14:07  barnes
 Audit - Enforced warning "all".

 Revision 1.10  2004/01/08 22:20:39  barnes
 Kile Baker's Modifications for improved velocity error and ground scatter
 determination.

 Revision 1.9  2003/09/13 22:39:29  barnes
 Modifications to use the new data structures.

 Revision 1.8  2002/03/28 15:47:58  barnes
 Fixed bug in passing control program number.

 Revision 1.7  2002/02/05 22:44:52  barnes
 Modification to deal with stereo badlags.

 Revision 1.6  2001/06/27 20:48:31  barnes
 Added license tag

 Revision 1.5  2001/01/29 18:11:53  barnes
 Added Author Name

 Revision 1.4  2000/09/18 17:42:56  barnes
 Removed redundant id header.

 Revision 1.3  1998/11/09 22:44:04  barnes
 Incorporated the rawdata header.

 Revision 1.2  1998/06/11 15:10:29  barnes
 Changed the name of the hardware structure to radar_site.

 Revision 1.1  1998/06/10 13:49:41  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "rmath.h"
#include "rtypes.h"
#include "limit.h"
#include "rprm.h"
#include "radar.h"
#include "rawdata.h"
#include "fitdata.h"
#include "fitblk.h"
#include "dofit.h"
#include "fitacfversion.h"

#define GOOSEBAY 1

void FitACFEnd(struct FitBlock *fptr) {
  int i;
  for (i=0;i<MAX_RANGE;i++) {
    free(fptr->acfd[i]);
    free(fptr->xcfd[i]);
  }
}

 
void FitACFStart(struct RadarSite *hd,
	       int year,struct FitBlock *fptr) {
  int i;
  if (year < 1993) fptr->prm.old=1;
  for (i=0;i<3;i++) fptr->prm.interfer[i]=hd->interfer[i];
  fptr->prm.bmsep=hd->bmsep;
  fptr->prm.phidiff=hd->phidiff;
  fptr->prm.tdiff=hd->tdiff;
  fptr->prm.vdir=hd->vdir;
  fptr->prm.maxbeam=hd->maxbeam;
  for (i=0;i<MAX_RANGE;i++) {
    fptr->acfd[i]=malloc(sizeof(struct complex)*LAG_SIZE);
    fptr->xcfd[i]=malloc(sizeof(struct complex)*LAG_SIZE);
  }
}

void FitACF(struct RadarParm *prm,
            struct RawData *raw,struct FitBlock *input,
	    struct FitData *fit) {
  int i,j;
  int fnum,goose;
  if (prm->time.yr < 1993) input->prm.old=1;

  fit->revision.major=FITACF_MAJOR_REVISION;
  fit->revision.minor=FITACF_MINOR_REVISION;

  input->prm.xcf=prm->xcf;
  input->prm.tfreq=prm->tfreq;
  input->prm.noise=prm->noise.search;
  input->prm.nrang=prm->nrang;
  input->prm.smsep=prm->smsep;
  input->prm.nave=prm->nave;
  input->prm.mplgs=prm->mplgs;
  input->prm.mpinc=prm->mpinc;
  input->prm.txpl=prm->txpl;
  input->prm.lagfr=prm->lagfr;
  input->prm.mppul=prm->mppul;
  input->prm.bmnum=prm->bmnum;
  input->prm.cp=prm->cp;
  input->prm.channel=prm->channel;
  input->prm.offset=prm->offset; /* stereo offset */


  /* need to incorporate Sessai's code for setting the offset
     for legacy data here.
  */

  for (i=0;i<input->prm.mppul;i++) input->prm.pulse[i]=prm->pulse[i];
  for (i=0;i<input->prm.mplgs;i++) {
    input->prm.lag[0][i]=prm->lag[i][0];
    input->prm.lag[1][i]=prm->lag[i][1];
  }

  for (i=0;i<input->prm.nrang;i++) {
    input->prm.pwr0[i]=raw->pwr0[i];
   
    for (j=0;j<input->prm.mplgs;j++) {
      input->acfd[i][j].x=raw->acfd[i][j][0];
      input->acfd[i][j].y=raw->acfd[i][j][1];
    }
    if (input->prm.xcf) {
      for (j=0;j<input->prm.mplgs;j++) {
        input->xcfd[i][j].x=raw->xcfd[i][j][0];
        input->xcfd[i][j].y=raw->xcfd[i][j][1];
      }
    } 
  }   
  goose=((prm->stid)==GOOSEBAY);
  fnum=do_fit(input,5,goose,fit->rng,fit->xrng,fit->elv,&fit->noise);
}
