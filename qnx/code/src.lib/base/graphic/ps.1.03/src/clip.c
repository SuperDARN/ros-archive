/* clip.c
   ====== 
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
#include "rps.h"

/*
 $Log: clip.c,v $
 Revision 1.1  2003/03/03 22:23:52  barnes
 Initial revision

*/

struct PostScriptClip *PostScriptMakeClip(float x,float y,float wdt,
                                          float hgt, 
                                          int num,float *px,
                                          float *py,int *t) {
  struct PostScriptClip *ptr=NULL;

  int i;
 
  if (px==NULL) return NULL;
  if (py==NULL) return NULL;
  if (t==NULL) return NULL;
  if (num==0) return NULL;

  ptr=malloc(sizeof(struct PostScriptClip));
  if (ptr==NULL) return NULL;
 
  ptr->num=num;
  ptr->px=malloc(sizeof(float)*num);
  if (ptr->px==NULL) {
    free(ptr);
    return NULL;
  }

  ptr->py=malloc(sizeof(float)*num);
  if (ptr->py==NULL) {
    free(ptr->px);
    free(ptr);
    return NULL;
  }
  ptr->t=malloc(sizeof(int)*num);
  if (ptr->t==NULL) {
    free(ptr->px);
    free(ptr->py);
    free(ptr);
    return NULL;
  }

  for (i=0;i<num;i++) {
    ptr->px[i]=x+px[i];
    ptr->py[i]=y+hgt-py[i];
    ptr->t[i]=t[i];
  }
 
  return ptr;
}

void PostScriptFreeClip(struct PostScriptClip *ptr) {
  if (ptr==NULL) return;
  if (ptr->px !=NULL) free(ptr->px);
  if (ptr->py !=NULL) free(ptr->py);
  if (ptr->t !=NULL) free(ptr->t);
  free(ptr);
  return;
}


int PostScriptClip(struct PostScript *ptr,
                   struct PostScriptClip *clip) {
  int s=0;
  int n,c;
  char buf[4096];
  if (ptr==NULL) return -1;
 
  if (clip==NULL) {
     sprintf(buf,"initclip\n");
     if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
     return s;
  }
 
  sprintf(buf,"n %g %g m ",
          clip->px[0],clip->py[0]);
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
  if (s !=0) return s;
  for (n=1;n<=clip->num;n++) {
    c=(n==clip->num) ? 0 : n;

    if (clip->t[c]==1) {
      n+=2;
      if (n>clip->num) break;
      c=(n==clip->num) ? 0 : n;
      sprintf(buf,"%g %g %g %g %g %g b ",
              clip->px[n-2],clip->py[n-2],
              clip->px[n-1],clip->py[n-1],
              clip->px[c],clip->py[c]);

    } else 
      sprintf(buf,"%g %g l ",clip->px[c],clip->py[c]);

    if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
  }
  if (n<=clip->num) return -1;

  sprintf(buf,"e clip n %g %g m\n",ptr->px,ptr->py);
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
  if (s !=0) return s;
  return 0;
}





