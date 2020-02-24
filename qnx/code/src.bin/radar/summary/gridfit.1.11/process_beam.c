/* process_beam.c
   ==============
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
 $Log: process_beam.c,v $
 Revision 2.11  2001/06/27 20:56:49  barnes
 Added license tag

 Revision 2.10  2001/01/29 18:12:09  barnes
 Added Author Name

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









