/* RplotFreeDash.c
   =============
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
  struct RplotDash *dash=NULL;


  float p[3]={5,2,3};
  

  float wdt=400,hgt=400;

  unsigned int bgcol=0xffffffff;
  unsigned int fgcol=0xff000000;

  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);   
  RplotMakePlot(rplot,"Rplot",wdt,hgt,24);

  dash=RplotMakeDash(p,0,3);

  RplotRectangle(rplot,NULL,0,0,wdt,hgt,
                  1,bgcol,0x0f,0,NULL);


  RplotRectangle(rplot,NULL,10,10,wdt-20,hgt-20,
                 0,fgcol,0x0f,0,dash);

  RplotFreeDash(dash);


  RplotEndPlot(rplot);
  return 0;
}

