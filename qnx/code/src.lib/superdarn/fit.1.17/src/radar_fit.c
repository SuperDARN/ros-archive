/* radar_fitc
   ==========
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
 $Log: radar_fit.c,v $
 Revision 1.1  2002/01/31 16:34:25  barnes
 Initial revision

*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rawdata.h"
#include "fitdata.h"
#include "cmpdata.h"
#include "radar_scan.h"
#include "cnv_data.h"
#include "cnv_time.h"

void add_fit_scan(struct radardata *ptr, struct fitdata *fit) {
  int bmnum,rng,cnt;
  
  bmnum=fit->prms.BMNUM;
  cnt=ptr->bmcnt[bmnum];
  ptr->bmcnt[bmnum]++;
  ptr->st_id=fit->prms.ST_ID;  
  if (cnt>0) {
    ptr->cpid[bmnum]=realloc(ptr->cpid[bmnum],(cnt+1)*sizeof(int));
    ptr->intt[bmnum]=realloc(ptr->intt[bmnum],(cnt+1)*sizeof(int));
    ptr->frang[bmnum]=realloc(ptr->frang[bmnum],(cnt+1)*sizeof(int));
    ptr->rsep[bmnum]=realloc(ptr->rsep[bmnum],(cnt+1)*sizeof(int));
    ptr->rxrise[bmnum]=realloc(ptr->rxrise[bmnum],(cnt+1)*sizeof(int));
    ptr->freq[bmnum]=realloc(ptr->freq[bmnum],(cnt+1)*sizeof(int));
    ptr->noise[bmnum]=realloc(ptr->noise[bmnum],(cnt+1)*sizeof(int));
    ptr->atten[bmnum]=realloc(ptr->atten[bmnum],(cnt+1)*sizeof(int));
    ptr->channel[bmnum]=realloc(ptr->channel[bmnum],(cnt+1)*sizeof(int));
    ptr->time[bmnum]=realloc(ptr->time[bmnum],(cnt+1)*sizeof(double));
  }
  
  ptr->cpid[bmnum][cnt]=fit->prms.CP;
  ptr->intt[bmnum][cnt]=fit->prms.INTT;
  ptr->frang[bmnum][cnt]=fit->prms.FRANG;
  ptr->rsep[bmnum][cnt]=fit->prms.RSEP;
  ptr->rxrise[bmnum][cnt]=fit->prms.RXRISE;
  ptr->freq[bmnum][cnt]=fit->prms.TFREQ;
  ptr->noise[bmnum][cnt]=fit->prms.NOISE;
  ptr->atten[bmnum][cnt]=fit->prms.ATTEN;
  ptr->channel[bmnum][cnt]=fit->prms.CHANNEL;
  ptr->time[bmnum][cnt]=time_epoch(fit->prms.YEAR,fit->prms.MONTH,
                                   fit->prms.DAY,fit->prms.HOUR,
				   fit->prms.MINUT,fit->prms.SEC);


  for (rng=0;rng<MAX_RANGE;rng++) {

    if (cnt>0) ptr->sct[bmnum][rng]=
	realloc(ptr->sct[bmnum][rng],(cnt+1)*sizeof(char));
    if (cnt>0) ptr->rng[bmnum][rng]=
	realloc(ptr->rng[bmnum][rng],(cnt+1)*sizeof(struct datapoint));
	 
     ptr->sct[bmnum][rng][cnt]=(fit->rng[rng].qflg==1);
     ptr->rng[bmnum][rng][cnt].gsct=fit->rng[rng].gsct;
     ptr->rng[bmnum][rng][cnt].v=fit->rng[rng].v;
     ptr->rng[bmnum][rng][cnt].p_l=fit->rng[rng].p_l;
     ptr->rng[bmnum][rng][cnt].w_l=fit->rng[rng].w_l;
     ptr->rng[bmnum][rng][cnt].v_e=fit->rng[rng].v_err;
	 
  }
  
}
