/* RplotText.c
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

  char *text="Hello world!";
  struct Rplot *rplot=NULL;

  float wdt=400,hgt=400;

  unsigned int bgcol=0xffffffff;
  unsigned int fgcol=0xff000000;

  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);   
  RplotMakePlot(rplot,"Rplot",wdt,hgt,24);

  RplotRectangle(rplot,NULL,0,0,wdt,hgt,
                  1,bgcol,0x0f,0,NULL);


  RplotText(rplot,NULL,"Helvetica",34.0,20,50,
            strlen(text),text,fgcol,0x0f,1);




  RplotEndPlot(rplot);
  return 0;
}

