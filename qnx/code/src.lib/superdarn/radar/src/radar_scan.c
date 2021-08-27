/* radar_scan.c
   ============ */

/*
 $Log: radar_scan.c,v $
 Revision 1.4  2000/03/21 20:32:09  barnes
 Added setting the station id to the code.

 Revision 1.3  2000/03/21 20:16:27  barnes
 Modifications to the copy_scan function to make it do what it is suppossed
 to do.

 Revision 1.2  2000/03/21 16:10:35  barnes
 Added the function add_cmp_scan.

 Revision 1.1  2000/03/21 15:31:52  barnes
 Initial revision

 */


/* manipulates scan data */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rawdata.h"
#include "fitdata.h"
#include "cmpdata.h"
#include "radar_scan.h"
#include "cnv_data.h"
#include "cnv_time.h"

void  reset_scan(struct radardata *ptr) {
  int bmnum,rng,cnt;
  for (bmnum=0;bmnum<16;bmnum++) {
    cnt=ptr->bmcnt[bmnum];
   
    if (ptr->cpid[bmnum]==NULL) ptr->cpid[bmnum]=malloc(sizeof(int));
    else if (cnt>1) ptr->cpid[bmnum]=realloc(ptr->cpid[bmnum],sizeof(int));

    if (ptr->intt[bmnum]==NULL) ptr->intt[bmnum]=malloc(sizeof(int));
    else if (cnt>1) ptr->intt[bmnum]=realloc(ptr->intt[bmnum],sizeof(int));

    if (ptr->frang[bmnum]==NULL) ptr->frang[bmnum]=malloc(sizeof(int));
    else if (cnt>1) ptr->frang[bmnum]=realloc(ptr->frang[bmnum],sizeof(int));

    if (ptr->rsep[bmnum]==NULL) ptr->rsep[bmnum]=malloc(sizeof(int));
    else if (cnt>1)  ptr->rsep[bmnum]=realloc(ptr->rsep[bmnum],sizeof(int));

    if (ptr->rxrise[bmnum]==NULL) ptr->rxrise[bmnum]=malloc(sizeof(int));
    else if (cnt>1) ptr->rxrise[bmnum]=realloc(ptr->rxrise[bmnum],sizeof(int));

    if (ptr->freq[bmnum]==NULL) ptr->freq[bmnum]=malloc(sizeof(int));
    else if (cnt>1) ptr->freq[bmnum]=realloc(ptr->freq[bmnum],sizeof(int));

    if (ptr->noise[bmnum]==NULL) ptr->noise[bmnum]=malloc(sizeof(int));
    else if (cnt>1) ptr->noise[bmnum]=realloc(ptr->noise[bmnum],sizeof(int));

    if (ptr->atten[bmnum]==NULL) ptr->atten[bmnum]=malloc(sizeof(int));
    else if (cnt>1) ptr->atten[bmnum]=realloc(ptr->atten[bmnum],sizeof(int));

    if (ptr->time[bmnum]==NULL) ptr->time[bmnum]=malloc(sizeof(double));
    else if (cnt>1) ptr->time[bmnum]=realloc(ptr->time[bmnum],sizeof(double));
  
    for (rng=0;rng<MAX_RANGE;rng++) {

       if (ptr->sct[bmnum][rng]==NULL) 
            ptr->sct[bmnum][rng]=malloc(sizeof(char));
       else if (cnt>1) ptr->sct[bmnum][rng]=
		realloc(ptr->sct[bmnum][rng],sizeof(char));


       if (ptr->rng[bmnum][rng]==NULL) 
            ptr->rng[bmnum][rng]=malloc(sizeof(struct datapoint));
       else if (cnt>1) ptr->rng[bmnum][rng]=
		realloc(ptr->rng[bmnum][rng],sizeof(struct datapoint));

     }
     ptr->bmcnt[bmnum]=0;
   } 
}

