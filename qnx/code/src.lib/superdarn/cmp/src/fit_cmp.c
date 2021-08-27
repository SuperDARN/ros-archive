/* fit_cmp.c
   ========= */

/*
 $Log: fit_cmp.c,v $
 Revision 1.3  1999/07/24 18:22:04  barnes
 Added version numbers to the file header.

 Revision 1.2  1999/03/29 16:01:37  barnes
 Fixed problem with the quality flag.

 Revision 1.1  1999/03/18 15:20:28  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "rawdata.h"
#include "fitdata.h"
#include "cmpdata.h"
#include "cnv_time.h"
#include "cnv_data.h"

void fit_cmphdr(double pmax,double vmax,double wmax,
				struct fitdata *fit,struct cmphdr *ptr) {

  ptr->pmax=pmax;
  ptr->vmax=vmax;
  ptr->wmax=wmax;
  ptr->st_id=fit->prms.ST_ID;
  ptr->frang=fit->prms.FRANG;
  ptr->rsep=fit->prms.RSEP;
  ptr->major_rev=MAJOR_REVISION;
  ptr->minor_rev=MINOR_REVISION;
}

int fit_cmp(int store,double min_pwr,
	    double pmax,double vmax,double wmax,struct fitdata *fit,
	    struct cmpdata *ptr,int scan) {
  /* This is a high rate compression algorithm designed for graphics only */
  int loop,rng_num,i,rflg;
 
  memset(ptr->store,0,75*5);

  /* time stamp the record */
  ptr->time=time_epoch(fit->prms.YEAR,fit->prms.MONTH,
					   fit->prms.DAY,
					   fit->prms.HOUR,fit->prms.MINUT,
					   fit->prms.SEC); 
  /*record  the beam number and a scan flag in the top bits of
    the dflag entry */

  ptr->noise=fit->prms.NOISE;
  ptr->tfreq=fit->prms.TFREQ;

  if ((ptr->frang !=fit->prms.FRANG) || 
      (ptr->rsep !=fit->prms.RSEP)) {
    rflg=1;
    ptr->frang=fit->prms.FRANG;
    ptr->rsep=fit->prms.RSEP;
  } else rflg=0;

 
  ptr->scan=0; 
  if ((fit->prms.SCAN == 1) || (scan !=0)) {
    ptr->scan=1;
    ptr->magic='z';
  } else ptr->magic='x';
  ptr->stored=store;
  ptr->bmnum=fit->prms.BMNUM;
   
  /* work out which ranges to store */
   
  rng_num=0;
  for (loop=0;loop<MAX_RANGE;loop++) {
    if (fit->rng[loop].gsct !=0) ptr->store[MAX_RANGE+loop]=1;
    if ((fit->rng[loop].qflg==1) && 
        (fit->rng[loop].p_l > min_pwr)) {
      ptr->store[loop]=1;
      rng_num++;
    }   
  }
  if (rng_num>0) { 
    for (loop=0;loop<MAX_RANGE;loop++) {
      if (ptr->store[loop]  !=0) {
        int p_l_c=0,v_c=0,w_l_c=0;
        if ((store & 0x01) !=0) { 
          p_l_c=(fit->rng[loop].p_l/pmax)*256;
          p_l_c=(p_l_c >255) ? 255 : p_l_c;
          p_l_c=(p_l_c <0) ? 0 : p_l_c;
          ptr->store[2*MAX_RANGE+loop]=p_l_c;
        }
        if ((store & 0x02) !=0) {
          v_c=(1+(fit->rng[loop].v/vmax))*128;
          v_c=(v_c >255) ? 255 : v_c;
          v_c=(v_c < 0) ? 0 : v_c;
          ptr->store[3*MAX_RANGE+loop]=v_c;
        }
        if ((store & 0x04) !=0) {
          w_l_c=(fit->rng[loop].w_l/wmax)*256;
          w_l_c=(w_l_c >255) ? 255 : w_l_c;
          w_l_c=(w_l_c <0) ? 0 : w_l_c;
          ptr->store[4*MAX_RANGE+loop]=w_l_c; 
        }
      } 
    }
  }
  return rflg;
}
 





