/* iqinx.c
   =======
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
#include "iqindex.h"

/*
 $Log: iqinx.c,v $
 Revision 1.1  2008/03/13 17:32:30  code
 Initial revision

*/


#define FILE_STEP 256

void IQIndexFree(struct IQIndex *ptr) {
  if (ptr==NULL) return;
  if (ptr->tme !=NULL) free(ptr->tme);
  if (ptr->inx !=NULL) free(ptr->inx);
  free(ptr);
}

struct IQIndex *IQIndexLoad(int fid) {
  void *tmp;
  double tme;
  int32 inx;
  int st;
  struct IQIndex *ptr;
  ptr=malloc(sizeof(struct IQIndex));
  if (ptr==NULL) return NULL;

  ptr->tme=malloc(sizeof(double)*FILE_STEP);
  ptr->inx=malloc(sizeof(int)*FILE_STEP);
  ptr->num=0;
 
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
    if ((ptr->num % FILE_STEP)==0) {
      int inc;
      inc=FILE_STEP*(ptr->num/FILE_STEP+1);
      tmp=realloc(ptr->tme,sizeof(double)*inc);
      if (tmp==NULL) break;
      ptr->tme=(double *) tmp;
      tmp=realloc(ptr->inx,sizeof(int)*inc);
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

struct IQIndex *IQIndexFload(FILE *fp) {
  return IQIndexLoad(fileno(fp));
}

