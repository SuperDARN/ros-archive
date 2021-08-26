
/* fit_scan.c
   ========== */

/*
 $Log: fit_scan.c,v $
 Revision 1.13  2000/03/21 16:45:43  barnes
 Fixed bug that meant that certain elements in the scan block could
 be set without reading data from a file.

 Revision 1.12  2000/03/21 15:33:21  barnes
 Migrated the reset_scan and add_scan_fit functions to another library.

 Revision 1.11  2000/03/21 15:27:48  barnes
 Modication so that if more than 256 beams are found in a single scan
 we report an error.

 Revision 1.10  2000/03/21 15:26:17  barnes
 Major overhaul to solve problems with segmentation faults.

 Revision 1.9  2000/01/26 15:53:11  barnes
 Fixed bug that caused beams to be lost when calling the scan_period function.

 Revision 1.8  1999/12/15 20:09:08  barnes
 Added reciver rise time to the structure.

 Revision 1.7  1999/10/14 14:02:28  barnes
 Added record of the attenuation setting.

 Revision 1.6  1999/10/08 20:38:53  barnes
 Added the program id to the structure.

 Revision 1.5  1999/07/26 16:52:17  barnes
 Modifications to stop us crashing if we get a file with the scan flags
 screwed up - The idea is to limit the number of beams in a scan to 32. If
 more beams occur then force a scan flag to be set.

 Revision 1.4  1999/07/24 19:15:28  barnes
 Added file version number to scan data.

 Revision 1.3  1999/07/23 19:48:51  barnes
 Improved the way memory is dynamically allocated. Rather than
 keep freeing and claiming memory we claim an intial buffer and only add to
 and subtract from it as neccessary.

 Revision 1.2  1999/03/29 16:03:50  barnes
 Fixed problem with quality flag.

 Revision 1.1  1999/03/16 20:20:31  barnes
 Initial revision

 */



/* decodes fit type files */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rawdata.h"
#include "fitdata.h"
#include "fit_read.h"
#include "radar_scan.h"
#include "cnv_data.h"
#include "cnv_time.h"





int fit_read_scan(struct fitfp *fp,struct radardata *ptr,
		  struct fitdata *fit) {

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
         fstatus=fit_read(fp,fit);
     }
     ptr->stime=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
		           fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);
     ptr->ftype=FIT_SCAN;

   }

   /* new file opened so must read a record */

   if (fit->prms.SCAN==2) 
     fstatus=fit_read(fp,fit);


   ptr->stored=0x07;
   ptr->st_id=fit->prms.ST_ID;
   ptr->major_rev=fp->major_rev;
   ptr->minor_rev=fp->minor_rev;

   
   fit->prms.SCAN=0;
   while ((fit->prms.SCAN==0) && (fstatus !=-1)) {
     if (fit->prms.SCAN==0) {
   
       add_fit_scan(ptr,fit);   
       ptr->etime=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
 		          fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);      
        
     }
     fstatus=fit_read(fp,fit);
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

int fit_read_scan_period(struct fitfp *fp,struct radardata *ptr,
		  struct fitdata *fit,int tlen) {

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
        fstatus=fit_read(fp,fit);
     }
     ptr->stime=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
		           fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);
     ptr->ftype=FIT_SCAN;

   }

   /* new file opened so must read a record */

   if (fit->prms.SCAN==2) 
     fstatus=fit_read(fp,fit);

   ptr->stored=0x07;
   ptr->st_id=fit->prms.ST_ID;
   ptr->major_rev=fp->major_rev;
   ptr->minor_rev=fp->minor_rev;

  


   if (fstatus !=-1) 
     tm=time_epoch(fit->prms.YEAR,fit->prms.MONTH,fit->prms.DAY,
 		          fit->prms.HOUR,fit->prms.MINUT,fit->prms.SEC);      
   
   fit->prms.SCAN=0;
   while ((tm<(ptr->stime+tlen)) && (fstatus !=-1)) {
     add_fit_scan(ptr,fit);   
     ptr->etime=tm;
     fstatus=fit_read(fp,fit);
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






