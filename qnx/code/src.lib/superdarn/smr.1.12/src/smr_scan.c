/* smr_scan.c
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
 $Log: smr_scan.c,v $
 Revision 1.21  2002/02/07 21:23:50  barnes
 Modified code to remove possible screw up when decoding channel.

 Revision 1.20  2002/02/06 17:43:09  barnes
 Simplified the channel filtering code.

 Revision 1.19  2002/02/06 16:19:19  barnes
 Added routines that can filter based on channel.

 Revision 1.18  2002/02/01 18:36:23  barnes
 Added missing header.

 Revision 1.17  2001/06/27 20:50:13  barnes
 Added license tag

 Revision 1.16  2001/01/29 18:11:54  barnes
 Added Author Name

 Revision 1.15  2000/03/21 16:46:43  barnes
 Fixed bug that meant certain elements of the scan block could be
 set with incorrect values.

 Revision 1.14  2000/03/21 16:44:05  barnes
 Fixed bug in setting the file type in the scan_period function.

 Revision 1.13  2000/03/21 16:11:11  barnes
 Fixed bug in setting the file type.

 Revision 1.12  2000/03/21 15:38:31  barnes
 Major revisions to tidy up the code.

 Revision 1.11  2000/01/26 16:10:31  barnes
 Fixed minor bug in the scan_period function.

 Revision 1.10  2000/01/26 15:57:57  barnes
 Fixed bug that meant beams were being lost from the scan_period function.

 Revision 1.9  1999/12/15 20:06:15  barnes
 Added receiver rise time to the structure

 Revision 1.8  1999/10/14 14:13:59  barnes
 Fixed bug in attenuation code.

 Revision 1.7  1999/10/14 14:13:11  barnes
 Added attenuation to record.

 Revision 1.6  1999/10/08 20:43:13  barnes
 Added program id to header.

 Revision 1.5  1999/07/24 19:17:26  barnes
 Fudged the version number passed back.

 Revision 1.4  1999/07/23 19:54:47  barnes
 Changed the way memory is dynamically allocated to improve speed.

 Revision 1.3  1999/03/29 16:03:06  barnes
 Fixed problem with quality flag.

 Revision 1.2  1999/03/17 23:11:53  barnes
 Fixed incrorrect call to the read summary routine.

 Revision 1.1  1999/03/16 20:18:31  barnes
 Initial revision

 */



/* decodes smr type files */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rawdata.h"
#include "fitdata.h"
#include "smr_read.h"
#include "radar_scan.h"
#include "radar_fit.h"
#include "cnv_data.h"
#include "cnv_time.h"


int smr_fread_scan(FILE *fp,struct radardata *ptr,
		  struct fitdata *fit,int fbeam) {

  /* reads one scans worth of data from the current position 
   *  in the fit file.
   */
   int bmnum,cnt;
   int rng,i,j,c=0;
   int fstatus=0;

   if (fp==NULL) return -1;
   if (fit->prms.SCAN==-1) return -1;

   /* if this is the first time through the file type flag 
    * will be zero - If you need to force a reset then set the
    * file type flag to zero
    *
    * So if this is the first call to the function, or we
    * are at the start of a new scan, reset the scan array. 
    */

  
   if ((ptr->ftype==0) || (fit->prms.SCAN==1)) {
   
     reset_scan(ptr);
    
     if (ptr->ftype==0) { 
       /* this is the first time we've been called so try and sync
        *  to a scan boundary. 
        */

       while ((fit->prms.SCAN==0) && (fstatus !=-1)) 
          fstatus=smr_fread(fp,fit,fbeam);
     }
     ptr->stime=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
		           fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);
     ptr->ftype=SMR_SCAN;

   }

   /* new file opened so must read a record */

   if (fit->prms.SCAN==2) 
     fstatus=smr_fread(fp,fit,fbeam);
 
   ptr->stored=0x07;
   ptr->st_id=fit->prms.ST_ID;
   ptr->major_rev=1;
   ptr->minor_rev=0;


   fit->prms.SCAN=0;
   while ((fit->prms.SCAN==0) && (fstatus !=-1)) {
     if (fit->prms.SCAN==0) {
   
       add_fit_scan(ptr,fit);   
       ptr->etime=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
 		          fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);      
        
     }
     fstatus=smr_fread(fp,fit,fbeam);
     if (fstatus !=-1) c++;

     if (c>256) { /* can't have 256 beams in a scan - probable file fault */
        fstatus=-1; 
        break;
     }
   }
 
   if ((fstatus !=-1) && (fit->prms.SCAN==1)) {
      ptr->etime=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
 		          fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);      
      return 1;
   }
   if (fstatus==-1) fit->prms.SCAN=-1;
   if (c>0) return 0;
   return -1;
}



