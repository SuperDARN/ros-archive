/* state.c
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
 $Log: state.c,v $
 Revision 1.1  2003/03/03 15:41:35  barnes
 Initial revision

*/



int PostScriptState(struct PostScript *ptr,
                    unsigned int color,float width,
                    struct PostScriptDash *dash,
                    struct PostScriptClip *clip) {
  int flg=0;
  int s=0;
  char buf[4096];
  if (ptr==NULL) return -1;
  if (color !=ptr->color) flg=1;
  if (width !=ptr->width) flg=1;
  if (dash !=ptr->dash) flg=1;
  if (clip !=ptr->clip) flg=1;
 
  if (flg==0) return 0;
  if (ptr->cnt >0) {
    sprintf(buf,"\ncurrentpoint d m ");
    if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
    ptr->cnt=0;
  }
  if (s !=0) return s;
  
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
  if (width !=ptr->width) {   
    sprintf(buf,"%g w\n", width);
    if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
    ptr->width=width;
  }
 
  if (dash !=ptr->dash) {
    int i;
    if (dash !=NULL) {
      sprintf(buf,"[");
      if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
      if (s !=0) return s;
      for (i=0;i<dash->sze;i++) {
        sprintf(buf,"%g ",dash->p[i]);
        if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
        if (s !=0) break;
      }
      if (i !=dash->sze) return -1;
      sprintf(buf,"] %g setdash\n",dash->phase);
      if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
      if (s !=0) return s;
    } else {
      sprintf(buf,"[] 0 setdash\n");
      if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
      if (s !=0) return s;

    }
    ptr->dash=dash;
  }
  if (clip !=ptr->clip) {
    s=PostScriptClip(ptr,clip);
    if (s !=0) return s;
    ptr->clip=clip;
  }

  return 0;
}

