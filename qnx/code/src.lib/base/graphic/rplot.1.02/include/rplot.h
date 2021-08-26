/* rplot.h
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

/*
 $Log: rplot.h,v $
 Revision 1.2  2004/04/27 14:05:56  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/22 23:26:35  barnes
 Initial revision

*/


#ifndef _RPLOT_H
#define _RPLOT_H

struct Rplot { 
  struct {
    int (*user)(char *,int,void *);
    void *data;
  } text;
};

struct RplotDash {
  float *p;
  int sze;
  float phase;
};


struct RplotMatrix {
  float a,b,c,d;
};

struct Rplot *RplotMake();
void RplotFree(struct Rplot *ptr);
int RplotSetText(struct Rplot *ptr,int (*text)(char *,int,void *),void *data);
 

struct RplotDash *RplotMakeDash(float *p,float phase,int sze); 
struct RplotDash *RplotMakeDashString(char *str);
void RplotFreeDash(struct RplotDash *ptr);
 
struct RplotMatrix *RplotMatrixString(char *str);
int RplotScaleMatrix(struct RplotMatrix *ptr,
		     float xscale,float yscale);
int RplotRotateMatrix(struct RplotMatrix *ptr,
                      float angle);
 
int RplotMatrixTransform(struct RplotMatrix *ptr,float ix,float iy,
			 float *ox,float *oy); 

int RplotMatrixMultiply(struct RplotMatrix *x,struct RplotMatrix *y,
			struct RplotMatrix *z);

int RplotMakePlot(struct Rplot *ptr,char *name,float wdt,float hgt,int depth);
int RplotEndPlot(struct Rplot *ptr);


int RplotLine(struct Rplot *ptr,
              float ax,float ay,float bx,float by,
              unsigned int color,unsigned char mask,float width,
              struct RplotDash *dash);
int RplotBezier(struct Rplot *ptr,
                float x1,float y1,float x2,float y2,
                float x3,float y3,float x4,float y4,
                unsigned int color,unsigned char mask,float width,
                struct RplotDash *dash);
unsigned int RplotColor(int r,int g,int b,int a);
unsigned int RplotColorStringRGB(char *txt);
unsigned int RplotColorStringRGBA(char *txt);


int RplotRectangle(struct Rplot *ptr,
	           struct RplotMatrix *matrix,
                   float,float y,float w,float h,
                   int fill,unsigned int color,unsigned  char mask,
                   float width,
		   struct RplotDash *dash);

int RplotEllipse(struct Rplot *ptr,
	           struct RplotMatrix *matrix,
                   float,float y,float w,float h,
                   int fill,unsigned int color,unsigned  char mask,
                   float width,
		   struct RplotDash *dash);

int RplotPolygon(struct Rplot *ptr,struct RplotMatrix *matrix,
                 float x,float y,
                 int num,float *px,float *py,int *t,int fill,
                 unsigned int color,unsigned char mask,float width,
                 struct RplotDash *dash);


int RplotText(struct Rplot *ptr,
                    struct RplotMatrix *matrix,
                    char *fname,float fsize,float x,float y,
                    int num,char *txt,unsigned int color,unsigned char m,
                    int sflg);

int RplotImage(struct Rplot *ptr,
	       struct RplotMatrix *matrix,
	       struct FrameBuffer *img,
	       unsigned  char mask,
	       float x,float y,int sflg);

int RplotImageName(struct Rplot *ptr,
	       struct RplotMatrix *matrix,
	       char *name,
	       unsigned  char mask,
	       float x,float y,int sflg);


int RplotEmbed(struct Rplot *ptr,
               struct RplotMatrix *matrix,
               float x,float y);

int RplotInclude(struct Rplot *ptr,char *name);


int RplotEndEmbed(struct Rplot *ptr);

int RplotInfoStart(struct Rplot *ptr);
int RplotInfo(struct Rplot *ptr,char *buf,int sze);
int RplotInfoEnd(struct Rplot *ptr);


int RplotClip(struct Rplot *ptr,
                     int num,float *px,float *py,int *t);


int RplotStartInfo(struct Rplot *ptr);
int RplotInfo(struct Rplot *ptr,char *buf,int sze);
int RplotEndInfo(struct Rplot *ptr);


#endif
