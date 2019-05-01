/* csdopen.c
   =========
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


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "rtypes.h"
#include "limit.h"
#include "csddata.h"
#include "csdread.h"

/*
 $Log: csdopen.c,v $
 Revision 1.3  2004/05/04 18:23:40  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 21:56:30  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2003/09/13 22:37:47  barnes
 Initial revision

*/



struct CSDfp *CSDOpen(char *name) {
  struct stat cstat;
  struct CSDfp *fptr=NULL;

  fptr=malloc(sizeof(struct CSDfp));
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



