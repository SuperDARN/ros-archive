/* bezier.c
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
#include <math.h>
#include "rfbuffer.h"

/*
 $Log: bezier.c,v $
 Revision 1.1  2003/03/03 18:43:23  barnes
 Initial revision

 */

int FrameBufferBezier(struct FrameBuffer *ptr,
                      int x1,int y1,int x2,int y2,
                      int x3,int y3,int x4,int y4,float step,
                      unsigned int color,unsigned char m,int width,
                      struct FrameBufferDash *dash,
                      struct FrameBufferClip *clip) {


  float ax,bx,cx;
  float ay,by,cy;
  int ox,oy,nx,ny;

  int s=0;
  int sze;
  float t=0;
  unsigned char r,g,b,a;
 
  if (ptr==NULL) return -1;

  sze=ptr->wdt*ptr->hgt;
  a=color>>24;;
  r=color>>16;
  g=color>>8;
  b=color;

  if (step>0.5) step=0.5;
  if (step<0.01) step=0.01;


  cx=(x2-x1)*3.0;  
  bx=(x3-x2)*3.0-cx;
  ax=x4-x1-bx-cx;
  
  cy=(y2-y1)*3.0;  
  by=(y3-y2)*3.0-cy;
  ay=y4-y1-by-cy;
  
  oy=y1;
  ox=x1;
  for (t=step;t<1;t+=step) {
    nx=ax*t*t*t+bx*t*t+cx*t+x1;
    ny=ay*t*t*t+by*t*t+cy*t+y1;
    s=FrameBufferRawLine(ptr,width,ox,oy,nx,ny,sze,r,g,b,a,m,dash,clip);
    if (s !=0) break;
    ox=nx;
    oy=ny;
  } 
  s=FrameBufferRawLine(ptr,width,ox,oy,x4,y4,sze,r,g,b,a,m,dash,clip);
  return s;
}
