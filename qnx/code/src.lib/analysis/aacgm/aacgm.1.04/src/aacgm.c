/* aacgm.c
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
 $Log: aacgm.c,v $
 Revision 1.8  2001/06/27 20:24:39  barnes
 Added license tag.

 Revision 1.7  2001/04/09 14:29:25  barnes
 Switched to year 2000 co-efficients.

 Revision 1.6  2001/01/29 18:11:47  barnes
 Added Author Name

 Revision 1.5  1998/11/03 20:04:16  barnes
 Fixed problem of the r variable not being set in the coordinate conversion.

 Revision 1.4  1998/06/17 18:59:41  barnes
 Changed the code so that the order of the fit can be specified.

 Revision 1.3  1998/06/17 12:51:32  barnes
 Fixed problem with sign of latitude sometimes being reversed.

 Revision 1.2  1998/06/05 20:46:36  barnes
 Modifications to fix problems with return codes.

 Revision 1.1  1998/05/12 14:04:48  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "default.h"
#include "convert_geo_coord.h"
#include "mlt.h"

extern struct {
    double coef[121][3][5][2];
} sph_harm_model;


int load_coef_aacgm(char *fname) {
  char tmp[64];
  FILE *fp;
  int f,l,a,t,i;
  fp=fopen(fname,"r");
  if (fp==NULL) return -1;
  for (f=0;f<2;f++) { 
    for (l=0;l<5;l++) {
      for (a=0;a<3;a++) { 
        for (t=0;t<121;t++) {
	  if (fscanf(fp,"%s",&tmp) !=1) {
             fclose(fp);
             return -1;
	  }
          for (i=0;(tmp[i] !=0) && (tmp[i] !='D');i++);
          if (tmp[i]=='D') tmp[i]='e';
          sph_harm_model.coef[t][a][l][f]=atof(tmp);
	}
      }
    }
  }
  fclose(fp);
  return 0;
}

int init_aacgm(int year) {
  char fname[256];
  char yrstr[32];  
  if (year==0) year=DEFAULT_YEAR;
  sprintf(yrstr,"%4.4d",year);  
  strcpy(fname,getenv("AACGM_DAT_PREFIX"));  
  if (strlen(fname)==0) return -1;
  strcat(fname,yrstr);
  strcat(fname,".asc");
  return load_coef_aacgm(fname);
}

int cnv_aacgm(double in_lat,double in_lon,double height,
              double *out_lat,double *out_lon,double *r,
              int flag) {

   int err;
   err=convert_geo_coord(in_lat,in_lon,height,
		     out_lat,out_lon,flag,10);
   *r=1.0;
   if (err !=0) return -1;
   return 0;
}

double mlt_aacgm(int yr,int yr_sec,double mlon) {
  double mslon;
  return mlt(yr,yr_sec,mlon,&mslon);
}
     




