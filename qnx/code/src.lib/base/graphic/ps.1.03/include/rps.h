/* rps.h
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
 $Log: rps.h,v $
 Revision 1.1  2003/03/03 21:55:44  barnes
 Initial revision

*/


#ifndef _RPS_H
#define _RPS_H

#define STROKE_COUNT 100

struct PostScriptDash {
  float *p;
  int sze;
  float phase;
};

struct PostScriptClip {
  int num;
  float *px;
  float *py;
  int *t;
};

struct PostScript { 
  int cnt;
  float x,y,wdt,hgt;
  int land;
  int num;
  int pagenum;

  float width;
  unsigned int color;
  struct PostScriptDash *dash;
  struct PostScriptClip *clip;
  float px,py;


  struct {
    int (*user)(char *,int,void *);
    void *data;
  } text;
};




struct PostScriptMatrix {
  float a,b,c,d;
};

struct PostScript *PostScriptMake();
void PostScriptFree(struct PostScript *ptr);
int PostScriptSetText(struct PostScript *ptr,
                      int (*text)(char *,int,void *),void *data);

struct PostScriptMatrix *PostScriptMatrixString(char *str);
int PostScriptScaleMatrix(struct PostScriptMatrix *ptr,
			  float xscale,float yscale);
int PostScriptRotateMatrix(struct PostScriptMatrix *ptr,
			   float angle);
unsigned int PostScriptColor(int r,int g,int b);



int PostScriptMakePlot(struct PostScript *ptr,
                       float x,float y,float wdt,float hgt,
                       int land);
int PostScriptNewPage(struct PostScript *ptr);
int PostScriptEndPlot(struct PostScript *ptr);

struct PostScriptClip *PostScriptMakeClip(float x,float y,float wdt,float hgt,
                                          int num,float *px,float *py,int *t);
void PostScriptFreeClip(struct PostScriptClip *ptr);
int PostScriptClip(struct PostScript *ptr,struct PostScriptClip *clip);

struct PostScriptDash *PostScriptMakeDash(float *p,float phase,int sze);
struct PostScriptDash *PostScriptMakeDashString(char *str);
void PostScriptFreeDash(struct PostScriptDash *ptr);


int PostScriptState(struct PostScript *ptr,
                    unsigned int color,float width,
                    struct PostScriptDash *dash,
                    struct PostScriptClip *clip);

int PostScriptLine(struct PostScript *ptr,
              float ax,float ay,float bx,float by,
              unsigned int color,float width,
		   struct PostScriptDash *dash,
                   struct PostScriptClip *clip);

int PostScriptBezier(struct PostScript *ptr,
                     float x1,float y1,float x2,float y2,float x3,float y3,
                     float x4,float y4,
                     unsigned int color,float width,
                     struct PostScriptDash *dash,
                     struct PostScriptClip *clip);

int PostScriptPolygon(struct PostScript *ptr,
                   struct PostScriptMatrix *matrix,
                   float x,float y,
                   int num,float *px,float *py,int *t,int fill,
                   unsigned int color,float width,
		   struct PostScriptDash *dash,
                   struct PostScriptClip *clip);

int PostScriptEllipse(struct PostScript *ptr,
                      struct PostScriptMatrix *matrix,
                      float x,float y,float w,float h,
                      int fill,unsigned int color,
                      float width,
                      struct PostScriptDash *dash,
                      struct PostScriptClip *clip);

int PostScriptRectangle(struct PostScript *ptr,
                 struct PostScriptMatrix *matrix,
                 float x,float y,float w,float h,
                 int fill,unsigned int color,
                 float width,
			struct PostScriptDash *dash,
                        struct PostScriptClip *clip);

int PostScriptText(struct PostScript *ptr,
	           struct PostScriptMatrix *matrix,
                   char *fname,float fsize,
                   float x,float y,int num,char *txt,
		   unsigned int color,
                   struct PostScriptClip *clip);

int PostScriptImage(struct PostScript *ptr,
	           struct PostScriptMatrix *matrix,
		   struct FrameBuffer *img,unsigned char mask,
                   float x,float y,
                   struct PostScriptClip *clip);

#endif
