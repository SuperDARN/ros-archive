/* do_fit.h
   ========
   Author: R.J.Barnes
*/

#ifndef _FIT_DO_FIT_H
#define _FIT_DO_FIT_H

#ifndef _FIT_BLK_H
#include "fit_blk.h"
#endif

int do_fit(struct fit_block *iptr,int lag_lim,int goose,
	   struct range_data *ptr,struct range_data *xptr,struct elv *elv,
		   struct noise *nptr);

#endif
