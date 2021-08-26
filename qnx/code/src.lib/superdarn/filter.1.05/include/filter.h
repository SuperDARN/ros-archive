/* filter.h
   ======== */

#ifndef _FILTER_FILTER_H
#define _FILTER_FILTER_H

#ifndef _RADAR_SCAN_H
#include "radar_scan.h"
#endif


void filter_raw(int mode,struct radardata *input,
		struct radardata *output,
		int prm);
int middle(struct radardata *input,
           struct radardata *output);


#endif
