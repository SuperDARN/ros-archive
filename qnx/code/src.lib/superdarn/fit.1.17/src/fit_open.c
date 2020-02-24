/* fit_open.c
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
 $Log: fit_open.c,v $
 Revision 1.5  2001/06/27 20:48:03  barnes
 Added license tag

 Revision 1.4  2001/01/29 18:11:52  barnes
 Added Author Name

 Revision 1.3  1999/07/24 19:16:35  barnes
 Removed debugging code.

 Revision 1.2  1999/07/24 19:15:28  barnes
 Added algorithm to decode the version number from the header of the file.

 Revision 1.1  1999/03/16 14:36:30  barnes
 Initial revision

 */


/* opens a fit file */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "cnv_data.h"
#include "cnv_time.h"
#include "rawdata.h"
#include "fitdata.h"

#include "fit_read.h"
#include "fit_str.h"
#include "fit_versions.h"

struct fitfp *fit_open(char *fitfile,char *inxfile) {

  
  int r1_pat[]={4,2,1,2,2,17,4,2,2,14,4,4,2,4,
                2,PULSE_PAT_LEN,2,2*LAG_TAB_LEN,1,COMBF_SIZE,4,3,
		2,2*MAX_RANGE,1,MAX_RANGE,0,0};
  int status=0;
  union fit_out r;
  struct fitfp *ptr=NULL;
  int16 tmp,i,j;
  int32 inx_buf[4];
  struct radar_parms *prms;
  char *tmpbuf;
  ptr=malloc(sizeof(struct fitfp));
  if (ptr==NULL) return NULL;
 
  ptr->fitfp=-1;
  ptr->inxfp=-1;
  ptr->ibuf=NULL;
  ptr->fbuf=NULL;
  ptr->iptr=0;
  ptr->fptr=0;
  ptr->ctime=-1;
  ptr->stime=-1;
  ptr->etime=-1;
  ptr->fitfp=open(fitfile,O_RDONLY);
  if (ptr->fitfp==-1) {
    free(ptr);
    return NULL;
  }

  fstat(ptr->fitfp,&ptr->fstat);
  

  /* buffering disabled as it is actually slower to load into memory!  
  ptr->fbuf=malloc(ptr->fstat.st_size);
  if (ptr->fbuf !=NULL) {
    if (read(ptr->fitfp,ptr->fbuf,
             ptr->fstat.st_size) !=ptr->fstat.st_size) {
       close(ptr->fitfp);
       free(ptr->fbuf);
       free(ptr);
       return NULL;
    }
    close(ptr->fitfp);
    ptr->fitfp=-1;
  }
  */
  
  if (inxfile !=NULL) { /* open the index file */
    ptr->inxfp=open(inxfile,O_RDONLY);
    if (ptr->inxfp !=-1) { 
      fstat(ptr->inxfp,&ptr->istat);
      ptr->ibuf=malloc(ptr->istat.st_size);
      if ((ptr->ibuf !=NULL) &&
          (read(ptr->inxfp,ptr->ibuf,ptr->istat.st_size) 
           !=ptr->istat.st_size)) free(ptr->ibuf);
       close(ptr->inxfp);
       ptr->inxfp=-1;
    }
  } 

  if (ptr->fitfp !=-1) {
    read_short(ptr->fitfp,&tmp);
    ptr->fit_recl=tmp;
    read_short(ptr->fitfp,&tmp);
    ptr->inx_recl=tmp;
  
    tmpbuf=malloc(ptr->fit_recl);
    if (tmpbuf !=NULL) { /* get the header information */
       memset(tmpbuf,0,ptr->fit_recl);
       status=(read(ptr->fitfp,tmpbuf,ptr->fit_recl) !=ptr->fit_recl);
       /* now decode the header information */
       if (status==0) {
          for (i=0;(tmpbuf[i] !='\n') && (tmpbuf[i] !=0) && (i<80);i++) 
            ptr->header[i]=tmpbuf[i];
          ptr->header[i]=0;
          j=i+1;
          for (i=0;(tmpbuf[j+i] !='\n') && (tmpbuf[j+i] !=0) && (i<32);i++) 
            ptr->date[i]=tmpbuf[j+i];
          ptr->date[i]=0;
          j+=i+1;
          for (i=0;(tmpbuf[j+i] !=0) && (i<256);i++) 
            ptr->extra[i]=tmpbuf[j+i];
          ptr->extra[i]=0;
         
          /* okay extra the version information - look for the '.' */

          for (i=0;(ptr->header[i] !='.') && (ptr->header[i] !=0);i++);
          if (ptr->header[i] !=0) { 
             tmpbuf[i]=0; /* work backwards for the major number */
             for (j=0;(ptr->header[i-1-j] !=' ') && ((i-1-j)>0);j++)
               tmpbuf[i-1-j]=ptr->header[i-1-j];
             ptr->major_rev=atoi(tmpbuf+i-j);
             for (j=0;(ptr->header[i+1+j] !=' ') && 
                 (ptr->header[i+1+j] !=0);i++) tmpbuf[j]=ptr->header[i+1+j]; 
             tmpbuf[j]=0;
             ptr->minor_rev=atoi(tmpbuf);
             
          }
       }
       free(tmpbuf);
    }
    
    lseek(ptr->fitfp,ptr->fit_recl,SEEK_SET);

    /* read the first parameter block se we can determine the year */
    status=(read(ptr->fitfp,&r,sizeof(union fit_out)) !=sizeof(union fit_out));
    if (status==0) {
      cnv_block( (unsigned char *) &r,r1_pat);
      status=(r.r1.rrn !=0);
      prms=(struct radar_parms *) &(r.r1.plist[0]);
    }
    lseek(ptr->fitfp,ptr->fit_recl,SEEK_SET);
  } else {
    cnv_short(ptr->fbuf,&tmp);
    ptr->fit_recl=tmp;
    cnv_short(ptr->fbuf+2,&tmp);
    ptr->inx_recl=tmp;
    ptr->fptr=ptr->fit_recl;
    memcpy( (unsigned char *) &r,
            ptr->fbuf+ptr->fit_recl,sizeof(union fit_out));
    cnv_block( (unsigned char *) &r,r1_pat);
    status=(r.r1.rrn !=0);
    prms=(struct radar_parms *) &(r.r1.plist[0]);
  }

  if (status !=0) {
    if (ptr->fitfp !=-1) close(ptr->fitfp);
    if (ptr->fbuf !=NULL) free(ptr->fbuf);
    if (ptr->inxfp !=-1) close(ptr->inxfp);
    if (ptr->ibuf !=NULL) free(ptr->ibuf);
    free(ptr);
    return NULL; 
  }

  ptr->ctime=time_epoch(prms->YEAR,prms->MONTH,prms->DAY,
			prms->HOUR,prms->MINUT,prms->SEC);

  ptr->stime=ptr->ctime;  
  


  if (fit_read_inx(ptr,inx_buf,-1)==0) {

    if (inx_buf[0] > inx_buf[1]) prms->YEAR++;
    /* generate the end time of the file */
 
    ptr->etime=time_epoch(prms->YEAR,1,1,0,0,0)
                +inx_buf[1];   
    ptr->inx_srec=inx_buf[2]-2;
    ptr->inx_erec=inx_buf[3];     
  } else { /* corrupt index file */
    if (ptr->inxfp !=-1) close(ptr->inxfp);
    if (ptr->ibuf !=NULL) free(ptr->ibuf);
    ptr->ibuf=NULL;
    ptr->inxfp=-1;
  } 
  ptr->fitread=fit_read_current;
  return ptr;        
}




