/* makeplot.c
   ========== 
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
#include "rxml.h"
#include "rfbuffer.h"
#include "rplotin.h"

/*
 $Log: makeplot.c,v $
 Revision 1.1  2003/03/22 23:16:32  barnes
 Initial revision

*/


void RplotInFree(struct RplotInPlot *ptr) {
  if (ptr==NULL) return;
  free(ptr);
  return;
}


struct RplotInPlot *RplotInMake(struct XMLdata *xml) {
  struct RplotInPlot *ptr;
  ptr=malloc(sizeof(struct RplotInPlot));
  if (ptr==NULL) return NULL;
  ptr->xml=xml;
  ptr->x=0;
  ptr->y=0;
  ptr->matrix=NULL;
  ptr->clp.num=0;
  ptr->clp.px=NULL;
  ptr->clp.py=NULL;
  ptr->clp.t=NULL;
  ptr->info.start.user=NULL;
  ptr->info.start.data=NULL;
  ptr->info.text.user=NULL;
  ptr->info.text.data=NULL;
  ptr->info.end.user=NULL;
  ptr->info.end.data=NULL;
  ptr->pass.user=NULL;
  ptr->pass.data=NULL;
  ptr->make.user=NULL;
  ptr->make.data=NULL;
  ptr->end.user=NULL;
  ptr->end.data=NULL;
  ptr->dash.user=NULL;
  ptr->dash.data=NULL;
  ptr->clip.user=NULL;
  ptr->clip.data=NULL;
  ptr->line.user=NULL;
  ptr->line.data=NULL;
  ptr->bezier.user=NULL;
  ptr->bezier.data=NULL;
  ptr->shape.user=NULL;
  ptr->shape.data=NULL;
  ptr->polygon.user=NULL;
  ptr->polygon.data=NULL;
  ptr->text.user=NULL;
  ptr->text.data=NULL;
  ptr->image.user=NULL;
  ptr->image.data=NULL;
  ptr->imagename.user=NULL;
  ptr->imagename.data=NULL;
  ptr->inc.user=NULL;
  ptr->inc.data=NULL;
  return ptr;

}

int RplotInSetInfoText(struct RplotInPlot *ptr,
	               int (*user)(char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->info.text.user=user;
  ptr->info.text.data=data;
  return 0;
}

int RplotInSetInfoStart(struct RplotInPlot *ptr,
		int (*user)(char *,int,char **,char **,
			    char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->info.start.user=user;
  ptr->info.start.data=data;
  return 0;
}

int RplotInSetInfoEnd(struct RplotInPlot *ptr,
	      int (*user)(char *,char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->info.end.user=user;
  ptr->info.end.data=data;
  return 0;
}





int RplotInSetPass(struct RplotInPlot *ptr,
                   int (*text)(char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->pass.user=text;
  ptr->pass.data=data;
  return 0;
}


int RplotInSetMake(struct RplotInPlot *ptr,
                   int (*text)(char *,int,float,float,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->make.user=text;
  ptr->make.data=data;
  return 0;

}




int RplotInSetDash(struct RplotInPlot *ptr,
                   int (*text)(char *,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->dash.user=text;
  ptr->dash.data=data;
  return 0;

}

int RplotInSetClip(struct RplotInPlot *ptr,
                   int (*text)(int,float *,float *,int *,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->clip.user=text;
  ptr->clip.data=data;
  return 0;
}

int RplotInSetEnd(struct RplotInPlot *ptr,
                   int (*text)(void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->end.user=text;
  ptr->end.data=data;
  return 0;

}

int RplotInSetLine(struct RplotInPlot *ptr,
                  int (*text)(float,float,float,float,unsigned int,
                              unsigned char,float,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->line.user=text;
  ptr->line.data=data;
  return 0;

}

int RplotInSetBezier(struct RplotInPlot *ptr,
                  int (*text)(float,float,float,float,float,float,float,float,
                              unsigned int,unsigned char,float,void *),
                              void *data) {
  if (ptr==NULL) return -1;
  ptr->bezier.user=text;
  ptr->bezier.data=data;
  return 0;

}

int RplotInSetShape(struct RplotInPlot *ptr,
                  int (*text)(char *,struct RplotInMatrix *,
                             float,float,float,float,int,unsigned int,
                              unsigned char,float,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->shape.user=text;
  ptr->shape.data=data;
  return 0;

}

int RplotInSetPolygon(struct RplotInPlot *ptr,
                  int (*text)(struct RplotInMatrix *,
                              float,float,int,float *,float *,int *,
                              int,unsigned int,
                              unsigned char,float,void *),void *data) {

 if (ptr==NULL) return -1;
  ptr->polygon.user=text;
  ptr->polygon.data=data;
  return 0;
}


int RplotInSetText(struct RplotInPlot *ptr,
                  int (*text)(char *,float,struct RplotInMatrix *,
                              float,float,int,char *,unsigned int,
                              unsigned char,int,void *),void *data) {

 if (ptr==NULL) return -1;
  ptr->text.user=text;
  ptr->text.data=data;
  return 0;
}


int RplotInSetImage(struct RplotInPlot *ptr,
                  int (*text)(struct FrameBuffer *,struct RplotInMatrix *,
                              float,float,unsigned char,
                              int,void *),void *data) {

  if (ptr==NULL) return -1;
  ptr->image.user=text;
  ptr->image.data=data;
  return 0;
}

int RplotInSetImageName(struct RplotInPlot *ptr,
                  int (*text)(char *,struct RplotInMatrix *,
                              float,float,unsigned char,
                              int,void *),void *data) {

  if (ptr==NULL) return -1;
  ptr->imagename.user=text;
  ptr->imagename.data=data;
  return 0;
}

int RplotInSetInclude(struct RplotInPlot *ptr,
			int (*text)(char *,int (*)(char *,int,void *),
                                    void *,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->inc.user=text;
  ptr->inc.data=data;
  return 0;
}










