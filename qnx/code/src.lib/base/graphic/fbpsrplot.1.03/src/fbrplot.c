/* fbrplot.c 
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
#include "rplotin.h"
#include "fontdb.h"
#include "imagedb.h"
#include "fbrplot.h"
#include "include.h"

/*
 $Log: fbrplot.c,v $
 Revision 1.2  2004/06/16 16:21:17  barnes
 Added missing headers.

 Revision 1.1  2003/03/22 23:21:44  barnes
 Initial revision

*/

int FrameBufferRplotMakePlot(char *name,int depth,float wdt,float hgt,
                         void *data) {
  struct FrameBufferRplot *ptr;
  ptr=(struct FrameBufferRplot *) data;
  if (ptr==NULL) return -1;
  if (ptr->img !=NULL) FrameBufferFree(ptr->img);
  ptr->img=FrameBufferMake(name,wdt,hgt,depth);
  if (ptr->img==NULL) return -1;
  return 0;
}

int FrameBufferRplotDash(char *str,void *data) {

  struct FrameBufferRplot *ptr;
  ptr=(struct FrameBufferRplot *) data;
  if (ptr==NULL) return -1;
  if (str !=NULL) {
    if ((ptr->dash.txt !=NULL) && (strcmp(ptr->dash.txt,str)==0))
      return 0;
    if (ptr->dash.ptr !=NULL) FrameBufferFreeDash(ptr->dash.ptr);
    ptr->dash.ptr=FrameBufferMakeDashString(str);
    if (ptr->dash.txt !=NULL) free(ptr->dash.txt);
    ptr->dash.txt=malloc(strlen(str)+1);
    strcpy(ptr->dash.txt,str);
  } else {
    if (ptr->dash.txt !=NULL) free(ptr->dash.txt);
    ptr->dash.txt=NULL;
    if (ptr->dash.ptr !=NULL) FrameBufferFreeDash(ptr->dash.ptr);
    ptr->dash.ptr=NULL;
  }
  return 0;
}

int FrameBufferRplotClip(int num,float *px,float *py,int *t,void *data) {
  int  *ix=NULL,*iy=NULL;  
  int s=0; 
  int i;
  struct FrameBufferRplot *ptr;
  struct FrameBuffer *img;
  ptr=(struct FrameBufferRplot *) data;
  if (ptr==NULL) return -1;
  img=ptr->img;
  if (img==NULL) return -1;

  if (ptr->clip !=NULL) FrameBufferFreeClip(ptr->clip);
  ptr->clip=NULL;
  if (num==0) return 0;

  ix=malloc(sizeof(int)*num);
  if (ix==NULL) return -1;
  iy=malloc(sizeof(int)*num);
  if (iy==NULL) {
    free(ix);
    return -1;
  }
  for (i=0;i<num;i++) {
    ix[i]=px[i];
    iy[i]=py[i];
  }

  ptr->clip=FrameBufferMakeClip(img->wdt,img->hgt,num,ix,iy,t);
  if (ptr->clip==NULL) s=-1;
  free(ix);
  free(iy);
  return s;
}


int FrameBufferRplotLine(float x1,float y1,float x2,float y2,
                         unsigned int color,unsigned char mask,float width,
                         void *data) {
  struct FrameBufferRplot *ptr;
  ptr=(struct FrameBufferRplot *) data;
  if (ptr==NULL) return -1;

  FrameBufferLine(ptr->img,x1,y1,x2,y2,color,mask,width,ptr->dash.ptr,
                  ptr->clip);

  return 0;
}

int FrameBufferRplotBezier(float x1,float y1,float x2,float y2,
                           float x3,float y3,float x4,float y4,
                           unsigned int color,unsigned char mask,float width,
                           void *data) {
  struct FrameBufferRplot *ptr;
  ptr=(struct FrameBufferRplot *) data;
  if (ptr==NULL) return -1;

  FrameBufferBezier(ptr->img,x1,y1,x2,y2,x3,y3,x4,y4,0.05,
                    color,mask,width,ptr->dash.ptr,ptr->clip);

  return 0;
}


