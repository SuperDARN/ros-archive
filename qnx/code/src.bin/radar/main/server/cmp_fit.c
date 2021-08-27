/* cmp_fit.c
   ========= */

/*
 $Log: cmp_fit.c,v $
 Revision 2.3  1999/03/29 16:00:36  barnes
 Fixed problem with the scan flag.

 Revision 2.2  1998/11/09 23:03:03  barnes
 Incorporated rawdata header.

 Revision 2.1  1997/12/18 13:24:03  root
 New version of the server task using pipes and non-blocking sockets.

 */

#include <stdio.h>
#include <stdlib.h>

#include "rawdata.h"
#include "fitdata.h"
#include "cmp_fit.h"

int compress_fit(struct fitdata *fit_data,struct frame_data *frame) {
  int rng_num,loop;
  double min_pwr=3.0;
  frame->msg='d';

  /* copy the radar parameter block */
  memcpy(&frame->prms,&fit_data->prms,sizeof(struct radar_parms));
  memset(frame->gsct,0,10);
  memset(frame->dflg,0,10);

  /* work out which ranges to store */
   
  rng_num=0;
  for (loop=0;loop<MAX_RANGE;loop++) {
    if (fit_data->rng[loop].gsct !=0) set_bit(frame->gsct,loop);
    if ((fit_data->rng[loop].qflg==1) && 
        (fit_data->rng[loop].p_0 > min_pwr)) {
      set_bit(frame->dflg,loop);
      rng_num++;
    }
  }
 
  /* store the ranges */
      
  if (rng_num>0) {
    int index=0;
    for (loop=0;loop<MAX_RANGE;loop++) {
      if (read_bit(frame->dflg,loop) !=0) {
        frame->store[index]=fit_data->rng[loop].p_l;
        frame->store[index+rng_num]=fit_data->rng[loop].v;
        frame->store[index+2*rng_num]=fit_data->rng[loop].w_l;  
//      frame->store[index+2*rng_num]=fit_data->rng[loop].v_err;
        index++;
      } 
    }
  }
  return (sizeof(struct radar_parms)+21+3*sizeof(double)*rng_num);
}
