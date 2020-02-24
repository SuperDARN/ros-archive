/* fitdata.h
   =========
   Author: R.J.Barnes
*/


#ifndef _FITDATA_H
#define _FITDATA_H

#ifndef _INT_TYPES_H
#include "types.h"
#endif

#ifndef _RADAR_LIMIT_H
#include "limit.h"
#endif

#ifndef _RAWDATA_H
#include "rawdata.h"
#endif


#ifndef _FIT_BLK
#include "fit_blk.h"
#endif

#define FIT_TYPE 1

/* This header defines the structure used for the fitted data produced
 *  by fitlib.
 */

struct fitdata { /* the data structure */
  struct radar_parms prms;
  int16 pulse[PULSE_PAT_LEN];
  int16 lag[2][LAG_TAB_LEN];
  char combf[COMBF_SIZE];
  struct noise noise;
  struct range_data rng[MAX_RANGE];
  struct range_data xrng[MAX_RANGE];
  struct elv elev[MAX_RANGE];
};
      
#endif
