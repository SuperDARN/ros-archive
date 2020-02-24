/* smr_write.c
   ===========
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
 $Log: smr_write.c,v $
 Revision 1.5  2002/01/31 16:13:57  barnes
 Added channel and program id to file format.

 Revision 1.4  2001/06/27 20:50:13  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:11:54  barnes
 Added Author Name

 Revision 1.2  2000/01/11 18:52:58  barnes
 Fixed bug in recording the number of good beams.

 Revision 1.1  1999/03/16 20:09:54  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "rawdata.h"
#include "fitdata.h"
#include "cnv_data.h"

int smr_header_fwrite(FILE * fp,char *version,char *text,char *rname) {
  long ctime;
  int status;

  /* first, clear the header buffer */

  fprintf(fp, "sd_summary %s\n",version);
  fprintf(fp, "%s\n",text);

  ctime = time((long *) 0);
  status = fprintf(fp, "%s %s\n", rname, asctime(gmtime(&ctime)));
  if (status==-1) return -1;
  return 0;
}


int smr_fwrite(FILE *fp,struct fitdata *ptr,int pwr) {
  int rec_time;
  int status;
  int i,n,ngood=0;
  char rng[MAX_RANGE];

 

  for (i=0; i < ptr->prms.NRANG; ++i) 
    if ((ptr->rng[i].qflg == 1) && (ptr->rng[i].p_0 >= pwr)) {
      rng[i]=1;
      ++ngood;
     } else rng[i] = 0;
		
  
	
  /* now print the time stamp and header info */
			
  rec_time = time_yrsec(ptr->prms.YEAR,
						  ptr->prms.MONTH,
						  ptr->prms.DAY,
						  ptr->prms.HOUR,
						  ptr->prms.MINUT,
						  ptr->prms.SEC);

  status = fprintf(fp,
     "time = %4hd %ld %2hd %2hd %6hd %ld %hd %2hd %3hd %4hd %3hd %3hd %3hd\n",
				  ptr->prms.YEAR,
				  rec_time,
				  ptr->prms.ST_ID,
				  ptr->prms.BMNUM,
				  ptr->prms.TFREQ,
				  ptr->prms.NOISE,
				  ngood,
				  ptr->prms.ATTEN*10,
				  ptr->prms.NRANG,
				  ptr->prms.FRANG,
				  ptr->prms.RSEP,ptr->prms.CP,
                                  ptr->prms.CHANNEL);
  if (status <0) return -1;
 			
   /* now put out the list of good ranges */
   n = 0;
   for (i = 0; i < ptr->prms.NRANG; ++i) if (rng[i] == 1) {
    
     status = fprintf(fp, "%3d", i);
	 if (status < 0 ) return -1;		
	 ++n;
     if (n >= 25) {
	   fprintf(fp, "\n");
	   n = 0;
	 }
   }
   if (n != 0) fprintf(fp, "\n");
		
   for (i=0; i < ptr->prms.NRANG; ++i) if (rng[i] == 1) {
    status = fprintf(fp, "%8.1f", ptr->rng[i].p_l);
    if (status<0) return -1;
	++n;
    if (n >= 25) {
	  fprintf(fp, "\n");
	  n = 0;
	}
  }
  if (n != 0) fprintf(fp, "\n");

  for (i=0; i < ptr->prms.NRANG; ++i) if (rng[i] == 1) {
    status = fprintf(fp, "%8.1f", ptr->rng[i].v);
    if (status<0) return -1;
	++n;
    if (n >= 25) {
	  fprintf(fp, "\n");
	  n = 0;
	}
  }
  if (n != 0) fprintf(fp, "\n");

  for (i=0; i < ptr->prms.NRANG; ++i) if (rng[i] == 1) {
    status = fprintf(fp, "%7.0f", ptr->rng[i].w_l);
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

 
