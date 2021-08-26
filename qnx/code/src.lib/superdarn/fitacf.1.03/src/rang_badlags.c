/* rang_badlags.c  
   ============== */

/*
 $Log: rang_badlags.c,v $
 Revision 1.1  1998/06/05 19:56:46  barnes
 Initial revision

 */

#include <stdio.h>
#include <math.h>
#include "fit_blk.h"

#define MIN_PWR_RATIO	.3

static int range_overlap[PULSE_PAT_LEN][PULSE_PAT_LEN];

/*  r_overlap sets up the table r_overlap which keeps track of the
 *  ranges which might cause interference.
 */ 

void r_overlap(struct fit_prm *ptr) {
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

void lag_overlap(int range,int *badlag,struct fit_prm *ptr) {
     
  int ck_pulse;
  int pulse;
  int lag;
  int ck_range;
  long min_pwr;
  long pwr_ratio;
  int bad_pulse[PULSE_PAT_LEN];  /* 1 if there is a bad pulse */
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

