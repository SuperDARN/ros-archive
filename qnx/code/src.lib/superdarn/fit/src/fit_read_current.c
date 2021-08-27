/* fit_read_current.c
   ================== */

/* 
 $Log: fit_read_current.c,v $
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
      fit_data->rng[rng-1].p_l=r.r2.r_pwr_l[i]/100.0;
      fit_data->rng[rng-1].p_s=r.r2.r_pwr_s[i]/100.0;
      fit_data->rng[rng-1].w_l=r.r2.r_w_l[i]/10.0;
      fit_data->rng[rng-1].w_s=r.r2.r_w_s[i]/10.0;
      fit_data->rng[rng-1].v=r.r2.r_vel[i]/10.0;
      fit_data->rng[rng-1].v_err=r.r2.r_vel_err[i]/10.0;
      fit_data->rng[rng-1].sdev_l=r.r2.r_sdev_l[i]/1000.0;
      fit_data->rng[rng-1].sdev_s=r.r2.r_sdev_s[i]/1000.0;
      fit_data->rng[rng-1].sdev_phi=r.r2.r_sdev_s[i]/100.0;
      inum++;
    }
  }
  inum=0;
  if (fit_data->prms.XCF ==0) return 0;
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
      fit_data->xrng[rng-1].p_l=r.r2.r_pwr_l[i]/100.0;
      fit_data->xrng[rng-1].p_s=r.r2.r_pwr_s[i]/100.0;
      fit_data->xrng[rng-1].w_l=r.r2.r_w_l[i]/10.0;
      fit_data->xrng[rng-1].w_s=r.r2.r_w_s[i]/10.0;
      fit_data->xrng[rng-1].v=r.r2.r_vel[i]/10.0;
      fit_data->xrng[rng-1].v_err=r.r2.r_vel_err[i]/10.0;
      fit_data->xrng[rng-1].sdev_l=r.r2.r_sdev_l[i]/1000.0;
      fit_data->xrng[rng-1].sdev_s=r.r2.r_sdev_s[i]/1000.0;
      fit_data->xrng[rng-1].sdev_phi=r.r2.r_sdev_s[i]/100.0;

      fit_data->xrng[rng-1].phi0=r.r2.r_phi0[i]/100.0;
      fit_data->xrng[rng-1].phi0_err=r.r2.r_phi0_err[i]/100.0;
      fit_data->elev[rng-1].normal=r.r2.r_elev[i]/100.0;
      fit_data->elev[rng-1].low=r.r2.r_elev_low[i]/100.0;
      fit_data->elev[rng-1].high=r.r2.r_elev_high[i]/100.0;
      inum++;
    }
  }
  return 0;
}

