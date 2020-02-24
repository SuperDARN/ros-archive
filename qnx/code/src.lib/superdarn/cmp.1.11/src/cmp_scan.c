/* cmp_scan.c
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

/*
 $Log: cmp_scan.c,v $
 Revision 1.20  2002/02/07 21:28:20  barnes
 Modification to prevent possible screw up in decoding channel.

 Revision 1.19  2002/02/06 17:44:57  barnes
 Simplified code for filtering channel.

 Revision 1.18  2002/02/06 17:11:17  barnes
 Added code to filter based on channel.

 Revision 1.17  2001/06/27 20:46:56  barnes
 Added license tag

 Revision 1.16  2001/01/29 18:11:52  barnes
 Added Author Name

 Revision 1.15  2000/03/21 16:44:59  barnes
 Fixed bug in setting the file type in the scan_period function.

 Revision 1.14  2000/03/21 16:42:55  barnes
 Modifications to use the scan_reset and add_cmp_scan functions.

 Revision 1.13  2000/01/26 16:08:48  barnes
 Fixed bug that meant that beams were lost from the scan_period function.

 Revision 1.12  1999/12/15 20:03:58  barnes
 Added receiver rise time to structure.

 Revision 1.11  1999/10/14 14:29:48  barnes
 Fixed bug in attenuation code.

 Revision 1.10  1999/10/14 14:07:30  barnes
 Added record of the attenuator settings.

 Revision 1.9  1999/10/08 20:40:51  barnes
 Added program id to structure.

 Revision 1.8  1999/07/26 16:50:51  barnes
 Modifications so that we don't tailspin on files with bad scan flags -
 The idea is to limit the number of beams in a single scan to 32, any more
 than that and we force a scan flag to be set.

 Revision 1.7  1999/07/24 18:44:45  barnes
 Fixed bug that meant the version number was not being propagated scans read
 using cmp_read_scan_period.

 Revision 1.6  1999/07/24 18:30:29  barnes
 Added version information to the scan data.

 Revision 1.5  1999/07/23 19:28:23  barnes
 Modified the memory allocation system so that it is much more efficient.

 Revision 1.4  1999/05/21 15:16:59  barnes
 Fixed bugs in reading records.

 Revision 1.3  1999/05/19 21:48:13  barnes
 Fixed bug in the scan period function.

 Revision 1.2  1999/03/18 15:20:28  barnes
 Modified the internal format of the cmp file to use epoch time.

 Revision 1.1  1999/03/16 20:17:14  barnes
 Initial revision

 */



/* decodes fit type files */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmpdata.h"
#include "cmp_read.h"
#include "radar_scan.h"
#include "cnv_data.h"
#include "cnv_time.h"


int cmp_read_scan(struct cmpfp *fp,struct radardata *ptr,
		  struct cmpdata *cmp) {

  /* reads one scans worth of data from the current position 
   *  in the fit file.
   */
   int bmnum,cnt;
   int rng,i,j,c=0;
   int fstatus=0;

   if (fp==NULL) return -1;
   if (cmp->scan==255) return -1;

  

   /* if this is the first time through the file type flag 
    * will be zero - If you need to force a reset then set the
    * file type flag to zero
    *
    * So if this is the first call to the function, or we
    * are at the start of a new scan, reset the scan array. 
    */

  
   if ((ptr->ftype==0) || (cmp->scan==1)) {
   
     reset_scan(ptr);
    
     if (ptr->ftype==0) { 
       /* this is the first time we've been called so try and sync
        *  to a scan boundary. 
        */

       while ((cmp->scan==0) && (fstatus !=-1)) 
          fstatus=cmp_read(fp,cmp);
     }
     ptr->stime=cmp->time;
     ptr->ftype=CMP_SCAN;

   }

   /* new file opened so must read a record */

   if (cmp->scan==2) 
     fstatus=cmp_read(fp,cmp);

   
   ptr->stored=cmp->stored;
   ptr->st_id=fp->hdr.st_id;
   ptr->major_rev=fp->hdr.major_rev;
   ptr->minor_rev=fp->hdr.minor_rev;


   cmp->scan=0;
   while ((cmp->scan==0) && (fstatus !=-1)) {
     if (cmp->scan==0) {
   
       add_cmp_scan(ptr,cmp,&fp->hdr);   
       ptr->etime=cmp->time;    
        
     }
     fstatus=cmp_read(fp,cmp);
     if (fstatus !=-1) c++;

     if (c>256) { /* can't have 256 beams in a scan - probable file fault */
        fstatus=-1; 
        break;
     }
   }
 
   if ((fstatus !=-1) && (cmp->scan==1)) {
      ptr->etime=cmp->time;  
      return 1;
   }
   if (fstatus==-1) cmp->scan=255;
   if (c>0) return 0;
   return -1;
}

