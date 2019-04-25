/* rfbuffer.h
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

/*
 $Log: rfbuffer.h,v $
 Revision 1.2  2006/09/15 18:00:44  code
 Fixed sign of arguments.

 Revision 1.1  2003/03/14 18:04:17  barnes
 Initial revision

*/

#ifndef _RFBUFFER_H
#define _RFBUFFER_H

struct FrameBufferMatrix {
  float a,b,c,d;
};

struct FrameBufferChar {
  int w,h; /* cell width */
  int l,r,b,t;
  int sze;
  unsigned char *data; 
};

struct FrameBufferFont {
  int num;
  int sze;
  char *name;
  struct FrameBufferChar *chr;
};


struct FrameBufferDash {
    int *p;
    int sze;
    int c,i;
    unsigned char state;
};

struct FrameBufferClip {
  int wdt,hgt;
  unsigned char *clp;
};

struct FrameBuffer {
  char *name;
  int wdt,hgt;
  int depth;
  unsigned char *img;
  unsigned char *msk;
  struct {
    int (*pixel)(int wdt,int hgt,unsigned char *img,
                 unsigned char *msk,int x,int y,int depth,
                 int off,int sze,
                 unsigned char r,unsigned char g,unsigned char b,
                 unsigned char a,
                 void *data);
    void *data;
  } user;
};

struct FrameBufferXMLBuffer {
  struct XMLdata *xml;
  void *data;
  int sze;
  int off;
  unsigned char *ptr;
  char num[5];
  int c;
};

struct FrameBufferXML {
  int state;
  struct XMLdata *xml;
  struct FrameBuffer **img;
};

struct FrameBufferClip *FrameBufferMakeClip(int wdt,int hgt,int num,int *px,
					    int *py,int *t);

void FrameBufferFreeClip(struct FrameBufferClip *ptr);
int FrameBufferClip(struct FrameBufferClip *ptr,int x,int y);

int FrameBufferRawPixel(struct FrameBuffer *ptr,int x,int y,int sze,
                        unsigned char r,unsigned char g,unsigned char b,
                        unsigned char a,unsigned char m,
                        struct FrameBufferClip *clip);
int FrameBufferRawDot(struct FrameBuffer *ptr,int width,int x,int y,int sze,
                      unsigned char r,unsigned g,unsigned b,
                      unsigned char a,unsigned char m,
                      struct FrameBufferDash *dash,
                      struct FrameBufferClip *clip);
int FrameBufferRawLineSegment(
                      struct FrameBuffer *ptr,int width,
                      int ax,int bx,int y,int sze,
                      unsigned char r,unsigned g,unsigned b,
                      unsigned char a,unsigned char m,
                      struct FrameBufferDash *dash,
                      struct FrameBufferClip *clip);

int FrameBufferRawLine(
                      struct FrameBuffer *ptr,int width,
                      int ax,int ay,int bx,int by,int sze,
                      unsigned char r,unsigned g,unsigned b,
                      unsigned char a,unsigned char m,
                      struct FrameBufferDash *dash,
                      struct FrameBufferClip *clip);



struct FrameBuffer *FrameBufferMake(char *name,
				    int wdt,int hgt,int depth);
struct FrameBuffer *FrameBufferLoadPPM(FILE *fp,char *name);
int FrameBufferSavePPM(struct FrameBuffer *ptr,FILE *fp);
int FrameBufferSavePPMAlpha(struct FrameBuffer *ptr,FILE *fp);

struct FrameBuffer *FrameBufferLoadPPMX(FILE *fp);
int FrameBufferSavePPMX(struct FrameBuffer *ptr,FILE *fp);

int FrameBufferSaveXML(struct FrameBuffer *ptr,
                       int (*text)(char *,int,void *),void *data );
int FrameBufferXMLStart(char *name,int atnum,char **atname,char **atval,
                        char *buf,int sze,void *data);

int FrameBufferXMLEnd(char *name,char *buf,int sze,void *data);

void FrameBufferFree(struct FrameBuffer *ptr);
int FrameBufferSetUser(struct FrameBuffer *ptr,
                       int (*pixel)(int wdt,int hgt,unsigned char *img,
                       unsigned char *msk,
                       int x,int y,int depth,int off,int sze,
                       unsigned char r,unsigned char g,unsigned char b,
				    unsigned char a,void *data),void *data);
struct FrameBuffer *FrameBufferCopy(struct FrameBuffer *src);

int FrameBufferClear(struct FrameBuffer *ptr,unsigned int color,
                     unsigned char m);

unsigned int FrameBufferColor(unsigned int r,unsigned int g,unsigned int b,
                              unsigned int a);
 
struct FrameBufferDash *FrameBufferMakeDash(int *p,int phase,int sze);
void FrameBufferFreeDash(struct FrameBufferDash *ptr);
struct FrameBufferDash *FrameBufferMakeDashString(char *str);
 
int FrameBufferDashState(struct FrameBufferDash *ptr);  


struct FrameBufferFont *FrameBufferLoadFont(FILE *fp);
void FrameBufferFreeFont(struct FrameBufferFont *ptr);



/* path elements */

int FrameBufferLine(struct FrameBuffer *ptr,
                    int ax,int ay,int bx,int by,
                    unsigned int color,unsigned char m,int width,
                    struct FrameBufferDash *dash,
                    struct FrameBufferClip *clip);
int FrameBufferBezier(struct FrameBuffer *ptr,
                      int x1,int y1,int x2,int y2,
                      int x3,int y3,int x4,int y4,float step,
                      unsigned int color,unsigned char m,int width,
                      struct FrameBufferDash *dash,
                      struct FrameBufferClip *clip);


/* objects */


int FrameBufferPolygon(struct FrameBuffer *ptr,
		       struct FrameBufferMatrix *matrix,
                       int x,int y,
                       int num,int *px,int *py,int *t,
                       int fill,unsigned int color,unsigned char m,
                       int width,
                       struct FrameBufferDash *dash,
                       struct FrameBufferClip *clip);


int FrameBufferEllipse(struct FrameBuffer *ptr,
                       struct FrameBufferMatrix *matrix,
                       int x,int y,int w,int h,
                       int fill,unsigned int color,
                       unsigned char m,int width,
                       struct FrameBufferDash *dash,
                       struct FrameBufferClip *clip);

int FrameBufferRectangle(struct FrameBuffer *ptr,
	         	 struct FrameBufferMatrix *matrix,
                        int x,int y,int w,int h,
                        int fill,unsigned int color,unsigned  char m,
                        int width,
			struct FrameBufferDash *dash,
                        struct FrameBufferClip *clip);

int FrameBufferText(struct FrameBuffer *ptr,
                    struct FrameBufferMatrix *matrix,
                    struct FrameBufferFont *fnt,int x,int y,
                    int num,char *txt,unsigned int color,unsigned char m,
                    int sflg,
                    struct FrameBufferClip *clip);

int FrameBufferImage(struct FrameBuffer *ptr,
                     struct FrameBufferMatrix *matrix,
                     struct FrameBuffer *img,
                     unsigned char m,
                     int x,int y,int s,
                     struct FrameBufferClip *clip);



int FrameBufferTextBox(struct FrameBufferFont *fnt,
                       int num,char *txt,int *box);



int FrameBufferScaleMatrix(struct FrameBufferMatrix *ptr,
			   float xscale,float yscale);

int FrameBufferRotateMatrix(struct FrameBufferMatrix *ptr,
			    float angle);

int FrameBufferTransform(struct FrameBufferMatrix *ptr,
			 int ix,int iy,int *ox,int *oy);



#endif
