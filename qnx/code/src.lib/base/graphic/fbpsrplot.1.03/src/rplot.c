/* rplot.c 
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
#include <string.h>
#include "rxml.h"
#include "rfbuffer.h"
#include "rps.h"
#include "rplot.h"
#include "imagedb.h"
#include "rplotin.h"
#include "rplotrplot.h"
#include "include.h"

/*
 $Log: rplot.c,v $
 Revision 1.3  2004/06/16 16:21:17  barnes
 Added missing headers.

 Revision 1.2  2004/04/26 19:42:44  barnes
 Audit - Enforced warnings "all".

 Revision 1.1  2003/03/22 23:21:44  barnes
 Initial revision

*/

int RplotRplotInfoText(char *buf,int sze,void *data) {
  struct RplotRplot *ptr;
  struct Rplot *rplot;
  ptr=(struct RplotRplot *) data;
  if (ptr==NULL) return -1;
  rplot=ptr->rplot;
  if (rplot==NULL) return -1;
  if (rplot->text.user !=NULL) 
    return (rplot->text.user)(buf,sze,rplot->text.data);
  return 0;
}



int RplotRplotInfoStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data) {

  int s=0;
  struct RplotRplot *ptr;
  struct Rplot *rplot;
  ptr=(struct RplotRplot *) data;
  if (ptr==NULL) return -1;
  rplot=ptr->rplot;
  if (rplot==NULL) return -1;
  if (rplot->text.user !=NULL)
    s=(rplot->text.user)("<",1,rplot->text.data);

  if ((s==0) && (rplot->text.user !=NULL))
    s=(rplot->text.user)(buf,sze,rplot->text.data);

  if ((s==0) && (rplot->text.user !=NULL))
    s=(rplot->text.user)(">",1,rplot->text.data);
  return s;
}

int RplotRplotInfoEnd(char *name,char *buf,int sze,void *data) {
  int s=0;
  struct RplotRplot *ptr;
  struct Rplot *rplot;
  ptr=(struct RplotRplot *) data;
  if (ptr==NULL) return -1;
  rplot=ptr->rplot;
  if (rplot==NULL) return -1;
  if (rplot->text.user !=NULL)
    s=(rplot->text.user)("<",1,rplot->text.data);

  if ((s==0) && (rplot->text.user !=NULL))
    s=(rplot->text.user)(buf,sze,rplot->text.data);

  if ((s==0) && (rplot->text.user !=NULL))
    s=(rplot->text.user)(">",1,rplot->text.data);
  return s;

}

int RplotRplotEnd(void *data) {
  struct RplotRplot *ptr;
  ptr=(struct RplotRplot *) data;
  if (ptr==NULL) return -1;
  RplotEndPlot(ptr->rplot);
  return 0;
}

int RplotRplotMakePlot(char *name,int depth,float wdt,float hgt,
                         void *data) {
  struct RplotRplot *ptr;
  ptr=(struct RplotRplot *) data;
  if (ptr==NULL) return -1;
  
  RplotMakePlot(ptr->rplot,name,wdt,hgt,depth);
  return 0;
}

int RplotRplotDash(char *str,void *data) {

  struct RplotRplot *ptr;
  ptr=(struct RplotRplot *) data;
  if (ptr==NULL) return -1;
  if (str !=NULL) {
    if ((ptr->dash.txt !=NULL) && (strcmp(ptr->dash.txt,str)==0))
      return 0;
    if (ptr->dash.ptr !=NULL) RplotFreeDash(ptr->dash.ptr);
    ptr->dash.ptr=RplotMakeDashString(str);
    if (ptr->dash.txt !=NULL) free(ptr->dash.txt);
    ptr->dash.txt=malloc(strlen(str)+1);
    strcpy(ptr->dash.txt,str);
  } else {
    if (ptr->dash.txt !=NULL) free(ptr->dash.txt);
    ptr->dash.txt=NULL;
    if (ptr->dash.ptr !=NULL) RplotFreeDash(ptr->dash.ptr);
    ptr->dash.ptr=NULL;
  }
  return 0;
}

