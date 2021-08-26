/* line.c
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
 $Log: line.c,v $
 Revision 1.1  2003/03/03 15:40:13  barnes
 Initial revision

*/

int PostScriptLine(struct PostScript *ptr,
              float ax,float ay,float bx,float by,
              unsigned int color,float width,
              struct PostScriptDash *dash,
              struct PostScriptClip *clip) {
  int s=0;
  char buf[4096];
  if (ptr==NULL) return -1;
  s=PostScriptState(ptr,color,width,dash,clip);
  if (s !=0) return s;

  if ((ax !=ptr->px) || (ay !=ptr->py)) {
    sprintf(buf,"%g %g m ",ptr->x+ax,ptr->y+ptr->hgt-ay);
    if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
  } 

  sprintf(buf,"%g %g l ",ptr->x+bx,ptr->y+ptr->hgt-by);
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
  
  ptr->cnt++;
  if (ptr->cnt>STROKE_COUNT) {
  
    sprintf(buf,"d\n %g %g m ",ptr->x+bx,ptr->y+ptr->hgt-by);
    if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                                  ptr->text.data);
    if (s !=0) return s;
    ptr->cnt=0;
  }

  ptr->px=bx;
  ptr->py=by;
  return 0;
}
