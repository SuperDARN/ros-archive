/* nrfit.c
   =======
   Numerical Recipes 
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
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


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
 $Log: nrfit.c,v $
 Revision 1.2  2007/02/16 21:53:03  code
 Changed argument types.

 Revision 1.1  2007/02/16 21:50:32  code
 Initial revision

*/


void nrfit(float *x,float *y,int ndata,float *sig,int mwt,
                  float *a,float *b,float *siga,float *sigb,float *chi2,
                  float *q) {

  int i;
  float wt,t,sxoss,sx=0.0,sy=0.0,st2=0.0,ss,sigdat;
 
  *b=0.0;

  if (mwt) {
    ss=0.0;
    for (i=0;i<ndata;i++) {
      wt=1.0/(sig[i]*sig[i]);
      ss+=wt;
      sx+=x[i]*wt;
      sy+=y[i]*wt;
    }
  } else {
    for (i=0;i<ndata;i++) {
      sx+=x[i];
      sy+=y[i];
    }
    ss=ndata;
  }
  sxoss=sx/ss;

  if (mwt) {
    for (i=0;i<ndata;i++) {
      t=(x[i]-sxoss)/sig[i];
      st2+=t*t;
      *b+=t*y[i]/sig[i];
    }
  } else {
    for (i=0;i<ndata;i++) {
      t=x[i]-sxoss;
      st2+=t*t;
      *b+=t*y[i];
    }
  }
  *b/=st2;
  *a=(sy-sx*(*b))/ss;
  *siga=sqrt((1.0+sx*sx/(ss*st2))/ss);
  *sigb=sqrt(1.0/st2);
  
  *chi2=0.0;
  if (mwt==0) {
    for (i=0;i<ndata;i++)
      *chi2+=(y[i]-(*a)-(*b)*x[i])*(y[i]-(*a)-(*b)*x[i]);
    *q=1.0;
    sigdat=sqrt((*chi2)/(ndata-2));
    *siga *= sigdat; 
    *sigb *= sigdat;
  } else {
    for (i=0;i<ndata;i++) 
      *chi2+=((y[i]-(*a)-(*b)*x[i])/sig[i])*((y[i]-(*a)-(*b)*x[i])/sig[i]);
    /*    *q=gammaq(0.5*(ndata-2),0.5*(*chi2)); */
    *q=1.0;
  }
}