int cmp_read_scan_period(struct cmpfp *fp,struct radardata *ptr,
		  struct cmpdata *cmp,int tlen) {

  /* reads one scans worth of data from the current position 
   *  in the fit file.
   */
   double tm;
   int bmnum,cnt;
   int rng,i,j,c=0;
   int fstatus=0;

   if (fp==NULL) return -1;
   if (cmp->scan==255) return -1;


   /* if this is the first time through the file type flag 
    * will be zero - If you need to force a reset then set the
    * file type flag to zero
    *
    * So if this is the first call to the function, or we
    * are at the start of a new scan, reset the scan array. 
    */

   



   if ((ptr->ftype==0) || (cmp->scan==1)) {
     
     reset_scan(ptr);
    
     if (ptr->ftype==0) { 
       /* this is the first time we've been called so try and sync
        *  to a scan boundary. 
        */
         do {
           fstatus=cmp_read(fp,cmp);
         } while ((fstatus !=-1) && (fstatus !=0));  
     }
     ptr->stime=cmp->time;
     ptr->ftype=CMP_SCAN;

   }

  

   /* new file opened so must read a record */

   if (cmp->scan==2) {
      do {
           fstatus=cmp_read(fp,cmp);
         } while ((fstatus !=-1) && (fstatus !=0));  

   }

   ptr->stored=cmp->stored;
   ptr->st_id=fp->hdr.st_id;
   ptr->major_rev=fp->hdr.major_rev;
   ptr->minor_rev=fp->hdr.minor_rev;


   if (fstatus !=-1) 
     tm=cmp->time; 
   
   cmp->scan=0;
   while ((tm<(ptr->stime+tlen)) && (fstatus !=-1)) {
     add_cmp_scan(ptr,cmp,&fp->hdr);   
     ptr->etime=tm;
     fstatus=cmp_read(fp,cmp);
     if (fstatus !=-1) tm=cmp->time;  
  
     if (fstatus !=-1) c++;

     if (c>256) { /* can't have 256 beams in a scan - probable file fault */
        fstatus=-1; 
        break;
     }

   }



   if (fstatus !=-1) {
     cmp->scan=1;
     return 1;
   }
   if (fstatus==-1) cmp->scan=-1;
   if (c>0) return 0;
   return -1;
}



