/* rplotrplot.h
   ============ 
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

/*
 $Log: rplotrplot.h,v $
 Revision 1.1  2003/03/03 23:06:30  barnes
 Initial revision

*/


#ifndef _RPLOTRPLOT_H
#define _RPLOTRPLOT_H

struct RplotRplot {
  struct {
    char *txt;
    struct RplotDash *ptr;
  } dash;
  struct FrameBufferDB *imagedb;
  struct Rplot *rplot;
};

int RplotRplotEnd(void *data);

int RplotRplotMakePlot(char *name,int depth,float wdt,float hgt,
			void *data);
int RplotRplotDash(char *str,void *data);

int PostScriptRplotClip(int num,float *px,float *py,int *t,void *data);

int RplotRplotLine(float x1,float y1,float x2,float y2,
                         unsigned int color,unsigned char mask,float width,
			void *data);

int RplotRplotClear(unsigned int color,unsigned char mask,
			 void *data);


int RplotRplotBezier(float x1,float y1,float x2,float y2,
                           float x3,float y3,float x4,float y4,
                           unsigned int color,unsigned char mask,float width,
			  void *data);

int RplotRplotShape(char *name,struct RplotInMatrix *matrix,
                          float x,float y,
                          float w,float h,int fill,
                          unsigned int color,unsigned char mask,float width,
			  void *data);

int RplotRplotPolygon(struct RplotInMatrix *matrix,
			    float x,float y,int num,float *px,float *py,
                            int *t,int fill,
                            unsigned int color,unsigned char mask,float width,
			   void *data);

int RplotRplotText(char *fname,float fsize,
                         struct RplotInMatrix *matrix,
                         float x,float y,int num,char *txt,
                         unsigned int color,unsigned char mask,int smooth,
			void *data);

int RplotRplotImage(struct FrameBuffer *img,
                         struct RplotInMatrix *matrix,
                         float x,float y,
                         unsigned char mask,int smooth,
                         void *data);

int RplotRplotImageName(char *name,
                            struct RplotInMatrix *matrix,
                            float x,float y,
                            unsigned char mask,int smooth,
                            void *data);

struct RplotRplot *RplotRplotMake(struct Rplot *rplot,
                                  struct FrameBufferDB *imagedb);
void RplotRplotFree(struct RplotRplot *ptr);
int RplotRplotSetAll(struct RplotInPlot *rplot,
			  struct RplotRplot *ptr);


#endif
