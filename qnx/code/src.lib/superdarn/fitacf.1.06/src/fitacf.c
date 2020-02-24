/* fitacf.c
   ========
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

/*
 $Log: fitacf.c,v $
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
#include "math_constant.h"
#include "rawdata.h"
#include "fit_blk.h"
#include "do_fit.h"
#include "fitdata.h"
#include "hardware.h"


#define GOOSEBAY 3

void end_fit(struct fit_block *fptr) {
  int i;
  for (i=0;i<MAX_RANGE;i++) {
    free(fptr->acfd[i]);
    free(fptr->xcfd[i]);
  }
}

 
void start_fit(struct radar_site *hd,
	       int year,struct fit_block *fptr) {
  int ctime,i;
  if (year < 1993) fptr->prm.old=1;
  for (i=0;i<3;i++) fptr->prm.interfer_pos[i]=hd->interfer_pos[i];
  fptr->prm.beam_sep=hd->beam_sep;
  fptr->prm.phidiff=hd->phidiff;
  fptr->prm.tdiff=hd->tdiff;
  fptr->prm.vdir=hd->vdir;

  for (i=0;i<MAX_RANGE;i++) {
    fptr->acfd[i]=malloc(sizeof(struct complex)*LAG_TAB_LEN);
    fptr->xcfd[i]=malloc(sizeof(struct complex)*LAG_TAB_LEN);
  }
}

void fitacf(struct rawdata *ptr,struct fit_block *input,
			struct fitdata *fit) {
  int i,j;
  int fnum,goose;
  if (ptr->PARMS.YEAR < 1993) input->prm.old=1;
  input->prm.xcf=ptr->PARMS.XCF;
  input->prm.tfreq=ptr->PARMS.TFREQ;
  input->prm.noise=ptr->PARMS.NOISE;
  input->prm.nrang=ptr->PARMS.NRANG;
  input->prm.smsep=ptr->PARMS.SMSEP;
  input->prm.nave=ptr->PARMS.NAVE;
  input->prm.mplgs=ptr->PARMS.MPLGS;
  input->prm.mpinc=ptr->PARMS.MPINC;
  input->prm.txpl=ptr->PARMS.TXPL;
  input->prm.lagfr=ptr->PARMS.LAGFR;
  input->prm.mppul=ptr->PARMS.MPPUL;
  input->prm.bmnum=ptr->PARMS.BMNUM;
  input->prm.cp=ptr->PARMS.BMNUM;
  input->prm.channel=ptr->PARMS.CHANNEL;
  input->prm.offset=ptr->PARMS.usr_resL1; /* stereo offset */

  for (i=0;i<input->prm.mppul;i++) input->prm.pulse[i]=ptr->PULSE_PATTERN[i];
  for (i=0;i<input->prm.mplgs;i++) {
    input->prm.lag[0][i]=ptr->LAG_TABLE[0][i];
    input->prm.lag[1][i]=ptr->LAG_TABLE[1][i];
  }

  for (i=0;i<input->prm.nrang;i++) {
    input->prm.pwr0[i]=ptr->pwr0[i];
   
    for (j=0;j<input->prm.mplgs;j++) {
      input->acfd[i][j].x=ptr->acfd[i][j][0];
      input->acfd[i][j].y=ptr->acfd[i][j][1];
    }
    if (input->prm.xcf) {
      for (j=0;j<input->prm.mplgs;j++) {
        input->xcfd[i][j].x=ptr->xcfd[i][j][0];
        input->xcfd[i][j].y=ptr->xcfd[i][j][1];
      }
    } 
  }   
  goose=(ptr->PARMS.ST_ID==GOOSEBAY);
  fnum=do_fit(input,5,goose,fit->rng,fit->xrng,fit->elev,&fit->noise);
  memcpy(&fit->prms,ptr,sizeof(struct radar_parms)+COMBF_SIZE+
         sizeof(int16)*(LAG_TAB_LEN*2+PULSE_PAT_LEN));
}
