/* CSDReadRadarScan.c
   ==================
   Author: R.J.Barnes */


#include <stdio.h>
#include <stdlib.h>

#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "csddata.h"
#include "csdread.h"
#include "scandata.h"
#include "csdscan.h"

struct RadarScan scn;

int main(int argc,char *argv[]) {
 
  struct CSDfp *fp=NULL;
  struct CSDdata csd;

  int yr,mo,dy,hr,mt;
  double sc;

  int state=0;

  fp=CSDOpen(argv[1]);

  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  while(CSDReadRadarScan(fp,&state,&scn,&csd,0,0,0) !=-1) {
    TimeEpochToYMDHMS(scn.st_time,&yr,&mo,&dy,&hr,&mt,&sc);

    fprintf(stdout,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d\n",
            yr,mo,dy,hr,mt,(int) sc);

  }

  CSDClose(fp);

  return 0;
}
