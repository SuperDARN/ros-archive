/* rplotin.h
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

/*
 $Log: rplotin.h,v $
 Revision 1.2  2004/04/27 14:00:58  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/22 23:19:52  barnes
 Initial revision

*/


#ifndef _RPLOTIN_H
#define _RPLOTIN_H

struct RplotInIncludeBlock {
  int (*user)(char *name,
		 int (*user)(char *buf,int sze,void *data),
		 void *udata,
		 void *data);
  void *data;
};

struct RplotInPlot {
  struct XMLdata *xml;
  float x,y;
  struct RplotInMatrix *matrix;
  void *data;

  struct {
    int num;
    float *px;
    float *py;
    int *t;
  } clp;

  struct {
    struct {
      int (*user)(char *,int,void *);
      void *data;
    } text;

    struct {
      int (*user)(char *,int,char **,char **,char *,int,void *);
      void *data;
    } start;

    struct {
      int (*user)(char *,char *,int,void *);
      void *data;
    } end;
  } info;

  
  struct {
    int (*user)(char *buf,int sze,void *data);
    void *data;
  } pass;

  struct {
    int (*user)(char *str,void *data);
    void *data;   
  } dash;

  struct {
    int (*user)(int num,float *px,float *py,int *t,void *data);
    void *data;   
  } clip;


  struct {
    int (*user)(void *data);
    void *data;   
  } end;


  struct {
    int (*user)(char *name,int depth,float width,float height,void *data);
    void *data;   
  } make;

  struct {
    int (*user)(float x1,float y1,float x2,float y2,unsigned int color,
                unsigned char mask,float width,void *data);
    void *data;   
  } line;
 
  struct {
    int (*user)(float x1,float y1,float x2,float y2,
                float x3,float y3,float x4,float y4,unsigned int color,
                unsigned char mask,float width,void *data);
    void *data;   
  } bezier;

  struct {
    int (*user)(char *name,struct RplotInMatrix *matrix,float x,float y,
                float w,float h,int fill,
                unsigned int color,unsigned char mask,float width,void *data);
    void *data;   
  } shape;


 struct {
    int (*user)(struct RplotInMatrix *matrix,float x,float y,
                int num,float *px,float *py,int *t,int fill,
                unsigned int color,unsigned char mask,float width,void *data);
    void *data;   
  } polygon;


  struct {
    int (*user)(char *fname,float fsize,
                struct RplotInMatrix *matrix,float x,float y,
                int num,char *txt,
                unsigned int color,unsigned char mask,int smooth,void *data);
    void *data;   
  } text;

  struct {
    int (*user)(struct FrameBuffer *img,
                struct RplotInMatrix *matrix,float x,float y,
                unsigned char mask,int smooth,void *data);
    void *data;   
  } image;

  struct {
    int (*user)(char *name,
                struct RplotInMatrix *matrix,float x,float y,
                unsigned char mask,int smooth,void *data);
    void *data;   
  } imagename;

  struct RplotInIncludeBlock inc;
};

struct RplotInMatrix {
  float a,b,c,d;
};

struct RplotInString {
  struct RplotInIncludeBlock *inc;
  char *tag;
  char *buf;
  char **txt;
};


struct RplotInFloat {
  struct RplotInIncludeBlock *inc;
  char *tag;
  char *buf;
  float *val;
};

struct RplotInInt {
  struct RplotInIncludeBlock *inc;
  char *tag;
  char *buf;
  int *val;
};

struct RplotInPoint {
  struct RplotInIncludeBlock *inc;
  int state;
  float *x,*y;
};

struct RplotInPolyPoint {
  struct RplotInIncludeBlock *inc;
  int state;
  float *x,*y;
  int *t;
};

struct RplotInClipPoint {
  struct RplotInIncludeBlock *inc;
  int state;
  float *x,*y;
  int *t;
};



struct RplotInDimension {
  struct RplotInIncludeBlock *inc;
  int state;
  float *w,*h;
};

struct RplotInLine {
  int state;
  struct RplotInPlot *plt;
  unsigned int color;
  unsigned char mask;
  float width;
  float x1,x2;
  float y1,y2;
};




struct RplotInBezier {

  int state;
  struct RplotInPlot *plt;
  unsigned int color;
  unsigned char mask;
  float width;
  float x1,x2,x3,x4;
  float y1,y2,y3,y4;
};


