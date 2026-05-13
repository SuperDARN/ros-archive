/* AACGMConvertMLT.c
   =================
   Author: R.J.Barnes
*/


#include <stdio.h>
#include <stdlib.h>
#include "aacgm.h"


int main(int argc,char *argv[]) {

  int yr,yrsec;
  double mlon;
  double mlt;

  yr=2000;
  yrsec=3*24*3600;  
  mlon=45.0;

  mlt=AACGMConvertMLT(yr,yrsec,mlon);

  fprintf(stdout,"AACGMConvertMLT\n");
  fprintf(stdout,"Input: yr=%d, yrsec=%d, mlon=%g\n",yr,yrsec,mlon);
  fprintf(stdout,"Output: mlt=%g\n",mlt);
  return 0;
}
