/* psrplot.c 
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
#include "rxml.h"
#include "rfbuffer.h"
#include "rps.h"
#include "imagedb.h"
#include "rplotin.h"
#include "psrplot.h"
#include "include.h"

/*
 $Log: psrplot.c,v $
 Revision 1.3  2004/06/16 16:21:17  barnes
 Added missing headers.

 Revision 1.2  2004/04/26 19:42:44  barnes
 Audit - Enforced warnings "all".

 Revision 1.1  2003/03/22 23:21:44  barnes
 Initial revision

*/

int PostScriptRplotEnd(void *data) {
  struct PostScriptRplot *ptr;
  ptr=(struct PostScriptRplot *) data;
  if (ptr==NULL) return -1;
  PostScriptEndPlot(ptr->ps);
  return 0;
}

int PostScriptRplotMakePlot(char *name,int depth,float wdt,float hgt,
                         void *data) {
  struct PostScriptRplot *ptr;
  ptr=(struct PostScriptRplot *) data;
  if (ptr==NULL) return -1;
  
  PostScriptMakePlot(ptr->ps,ptr->xoff,ptr->yoff,wdt,hgt,ptr->land);
  return 0;
}

int PostScriptRplotDash(char *str,void *data) {

  struct PostScriptRplot *ptr;
  ptr=(struct PostScriptRplot *) data;
  if (ptr==NULL) return -1;
  if (str !=NULL) {
    if ((ptr->dash.txt !=NULL) && (strcmp(ptr->dash.txt,str)==0))
      return 0;
    if (ptr->dash.ptr !=NULL) PostScriptFreeDash(ptr->dash.ptr);
    ptr->dash.ptr=PostScriptMakeDashString(str);
    if (ptr->dash.txt !=NULL) free(ptr->dash.txt);
    ptr->dash.txt=malloc(strlen(str)+1);
    strcpy(ptr->dash.txt,str);
  } else {
    if (ptr->dash.txt !=NULL) free(ptr->dash.txt);
    ptr->dash.txt=NULL;
    if (ptr->dash.ptr !=NULL) PostScriptFreeDash(ptr->dash.ptr);
    ptr->dash.ptr=NULL;
  }
  return 0;
}

int PostScriptRplotClip(int num,float *px,float *py,int *t,void *data) {
  int s=0; 
  struct PostScriptRplot *ptr;
  struct PostScript *ps;

  ptr=(struct PostScriptRplot *) data;
  if (ptr==NULL) return -1;
  ps=ptr->ps;
  if (ps==NULL) return -1;

  if (ptr->clip !=NULL) PostScriptFreeClip(ptr->clip);
  ptr->clip=NULL;
  if (num==0) return 0;
  ptr->clip=PostScriptMakeClip(ps->x,ps->y,ps->wdt,ps->hgt,num,px,py,t);
  if (ptr->clip==NULL) s=-1;
  return s;
}



int PostScriptRplotLine(float x1,float y1,float x2,float y2,
                         unsigned int color,unsigned char mask,float width,
                         void *data) {
  struct PostScriptRplot *ptr;
  ptr=(struct PostScriptRplot *) data;
  if (ptr==NULL) return -1;

  PostScriptLine(ptr->ps,x1,y1,x2,y2,color,width,ptr->dash.ptr,ptr->clip);

  return 0;
}

int PostScriptRplotBezier(float x1,float y1,float x2,float y2,
                           float x3,float y3,float x4,float y4,
                           unsigned int color,unsigned char mask,float width,
                           void *data) {
  struct PostScriptRplot *ptr;
  ptr=(struct PostScriptRplot *) data;
  if (ptr==NULL) return -1;

  PostScriptBezier(ptr->ps,x1,y1,x2,y2,x3,y3,x4,y4,
                   color,width,ptr->dash.ptr,ptr->clip);

  return 0;
}


int PostScriptRplotShape(char *name,struct RplotInMatrix *matrix,
                          float x,float y,
                          float w,float h,int fill,
                          unsigned int color,unsigned char mask,float width,
                          void *data) {
  struct PostScriptRplot *ptr;
  struct PostScriptMatrix psmatrix;
  struct PostScriptMatrix *mptr=NULL;

  ptr=(struct PostScriptRplot *) data;
  if (ptr==NULL) return -1;

  if (matrix !=NULL) {
      psmatrix.a=matrix->a;
      psmatrix.b=matrix->b;
      psmatrix.c=matrix->c;
      psmatrix.d=matrix->d;
      mptr=&psmatrix;
    }

   if (strcmp(name,"ellipse")==0)
      PostScriptEllipse(ptr->ps,mptr,x,y,
			 w,h,fill,color,
			 width,ptr->dash.ptr,ptr->clip);
  
  if (strcmp(name,"rectangle")==0)
      PostScriptRectangle(ptr->ps,mptr,x,y,
			 w,h,fill,color,
			 width,ptr->dash.ptr,ptr->clip);

  return 0;
}