int FrameBufferRplotShape(char *name,struct RplotInMatrix *matrix,
                          float x,float y,
                          float w,float h,int fill,
                          unsigned int color,unsigned char mask,float width,
                          void *data) {
  struct FrameBufferRplot *ptr;
  struct FrameBufferMatrix fbmatrix;
  struct FrameBufferMatrix *mptr=NULL;

  ptr=(struct FrameBufferRplot *) data;
  if (ptr==NULL) return -1;

  if (matrix !=NULL) {
      fbmatrix.a=matrix->a;
      fbmatrix.b=matrix->b;
      fbmatrix.c=matrix->c;
      fbmatrix.d=matrix->d;
      mptr=&fbmatrix;
    }

   if (strcmp(name,"ellipse")==0)
      FrameBufferEllipse(ptr->img,mptr,x,y,
			 w,h,fill,color,mask,
			 width,ptr->dash.ptr,ptr->clip);
  
  if (strcmp(name,"rectangle")==0)
      FrameBufferRectangle(ptr->img,mptr,x,y,
			 w,h,fill,color,mask,
			 width,ptr->dash.ptr,ptr->clip);

  return 0;
}


int FrameBufferRplotPolygon(struct RplotInMatrix *matrix,
			    float x,float y,int num,float *px,float *py,
                            int *t,int fill,
                            unsigned int color,unsigned char mask,float width,
                            void *data) {
  struct FrameBufferRplot *ptr;
  struct FrameBufferMatrix fbmatrix;
  struct FrameBufferMatrix *mptr=NULL;
  int n;
  int *ipx=NULL;
  int *ipy=NULL;

  ptr=(struct FrameBufferRplot *) data;
  if (ptr==NULL) return -1;

  if (matrix !=NULL) {
      fbmatrix.a=matrix->a;
      fbmatrix.b=matrix->b;
      fbmatrix.c=matrix->c;
      fbmatrix.d=matrix->d;
      mptr=&fbmatrix;
    }


   ipx=malloc(sizeof(int)*num);
   ipy=malloc(sizeof(int)*num);

    for (n=0;n<num;n++) {
      ipx[n]=(int) px[n];
      ipy[n]=(int) py[n];
   
    }
    
    FrameBufferPolygon(ptr->img,mptr,x,y,
		       num,ipx,ipy,t,fill,color,mask,
		       width,ptr->dash.ptr,ptr->clip);
    
    free(ipx);
    free(ipy);
    return 0;  
}


int FrameBufferRplotText(char *fname,float fsize,
                         struct RplotInMatrix *matrix,
                         float x,float y,int num,char *txt,
                         unsigned int color,unsigned char mask,int smooth,
                         void *data) {
  struct FrameBufferRplot *ptr;
  struct FrameBufferMatrix fbmatrix;
  struct FrameBufferMatrix *mptr=NULL;
  struct FrameBufferFont *font=NULL;

  ptr=(struct FrameBufferRplot *) data;
  if (ptr==NULL) return -1;

  if (matrix !=NULL) {
      fbmatrix.a=matrix->a;
      fbmatrix.b=matrix->b;
      fbmatrix.c=matrix->c;
      fbmatrix.d=matrix->d;
      mptr=&fbmatrix;
    }

  font=FrameBufferFontDBFind(ptr->fontdb,fname,(int) fsize);

  FrameBufferText(ptr->img,mptr,font,x,y,num,txt,
                  color,mask,smooth,ptr->clip);

  return 0;

}

