/* smrwrite.c
   ==========
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"

/* 
 $Log: smrwrite.c,v $
 Revision 1.4  2004/05/04 18:35:57  barnes
 Renamed the type header to avoid confusion.

 Revision 1.3  2004/04/28 18:28:50  barnes
 Fixed time_t.

 Revision 1.2  2004/04/26 22:33:46  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2003/09/13 22:39:29  barnes
 Initial revision

*/


int SmrHeaderFwrite(FILE * fp,char *version,char *text,char *rname) {
  time_t ctime;
  int status;

  /* first, clear the header buffer */

  fprintf(fp, "sd_summary %s\n",version);
  fprintf(fp, "%s\n",text);

  ctime = time((time_t *) 0);
  status = fprintf(fp, "%s %s\n", rname, asctime(gmtime(&ctime)));
  if (status==-1) return -1;
  return 0;
}


int SmrFwrite(FILE *fp,struct RadarParm *prm,struct FitData *fit,int pwr) {
  int rec_time;
  int status;
  int i,n,ngood=0;
  char rng[MAX_RANGE];

  for (i=0; i < prm->nrang; ++i) 
    if ((fit->rng[i].qflg == 1) && (fit->rng[i].p_0 >= pwr)) {
      rng[i]=1;
      ++ngood;
     } else rng[i] = 0;
		
  
	
  /* now print the time stamp and header info */
			
  rec_time = TimeYMDHMSToYrsec(prm->time.yr,prm->time.mo,prm->time.dy,
                               prm->time.hr,prm->time.mt,prm->time.sc);
  status = fprintf(fp,
     "time = %4hd %d %2hd %2hd %6hd %d %hd %2hd %3hd %4hd %3hd %3hd %3hd\n",
				  prm->time.yr,
				  rec_time,
				  prm->stid,
				  prm->bmnum,
				  prm->tfreq,
				  (int) prm->noise.search,
				  ngood,
				  prm->atten*10,
				  prm->nrang,
				  prm->frang,
				  prm->rsep,prm->cp,
                                  prm->channel);
  if (status <0) return -1;
 			
   /* now put out the list of good ranges */
   n = 0;
   for (i = 0; i < prm->nrang; ++i) if (rng[i] == 1) {
    
     status = fprintf(fp, "%3d", i);
	 if (status < 0 ) return -1;		
	 ++n;
     if (n >= 25) {
	   fprintf(fp, "\n");
	   n = 0;
	 }
   }
   if (n != 0) fprintf(fp, "\n");
		
   for (i=0; i < prm->nrang; ++i) if (rng[i] == 1) {
    status = fprintf(fp, "%8.1f", fit->rng[i].p_l);
    if (status<0) return -1;
	++n;
    if (n >= 25) {
	  fprintf(fp, "\n");
	  n = 0;
	}
  }
  if (n != 0) fprintf(fp, "\n");

  for (i=0; i < prm->nrang; ++i) if (rng[i] == 1) {
    status = fprintf(fp, "%8.1f", fit->rng[i].v);
    if (status<0) return -1;
	++n;
    if (n >= 25) {
	  fprintf(fp, "\n");
	  n = 0;
	}
  }
  if (n != 0) fprintf(fp, "\n");

  for (i=0; i < prm->nrang; ++i) if (rng[i] == 1) {
    status = fprintf(fp, "%7.0f", fit->rng[i].w_l);
    if (status<0) return -1;
	++n;
    if (n >= 25) {
	  fprintf(fp, "\n");
	  n = 0;
	}
  }
  if (n != 0) fprintf(fp, "\n");
  return 0;
}

 
