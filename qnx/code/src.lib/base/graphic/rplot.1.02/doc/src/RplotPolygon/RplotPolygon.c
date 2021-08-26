/* RplotPolygon.c
   ==============
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

  struct Rplot *rplot=NULL;

  float wdt=400,hgt=400;

  float x[4],y[4];
  int t[4]={0,0,0,0};

  unsigned int bgcol=0xffffffff;
  unsigned int fgcol=0xff000000;

  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);   
  RplotMakePlot(rplot,"Rplot",wdt,hgt,24);

  RplotRectangle(rplot,NULL,0,0,wdt,hgt,
                  1,bgcol,0x0f,0,NULL);



  x[0]=50;
  x[1]=300;
  x[2]=350;
  x[3]=100;

  y[0]=100;
  y[1]=100;
  y[2]=300;
  y[3]=300;

  RplotPolygon(rplot,NULL,0,0,4,x,y,t,
                  1,fgcol,0x0f,0,NULL);

 



  RplotEndPlot(rplot);
  return 0;
}

