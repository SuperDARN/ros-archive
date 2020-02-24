/* error.c
   =======
   Author: R.J.Barnes
*/

/*
 Copyright © 2001 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 RST-ROS (hereinafter "the Program").
 
 The Program was developed at The Johns Hopkins University/Applied Physics
 Laboratory (JHU/APL) which is the author thereof under the "work made for
 hire" provisions of the copyright law.  
 
 JHU/APL assumes no obligation to provide support of any kind with regard to
 the Program.  This includes no obligation to provide assistance in using the
 Program or to provide updated versions of the Program.
 
 THE PROGRAM AND ITS DOCUMENTATION ARE PROVIDED AS IS AND WITHOUT ANY EXPRESS
 OR IMPLIED WARRANTIES WHATSOEVER.  ALL WARRANTIES INCLUDING, BUT NOT LIMITED
 TO, PERFORMANCE, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 HEREBY DISCLAIMED.  YOU ASSUME THE ENTIRE RISK AND LIABILITY OF USING THE
 PROGRAM TO INCLUDE USE IN COMPLIANCE WITH ANY THIRD PARTY RIGHTS.  YOU ARE
 ADVISED TO TEST THE PROGRAM THOROUGHLY BEFORE RELYING ON IT.  IN NO EVENT
 SHALL JHU/APL BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING, WITHOUT
 LIMITATION, ANY LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, ARISING OUT OF THE USE OR INABILITY TO USE THE
 PROGRAM."
 
 
 
 
 
 
*/


/* 
 $Log: error.c,v $
 Revision 1.3  2001/06/27 20:47:43  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:52  barnes
 Added Author Name

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



















