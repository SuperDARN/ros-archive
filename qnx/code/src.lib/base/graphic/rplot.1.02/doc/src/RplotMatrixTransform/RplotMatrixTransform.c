/* RplotMatrixTransform.c
   ======================
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "rfbuffer.h"
#include "rplot.h"

int stream(char *buf,int sze,void *data) {
  FILE *fp;
  fp=(FILE *) data;
  fwrite(buf,sze,1,stdout);
  return 0;
} 


int main(int argc,char *argv[]) {

  struct RplotMatrix m;
  struct Rplot *rplot=NULL;
  int f=0;
  float wdt=400,hgt=400;

  unsigned int bgcol=0xffffffff;
  unsigned int fgcol=0xff000000;

  float ix,iy,ox,oy;

  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);   
  RplotMakePlot(rplot,"Rplot",wdt,hgt,24);

  RplotRectangle(rplot,NULL,0,0,wdt,hgt,
                  1,bgcol,0x0f,0,NULL);


  m.a=0.5;
  m.b=0.0;
  m.c=0.0;
  m.d=0.5;

  for (ix=0;ix<wdt;ix+=10) {
    for (iy=0;iy<hgt;iy+=10) {
      RplotMatrixTransform(&m,ix,iy,&ox,&oy);

      if (f !=0)  RplotRectangle(rplot,NULL,ox,oy,5,5,
                  1,fgcol,0x0f,0,NULL);
      f=! f;
    }
    f=! f;
  }



  RplotEndPlot(rplot);
  return 0;
}

