/* error.c 
   ======= */


/* 
 $Log: error.c,v $
 Revision 1.1  1999/10/08 22:13:00  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "radar_scan.h"
#include "cell.h"

int limit_error(struct cell *cell,double mean,double s_d,
                double (*prmfn)(struct datapoint *)) {

  int i,j,k,l;
  int c=0;
 
  for (i=0;i<3;i++) {
    for (j=0;j<3;j++) {
      for (k=0;k<3;k++) {   
        for (l=0;l<cell[i*9+j*3+k].cnt;l++) {
           if ((cell[i*9+j*3+k].sct[l] !=0) && 
              (fabs(prmfn(&cell[i*9+j*3+k].data[l])-mean)>(2*s_d))) {
                cell[i*9+j*3+k].sct[l]=0;
                c++;
	   }
	 }
      }
    }
  }
  return c;
}

int calc_mean(struct cell *cell,double *mean,
              double (*prmfn)(struct datapoint *)) {
  int i,j,k,l;
  int c=0;

  *mean=0;
  for (i=0;i<3;i++) {
    for (j=0;j<3;j++) {
      for (k=0;k<3;k++) { 
        for (l=0;l<cell[i*9+j*3+k].cnt;l++) {
           if (cell[i*9+j*3+k].sct[l] !=0) {
             c++;
             *mean+=prmfn(&cell[i*9+j*3+k].data[l]);
	   }
	 }
      }
    }
  }
  if (c !=0) *mean=*mean/c;
  return c;
}

double calc_variance(struct cell *cell,double mean,
                     double (*prmfn)(struct datapoint *)) {
  int i,j,k,l;
  int c=0;

  double variance=0;
  double val=0;
  for (i=0;i<3;i++) {
    for (j=0;j<3;j++) {
      for (k=0;k<3;k++) { 
        for (l=0;l<cell[i*9+j*3+k].cnt;l++) {
           if (cell[i*9+j*3+k].sct[l] !=0) {
             c++;
             val=prmfn(&cell[i*9+j*3+k].data[l]);
             variance+=(val-mean)*
                       (val-mean);
	   }
	 }
      }
    }
  }
  if (c !=0) variance=variance/c;
  return variance;
}

double find_error(struct cell *cell,double (*prmfn)(struct datapoint *)) {

  int c=0;
  int zero=0;
  double mean;
  double variance;
  double sigma=0;

  c=calc_mean(cell,&mean,prmfn);
  variance=calc_variance(cell,mean,prmfn);
  
  if ((c !=0) && (variance >0)) { 
     sigma=sqrt(variance);
     zero=limit_error(cell,mean,sigma,prmfn);
     c=calc_mean(cell,&mean,prmfn);
     variance=calc_variance(cell,mean,prmfn);
  }
  
  if ((c!=0) && (variance>0)) sigma=sqrt(variance);
  else sigma=0;
  return sigma;
}



