struct RplotInShape {
  int state;
  int fill;
  struct RplotInPlot *plt;
  unsigned int color;
  unsigned char mask;
  struct RplotInMatrix *matrix;
  float width;
  float x,y;
  float w,h;
};


struct RplotInPolygon {
  int state;
  int fill;
  struct RplotInPlot *plt;
  unsigned int color;
  unsigned char mask;
  struct RplotInMatrix *matrix;
  float width;
  float x,y;
  int num;
  float *px,*py;
  int *t;
};

struct RplotInText {
  int state;
  struct RplotInPlot *plt;
  unsigned int color;
  unsigned char mask;
  struct RplotInMatrix *matrix;
  float x,y;
  int smooth;
  char *fname;
  float fsize;
  char *txt;
};


struct RplotInImage {
  int state;
  struct RplotInPlot *plt;
  unsigned char mask;
  struct RplotInMatrix *matrix;
  float x,y;
  int smooth;
  struct FrameBuffer *img;
};

struct RplotInImageName {
  int state;
  struct RplotInPlot *plt;
  unsigned char mask;
  struct RplotInMatrix *matrix;
  float x,y;
  int smooth;
  char *name;
};

struct RplotInEmbed {
  int state;
  struct RplotInPlot *plt;
  struct RplotInMatrix *matrix;
  float x;
  float y;
};

struct RplotInClip {
  struct RplotInPlot *plt;
  int num;
  float *px,*py;
  int *t;
};

struct RplotInInclude {
  struct RplotInIncludeBlock *inc;
  char *name;
};



int RplotInPointString(char *str,float *x,float *y);

struct RplotInMatrix *RplotInMatrixString(char *str);
int RplotInMatrixTransform(struct RplotInMatrix *ptr,float ix,float iy,
			   float *ox,float *oy);
int RplotInMatrixMultiply(struct RplotInMatrix *x,struct RplotInMatrix *y,
			  struct RplotInMatrix *z);


struct RplotInPlot *RplotInMake(struct XMLdata *xml);
void RplotInFree(struct RplotInPlot *ptr);



int RplotInSetMake(struct RplotInPlot *ptr,
                   int (*text)(char *,int,float,float,void *),void *data);
int RplotInSetDash(struct RplotInPlot *ptr,
                   int (*text)(char *,void *),void *data);


int RplotInSetClip(struct RplotInPlot *ptr,
                   int (*text)(int,float *,float *,int *,void *),void *data);

int RplotInSetEnd(struct RplotInPlot *ptr,
                   int (*text)(void *),void *data);


int RplotInSetLine(struct RplotInPlot *ptr,
                   int (*text)(float,float,float,float,unsigned int,
                               unsigned char,float,void *),void *data);


int RplotInSetBezier(struct RplotInPlot *ptr,
                     int (*text)(float,float,float,float,float,float,
                                 float,float,unsigned int,
                                 unsigned char,float,void *),void *data);

int RplotInSetShape(struct RplotInPlot *ptr,
                  int (*text)(char *,struct RplotInMatrix *,
                              float,float,float,float,int,unsigned int,
                              unsigned char,float,void *),void *data);

int RplotInSetPolygon(struct RplotInPlot *ptr,
                  int (*text)(struct RplotInMatrix *,
                              float,float,int, float *,float *,int *,
                              int,unsigned int,
                              unsigned char,float,void *),void *data);

int RplotInSetText(struct RplotInPlot *ptr,
                  int (*text)(char *,float,struct RplotInMatrix *,
                              float,float,int,char *,unsigned int,
                              unsigned char,int,void *),void *data);


int RplotInSetImage(struct RplotInPlot *ptr,
                  int (*text)(struct FrameBuffer *,struct RplotInMatrix *,
                              float,float,unsigned char,
                              int,void *),void *data);

int RplotInSetImageName(struct RplotInPlot *ptr,
                  int (*text)(char *,struct RplotInMatrix *,
                              float,float,unsigned char,
                              int,void *),void *data);

int RplotInSetInclude(struct RplotInPlot *ptr,
			int (*text)(char *,int (*)(char *,int,void *),
				    void *,void *),void *data);