int cmp_read_scan_channel(struct cmpfp *fp,struct radardata *ptr,
		  struct cmpdata *cmp,int channel) {

  /* reads one scans worth of data from the current position 
   *  in the fit file.
   */
   int bmnum,cnt;
   int rng,i,j,c=0;
   int fstatus=0;

   if (fp==NULL) return -1;
   if (cmp->scan==255) return -1;

  

   /* if this is the first time through the file type flag 
    * will be zero - If you need to force a reset then set the
    * file type flag to zero
    *
    * So if this is the first call to the function, or we
    * are at the start of a new scan, reset the scan array. 
    */

  
   if ((ptr->ftype==0) || (cmp->scan==1)) {
   
     reset_scan(ptr);
    
     if (ptr->ftype==0) { 
       /* this is the first time we've been called so try and sync
        *  to a scan boundary. 
        */

       while ((cmp->scan==0) && (fstatus !=-1)) {
          fstatus=cmp_read(fp,cmp);
           if (((channel==1) && (cmp->channel==2)) ||
             ((channel==2) && (cmp->channel!=2))) cmp->scan=0;
       }
     }
     ptr->stime=cmp->time;
     ptr->ftype=CMP_SCAN;

   }

   /* new file opened so must read a record */

   if (cmp->scan==2) 
     fstatus=cmp_read(fp,cmp);

   
   ptr->stored=cmp->stored;
   ptr->st_id=fp->hdr.st_id;
   ptr->major_rev=fp->hdr.major_rev;
   ptr->minor_rev=fp->hdr.minor_rev;


   cmp->scan=0;
   while ((cmp->scan==0) && (fstatus !=-1)) {
     if ((cmp->scan==0) && 
          (((channel==1) && (cmp->channel!=2)) ||
	  ((channel==2) && (cmp->channel==2)))) {
       add_cmp_scan(ptr,cmp,&fp->hdr);   
       ptr->etime=cmp->time;    
        
     }
     fstatus=cmp_read(fp,cmp);
     if (((channel==1) && (cmp->channel==2)) ||
        ((channel==2) && (cmp->channel!=2))) cmp->scan=0;
     if (fstatus !=-1) c++;

     if (c>256) { /* can't have 256 beams in a scan - probable file fault */
        fstatus=-1; 
        break;
     }
   }
 
   if ((fstatus !=-1) && (cmp->scan==1)) {
      ptr->etime=cmp->time;  
      return 1;
   }
   if (fstatus==-1) cmp->scan=255;
   if (c>0) return 0;
   return -1;
}

int cmp_read_scan_period_channel(struct cmpfp *fp,struct radardata *ptr,
		  struct cmpdata *cmp,int tlen,int channel) {

  /* reads one scans worth of data from the current position 
   *  in the fit file.
   */
   double tm;
   int bmnum,cnt;
   int rng,i,j,c=0;
   int fstatus=0;

   if (fp==NULL) return -1;
   if (cmp->scan==255) return -1;


   /* if this is the first time through the file type flag 
    * will be zero - If you need to force a reset then set the
    * file type flag to zero
    *
    * So if this is the first call to the function, or we
    * are at the start of a new scan, reset the scan array. 
    */

   



   if ((ptr->ftype==0) || (cmp->scan==1)) {
     
     reset_scan(ptr);
    
     if (ptr->ftype==0) { 
       /* this is the first time we've been called so try and sync
        *  to a scan boundary. 
        */
         do {
           fstatus=cmp_read(fp,cmp);  
         } while ((fstatus !=-1) && (fstatus !=0) && 
               (((channel==1) && (cmp->channel==2)) ||
                 ((channel==2) && (cmp->channel!=2))));                                 
     }
     ptr->stime=cmp->time;
     ptr->ftype=CMP_SCAN;

   }

  

   /* new file opened so must read a record */

   if (cmp->scan==2) {
      do {
           fstatus=cmp_read(fp,cmp);
         } while ((fstatus !=-1) && (fstatus !=0) && 
                 (((channel==1) && (cmp->channel==2)) ||
                 ((channel==2) && (cmp->channel!=2))));                                 
   }

   ptr->stored=cmp->stored;
   ptr->st_id=fp->hdr.st_id;
   ptr->major_rev=fp->hdr.major_rev;
   ptr->minor_rev=fp->hdr.minor_rev;


   if (fstatus !=-1) 
     tm=cmp->time; 
   
   cmp->scan=0;
   while ((tm<(ptr->stime+tlen)) && (fstatus !=-1)) {
     if (((channel==1) && (cmp->channel!=2)) ||
          ((channel==2) && (cmp->channel==2))) {
       add_cmp_scan(ptr,cmp,&fp->hdr);   
       ptr->etime=tm;
     }
     fstatus=cmp_read(fp,cmp);
     if ((fstatus !=-1) &&  (((channel==1) && (cmp->channel!=2)) ||
	 ((channel==2) && (cmp->channel==2)))) tm=cmp->time;  
  
     if (fstatus !=-1) c++;

     if (c>256) { /* can't have 256 beams in a scan - probable file fault */
        fstatus=-1; 
        break;
     }

   }



   if (fstatus !=-1) {
     cmp->scan=1;
     return 1;
   }
   if (fstatus==-1) cmp->scan=-1;
   if (c>0) return 0;
   return -1;
}












  



