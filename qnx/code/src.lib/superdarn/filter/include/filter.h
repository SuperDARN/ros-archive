/* filter.h
   ======== */

#ifndef _FILTER_FILTER_H
#define _FILTER_FILTER_H

#ifndef _RADAR_SCAN_H
#include "radar_scan.h"
#endif



void filter(int mode,struct radardata *input,
		 struct radardata *output);
void filter_raw(int mode,struct radardata *input,
		struct radardata *output,
		int prm);

#endif
