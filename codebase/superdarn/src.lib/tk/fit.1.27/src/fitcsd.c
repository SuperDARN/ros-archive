/* fitcsd.c
   ========
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
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
#include "csddata.h"





int FitToCSD(int store,double min_pwr,
	     double pmax,double vmax,double wmax,
             struct RadarParm *prm,
             struct FitData *fit,
	     struct CSDdata *ptr) {


  int loop,rng_num;
 
  ptr->pmax=pmax;
  ptr->vmax=vmax;
  ptr->wmax=wmax;
  ptr->version.major=CSD_MAJOR_REVISION;
  ptr->version.minor=CSD_MINOR_REVISION;
  ptr->stored=store;

  memset(ptr->store,0,MAX_RANGE*5);

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

  /* work out which ranges to store */
   
  rng_num=0;
  for (loop=0;loop<MAX_RANGE;loop++) {
    if (fit->rng[loop].gsct !=0) ptr->store[MAX_RANGE+loop]=1;
    if ((fit->rng[loop].qflg==1) && 
        (fit->rng[loop].p_0 > min_pwr)) {
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
  return 0;
}
 






