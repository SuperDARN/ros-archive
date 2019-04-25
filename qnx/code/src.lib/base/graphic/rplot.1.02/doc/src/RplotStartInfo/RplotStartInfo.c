/* RplotStartInfo.c
   ================
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

  char *info="Example plot";


  struct Rplot *rplot=NULL;

  float wdt=400,hgt=400;

  unsigned int bgcol=0xffffffff;
  unsigned int fgcol=0xff808080;

  rplot=RplotMake();
  RplotSetText(rplot,stream,stdout);   

  RplotStartInfo(rplot);
  
  RplotInfo(rplot,info,strlen(info));
  
  RplotEndInfo(rplot);  

  RplotMakePlot(rplot,"Rplot",wdt,hgt,24);

  RplotRectangle(rplot,NULL,0,0,wdt,hgt,
                  1,bgcol,0x0f,0,NULL);


  RplotRectangle(rplot,NULL,10,10,wdt-20,hgt-20,
                  0,fgcol,0x0f,0,NULL);




  RplotEndPlot(rplot);
  return 0;
}