int FrameBufferRplotImage(struct FrameBuffer *img,
                         struct RplotInMatrix *matrix,
                         float x,float y,
                         unsigned char mask,int smooth,
                         void *data) {
  struct FrameBufferRplot *ptr;
  struct FrameBufferMatrix fbmatrix;
  struct FrameBufferMatrix *mptr=NULL;

  ptr=(struct FrameBufferRplot *) data;
  if (ptr==NULL) return -1;

  if (matrix !=NULL) {
      fbmatrix.a=matrix->a;
      fbmatrix.b=matrix->b;
      fbmatrix.c=matrix->c;
      fbmatrix.d=matrix->d;
      mptr=&fbmatrix;
    }
  
  FrameBufferImage(ptr->img,mptr,img,mask,x,y,
                   smooth,ptr->clip);  
  
  return 0;

}

int FrameBufferRplotImageName(char *name,
                         struct RplotInMatrix *matrix,
                         float x,float y,
                         unsigned char mask,int smooth,
                         void *data) {
  struct FrameBuffer *img;
  struct FrameBufferRplot *ptr;
  struct FrameBufferMatrix fbmatrix;
  struct FrameBufferMatrix *mptr=NULL;

  ptr=(struct FrameBufferRplot *) data;
  if (ptr==NULL) return -1;

  if (matrix !=NULL) {
      fbmatrix.a=matrix->a;
      fbmatrix.b=matrix->b;
      fbmatrix.c=matrix->c;
      fbmatrix.d=matrix->d;
      mptr=&fbmatrix;
    }
  img=FrameBufferDBFind(ptr->imagedb,name);
 
  if (img==NULL) return 0;
  FrameBufferImage(ptr->img,mptr,img,mask,x,y,
                   smooth,ptr->clip);  
  
  return 0;

}

struct FrameBufferRplot *FrameBufferRplotMake(struct FrameBufferFontDB *fontdb,
                                              struct FrameBufferDB *imagedb) {
  struct FrameBufferRplot *ptr=NULL;
  ptr=malloc(sizeof(struct FrameBufferRplot));
  if (ptr==NULL) return NULL;
  ptr->fontdb=fontdb;
  ptr->imagedb=imagedb;
  ptr->img=NULL;
  ptr->clip=NULL;
  ptr->dash.txt=NULL;
  ptr->dash.ptr=NULL;
  return ptr;
}
                                              
void FrameBufferRplotFree(struct FrameBufferRplot *ptr) {
  if (ptr==NULL) return;
  if (ptr->dash.txt !=NULL) free(ptr->dash.txt);
  if (ptr->dash.ptr !=NULL) FrameBufferFreeDash(ptr->dash.ptr);
  if (ptr->clip !=NULL) FrameBufferFreeClip(ptr->clip);
  if (ptr->img !=NULL) FrameBufferFree(ptr->img);   
  if (ptr->fontdb !=NULL) FrameBufferFontDBFree(ptr->fontdb);
  if (ptr->imagedb !=NULL) FrameBufferDBFree(ptr->imagedb);
  free(ptr);
}                                          


int FrameBufferRplotSetAll(struct RplotInPlot *rplot,
                           struct FrameBufferRplot *ptr) { 

  if (rplot==NULL) return -1;
  if (ptr==NULL) return -1;

  RplotInSetMake(rplot,FrameBufferRplotMakePlot,ptr);
  RplotInSetDash(rplot,FrameBufferRplotDash,ptr);
  RplotInSetClip(rplot,FrameBufferRplotClip,ptr);
  RplotInSetLine(rplot,FrameBufferRplotLine,ptr);
  RplotInSetBezier(rplot,FrameBufferRplotBezier,ptr);
  RplotInSetShape(rplot,FrameBufferRplotShape,ptr);
  RplotInSetPolygon(rplot,FrameBufferRplotPolygon,ptr);
  RplotInSetText(rplot,FrameBufferRplotText,ptr);
  RplotInSetImage(rplot,FrameBufferRplotImage,ptr);
  RplotInSetImageName(rplot,FrameBufferRplotImageName,ptr);
  RplotInSetInclude(rplot,RplotInIncludeFile,NULL);
  return 0;
}


                                             



