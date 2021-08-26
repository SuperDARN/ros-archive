/* RplotMatrixMultiply.c
   =====================
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

  struct RplotMatrix a,b,c;
  struct Rplot *rplot=NULL;

  float wdt=400,hgt=400;

  unsigned int bgcol=0xffffffff;
  unsigned int fgcol=0xff000000;

  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);   
  RplotMakePlot(rplot,"Rplot",wdt,hgt,24);

  RplotRectangle(rplot,NULL,0,0,wdt,hgt,
                  1,bgcol,0x0f,0,NULL);


  a.a=1.0;
  a.b=0.0;
  a.c=0.0;
  a.d=1.0;

  b.a=0.4;
  b.b=0.0;
  b.c=0.0;
  b.d=0.4;

 
  RplotMatrixMultiply(&a,&b,&c);


  RplotRectangle(rplot,&c,10,10,wdt-20,hgt-20,
                  1,fgcol,0x0f,0,NULL);




  RplotEndPlot(rplot);
  return 0;
}