int smr_fread_scan_period(FILE *fp,struct radardata *ptr,
		          struct fitdata *fit,int fbeam,int tlen) {

  /* reads one scans worth of data from the current position 
   *  in the fit file.
   */
   double tm;
   int bmnum,cnt;
   int rng,i,j,c=0;
   int fstatus=0;

   if (fp==NULL) return -1;
   if (fit->prms.SCAN==-1) return -1;


   /* if this is the first time through the file type flag 
    * will be zero - If you need to force a reset then set the
    * file type flag to zero
    *
    * So if this is the first call to the function, or we
    * are at the start of a new scan, reset the scan array. 
    */

  
   if ((ptr->ftype==0) || (fit->prms.SCAN==1)) {
   
     reset_scan(ptr);
    
     if (ptr->ftype==0) { 
       /* this is the first time we've been called so try and sync
        *  to a scan boundary. 
        */
        fstatus=smr_fread(fp,fit,fbeam);
     }
     ptr->stime=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
		           fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);
     ptr->ftype=SMR_SCAN;

   }

   /* new file opened so must read a record */

   if (fit->prms.SCAN==2) 
     fstatus=smr_fread(fp,fit,fbeam);

   ptr->stored=0x07;
   ptr->st_id=fit->prms.ST_ID;
   ptr->major_rev=1;
   ptr->minor_rev=0;

   if (fstatus !=-1) 
     tm=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
 		          fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);      
   
   fit->prms.SCAN=0;
   while ((tm<(ptr->stime+tlen)) && (fstatus !=-1)) {
     add_fit_scan(ptr,fit);   
     ptr->etime=tm;
     fstatus=smr_fread(fp,fit,fbeam);
     if (fstatus !=-1)  
       tm=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
		     fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);


     if (fstatus !=-1) c++;

     if (c>256) { /* can't have 256 beams in a scan - probable file fault */
        fstatus=-1; 
        break;
     }

   }

   if (fstatus !=-1) {
     fit->prms.SCAN=1;
     return 1;
   }
   if (fstatus==-1) fit->prms.SCAN=-1;
   if (c>0) return 0;
   return -1;
}






