/* matcald.c
   ========= */

/*
 $Log: matcald.c,v $
 Revision 1.2  1999/09/08 20:17:31  barnes
 Fixed missing return value.

 Revision 1.1  1999/08/05 20:30:55  barnes
 Initial revision

 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "matcald.h"

int angl2d(double v1, double v2, double *angle) {
    if (v1 == 0. && v2 == 0.) {
	*angle = 0.;
    } else {
	*angle = atan2(v2, v1);
    }

    return 0;
} 


int angl3d(double v1, double v2, double v3, double *angle1, double *angle2) {
   
 
    double r12;

    r12 = sqrt(v1 * v1 + v2 * v2);
    angl2d(v3, r12, angle1);
    angl2d(v1, v2, angle2);
    return 0;
} 


double vlngth(double v1, double v2, double v3) {
  
    double ret_val;
    ret_val = sqrt(v1 * v1 + v2 * v2 + v3 * v3);
    return ret_val;
} 

int polcrd(double  v1, double v2, double v3, double *vln, 
           double *angle1, double *angle2) {
  
    *vln = vlngth(v1, v2, v3);

    angl3d(v1, v2, v3, angle1, angle2);
    return 0;
} 



int unifyv(double  *a,double *b) {
   
    double vlen;

    --b;
    --a;

    vlen = sqrt(a[1] * a[1] + a[2] * a[2] + a[3] * a[3]);
    b[1] = a[1] / vlen;
    b[2] = a[2] / vlen;
    b[3] = a[3] / vlen;

    return 0;
} 

double prdin(double *a, double *b) {
   
    double ret_val;

    --b;
    --a;

    ret_val = a[1] * b[1] + a[2] * b[2] + a[3] * b[3];

    return ret_val;
} 


int prdout(double *a, double *b, double *c) {

    --c;
    --b;
    --a;

  
    c[1] = a[2] * b[3] - a[3] * b[2];
    c[2] = a[3] * b[1] - a[1] * b[3];
    c[3] = a[1] * b[2] - a[2] * b[1];

    return 0;
} 




double det3(double *x) {
    
    double ret_val;
    x -= 4;

  
    ret_val = x[4] * (x[8] * x[12] - x[11] * x[9]) + x[7] * 
             (x[11] * x[6] - x[5] * x[12]) + x[10] * 
             (x[5] * x[9] - x[8] * x[6]);
    return ret_val;
}




int inver3(double *x, double *y) {

    double xx;
    y -= 4;
    x -= 4; 


    xx = det3(&x[4]);

    y[4] = (x[8] * x[12] - x[11] * x[9]) / xx;
    y[5] = (x[5] * x[12] - x[11] * x[6]) * -1. / xx;
    y[6] = (x[5] * x[9] - x[6] * x[8]) / xx;
    y[7] = (x[7] * x[12] - x[10] * x[9]) * -1. / xx;
    y[8] = (x[4] * x[12] - x[10] * x[6]) / xx;
    y[9] = (x[4] * x[9] - x[7] * x[6]) * -1. / xx;
    y[10] = (x[7] * x[11] - x[10] * x[8]) / xx;
    y[11] = (x[4] * x[11] - x[10] * x[5]) * -1. / xx;
    y[12] = (x[4] * x[8] - x[5] * x[7]) / xx;
    return 0;
} 



int mtrvct(double *mtrx, double *vectp, double *vecta) {
    int i, j;

    --vecta;
    --vectp;
    mtrx -= 4;

    for (i = 1; i <= 3; ++i) {
	vecta[i] = 0.;
	for (j = 1; j <= 3; ++j) {
	    vecta[i] += mtrx[i + j * 3] * vectp[j];
	}
    }
    return 0;
} 



int mtrprd(double *a, double *b, double *c) {
    int i, j, k;

    c -= 4;
    b -= 4;
    a -= 4;

    /* Function Body */
    for (i = 1; i <= 3; ++i) {
	for (j = 1; j <= 3; ++j) {
	    c[i + j * 3] = 0.;
	    for (k = 1; k <= 3; ++k) {
		c[i + j * 3] += a[i + k * 3] * b[k + j * 3];
	    }
	}
    }
    return 0;
} 

int asmunt(double *vx, double *vy, double *vz, double *trmtrx) {

    trmtrx -= 4;
    --vz;
    --vy;
    --vx;

    trmtrx[4] = vx[1];
    trmtrx[7] = vx[2];
    trmtrx[10] = vx[3];
    trmtrx[5] = vy[1];
    trmtrx[8] = vy[2];
    trmtrx[11] = vy[3];
    trmtrx[6] = vz[1];
    trmtrx[9] = vz[2];
    trmtrx[12] = vz[3];

    return 0;
} 


int rotmtr(int idaxis, double angl, double *a) {
    

   double dumcos, dumsin;

   a -= 4;

 
   dumsin = sin(angl);
   dumcos = cos(angl);

   if (idaxis == 1) {
     a[4] = 1.;
     a[7] = 0.;
     a[10] = 0.;
     a[5] = 0.;
     a[8] = dumcos;
     a[11] = dumsin;
     a[6] = 0.;
     a[9] = dumsin * -1.;
     a[12] = dumcos;
   }

   if (idaxis == 2) {
     a[4] = dumcos;
     a[7] = 0.;
     a[10] = dumsin * -1.;
     a[5] = 0.;
     a[8] = 1.;
     a[11] = 0.;
     a[6] = dumsin;
     a[9] = 0.;
     a[12] = dumcos;
   }

   if (idaxis == 3) {
      a[4] = dumcos;
      a[7] = dumsin;
      a[10] = 0.;
      a[5] = dumsin * -1.;
      a[8] = dumcos;
      a[11] = 0.;
      a[6] = 0.;
      a[9] = 0.;
      a[12] = 1.;
   }
   return 0;
} 

