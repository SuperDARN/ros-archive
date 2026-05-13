/* CSDSeek.c
   =========
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "csddata.h"
#include "csdread.h"



int main(int argc,char *argv[]) {
 
  struct CSDfp *fp=NULL;
  struct CSDdata csd;

  int yr=2004,mo=11,dy=10,hr=10,mt=0;
  double sc=0,atme;

  int s;


  fp=CSDOpen(argv[1]);

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  s=CSDSeek(fp,yr,mo,dy,hr,mt,(int) sc,&atme);
  if (s==-1) {
    fprintf(stderr,"file does not contain that interval.\n");
    exit(-1);
  }

  fprintf(stdout,"Requested:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

  TimeEpochToYMDHMS(atme,&yr,&mo,&dy,&hr,&mt,&sc);
  fprintf(stdout,"Found:%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

  while(CSDRead(fp,&csd) !=-1) {
    
    TimeEpochToYMDHMS(csd.time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d bmnum=%d scan=%d\n",
            yr,mo,dy,hr,mt,(int) sc,csd.bmnum,csd.scan);


   


  }
  CSDClose(fp);

  return 0;
}