int RplotInSetInfoText(struct RplotInPlot *ptr,
	               int (*user)(char *,int,void *),void *data);
int RplotInSetInfoStart(struct RplotInPlot *ptr,
		int (*user)(char *,int,char **,char **,
			    char *,int,void *),void *data);
int RplotInSetInfoEnd(struct RplotInPlot *ptr,
	      int (*user)(char *,char *,int,void *),void *data);


int RplotInSetPass(struct RplotInPlot *ptr,
                   int (*text)(char *,int,void *),void *data);


int RplotInStringText(char *buf,int sze,void *data);
int RplotInStringStart(char *name,int atnum,char **atname,char **atval,
                      char *buf,int sze,void *data);
int RplotInStringEnd(char *name,char *buf,int sze,void *data);

int RplotInFloatText(char *buf,int sze,void *data);
int RplotInFloatStart(char *name,int atnum,char **atname,char **atval,
                      char *buf,int sze,void *data);
int RplotInFloatEnd(char *name,char *buf,int sze,void *data);

int RplotInIntText(char *buf,int sze,void *data);
int RplotInIntStart(char *name,int atnum,char **atname,char **atval,
                      char *buf,int sze,void *data);
int RplotInIntEnd(char *name,char *buf,int sze,void *data);

int RplotInPointStart(char *name,int atnum,char **atname,char **atval,
		      char *buf,int sze,void *data);
int RplotInPointEnd(char *name,char *buf,int sze,void *data);


int RplotInDimensionStart(char *name,int atnum,char **atname,char **atval,
			  char *buf,int sze,void *data);

int RplotInDimensionEnd(char *name,char *buf,int sze,void *data);

int RplotInClipStart(char *name,int atnum,char **atname,char **atval,
		      char *buf,int sze,void *data);
int RplotInClipEnd(char *name,char *buf,int sze,void *data);

int RplotInLineStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data);
int RplotInLineEnd(char *name,char *buf,int sze,void *data);

int RplotInBezierStart(char *name,int atnum,char **atname,char **atval,
                     char *buf,int sze,void *data);
int RplotInBezierEnd(char *name,char *buf,int sze,void *data);

int RplotInShapeStart(char *name,int atnum,char **atname,char **atval,
		       char *buf,int sze,void *data);
int RplotInShapeEnd(char *name,char *buf,int sze,void *data);

int RplotInPolygonStart(char *name,int atnum,char **atname,char **atval,
		      char *buf,int sze,void *data);
int RplotInPolygonEnd(char *name,char *buf,int sze,void *data);

int RplotInTextStart(char *name,int atnum,char **atname,char **atval,
			char *buf,int sze,void *data);
int RplotInTextEnd(char *name,char *buf,int sze,void *data);

int RplotInImageStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data);
int RplotInImageEnd(char *name,char *buf,int sze,void *data);


int RplotInImageNameStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data);
int RplotInImageNameEnd(char *name,char *buf,int sze,void *data);

int RplotInEmbedStart(char *name,int atnum,char **atname,char **atval,
                     char *buf,int sze,void *data);
int RplotInEmbedEnd(char *name,char *buf,int sze,void *data);

int RplotInIncludeStart(char *name,int atnum,char **atname,char **atval,
			  char *buf,int sze,void *data);
int RplotInIncludeEnd(char *name,char *buf,int sze,void *data);

int RplotInInfoStart(char *name,int atnum,char **atname,char **atval,
                          char *buf,int sze,void *data);
int RplotInInfoText(char *buf,int sze,void *data);

int RplotInInfoEnd(char *name,char *buf,int sze,void *data);


int RplotInIgnoreStart(char *name,int atnum,char **atname,char **atval,
                          char *buf,int sze,void *data);
int RplotInIgnoreText(char *buf,int sze,void *data);

int RplotInIgnoreEnd(char *name,char *buf,int sze,void *data);



int RplotInPlotStart(char *name,int atnum,char **atname,char **atval,
		     char *buf,int sze,void *data);
int RplotInPlotEnd(char *name,char *buf,int sze,void *data);

int RplotInStart(char *name,int atnum,char **atname,char **atval,
		 char *buf,int sze,void *data);
 
int RplotInEnd(char *name,char *buf,int sze,void *data);

int RplotInText(char *buf,int sze,void *data);

#endif