void add_cmp_scan(struct radardata *ptr,struct cmpdata *cmp,
                  struct cmphdr *hdr) {
  int bmnum,rng,cnt;

  bmnum=cmp->bmnum;
  cnt=ptr->bmcnt[bmnum];
  ptr->bmcnt[bmnum]++;
  ptr->st_id=hdr->st_id;
  if (cnt>0) {
    ptr->cpid[bmnum]=realloc(ptr->cpid[bmnum],(cnt+1)*sizeof(int));
    ptr->intt[bmnum]=realloc(ptr->intt[bmnum],(cnt+1)*sizeof(int));
    ptr->frang[bmnum]=realloc(ptr->frang[bmnum],(cnt+1)*sizeof(int));
    ptr->rsep[bmnum]=realloc(ptr->rsep[bmnum],(cnt+1)*sizeof(int));
    ptr->rxrise[bmnum]=realloc(ptr->rxrise[bmnum],(cnt+1)*sizeof(int));
    ptr->freq[bmnum]=realloc(ptr->freq[bmnum],(cnt+1)*sizeof(int));
    ptr->noise[bmnum]=realloc(ptr->noise[bmnum],(cnt+1)*sizeof(int));
    ptr->atten[bmnum]=realloc(ptr->atten[bmnum],(cnt+1)*sizeof(int));
    ptr->time[bmnum]=realloc(ptr->time[bmnum],(cnt+1)*sizeof(double));
  }
  
 

  ptr->cpid[bmnum][cnt]=0;
  ptr->intt[bmnum][cnt]=7;
  ptr->frang[bmnum][cnt]=cmp->frang;
  ptr->rsep[bmnum][cnt]=cmp->rsep;
  ptr->rxrise[bmnum][cnt]=0;
  ptr->freq[bmnum][cnt]=cmp->tfreq;
  ptr->noise[bmnum][cnt]=cmp->noise;
  ptr->atten[bmnum][cnt]=0;
  ptr->time[bmnum][cnt]=cmp->time;

  for (rng=0;rng<MAX_RANGE;rng++) {

    if (cnt>0) ptr->sct[bmnum][rng]=
      realloc(ptr->sct[bmnum][rng],(cnt+1)*sizeof(char));  
    ptr->sct[bmnum][rng][cnt]=cmp->store[rng];
    if (cnt>0)  ptr->rng[bmnum][rng]=
      realloc(ptr->rng[bmnum][rng],(cnt+1)*sizeof(struct datapoint));


    ptr->rng[bmnum][rng][cnt].gsct=cmp->store[rng+MAX_RANGE];
    ptr->rng[bmnum][rng][cnt].p_l=hdr->pmax*
                                    cmp->store[rng+2*MAX_RANGE]/256.0; 
    ptr->rng[bmnum][rng][cnt].w_l=hdr->wmax*
                                    cmp->store[rng+4*MAX_RANGE]/256.0; 
    ptr->rng[bmnum][rng][cnt].v=hdr->vmax*
                                    (cmp->store[rng+3*MAX_RANGE]-128.0)/128.0; 
    ptr->rng[bmnum][rng][cnt].v_e=50.0;

  
	 
  }
}


void add_fit_scan(struct radardata *ptr, struct fitdata *fit) {
  int bmnum,rng,cnt;
  
  bmnum=fit->prms.BMNUM;
  cnt=ptr->bmcnt[bmnum];
  ptr->bmcnt[bmnum]++;
  ptr->st_id=fit->prms.ST_ID;  
  if (cnt>0) {
    ptr->cpid[bmnum]=realloc(ptr->cpid[bmnum],(cnt+1)*sizeof(int));
    ptr->intt[bmnum]=realloc(ptr->intt[bmnum],(cnt+1)*sizeof(int));
    ptr->frang[bmnum]=realloc(ptr->frang[bmnum],(cnt+1)*sizeof(int));
    ptr->rsep[bmnum]=realloc(ptr->rsep[bmnum],(cnt+1)*sizeof(int));
    ptr->rxrise[bmnum]=realloc(ptr->rxrise[bmnum],(cnt+1)*sizeof(int));
    ptr->freq[bmnum]=realloc(ptr->freq[bmnum],(cnt+1)*sizeof(int));
    ptr->noise[bmnum]=realloc(ptr->noise[bmnum],(cnt+1)*sizeof(int));
    ptr->atten[bmnum]=realloc(ptr->atten[bmnum],(cnt+1)*sizeof(int));
    ptr->time[bmnum]=realloc(ptr->time[bmnum],(cnt+1)*sizeof(double));
  }
  
  ptr->cpid[bmnum][cnt]=fit->prms.CP;
  ptr->intt[bmnum][cnt]=fit->prms.INTT;
  ptr->frang[bmnum][cnt]=fit->prms.FRANG;
  ptr->rsep[bmnum][cnt]=fit->prms.RSEP;
  ptr->rxrise[bmnum][cnt]=fit->prms.RXRISE;
  ptr->freq[bmnum][cnt]=fit->prms.TFREQ;
  ptr->noise[bmnum][cnt]=fit->prms.NOISE;
  ptr->atten[bmnum][cnt]=fit->prms.ATTEN;
  ptr->time[bmnum][cnt]=time_epoch(fit->prms.YEAR,fit->prms.MONTH,
                                   fit->prms.DAY,fit->prms.HOUR,
				   fit->prms.MINUT,fit->prms.SEC);


  for (rng=0;rng<MAX_RANGE;rng++) {

    if (cnt>0) ptr->sct[bmnum][rng]=
	realloc(ptr->sct[bmnum][rng],(cnt+1)*sizeof(char));
    if (cnt>0) ptr->rng[bmnum][rng]=
	realloc(ptr->rng[bmnum][rng],(cnt+1)*sizeof(struct datapoint));
	 
     ptr->sct[bmnum][rng][cnt]=(fit->rng[rng].qflg==1);
     ptr->rng[bmnum][rng][cnt].gsct=fit->rng[rng].gsct;
     ptr->rng[bmnum][rng][cnt].v=fit->rng[rng].v;
     ptr->rng[bmnum][rng][cnt].p_l=fit->rng[rng].p_l;
     ptr->rng[bmnum][rng][cnt].w_l=fit->rng[rng].w_l;
     ptr->rng[bmnum][rng][cnt].v_e=fit->rng[rng].v_err;
	 
  }
  
}

