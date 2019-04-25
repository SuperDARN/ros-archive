/* Polygon.c
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
#include "rps.h"

/*
 $Log: polygon.c,v $
 Revision 1.1  2003/03/03 15:40:38  barnes
 Initial revision

*/


int PostScriptPolygon(struct PostScript *ptr,
                   struct PostScriptMatrix *matrix,
                   float x,float y,
                   int num,float *px,float *py,int *t,int fill,
                   unsigned int color,float width,
                   struct PostScriptDash *dash,struct PostScriptClip *clip) {
  int s=0;
  int n,c;
  float ma=1.0,mb=0.0,mc=0.0,md=1.0;
  char buf[4096];
  if (ptr==NULL) return -1;
 
  s=PostScriptState(ptr,color,width,dash,clip);
  if (s !=0) return s;

  if (ptr->cnt > 0) {
    sprintf(buf,"d\n");
    if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
    ptr->cnt=0;
  }

  if (matrix !=NULL) {
    ma=matrix->a;
    mb=matrix->b;
    mc=matrix->c;
    md=matrix->d;
  }

  sprintf(buf,"s n [%g %g %g %g %g %g] concat %g %g m ",
          ma,mb,mc,md,ptr->x+x,ptr->y+ptr->hgt-y,px[0],-py[0]);
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
  if (s !=0) return s;
  for (n=1;n<=num;n++) {
    c=(n==num) ? 0 : n;

    if (t[c]==1) {
      n+=2;
      if (n>num) break;
      c=(n==num) ? 0 : n;
      sprintf(buf,"%g %g %g %g %g %g b ",
              px[n-2],-py[n-2],
              px[n-1],-py[n-1],
              px[c],-py[c]);

    } else 
      sprintf(buf,"%g %g l ",px[c],-py[c]);

    if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
  }
  if (n<=num) return -1;


  if (fill !=0) sprintf(buf,"e f r\n");
  else sprintf(buf,"e d r\n");
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
  if (s !=0) return s;
  return 0;
}
