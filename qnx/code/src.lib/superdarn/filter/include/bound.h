/* bound.h
   ======= */


#ifndef _FILTER_BOUND_H
#define _FILTER_BOUND_H

#ifndef _RADAR_SCAN_H
#include "radar_scan.h"
#endif


void bound_type(struct radardata *input,int type);
void bound(int prm,struct radardata *input,double *min,double *max);

#endif








