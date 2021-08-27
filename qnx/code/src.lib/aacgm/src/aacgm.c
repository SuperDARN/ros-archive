/* aacgm.c
   ======= */

/*
 $Log: aacgm.c,v $
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
  if (year==0) year=1995;
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
     




