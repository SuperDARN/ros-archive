/* process_beam.c
   ============== */

/*
 $Log: process_beam.c,v $
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

void zero_scan(struct radardata *scan) {
  int bmnum,rng,cnt;
 
   scan->stime=-1;
   scan->etime=-1;

   for (bmnum=0;bmnum<16;bmnum++) {
    cnt=scan->bmcnt[bmnum];
    if (scan->cpid[bmnum]==NULL) scan->cpid[bmnum]=malloc(sizeof(int));
    else if (cnt>1) scan->cpid[bmnum]=realloc(scan->cpid[bmnum],sizeof(int));
    if (scan->intt[bmnum]==NULL) scan->intt[bmnum]=malloc(sizeof(int));
    else if (cnt>1) scan->intt[bmnum]=realloc(scan->intt[bmnum],sizeof(int));
    if (scan->frang[bmnum]==NULL) scan->frang[bmnum]=malloc(sizeof(int));
    else if (cnt>1) scan->frang[bmnum]=realloc(scan->frang[bmnum],sizeof(int));
    if (scan->rsep[bmnum]==NULL) scan->rsep[bmnum]=malloc(sizeof(int));
    else if (cnt>1)  scan->rsep[bmnum]=realloc(scan->rsep[bmnum],sizeof(int));
    if (scan->rxrise[bmnum]==NULL) scan->rxrise[bmnum]=malloc(sizeof(int));
    else if (cnt>1) 
      scan->rxrise[bmnum]=realloc(scan->rxrise[bmnum],sizeof(int));

    if (scan->freq[bmnum]==NULL) scan->freq[bmnum]=malloc(sizeof(int));
    else if (cnt>1) scan->freq[bmnum]=realloc(scan->freq[bmnum],sizeof(int));
    if (scan->noise[bmnum]==NULL) scan->noise[bmnum]=malloc(sizeof(int));
    else if (cnt>1) scan->noise[bmnum]=realloc(scan->noise[bmnum],sizeof(int));
    if (scan->atten[bmnum]==NULL) scan->atten[bmnum]=malloc(sizeof(int));
    else if (cnt>1) scan->atten[bmnum]=realloc(scan->atten[bmnum],sizeof(int));

    if (scan->time[bmnum]==NULL) scan->time[bmnum]=malloc(sizeof(double));
    else if (cnt>1) scan->time[bmnum]=realloc(scan->time[bmnum],
                                      sizeof(double));
  
    for (rng=0;rng<MAX_RANGE;rng++) {

       if (scan->sct[bmnum][rng]==NULL) 
            scan->sct[bmnum][rng]=malloc(sizeof(char));
       else if (cnt>1) scan->sct[bmnum][rng]=
		realloc(scan->sct[bmnum][rng],sizeof(char));


       if (scan->rng[bmnum][rng]==NULL) 
            scan->rng[bmnum][rng]=malloc(sizeof(struct datapoint));
       else if (cnt>1)  scan->rng[bmnum][rng]=
		realloc(scan->rng[bmnum][rng],sizeof(struct datapoint));

     }
     scan->bmcnt[bmnum]=0;
   } 
}

void process_beam(struct fitdata *fit,struct radardata *scan) {
   int rng,bmnum,cnt;

   if (fit->prms.SCAN==-1) return;

   if (fit->prms.SCAN==1) {
      scan->st_id=fit->prms.ST_ID;
      scan->stime=time_epoch(fit->prms.YEAR,fit->prms.MONTH,
			     fit->prms.DAY,fit->prms.HOUR,
			     fit->prms.MINUT,fit->prms.SEC);
   }
   
   bmnum=fit->prms.BMNUM;  
   cnt=scan->bmcnt[bmnum];
   scan->bmcnt[bmnum]++;


   if (scan->cpid[bmnum]==NULL) scan->cpid[bmnum]=malloc(sizeof(int));
   else if (cnt>1) scan->cpid[bmnum]=realloc(scan->cpid[bmnum],sizeof(int));
   if (scan->intt[bmnum]==NULL) scan->intt[bmnum]=malloc(sizeof(int));
   else scan->intt[bmnum]=realloc(scan->intt[bmnum],(cnt+1)*sizeof(int));
   if (scan->frang[bmnum]==NULL) scan->frang[bmnum]=malloc(sizeof(int));
   else scan->frang[bmnum]=realloc(scan->frang[bmnum],(cnt+1)*sizeof(int));
   if (scan->rsep[bmnum]==NULL) scan->rsep[bmnum]=malloc(sizeof(int));
   else scan->rsep[bmnum]=realloc(scan->rsep[bmnum],(cnt+1)*sizeof(int));
   if (scan->rxrise[bmnum]==NULL) scan->rxrise[bmnum]=malloc(sizeof(int));
   else scan->rxrise[bmnum]=realloc(scan->rxrise[bmnum],(cnt+1)*sizeof(int));

   if (scan->freq[bmnum]==NULL) scan->freq[bmnum]=malloc(sizeof(int));
   else scan->freq[bmnum]=realloc(scan->freq[bmnum],(cnt+1)*sizeof(int));
   if (scan->noise[bmnum]==NULL) scan->noise[bmnum]=malloc(sizeof(int));
   else scan->noise[bmnum]=realloc(scan->noise[bmnum],(cnt+1)*sizeof(int));
   if (scan->atten[bmnum]==NULL) scan->atten[bmnum]=malloc(sizeof(int));
   else scan->atten[bmnum]=realloc(scan->atten[bmnum],(cnt+1)*sizeof(int));
   if (scan->time[bmnum]==NULL) scan->time[bmnum]=malloc(sizeof(double));
   else scan->time[bmnum]=realloc(scan->time[bmnum],(cnt+1)*sizeof(double));

   scan->cpid[bmnum][cnt]=fit->prms.CP;
   scan->intt[bmnum][cnt]=fit->prms.INTT;
   scan->frang[bmnum][cnt]=fit->prms.FRANG;
   scan->rsep[bmnum][cnt]=fit->prms.RSEP;
   scan->rxrise[bmnum][cnt]=fit->prms.RXRISE;
   scan->freq[bmnum][cnt]=fit->prms.TFREQ;
   scan->noise[bmnum][cnt]=fit->prms.NOISE;
   scan->atten[bmnum][cnt]=fit->prms.ATTEN;

   for (rng=0;rng<MAX_RANGE;rng++) {
     if (scan->sct[bmnum][rng]==NULL) 
         scan->sct[bmnum][rng]=malloc(sizeof(char));
     else scan->sct[bmnum][rng]=
	 realloc(scan->sct[bmnum][rng],(cnt+1)*sizeof(char));
     scan->sct[bmnum][rng][cnt]=(fit->rng[rng].qflg>0);

     if (scan->rng[bmnum][rng]==NULL) 
       scan->rng[bmnum][rng]=malloc(sizeof(struct datapoint));
     else scan->rng[bmnum][rng]=
	realloc(scan->rng[bmnum][rng],(cnt+1)*sizeof(struct datapoint));

	 
     scan->rng[bmnum][rng][cnt].gsct=fit->rng[rng].gsct;
     scan->rng[bmnum][rng][cnt].v=fit->rng[rng].v;
     scan->rng[bmnum][rng][cnt].p_l=fit->rng[rng].p_l;
     scan->rng[bmnum][rng][cnt].w_l=fit->rng[rng].w_l;
     scan->rng[bmnum][rng][cnt].v_e=fit->rng[rng].v_err;
   }

   scan->etime=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
 		          fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);
}









