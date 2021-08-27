/* main.c
   ====== */

#include <stdio.h>
#include <stdlib.h>
#include "cnv_geo_gsm.h"

void main() {
   
  double bgeo[3];
  double bgsm[3];
  
  bgeo[0]=10;
  bgeo[1]=20;
  bgeo[2]=30;
 
 
  fprintf(stderr,"%lg %lg %lg\n",bgeo[0],bgeo[1],bgeo[2]);
  cnv_geo_gsm(1998,11,20,22,30,0,bgeo,bgsm);
  
  fprintf(stderr,"%lg %lg %lg\n",bgsm[0],bgsm[1],bgsm[2]);
  

}
  
