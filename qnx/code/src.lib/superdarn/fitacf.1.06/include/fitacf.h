/* fitacf.h
   ========
   Author: R.J.Barnes
*/

#ifndef _FIT_FITACF_H
#define _FIT_FITACT_H

#ifndef _HARDWARE_H
#include "hardware.h"
#endif

#ifndef _FIT_BLK_H
#include "fit_blk.h"
#endif

#ifndef _RAWDATA_H
#include "rawdata.h"
#endif

#ifndef _FITDATA_H
#include "fitdata.h"
#endif

void end_fit(struct fit_block *fptr); 
void start_fit(struct radar_site *hd,int year,struct fit_block *fptr);
void fitacf(struct rawdata *ptr,struct fit_block *input,
			struct fitdata *fit);

#endif
