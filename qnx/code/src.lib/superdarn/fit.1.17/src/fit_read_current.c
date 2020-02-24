/* fit_read_current.c
   ==================
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
 $Log: fit_read_current.c,v $
 Revision 1.8  2001/11/20 16:56:55  barnes
 Fixed major bug in decoding errors on power and spectral width.

 Revision 1.7  2001/06/27 20:48:03  barnes
 Added license tag

 Revision 1.6  2001/02/15 14:20:20  barnes
 Another modification to deal with bad beams.

 Revision 1.5  2001/02/14 17:19:14  barnes
 Modification to deal with buggy data containing negative beam numbers.

 Revision 1.4  2001/01/29 18:11:52  barnes
 Added Author Name

 Revision 1.3  1999/03/17 20:11:05  barnes
 Removed debug code.

 Revision 1.2  1999/03/17 19:46:45  barnes
 Added check to make sure that file contains valid data.

 Revision 1.1  1999/03/16 14:36:30  barnes
 Initial revision

 */


/* This routine reads in and decodes a record from a fit file */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "cnv_data.h"
#include "cnv_time.h"
#include "rawdata.h"
#include "fitdata.h"

#include "fit_read.h"
#include "fit_str.h"

int fit_read_current(struct fitfp *ptr,struct fitdata *fit_data) {
  int r1_pat[]={4,2,1,2,2,17,4,2,2,14,4,4,2,4,
                2,PULSE_PAT_LEN,2,2*LAG_TAB_LEN,1,COMBF_SIZE,4,3,
		2,2*MAX_RANGE,1,MAX_RANGE,0,0};

  int r2_pat[]={4,3,1,25,1,25,2,475,0,0};

  int hlength;
  int rng,i,icnt=0,inum=0;
  union fit_out r;
  int b=0;
   
  /* read in a record block */

  memset(fit_data,0,sizeof(struct fitdata));
  if (ptr->fitfp !=-1) {
    if (read(ptr->fitfp,&r,sizeof(union fit_out)) !=sizeof(union fit_out)) 
      return -1;
  } else {
    if ((ptr->fptr+sizeof(union fit_out))>=ptr->fstat.st_size) return -1;
    memcpy((unsigned char *) &r,ptr->fbuf+ptr->fptr,sizeof(union fit_out));
    ptr->fptr+=sizeof(union fit_out);
  } 
  cnv_block( (unsigned char *) &r,r1_pat);
  if (r.r1.rrn !=0) return -1;
  
  /* ptr->ctime=r.r1.r_time; */
  ptr->blen=1;
  hlength=sizeof(struct radar_parms)+sizeof(int16)*
                (PULSE_PAT_LEN+2*LAG_TAB_LEN)+COMBF_SIZE;

  memcpy(&fit_data->prms,&(r.r1.plist[0]),hlength);

  if (fit_data->prms.YEAR==0) return -1;

  fit_data->noise.skynoise=r.r1.r_noise_lev;
  fit_data->noise.lag0=r.r1.r_noise_lag0;
  fit_data->noise.vel=r.r1.r_noise_vel; 

  /* substitute the actual time past epoch for the old year sec time */
  ptr->ctime=time_epoch(fit_data->prms.YEAR,
                        fit_data->prms.MONTH,
			fit_data->prms.DAY,
			fit_data->prms.HOUR,
			fit_data->prms.MINUT,
			fit_data->prms.SEC);


  /* calculate how many records to store the data*/
  for (i=0;i<fit_data->prms.NRANG;i++) {
     fit_data->rng[i].p_0=r.r1.r_pwr0[i]/100.0;
     if (r.r1.r_slist[i] !=0) {
        icnt++;
        fit_data->rng[r.r1.r_slist[i]-1].nump=r.r1.r_numlags[i];
     }
  }
  if (icnt==0) return 0;
  while (inum<icnt) {
    if (ptr->fitfp !=-1) {
      if (read(ptr->fitfp,&r,sizeof(union fit_out)) 
          != sizeof(union fit_out)) return -1;
    } else {
      if ((ptr->fptr+sizeof(union fit_out))>=ptr->fstat.st_size) return -1;
      memcpy((unsigned char *) &r,ptr->fbuf+ptr->fptr,sizeof(union fit_out));
      ptr->fptr+=sizeof(union fit_out);
    } 
    cnv_block( (unsigned char *) &r,r2_pat);
    ptr->blen++;
    if ((r.r2.rrn==0) || (r.r2.r_xflag!=0)) return -1;

    for (i=0;(i<25) && (inum<icnt);i++) {
      rng=r.r2.range[i];


      fit_data->rng[rng-1].qflg=r.r2.r_qflag[i];
      fit_data->rng[rng-1].gsct=r.r2.r_gscat[i];
      fit_data->rng[rng-1].p_l=((double) r.r2.r_pwr_l[i])/100.0;
      fit_data->rng[rng-1].p_s=((double) r.r2.r_pwr_s[i])/100.0;
      fit_data->rng[rng-1].p_l_err=((double) r.r2.r_pwr_l_err[i])/100.0;
      fit_data->rng[rng-1].p_s_err=((double) r.r2.r_pwr_s_err[i])/100.0;
      fit_data->rng[rng-1].w_l=((double) r.r2.r_w_l[i])/10.0;
      fit_data->rng[rng-1].w_s=((double) r.r2.r_w_s[i])/10.0;
      fit_data->rng[rng-1].w_l_err=((double) r.r2.r_w_l_err[i])/10.0;
      fit_data->rng[rng-1].w_s_err=((double) r.r2.r_w_s_err[i])/10.0;

      fit_data->rng[rng-1].v=((double) r.r2.r_vel[i])/10.0;
      fit_data->rng[rng-1].v_err=((double) r.r2.r_vel_err[i])/10.0;
      fit_data->rng[rng-1].sdev_l=((double) r.r2.r_sdev_l[i])/1000.0;
      fit_data->rng[rng-1].sdev_s=((double) r.r2.r_sdev_s[i])/1000.0;
      fit_data->rng[rng-1].sdev_phi=((double) r.r2.r_sdev_s[i])/100.0;
      inum++;
    }
  }
  inum=0;
  if (fit_data->prms.XCF ==0) {
    if ((fit_data->prms.BMNUM<0) || (fit_data->prms.BMNUM>15)) 
      return fit_read_current(ptr,fit_data);
    return 0;
  }
  while (inum<icnt) {
    if (ptr->fitfp !=-1) {
      if (read(ptr->fitfp,&r,sizeof(union fit_out)) 
          != sizeof(union fit_out)) return -1;
    } else {
      if ((ptr->fptr+sizeof(union fit_out))>=ptr->fstat.st_size) return -1;
      memcpy((unsigned char *) &r,ptr->fbuf+ptr->fptr,sizeof(union fit_out));
      ptr->fptr+=sizeof(union fit_out);
    } 
    cnv_block( (unsigned char *) &r,r2_pat);
    ptr->blen++;
    if ((r.r2.rrn==0) || (r.r2.r_xflag ==0)) return -1;

    for (i=0;(i<25) && (inum<icnt);i++) {
      rng=r.r2.range[i];
      fit_data->xrng[rng-1].qflg=r.r2.r_qflag[i];
      fit_data->xrng[rng-1].gsct=r.r2.r_gscat[i];
      fit_data->xrng[rng-1].p_l=((double) r.r2.r_pwr_l[i])/100.0;
      fit_data->xrng[rng-1].p_s=((double) r.r2.r_pwr_s[i])/100.0;
      fit_data->xrng[rng-1].p_l_err=((double) r.r2.r_pwr_l_err[i])/100.0;
      fit_data->xrng[rng-1].p_s_err=((double) r.r2.r_pwr_s_err[i])/100.0;

      fit_data->xrng[rng-1].w_l=((double) r.r2.r_w_l[i])/10.0;
      fit_data->xrng[rng-1].w_s=((double) r.r2.r_w_s[i])/10.0;
      fit_data->xrng[rng-1].w_l_err=((double) r.r2.r_w_l_err[i])/10.0;
      fit_data->xrng[rng-1].w_s_err=((double) r.r2.r_w_s_err[i])/10.0;

      fit_data->xrng[rng-1].v=((double) r.r2.r_vel[i])/10.0;
      fit_data->xrng[rng-1].v_err=((double) r.r2.r_vel_err[i])/10.0;
      fit_data->xrng[rng-1].sdev_l=((double) r.r2.r_sdev_l[i])/1000.0;
      fit_data->xrng[rng-1].sdev_s=((double) r.r2.r_sdev_s[i])/1000.0;
      fit_data->xrng[rng-1].sdev_phi=((double) r.r2.r_sdev_s[i])/100.0;

      fit_data->xrng[rng-1].phi0=((double) r.r2.r_phi0[i])/100.0;
      fit_data->xrng[rng-1].phi0_err=((double) r.r2.r_phi0_err[i])/100.0;
      fit_data->elev[rng-1].normal=((double) r.r2.r_elev[i])/100.0;
      fit_data->elev[rng-1].low=((double) r.r2.r_elev_low[i])/100.0;
      fit_data->elev[rng-1].high=((double) r.r2.r_elev_high[i])/100.0;
      inum++;
    }
  }
  

  if ((fit_data->prms.BMNUM<0) || (fit_data->prms.BMNUM>15)) 
    return fit_read_current(ptr,fit_data);

  return 0;
}

