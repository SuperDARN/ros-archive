/* rawinx.c
   ========
   Author: R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
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


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rconvert.h"
#include "rawindex.h"

/*
 $Log: rawinx.c,v $
 Revision 1.4  2004/06/22 17:42:20  barnes
 Changed references of Long to Int.

 Revision 1.3  2004/05/05 15:49:13  barnes
 Added missing header.

 Revision 1.2  2004/05/04 18:37:48  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2003/09/13 22:39:29  barnes
 Initial revision

*/


#define FILE_STEP 256

void RawIndexFree(struct RawIndex *ptr) {
  if (ptr==NULL) return;
  if (ptr->tme !=NULL) free(ptr->tme);
  if (ptr->inx !=NULL) free(ptr->inx);
  free(ptr);
}

struct RawIndex *RawIndexLoad(int fid) {
  void *tmp;
  double tme;
  int32 inx;
  int sze=0;
  int st;
  struct RawIndex *ptr;
  ptr=malloc(sizeof(struct RawIndex));
  if (ptr==NULL) return NULL;

  ptr->tme=malloc(sizeof(double)*FILE_STEP);
  ptr->inx=malloc(sizeof(int)*FILE_STEP);
  ptr->num=0;
  sze=FILE_STEP; 

  if ((ptr->tme==NULL) || (ptr->inx==NULL)) {
     if (ptr->tme !=NULL) free(ptr->tme);
     if (ptr->inx !=NULL) free(ptr->inx);
     free(ptr);
     return NULL;
  }

  do {

    st=ConvertReadDouble(fid,&tme);
    if (st !=0) break;
    st=ConvertReadInt(fid,&inx);
    if (st !=0) break;
    
    ptr->tme[ptr->num]=tme;
    ptr->inx[ptr->num]=inx;
    ptr->num++;
    
    st=1; 
    if (ptr->num>=sze) {
      sze+=FILE_STEP;
      tmp=realloc(ptr->tme,sizeof(double)*sze);
      if (tmp==NULL) break;
      ptr->tme=(double *) tmp;
      tmp=realloc(ptr->inx,sizeof(int)*sze);
      if (tmp==NULL) break;
      ptr->inx=(int *) tmp;
    }
    
  } while (1);
  
  if (st==1) {
    free(ptr->tme);
    free(ptr->inx);
    free(ptr);
    return NULL;
  }
  tmp=realloc(ptr->tme,sizeof(double)*ptr->num);
  if (tmp==NULL) {
    free(ptr->tme);
    free(ptr->inx);
    free(ptr);
    return NULL;
  }
  ptr->tme=tmp;
  tmp=realloc(ptr->inx,sizeof(int)*ptr->num);
  if (tmp==NULL) {
    free(ptr->tme);
    free(ptr->inx);
    free(ptr);
    return NULL;
  }
  ptr->inx=tmp;
  return ptr;

}

struct RawIndex *RawIndexFload(FILE *fp) {
  return RawIndexLoad(fileno(fp));
}

