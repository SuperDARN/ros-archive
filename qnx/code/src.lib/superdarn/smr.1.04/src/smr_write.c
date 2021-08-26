/* write_smr.c
   =========== */

/*
 $Log: smr_write.c,v $
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
            "time = %4hd %ld %2hd %2hd %6hd %ld %hd %2hd %3hd %4hd %3hd\n",
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
				  ptr->prms.RSEP);
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

 
