/* imagedb.c
   ========= 
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
#include "rfbuffer.h"
#include "imagedb.h"

/*
 $Log: imagedb.c,v $
 Revision 1.1  2003/03/01 18:53:58  barnes
 Initial revision

*/

void FrameBufferDBFree(struct FrameBufferDB *ptr) {
  int n;
  if (ptr==NULL) return;
  if (ptr->img !=NULL) {
    for (n=0;n<ptr->num;n++) 
      if (ptr->img[n] !=NULL) FrameBufferFree(ptr->img[n]);
    free(ptr->img);
  }
  free(ptr);
}

struct FrameBufferDB *FrameBufferDBMake() {
  struct FrameBufferDB *ptr;
  ptr=malloc(sizeof(struct FrameBufferDB));
  if (ptr==NULL) return NULL;
  ptr->num=0;
  ptr->img=NULL;
  return ptr;
};


int FrameBufferDBAdd(struct FrameBufferDB *ptr,struct FrameBuffer *img) {
  struct FrameBuffer **tmp;
  
  if (ptr==NULL) return -1;
  if (img==NULL) return -1;

  if (ptr->img==NULL) ptr->img=malloc(sizeof(struct FrameBuffer *));
  else {
    tmp=realloc(ptr->img,sizeof(struct FrameBuffer *)*(ptr->num+1));
    if (tmp==NULL) return -1;
    ptr->img=tmp;
  }

  if (ptr->img==NULL) return -1;
  ptr->img[ptr->num]=img;
  ptr->num++;
  return 0;
}

struct FrameBuffer *FrameBufferDBFind(struct FrameBufferDB *ptr,char *name) {
  int n;

  if (ptr==NULL) return NULL;

  if (ptr->img==NULL) return NULL;
  
  for (n=0;n<ptr->num;n++) {
    if ((ptr->img[n])->name==NULL) continue;
    if (strcmp((ptr->img[n])->name,name)==0) return ptr->img[n];
  }
  return NULL;
}