void copy_scan(struct radardata *dst,
	      struct radardata *src) {

  /* reads one scans worth of data from the current position 
   *  in the fit file.
   */
   int bmnum,cnt;
   int rng,i,j,c=0;
   int fstatus=0;

   dst->scnt=src->scnt;
   dst->st_id=src->st_id;
   dst->stored=src->stored;
   dst->major_rev=src->major_rev;
   dst->minor_rev=src->minor_rev;
   dst->ftype=src->ftype;
   dst->stime=src->stime;
   dst->etime=src->etime;
  
   for (i=0;i<16;i++) {
     dst->bmcnt[i]=src->bmcnt[i];
     cnt=dst->bmcnt[i];
     if (cnt==0) continue;
    
     if (dst->cpid[i]==NULL) dst->cpid[i]=malloc(sizeof(int)*cnt);
     else dst->cpid[i]=realloc(dst->cpid[i],sizeof(int)*cnt);

     if (dst->intt[i]==NULL) dst->intt[i]=malloc(sizeof(int)*cnt);
     else dst->intt[i]=realloc(dst->intt[i],sizeof(int)*cnt);

     if (dst->frang[i]==NULL) dst->frang[i]=malloc(sizeof(int)*cnt);
     else dst->frang[i]=realloc(dst->frang[i],sizeof(int)*cnt);

    if (dst->rsep[i]==NULL) dst->rsep[i]=malloc(sizeof(int)*cnt);
     else dst->rsep[i]=realloc(dst->rsep[i],sizeof(int)*cnt);

     if (dst->rxrise[i]==NULL) dst->rxrise[i]=malloc(sizeof(int)*cnt);
     else dst->rxrise[i]=realloc(dst->rxrise[i],sizeof(int)*cnt);

     if (dst->freq[i]==NULL) dst->freq[i]=malloc(sizeof(int)*cnt);
     else dst->freq[i]=realloc(dst->freq[i],sizeof(int)*cnt);

     if (dst->noise[i]==NULL) dst->noise[i]=malloc(sizeof(int)*cnt);
     else dst->noise[i]=realloc(dst->noise[i],sizeof(int)*cnt);

     if (dst->atten[i]==NULL) dst->atten[i]=malloc(sizeof(int)*cnt);
     else dst->atten[i]=realloc(dst->atten[i],sizeof(int)*cnt);

     if (dst->time[i]==NULL) dst->time[i]=malloc(sizeof(double)*cnt);
     else dst->time[i]=realloc(dst->time[i],sizeof(double)*cnt);
  
     memcpy(dst->cpid[i],src->cpid[i],cnt*sizeof(int));
     memcpy(dst->intt[i],src->intt[i],cnt*sizeof(int));
     memcpy(dst->frang[i],src->frang[i],cnt*sizeof(int));
     memcpy(dst->rsep[i],src->rsep[i],cnt*sizeof(int));
     memcpy(dst->rxrise[i],src->rxrise[i],cnt*sizeof(int));
     memcpy(dst->freq[i],src->freq[i],cnt*sizeof(int));
     memcpy(dst->noise[i],src->noise[i],cnt*sizeof(int));
     memcpy(dst->atten[i],src->atten[i],cnt*sizeof(int));
     memcpy(dst->time[i],src->time[i],cnt*sizeof(double));

     
     for (rng=0;rng<MAX_RANGE;rng++) {
        
        if (dst->sct[i][rng]==NULL) 
          dst->sct[i][rng]=malloc(sizeof(char)*cnt);
        else dst->sct[i][rng]=realloc(dst->sct[i][rng],sizeof(char)*cnt);
        if (dst->rng[i][rng]==NULL) 
          dst->rng[i][rng]=malloc(sizeof(struct datapoint)*cnt);
        else dst->rng[i][rng]=realloc(dst->rng[i][rng],
                                      sizeof(struct datapoint)*cnt);
      
         memcpy(dst->sct[i][rng],src->sct[i][rng],cnt*sizeof(char));
         memcpy(dst->rng[i][rng],src->rng[i][rng],
		cnt*sizeof(struct datapoint));
     }
    
     
   }  
}

  
