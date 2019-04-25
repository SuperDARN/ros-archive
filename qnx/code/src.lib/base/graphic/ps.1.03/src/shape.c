/* shape.c
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
 $Log: shape.c,v $
 Revision 1.1  2003/03/03 15:41:05  barnes
 Initial revision

*/

int PostScriptEllipse(struct PostScript *ptr,
	              struct PostScriptMatrix *matrix,
                      float x,float y,float w,float h,
                      int fill,unsigned int color,
                      float width,
		      struct PostScriptDash *dash,
                      struct PostScriptClip *clip) {

  int i;
  float sf=0.55;
  float px[12];
  float py[12];
  int t[12];

  px[0]=w;
  py[0]=0;

  px[1]=w;
  py[1]=h*sf;
  px[2]=w*sf;
  py[2]=h;
  px[3]=0;
  py[3]=h;

  px[4]=-w*sf;
  py[4]=h;
  px[5]=-w;
  py[5]=h*sf;
  px[6]=-w;
  py[6]=0;

  px[7]=-w;
  py[7]=-h*sf;
  px[8]=-w*sf;
  py[8]=-h;
  px[9]=0;
  py[9]=-h;

  px[10]=w*sf;
  py[10]=-h;
  px[11]=w;
  py[11]=-h*sf;

  for (i=0;i<12;i++) t[i]=1;

  return PostScriptPolygon(ptr,matrix,x,y,12,
                            px,py,t,fill,color,width,dash,clip); 



}

int PostScriptRectangle(struct PostScript *ptr,
	         struct PostScriptMatrix *matrix,
                 float x,float y,float w,float h,
			int fill,unsigned int color,
                 float width,
		 struct PostScriptDash *dash,struct PostScriptClip *clip) {

  float px[4]={0,0,0,0};
  float py[4]={0,0,0,0};
  int t[4]={0,0,0,0};

  px[1]=w;
  px[2]=w;
  py[2]=h;
  py[3]=h;

  return PostScriptPolygon(ptr,matrix,x,y,4,px,py,t,fill,color,
                           width,dash,clip); 
}