int RplotRplotClip(int num,float *px,float *py,int *t,void *data) {
  struct RplotRplot *ptr;
  ptr=(struct RplotRplot *) data;
  if (ptr==NULL) return -1;
  return RplotClip(ptr->rplot,num,px,py,t);
}



int RplotRplotLine(float x1,float y1,float x2,float y2,
                         unsigned int color,unsigned char mask,float width,
                         void *data) {
  struct RplotRplot *ptr;
  ptr=(struct RplotRplot *) data;
  if (ptr==NULL) return -1;

  RplotLine(ptr->rplot,x1,y1,x2,y2,color,mask,width,ptr->dash.ptr);

  return 0;
}

int RplotRplotBezier(float x1,float y1,float x2,float y2,
                           float x3,float y3,float x4,float y4,
                           unsigned int color,unsigned char mask,float width,
                           void *data) {
  struct RplotRplot *ptr;
  ptr=(struct RplotRplot *) data;
  if (ptr==NULL) return -1;

  RplotBezier(ptr->rplot,x1,y1,x2,y2,x3,y3,x4,y4,
                   color,mask,width,ptr->dash.ptr);

  return 0;
}


int RplotRplotShape(char *name,struct RplotInMatrix *matrix,
                          float x,float y,
                          float w,float h,int fill,
                          unsigned int color,unsigned char mask,float width,
                          void *data) {
  struct RplotRplot *ptr;
  struct RplotMatrix rplotmatrix;
  struct RplotMatrix *mptr=NULL;

  ptr=(struct RplotRplot *) data;
  if (ptr==NULL) return -1;

  if (matrix !=NULL) {
      rplotmatrix.a=matrix->a;
      rplotmatrix.b=matrix->b;
      rplotmatrix.c=matrix->c;
      rplotmatrix.d=matrix->d;
      mptr=&rplotmatrix;
    }

   if (strcmp(name,"ellipse")==0)
      RplotEllipse(ptr->rplot,mptr,x,y,
		      w,h,fill,color,mask,
			 width,ptr->dash.ptr);
  
  if (strcmp(name,"rectangle")==0)
      RplotRectangle(ptr->rplot,mptr,x,y,
		     w,h,fill,color,mask,
		     width,ptr->dash.ptr);

  return 0;
}


int RplotRplotPolygon(struct RplotInMatrix *matrix,
			    float x,float y,int num,float *px,float *py,
                            int *t,int fill,
                            unsigned int color,unsigned char mask,float width,
                            void *data) {
  struct RplotRplot *ptr;
  struct RplotMatrix rplotmatrix;
  struct RplotMatrix *mptr=NULL;
 
  ptr=(struct RplotRplot *) data;
  if (ptr==NULL) return -1;

  if (matrix !=NULL) {
      rplotmatrix.a=matrix->a;
      rplotmatrix.b=matrix->b;
      rplotmatrix.c=matrix->c;
      rplotmatrix.d=matrix->d;
      mptr=&rplotmatrix;
    }


  
    
    RplotPolygon(ptr->rplot,mptr,x,y,
		       num,px,py,t,fill,color,mask,width,ptr->dash.ptr);
   
    return 0;  
}


int RplotRplotText(char *fname,float fsize,
                         struct RplotInMatrix *matrix,
                         float x,float y,int num,char *txt,
                         unsigned int color,unsigned char mask,int smooth,
                         void *data) {
  struct RplotRplot *ptr;
  struct RplotMatrix rplotmatrix;
  struct RplotMatrix *mptr=NULL;

  ptr=(struct RplotRplot *) data;
  if (ptr==NULL) return -1;

  if (matrix !=NULL) {
      rplotmatrix.a=matrix->a;
      rplotmatrix.b=matrix->b;
      rplotmatrix.c=matrix->c;
      rplotmatrix.d=matrix->d;
      mptr=&rplotmatrix;
    }

  
  RplotText(ptr->rplot,mptr,fname,fsize,x,y,num,txt,color,mask,smooth);

  return 0;

}