int smr_fread_scan_channel(FILE *fp,struct radardata *ptr,
		  struct fitdata *fit,int fbeam,int channel) {

  /* reads one scans worth of data from the current position 
   *  in the fit file.
   */
   int bmnum,cnt;
   int rng,i,j,c=0;
   int fstatus=0;

   if (fp==NULL) return -1;
   if (fit->prms.SCAN==-1) return -1;

   /* if this is the first time through the file type flag 
    * will be zero - If you need to force a reset then set the
    * file type flag to zero
    *
    * So if this is the first call to the function, or we
    * are at the start of a new scan, reset the scan array. 
    */

  
   if ((ptr->ftype==0) || (fit->prms.SCAN==1)) {
   
     reset_scan(ptr);
    
     if (ptr->ftype==0) { 
       /* this is the first time we've been called so try and sync
        *  to a scan boundary. 
        */

       while ((fit->prms.SCAN==0) && (fstatus !=-1)) {
          fstatus=smr_fread(fp,fit,fbeam);
          if (((channel==1) && (fit->prms.CHANNEL==2)) ||
             ((channel==2) && (fit->prms.CHANNEL!=2)))  fit->prms.SCAN=0;
       }
     }
     ptr->stime=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
		           fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);
     ptr->ftype=SMR_SCAN;

   }

   /* new file opened so must read a record */

   if (fit->prms.SCAN==2)
     fstatus=smr_fread(fp,fit,fbeam);
 
   ptr->stored=0x07;
   ptr->st_id=fit->prms.ST_ID;
   ptr->major_rev=1;
   ptr->minor_rev=0;


   fit->prms.SCAN=0;
   while ((fit->prms.SCAN==0) && (fstatus !=-1)) {
      if ((fit->prms.SCAN==0) && 
         (((channel==1) && (fit->prms.CHANNEL!=2)) ||
	  ((channel==2) && (fit->prms.CHANNEL==2)))) {
       add_fit_scan(ptr,fit);   
       ptr->etime=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
 		          fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);      
        
     }
     fstatus=smr_fread(fp,fit,fbeam);
     if (((channel==1) && (fit->prms.CHANNEL==2)) ||
        ((channel==2) && (fit->prms.CHANNEL!=2)))  fit->prms.SCAN=0;
     if (fstatus !=-1) c++;

     if (c>256) { /* can't have 256 beams in a scan - probable file fault */
        fstatus=-1; 
        break;
     }
   }
 
   if ((fstatus !=-1) && (fit->prms.SCAN==1)) {
      ptr->etime=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
 		          fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);      
      return 1;
   }
   if (fstatus==-1) fit->prms.SCAN=-1;
   if (c>0) return 0;
   return -1;
}



int smr_fread_scan_period_channel(FILE *fp,struct radardata *ptr,
		          struct fitdata *fit,int fbeam,int tlen,
                          int channel) {

  /* reads one scans worth of data from the current position 
   *  in the fit file.
   */
   double tm;
   int bmnum,cnt;
   int rng,i,j,c=0;
   int fstatus=0;

   if (fp==NULL) return -1;
   if (fit->prms.SCAN==-1) return -1;


   /* if this is the first time through the file type flag 
    * will be zero - If you need to force a reset then set the
    * file type flag to zero
    *
    * So if this is the first call to the function, or we
    * are at the start of a new scan, reset the scan array. 
    */

  
   if ((ptr->ftype==0) || (fit->prms.SCAN==1)) {
   
     reset_scan(ptr);
    
     if (ptr->ftype==0) { 
       /* this is the first time we've been called so try and sync
        *  to a scan boundary. 
        */
       do {
        fstatus=smr_fread(fp,fit,fbeam);
       } while ((fstatus !=-1) && 
                (((channel==1) && (fit->prms.CHANNEL==2)) ||
                 ((channel==2) && (fit->prms.CHANNEL!=2))));
        
     }
     ptr->stime=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
		           fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);
     ptr->ftype=SMR_SCAN;

   }

   /* new file opened so must read a record */

   if (fit->prms.SCAN==2) 
     fstatus=smr_fread(fp,fit,fbeam);

   ptr->stored=0x07;
   ptr->st_id=fit->prms.ST_ID;
   ptr->major_rev=1;
   ptr->minor_rev=0;

   if (fstatus !=-1) 
     tm=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
 		          fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);      
   
   fit->prms.SCAN=0;
   while ((tm<(ptr->stime+tlen)) && (fstatus !=-1)) {
     if (((channel==1) && (fit->prms.CHANNEL!=2)) ||
          ((channel==2) && (fit->prms.CHANNEL==2))) {
       add_fit_scan(ptr,fit);   
       ptr->etime=tm;
     }
     fstatus=smr_fread(fp,fit,fbeam);
   if ((fstatus !=-1) &&  (((channel==1) && (fit->prms.CHANNEL!=2)) ||
			     ((channel==2) && (fit->prms.CHANNEL==2))))  
       tm=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
		     fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);


     if (fstatus !=-1) c++;

     if (c>256) { /* can't have 256 beams in a scan - probable file fault */
        fstatus=-1; 
        break;
     }

   }

   if (fstatus !=-1) {
     fit->prms.SCAN=1;
     return 1;
   }
   if (fstatus==-1) fit->prms.SCAN=-1;
   if (c>0) return 0;
   return -1;
}






