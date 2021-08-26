/* process_beam.c
   ============== */

/*
 $Log: process_beam.c,v $
 Revision 2.9  2000/09/18 18:03:55  barnes
 Removed redundant header.

 Revision 2.8  1999/12/15 20:23:38  barnes
 Added the receiver rise time to the structure.

 Revision 2.7  1999/10/14 15:07:11  barnes
 Modifications to include attenuation.

 Revision 2.6  1999/10/13 21:17:21  barnes
 Modifications for the extended file format.

 Revision 2.5  1999/09/13 22:36:29  barnes
 Fixed bug that meant the routine was defaulting to recording power.

 Revision 2.4  1999/01/27 18:51:31  barnes
 Modifications to cope with the new radardata structure.

 Revision 2.3  1998/11/09 23:09:12  barnes
 Incorporated rawdata header.

 Revision 2.2  1998/06/07 18:49:13  root
 Modifications to use the new radardata structure.

 Revision 2.1  1998/06/05 10:54:57  root
 New version using the master library.

 */



#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "rawdata.h"
#include "fitdata.h"
#include "cnv_time.h"
#include "radar_scan.h"


void process_beam(struct fitdata *fit,struct radardata *scan) {
   int rng,bmnum,cnt;

   if (fit->prms.SCAN==-1) return;

   if (fit->prms.SCAN==1) {
      scan->st_id=fit->prms.ST_ID;
      scan->stime=time_epoch(fit->prms.YEAR,fit->prms.MONTH,
			     fit->prms.DAY,fit->prms.HOUR,
			     fit->prms.MINUT,fit->prms.SEC);
   }
   
   add_fit_scan(scan,fit);

   scan->etime=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
 		          fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);
}