int RplotRplotImage(struct FrameBuffer *img,
                         struct RplotInMatrix *matrix,
                         float x,float y,
                         unsigned char mask,int smooth,
                         void *data) {
  struct RplotRplot *ptr;
  struct RplotMatrix rplotmatrix;
  struct RplotMatrix *mptr=NULL;

  ptr=(struct RplotRplot *) data;
  if (ptr==NULL) return -1;

  if (matrix !=NULL) {
      rplotmatrix.a=matrix->a;
      rplotmatrix.b=matrix->b;
      rplotmatrix.c=matrix->c;
      rplotmatrix.d=matrix->d;
      mptr=&rplotmatrix;
    }
 
  RplotImage(ptr->rplot,mptr,img,mask,x,y,smooth);  
  
  return 0;

}



int RplotRplotImageName(char *name,
                             struct RplotInMatrix *matrix,
                             float x,float y,
                             unsigned char mask,int smooth,
                             void *data) {
  struct FrameBuffer *img;
  struct RplotRplot *ptr;
  struct RplotMatrix rplotmatrix;
  struct RplotMatrix *mptr=NULL;

  ptr=(struct RplotRplot *) data;
  if (ptr==NULL) return -1;

  if (matrix !=NULL) {
      rplotmatrix.a=matrix->a;
      rplotmatrix.b=matrix->b;
      rplotmatrix.c=matrix->c;
      rplotmatrix.d=matrix->d;
      mptr=&rplotmatrix;
    }
  img=FrameBufferDBFind(ptr->imagedb,name);
  RplotImage(ptr->rplot,mptr,img,mask,x,y,smooth);  
  
  return 0;

}

struct RplotRplot *RplotRplotMake(struct Rplot *rplot,
                                  struct FrameBufferDB *imagedb) {
  struct RplotRplot *ptr=NULL;
  ptr=malloc(sizeof(struct RplotRplot));
  if (ptr==NULL) return NULL;
  ptr->imagedb=imagedb;
  ptr->rplot=rplot;
  ptr->dash.txt=NULL;
  ptr->dash.ptr=NULL;
  return ptr;
}
                                              
void RplotRplotFree(struct RplotRplot *ptr) {
  if (ptr==NULL) return;
  if (ptr->dash.txt !=NULL) free(ptr->dash.txt);
  if (ptr->dash.ptr !=NULL) RplotFreeDash(ptr->dash.ptr);
  if (ptr->rplot !=NULL) RplotFree(ptr->rplot);   
  if (ptr->imagedb !=NULL) FrameBufferDBFree(ptr->imagedb);
  free(ptr);
}                                          


int RplotRplotSetAll(struct RplotInPlot *rplot,
                           struct RplotRplot *ptr) { 

  if (rplot==NULL) return -1;
  if (ptr==NULL) return -1;

  RplotInSetMake(rplot,RplotRplotMakePlot,ptr);
  RplotInSetDash(rplot,RplotRplotDash,ptr);
  RplotInSetClip(rplot,RplotRplotClip,ptr);
  RplotInSetLine(rplot,RplotRplotLine,ptr);
  RplotInSetBezier(rplot,RplotRplotBezier,ptr);
  RplotInSetShape(rplot,RplotRplotShape,ptr);
  RplotInSetPolygon(rplot,RplotRplotPolygon,ptr);
  RplotInSetText(rplot,RplotRplotText,ptr);
  RplotInSetImage(rplot,RplotRplotImage,ptr);
  RplotInSetImageName(rplot,RplotRplotImageName,ptr);
  RplotInSetInclude(rplot,RplotInIncludeFile,NULL);
  RplotInSetEnd(rplot,RplotRplotEnd,ptr);
  RplotInSetInfoStart(rplot,RplotRplotInfoStart,ptr);
  RplotInSetInfoText(rplot,RplotRplotInfoText,ptr);
  RplotInSetInfoEnd(rplot,RplotRplotInfoEnd,ptr);
  return 0;
}


                                             




