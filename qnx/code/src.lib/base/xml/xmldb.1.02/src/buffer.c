/* buffer.c
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
#include <string.h>
#include "rxml.h"
#include "rxmldb.h"

/*
 $Log: buffer.c,v $
 Revision 1.1  2003/03/01 17:42:50  barnes
 Initial revision

*/

struct XMLDBbuffer *XMLDBCopyBuffer(struct XMLDBbuffer *src) {

  struct XMLDBbuffer *dst;
  
  if (src==NULL) return NULL;

  dst=malloc(sizeof(struct XMLDBbuffer));
  if (dst==NULL) return NULL;

  dst->sze=src->sze;
  dst->stp=src->stp;
  dst->max=src->max;

  dst->buf=malloc(src->max);
  if (dst->buf==NULL) {
     free(dst);
     return NULL;
  }
   
  memcpy(dst->buf,src->buf,src->max);
  
  return dst;
}



struct XMLDBbuffer *XMLDBMakeBuffer(int stp) {
  struct XMLDBbuffer *ptr;

  ptr=malloc(sizeof(struct XMLDBbuffer));
  if (ptr==NULL) return NULL;
  ptr->sze=0;
  ptr->stp=stp;
  ptr->max=stp;
  ptr->buf=malloc(stp);
  if (ptr->buf==NULL) {
    free(ptr);
    return NULL;
  }
  return ptr;
}

void XMLDBFreeBuffer(struct XMLDBbuffer *ptr) {

  if (ptr==NULL) return;
  if (ptr->buf !=NULL) free(ptr->buf);
  free(ptr);
}




int XMLDBAddBuffer(struct XMLDBbuffer *ptr,char *str,int sze) { 
  int stp=0;
  int xnt=0;

  if (ptr==NULL) return -1;

  stp=ptr->stp;
  if (sze>stp) stp=sze+stp; /* always make sure we have spare capacity */

  xnt=ptr->sze+sze;

  if (xnt>ptr->max) {
    char *tmp;
    ptr->max+=stp;
    tmp=realloc(ptr->buf,ptr->max);
    if (tmp==NULL) return -1;
    ptr->buf=tmp;
  }
 
  if (ptr->buf==NULL) return -1;
  
  memcpy(ptr->buf+ptr->sze,str,sze);
  ptr->sze+=sze;
  return 0;
}



  



