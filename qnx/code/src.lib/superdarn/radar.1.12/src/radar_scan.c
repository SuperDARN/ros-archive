/* radar_scan.c
   ============
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
 $Log: radar_scan.c,v $
 Revision 1.11  2002/02/12 14:57:24  barnes
 Removed redundant headers.

 Revision 1.10  2002/01/31 16:36:38  barnes
 Moved the fit and cmp routines into the appropriate libraries. The radar
 library should not be dependent on the fit and cmp libraries.

 Revision 1.9  2002/01/30 17:46:59  barnes
 Added channel to scan structure.

 Revision 1.8  2002/01/29 21:49:07  barnes
 Added code to get the program id from cmp files.

 Revision 1.7  2001/06/27 20:49:59  barnes
 Added license tag

 Revision 1.6  2001/01/29 18:11:53  barnes
 Added Author Name

 Revision 1.5  2000/06/03 15:36:05  barnes
 Added the reset_scan_beam function to clear single beams of a scan.

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
#include "radar_scan.h"
#include "cnv_data.h"
#include "cnv_time.h"

void reset_scan_beam(struct radardata *ptr,int bmnum) {
  int rng,cnt;
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
 
 if (ptr->channel[bmnum]==NULL) ptr->channel[bmnum]=malloc(sizeof(int));
  else if (cnt>1) ptr->channel[bmnum]=realloc(ptr->channel[bmnum],sizeof(int));
 

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


void  reset_scan(struct radardata *ptr) {
  int bmnum;
  for (bmnum=0;bmnum<16;bmnum++) reset_scan_beam(ptr,bmnum);
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

    if (dst->channel[i]==NULL) dst->channel[i]=malloc(sizeof(int)*cnt);
     else dst->channel[i]=realloc(dst->channel[i],sizeof(int)*cnt);


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
     memcpy(dst->channel[i],src->channel[i],cnt*sizeof(int));
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

  
