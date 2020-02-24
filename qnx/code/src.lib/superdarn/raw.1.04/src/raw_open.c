/* raw_open.c
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
 $Log: raw_open.c,v $
 Revision 1.4  2001/06/27 20:50:04  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:11:54  barnes
 Added Author Name

 Revision 1.2  1999/07/25 15:14:32  barnes
 Changed the buffer to unsigned integers.

 Revision 1.1  1999/03/17 16:33:19  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "cnv_data.h"
#include "cnv_time.h"
#include "rawdata.h"
#include "raw_read.h"
#include "raw_versions.h"

struct rawfp *raw_open(char *rawfile,char *inxfile) {

  int radar_parms_pat[]={1,2,2,17,4,2,2,14,4,4,2,4,0,0};

  unsigned char *inbuf=NULL;
  int16 num_byte;
  int32 rec_num;
  int stat;
  struct radar_parms *prm;
  struct rawfp *ptr=NULL;

  inbuf=malloc(sizeof(struct rawdata));
  if (inbuf==NULL) return NULL;

  ptr=malloc(sizeof(struct rawfp));
  
  if (ptr==NULL) return NULL;

  ptr->rawfp=open(rawfile,O_RDONLY);
  ptr->stime=-1;
  ptr->ctime=-1;
  ptr->frec=0;
  ptr->rlen=0;
  ptr->ptr=0;
  if (ptr->rawfp==-1) {
    free(ptr);
    free(inbuf);
    return NULL;
  }
  fstat(ptr->rawfp,&ptr->rstat);

  if (read_short(ptr->rawfp,&num_byte) !=0) {
    close(ptr->rawfp);
    free(ptr);
    free(inbuf);
    return NULL;
  }

  num_byte = num_byte - 2;
  stat = read(ptr->rawfp,inbuf,num_byte);
  if (stat != num_byte) {
    close(ptr->rawfp);
    free(ptr);
    free(inbuf);
    return NULL;
  }
  cnv_long(inbuf,&rec_num);

  ptr->frec=num_byte+2;
  ptr->rlen=num_byte+2;
  ptr->ptr=num_byte+2;

  if (rec_num !=0) { /* not the header so rewind the file */
    lseek(ptr->rawfp,0L,SEEK_SET);
    ptr->rlen=0;
  }

  /* read the first record so that we can determine the start time of 
     the file */

  
 if (read_short(ptr->rawfp,&num_byte) !=0) {
    close(ptr->rawfp);
    free(ptr);
    free(inbuf);
    return NULL;
  }

  num_byte = num_byte - 2;
  stat = read(ptr->rawfp,inbuf,num_byte);
  if (stat != num_byte) {
    close(ptr->rawfp);
    free(ptr);
    free(inbuf);
    return NULL;
  }

  cnv_long(inbuf,&rec_num);

  /* now decode the parameter block */

  cnv_block(inbuf+12,radar_parms_pat);
  prm=(struct radar_parms *) (inbuf+12);   

  ptr->stime=time_epoch(prm->YEAR,prm->MONTH,prm->DAY,
	  prm->HOUR,prm->MINUT,prm->SEC);

  /* rewind to the first record */

  lseek(ptr->rawfp,ptr->frec,SEEK_SET);

  ptr->rawread=raw_read_current;

  free(inbuf);
  return ptr;
}