int PostScriptRplotPolygon(struct RplotInMatrix *matrix,
			    float x,float y,int num,float *px,float *py,
                            int *t,int fill,
                            unsigned int color,unsigned char mask,float width,
                            void *data) {
  struct PostScriptRplot *ptr;
  struct PostScriptMatrix psmatrix;
  struct PostScriptMatrix *mptr=NULL;
 
  ptr=(struct PostScriptRplot *) data;
  if (ptr==NULL) return -1;

  if (matrix !=NULL) {
      psmatrix.a=matrix->a;
      psmatrix.b=matrix->b;
      psmatrix.c=matrix->c;
      psmatrix.d=matrix->d;
      mptr=&psmatrix;
    }


  
    
    PostScriptPolygon(ptr->ps,mptr,x,y,
		       num,px,py,t,fill,color,width,ptr->dash.ptr,ptr->clip);
   
    return 0;  
}


int PostScriptRplotText(char *fname,float fsize,
                         struct RplotInMatrix *matrix,
                         float x,float y,int num,char *txt,
                         unsigned int color,unsigned char mask,int smooth,
                         void *data) {
  struct PostScriptRplot *ptr;
  struct PostScriptMatrix psmatrix;
  struct PostScriptMatrix *mptr=NULL;
 
  ptr=(struct PostScriptRplot *) data;
  if (ptr==NULL) return -1;

  if (matrix !=NULL) {
      psmatrix.a=matrix->a;
      psmatrix.b=matrix->b;
      psmatrix.c=matrix->c;
      psmatrix.d=matrix->d;
      mptr=&psmatrix;
    }

  
  PostScriptText(ptr->ps,mptr,fname,fsize,x,y,num,txt,color,ptr->clip);

  return 0;

}

int PostScriptRplotImage(struct FrameBuffer *img,
                         struct RplotInMatrix *matrix,
                         float x,float y,
                         unsigned char mask,int smooth,
                         void *data) {
  struct PostScriptRplot *ptr;
  struct PostScriptMatrix psmatrix;
  struct PostScriptMatrix *mptr=NULL;

  ptr=(struct PostScriptRplot *) data;
  if (ptr==NULL) return -1;

  if (matrix !=NULL) {
      psmatrix.a=matrix->a;
      psmatrix.b=matrix->b;
      psmatrix.c=matrix->c;
      psmatrix.d=matrix->d;
      mptr=&psmatrix;
    }
 
  PostScriptImage(ptr->ps,mptr,img,mask,x,y,ptr->clip);  
  
  return 0;

}



int PostScriptRplotImageName(char *name,
                             struct RplotInMatrix *matrix,
                             float x,float y,
                             unsigned char mask,int smooth,
                             void *data) {
  struct FrameBuffer *img;
  struct PostScriptRplot *ptr;
  struct PostScriptMatrix psmatrix;
  struct PostScriptMatrix *mptr=NULL;

  ptr=(struct PostScriptRplot *) data;
  if (ptr==NULL) return -1;

  if (matrix !=NULL) {
      psmatrix.a=matrix->a;
      psmatrix.b=matrix->b;
      psmatrix.c=matrix->c;
      psmatrix.d=matrix->d;
      mptr=&psmatrix;
    }
  img=FrameBufferDBFind(ptr->imagedb,name);
  PostScriptImage(ptr->ps,mptr,img,mask,x,y,ptr->clip);  
  
  return 0;

}

struct PostScriptRplot *PostScriptRplotMake(struct PostScript *ps,
                                            struct FrameBufferDB *imagedb) {
  struct PostScriptRplot *ptr=NULL;
  ptr=malloc(sizeof(struct PostScriptRplot));
  if (ptr==NULL) return NULL;
  ptr->imagedb=imagedb;
  ptr->ps=ps;
  ptr->dash.txt=NULL;
  ptr->dash.ptr=NULL;
  ptr->clip=NULL;
  ptr->xoff=0;
  ptr->yoff=0;
  ptr->land=0;
  return ptr;
}
                                              
void PostScriptRplotFree(struct PostScriptRplot *ptr) {
  if (ptr==NULL) return;
  if (ptr->dash.txt !=NULL) free(ptr->dash.txt);
  if (ptr->dash.ptr !=NULL) PostScriptFreeDash(ptr->dash.ptr);
  if (ptr->clip !=NULL) PostScriptFreeClip(ptr->clip);
  if (ptr->ps !=NULL) PostScriptFree(ptr->ps);   
  if (ptr->imagedb !=NULL) FrameBufferDBFree(ptr->imagedb);
  free(ptr);
}                                          


int PostScriptRplotSetAll(struct RplotInPlot *rplot,
                           struct PostScriptRplot *ptr) { 

  if (rplot==NULL) return -1;
  if (ptr==NULL) return -1;

  RplotInSetMake(rplot,PostScriptRplotMakePlot,ptr);
  RplotInSetDash(rplot,PostScriptRplotDash,ptr);
  RplotInSetClip(rplot,PostScriptRplotClip,ptr);
  RplotInSetLine(rplot,PostScriptRplotLine,ptr);
  RplotInSetBezier(rplot,PostScriptRplotBezier,ptr);
  RplotInSetShape(rplot,PostScriptRplotShape,ptr);
  RplotInSetPolygon(rplot,PostScriptRplotPolygon,ptr);
  RplotInSetText(rplot,PostScriptRplotText,ptr);
  RplotInSetImage(rplot,PostScriptRplotImage,ptr);
  RplotInSetImageName(rplot,PostScriptRplotImageName,ptr);
  RplotInSetInclude(rplot,RplotInIncludeFile,NULL);
  RplotInSetEnd(rplot,PostScriptRplotEnd,ptr);

  return 0;
}


                                             




