/* CSDClose.c
   ==========
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

  int yr,mo,dy,hr,mt;
  double sc;

  fp=CSDOpen(argv[1]);

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  while(CSDRead(fp,&csd) !=-1) {
    
    TimeEpochToYMDHMS(csd.time,&yr,&mo,&dy,&hr,&mt,&sc);
    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d bmnum=%d\n",
            yr,mo,dy,hr,mt,(int) sc,csd.bmnum);


   


  }
  CSDClose(fp);

  return 0;
}
