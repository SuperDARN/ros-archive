/* radar_cmp.c
   ========= */

/*
 $Log: radar_cmp.c,v $
 Revision 1.3  1999/07/24 18:22:04  barnes
 Added version numbers to the file header.

 Revision 1.2  1999/03/18 15:20:28  barnes
 Modified the internal format of the cmp file to use epoch time.

 Revision 1.1  1999/03/16 19:26:46  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "cnv_time.h"
#include "rawdata.h"
#include "radar_scan.h"
#include "cmpdata.h"

void radar_cmphdr(double pmax,double vmax,double wmax,
				struct radardata *scan,struct cmphdr *ptr) {
  int yr,mo,dy,hr,mt,i;
  double sc;

  for (i=0;i<16;i++) if (scan->bmcnt[i] !=0) break;
  if (i==16) {
    ptr->frang=180;
    ptr->rsep=45;
  } else {
    ptr->frang=scan->frang[i][0];
    ptr->rsep=scan->rsep[i][0];
  }
  ptr->pmax=pmax;
  ptr->vmax=vmax;
  ptr->wmax=wmax;
  ptr->st_id=scan->st_id;
  ptr->major_rev=MAJOR_REVISION;
  ptr->minor_rev=MINOR_REVISION;
}

int radar_cmp(int bmnum,int cnt,int store,double min_pwr,
	    double pmax,double vmax,double wmax,struct radardata *sptr,
	    struct cmpdata *ptr,int scan) {
  /* This is a high rate compression algorithm designed for graphics only */
  int loop,rng_num,i;
  int yr,mo,dy,hr,mt;
  double sc;


  memset(ptr->store,0,75*5);

  /* time stamp the record */

  ptr->time=sptr->time[bmnum][cnt];
 
  /* record  the beam number and a scan flag in the top bits of
     the dflag entry */

  ptr->noise=sptr->noise[bmnum][cnt];
  ptr->tfreq=sptr->freq[bmnum][cnt];
  ptr->scan=0; 
  if (scan !=0) {
    ptr->scan=1;
    ptr->magic='z';
  } else ptr->magic='x';
  ptr->stored=store;
  ptr->bmnum=bmnum;
   
  /* work out which ranges to store */
  min_pwr=-10000;   
  rng_num=0;
  for (loop=0;loop<MAX_RANGE;loop++) {
    if (sptr->rng[bmnum][loop][cnt].gsct !=0) ptr->store[MAX_RANGE+loop]=1;
    if ((sptr->sct[bmnum][loop][cnt] !=0) && 
        (sptr->rng[bmnum][loop][cnt].p_l > min_pwr)) {
      ptr->store[loop]=1;
      rng_num++;     }    
  }
  if (rng_num>0) { 
    for (loop=0;loop<MAX_RANGE;loop++) {
      if (ptr->store[loop]  !=0) {
        int p_l_c=0,v_c=0,w_l_c=0;
        if ((store & 0x01) !=0) { 
          p_l_c=(sptr->rng[bmnum][loop][cnt].p_l/pmax)*256;
          p_l_c=(p_l_c >255) ? 255 : p_l_c;
          p_l_c=(p_l_c <0) ? 0 : p_l_c;
          ptr->store[2*MAX_RANGE+loop]=p_l_c;
        }
        if ((store & 0x02) !=0) {
          v_c=(1+(sptr->rng[bmnum][loop][cnt].v/vmax))*128;
          v_c=(v_c >255) ? 255 : v_c;
          v_c=(v_c < 0) ? 0 : v_c;
          ptr->store[3*MAX_RANGE+loop]=v_c;
        }
        if ((store & 0x04) !=0) {
          w_l_c=(sptr->rng[bmnum][loop][cnt].w_l/wmax)*256;
          w_l_c=(w_l_c >255) ? 255 : w_l_c;
          w_l_c=(w_l_c <0) ? 0 : w_l_c;
          ptr->store[4*MAX_RANGE+loop]=w_l_c; 
        }
      } 
    }   
  }
  return 0;
}
 








