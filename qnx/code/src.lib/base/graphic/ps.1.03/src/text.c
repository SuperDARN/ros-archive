/* text.c
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
#include <string.h>
#include "rfbuffer.h"
#include "rps.h"

/*
 $Log: text.c,v $
 Revision 1.1  2003/10/21 13:45:07  barnes
 Initial revision

*/

int PostScriptText(struct PostScript *ptr,
	              struct PostScriptMatrix *matrix,
                      char *fname,float fsize,
                      float x,float y,int num,char *txt,
                      unsigned int color,
                      struct PostScriptClip *clip) {
  int s=0;
  float ma=1.0,mb=0.0,mc=0.0,md=1.0;

  char buf[4096];

  if (ptr->cnt >0) {
    sprintf(buf,"d\n ");
    if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
    ptr->cnt=0;
  }
  if (s !=0) return s;

 if (clip !=ptr->clip) {
    s=PostScriptClip(ptr,clip);
    if (s !=0) return s;
    ptr->clip=clip;
  }

  if (matrix !=NULL) {
    ma=matrix->a;
    mb=matrix->b;
    mc=matrix->c;
    md=matrix->d;
  }

  if (color !=ptr->color) {
    unsigned int ri,gi,bi;
    float r,g,b;

    ri=(color>>16) & 0xff;
    gi=(color>>8) & 0xff;
    bi=color & 0xff;
   
    r=(float) ri/255.0;
    g=(float) gi/255.0;
    b=(float) bi/255.0;
    
    sprintf(buf,"%g %g %g c\n",r,g,b); 
    if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
    ptr->color=color;
  }
  sprintf(buf,"s [%g %g %g %g %g %g] concat 0 0 m %g /%s F (",
          ma,mb,mc,md,ptr->x+x,ptr->y+ptr->hgt-y,fsize,fname); 
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                               ptr->text.data);
  if (s !=0) return s;

  if (ptr->text.user !=NULL) s=(ptr->text.user)(txt,num,
                                               ptr->text.data);
  if (s !=0) return s;

  sprintf(buf,") show r\n");
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                               ptr->text.data);

  return s;
}

