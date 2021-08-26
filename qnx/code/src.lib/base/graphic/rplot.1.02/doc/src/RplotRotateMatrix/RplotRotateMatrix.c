/* RplotRotateMatrix.c
   ===================
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

  float wdt=400,hgt=400;

  unsigned int bgcol=0xffffffff;
  unsigned int fgcol=0xff000000;
  int a;

  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);   
  RplotMakePlot(rplot,"Rplot",wdt,hgt,24);

  RplotRectangle(rplot,NULL,0,0,wdt,hgt,
                  1,bgcol,0x0f,0,NULL);


  m.a=1.0;
  m.b=0.0;
  m.c=0.0;
  m.d=1.0;


  for (a=0;a<360;a++) {
    RplotRotateMatrix(&m,10.0);


    RplotRectangle(rplot,&m,wdt/2,hgt/2,wdt/4,hgt/4,
                   0,fgcol,0x0f,0,NULL);
  }



  RplotEndPlot(rplot);
  return 0;
}

