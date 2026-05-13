/* rang_badlags.c
   ==============
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
 $Log: rang_badlags.c,v $
 Revision 1.4  2003/09/13 22:39:29  barnes
 Modifications to use the new data structures.

 Revision 1.3  2001/06/27 20:48:31  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:53  barnes
 Added Author Name

 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include <stdio.h>
#include <math.h>
#include "limit.h"
#include "fitblk.h"

#define MIN_PWR_RATIO	.3

static int range_overlap[PULSE_SIZE][PULSE_SIZE];

/*  r_overlap sets up the table r_overlap which keeps track of the
 *  ranges which might cause interference.
 */ 

void r_overlap(struct FitPrm *ptr) {
  int ck_pulse;
  int pulse;
  int tau;

  int diff_pulse;

  /* define constants */
  tau = ptr->mpinc / ptr->smsep;
 
  for (ck_pulse = 0; ck_pulse < ptr->mppul; ++ck_pulse) {
    for (pulse = 0; pulse < ptr->mppul; ++pulse) {
      diff_pulse = ptr->pulse[ck_pulse] - 
                      ptr->pulse[pulse];
      range_overlap[ck_pulse][pulse] = diff_pulse * tau;
    }
  }
  return;
} 


/* lag_overlap marks the badlag array for bad lags */

void lag_overlap(int range,int *badlag,struct FitPrm *ptr) {
     
  int ck_pulse;
  int pulse;
  int lag;
  int ck_range;
  long min_pwr;
  long pwr_ratio;
  int bad_pulse[PULSE_SIZE];  /* 1 if there is a bad pulse */
  int i;
  double nave;
   
  --range;  /* compensate for the index which starts from 0 instead of 1 */

  nave = (double) (ptr->nave);

  for (pulse = 0; pulse < ptr->mppul; ++pulse)
      bad_pulse[pulse] = 0;

  for (ck_pulse = 0;  ck_pulse < ptr->mppul; ++ck_pulse) {
    for (pulse = 0; pulse < ptr->mppul; ++pulse) {
      ck_range = range_overlap[ck_pulse][pulse] + range;
      if ((pulse != ck_pulse) && (0 <= ck_range) && 
	      (ck_range < ptr->nrang)) {
        pwr_ratio = (long) (nave * MIN_PWR_RATIO);
        min_pwr =  pwr_ratio * ptr->pwr0[range];
        if(min_pwr < ptr->pwr0[ck_range])
        bad_pulse[ck_pulse] = 1;
      }
    } 
  }           
  
  /* mark the bad lag */

  for (pulse = 0 ; pulse < ptr->mppul; ++pulse) {
    if (bad_pulse[pulse] == 1) {
      for (i=0; i < 2 ; ++i) {
        for (lag = 0 ; lag < ptr->mplgs ; ++lag) {
          if (ptr->lag[i][lag] == ptr->pulse[pulse])
            badlag[lag] = 1;  /* 1 for bad lag */
        }
      }
    }
  } 
  return;
} 

