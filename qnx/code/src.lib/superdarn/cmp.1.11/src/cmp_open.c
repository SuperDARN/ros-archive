/* cmp_open.c
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
 $Log: cmp_open.c,v $
 Revision 1.6  2001/06/27 20:46:56  barnes
 Added license tag

 Revision 1.5  2001/01/29 18:11:51  barnes
 Added Author Name

 Revision 1.4  1999/07/25 00:42:19  barnes
 Initialize the major and minor revision numbers.

 Revision 1.3  1999/07/23 19:28:23  barnes
 Modified the memory allocation system so that it is much more efficient.

 Revision 1.2  1999/03/18 15:20:28  barnes
 Modified the internal format of the cmp file to use epoch time.

 Revision 1.1  1999/03/16 19:24:26  barnes
 Initial revision

 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "limit.h"
#include "cmpdata.h"
#include "cmp_read.h"
#include "cnv_data.h"
#include "cnv_time.h"

struct cmpfp *cmp_open(char *name) {
  int sze;
  int size,magic;
  struct stat cstat;
  struct cmpfp *fptr=NULL;
  fptr=malloc(sizeof(struct cmpfp));
  fptr->fp=open(name,O_RDONLY);
  if (fptr->fp==-1) {
    free(fptr);
    return NULL;
  }

  fstat(fptr->fp,&cstat);

  fptr->fsze=cstat.st_size;
  
  fptr->ctime=-1;
  fptr->blen=0;
  fptr->fptr=0;
  fptr->fbuf=NULL;
  fptr->fbuf=malloc(fptr->fsze);
  
  fptr->hdr.major_rev=0;
  fptr->hdr.minor_rev=0;
  if (fptr->fbuf !=NULL) {
    if (read(fptr->fp,fptr->fbuf,fptr->fsze) !=fptr->fsze) {
      close(fptr->fp);  
      free(fptr->fbuf);
      free(fptr);
      return NULL;
    }
    close(fptr->fp);
    fptr->fp=-1;
  }

  return fptr;
} 



