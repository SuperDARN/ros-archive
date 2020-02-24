/* cmp_fit.c
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

/*
 $Log: cmp_fit.c,v $
 Revision 2.6  2001/09/05 21:06:24  barnes
 Modification to encode XCF elevation angles in the data stream.

 Revision 2.5  2001/06/27 20:56:29  barnes
 Added license tag

 Revision 2.4  2001/01/29 18:12:08  barnes
 Added Author Name

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


int compress_xcf(struct fitdata *fit_data,struct frame_data *frame) {
  int rng_num,loop;
  double min_pwr=3.0;
  frame->msg='x';

  /* copy the radar parameter block */
  memcpy(&frame->prms,&fit_data->prms,sizeof(struct radar_parms));
  memset(frame->gsct,0,10);
  memset(frame->dflg,0,10);

  /* work out which ranges to store */
   
  rng_num=0;
  for (loop=0;loop<MAX_RANGE;loop++) {
    if (fit_data->xrng[loop].gsct !=0) set_bit(frame->gsct,loop);
    if ((fit_data->xrng[loop].qflg==1) && 
        (fit_data->xrng[loop].p_0 > min_pwr)) {
      set_bit(frame->dflg,loop);
      rng_num++;
    }
  }
 
  /* store the ranges */
      
  if (rng_num>0) {
    int index=0;
    for (loop=0;loop<MAX_RANGE;loop++) {
      if (read_bit(frame->dflg,loop) !=0) {
        frame->store[index]=fit_data->xrng[loop].p_l;
        frame->store[index+rng_num]=fit_data->elev[loop].low;
        frame->store[index+2*rng_num]=fit_data->elev[loop].normal;  
        frame->store[index+3*rng_num]=fit_data->elev[loop].high;
        index++;
      } 
    }
  }
  return (sizeof(struct radar_parms)+21+4*sizeof(double)*rng_num);
}
