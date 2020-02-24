/* inv_mag.c
   =========
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
 $Log: inv_mag.c,v $
 Revision 1.5  2001/06/27 20:49:59  barnes
 Added license tag

 Revision 1.4  2001/01/29 18:11:53  barnes
 Added Author Name

 Revision 1.3  2000/11/02 17:17:09  barnes
 Added new function to return the azimuth and elevation angle at a field point.

 Revision 1.2  1999/12/15 19:41:34  barnes
 Modifications to pass in receiver rise time variations.

 Revision 1.1  1999/08/11 17:15:18  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "math_constant.h"
#include "aacgm_lib.h"
#include "magcmp.h"
#include "geo.h"
#include "geodtgc.h"

#include "hardware.h" 

void norm_vec(double *x,double *y,double *z) {

  double r;
  r=sqrt(*x**x+*y**y+*z**z);
  *x=*x/r;
  *y=*y/r;
  *z=*z/r;
}

void sphtocar(double r,double theta,double phi,double *x,double *y,double *z) {
  *x=r*sind(90.0-theta)*cosd(phi);
  *y=r*sind(90.0-theta)*sind(phi);
  *z=r*cosd(90.0-theta);
}


void fldpnt_sph(double frho,double flat,double flon,double az,
                double r,double *xlat,double *xlon) {
   
    double api,aside,bside,cside;
    double Aangl,Bangl,arg;

    api=4*atan(1.0);
    cside=90.0-flat;
    Aangl=az;

    if (Aangl > 180) Aangl=Aangl-360;

    bside=r/frho*(180.0/api);

    arg=cosd(bside)*cosd(cside)+sind(bside)*sind(cside)*cosd(Aangl);
    
    if (arg <= -1.0) arg=-1.0;
    if (arg >= 1.0) arg=1.0;
    
    aside=acosd(arg);
    arg=(cosd(bside)-cosd(aside)*cosd(cside))/(sind(aside)*sind(cside));
    
 
    if (arg <= -1.0) arg=-1.0;
    if (arg >= 1.0) arg=1.0;
    
    Bangl=acosd(arg);
    if (Aangl <0) Bangl=-Bangl;

    *xlat=90-aside;
    *xlon=flon+Bangl;

    if (*xlon < 0) *xlon+=360;
    if (*xlon > 360) *xlon-=360;
}


void fldpnt_azm(double mlat,double mlon,double nlat,double nlon,double *az) {
   
  double api;
  double aside,bside,cside;
  double Aangl,Bangl,arg;

  api=4*atan(1.0);
  aside=90-nlat;
  cside=90-mlat;
 
  Bangl=nlon-mlon;
  
  arg=cosd(aside)*cosd(cside)+sind(aside)*sind(cside)*cosd(Bangl);
  bside=acosd(arg);
  
  arg=(cosd(aside)-cosd(bside)*cosd(cside))/
      (sind(bside)*sind(cside));
  
  Aangl=acosd(arg);

  if (Bangl<0) Aangl=-Aangl;
  *az=Aangl;
 
} 


void glbthor(int iopt,double lat,double lon,
             double *rx,double *ry,double *rz,
             double *tx,double *ty,double *tz) {
   double sx,sy,sz,lax;
   if (iopt>0) {
     sx=cosd(lon)**rx+sind(lon)**ry;
     sy=-sind(lon)**rx+cosd(lon)**ry;
     sz=*rz;
     lax=90-lat;
     *tx=cosd(lax)*sx-sind(lax)*sz;
     *ty=sy;
     *tz=sind(lax)*sx+cosd(lax)*sz;
   } else {
     lax=90-lat;
     sx=cosd(lax)**tx+sind(lax)**tz;
     sy=*ty;
     sz=-sind(lax)**tx+cosd(lax)**tz;
     *rx=cosd(lon)*sx-sind(lon)*sy;
     *ry=sind(lon)*sx+cosd(lon)*sy;
     *rz=sz;
   }

}


void rngbm_azmelv(int bm,int rn,int year,struct radar_site *hdw,double lagfr,
                    double smsep,double rx,double height,
                    double *azm,double *elv) {

  double flat,flon,frho;
  double fx,fy,fz;

  double gx,gy,gz;
  double glat,glon;
  double gdlat,gdlon,gdrho;
  double gbx,gby,gbz; 
  double ghx,ghy,ghz;
  double bx,by,bz,b;
  double dummy;

  double r,tmp_ht;
  double xlat,xlon,nlat,nlon;

 
  gdlat=hdw->geo_lat;
  gdlon=hdw->geo_long;

  if (rx==0) rx=hdw->rec_rise;

  geographic(1,bm,rn,hdw,lagfr,smsep,rx,
             height,&frho,&flat,&flon);

  sphtocar(frho,flat,flon,&fx,&fy,&fz);       
  geodtgc(1,&gdlat,&gdlon,&gdrho,&glat,&glon,&dummy);
  sphtocar(gdrho,glat,glon,&gbx,&gby,&gbz);       
  gx=fx-gbx;
  gy=fy-gby;
  gz=fz-gbz;     
  norm_vec(&gx,&gy,&gz);
  glbthor(1,flat,flon,&gx,&gy,&gz,&ghx,&ghy,&ghz);
  norm_vec(&ghx,&ghy,&ghz);
            
  magcmp(year,frho,flat,flon,&bx,&by,&bz,&b);

  norm_vec(&bx,&by,&bz);
  ghz=-(bx*ghx+by*ghy)/bz;
  norm_vec(&ghx,&ghy,&ghz);
  *elv=atan2d(ghz,sqrt(ghx*ghx+ghy*ghy));
  *azm=atan2d(ghy,-ghx);
}





void inv_mag(int bm,int rn,int year,struct radar_site *hdw,double lagfr,
             double smsep,double rx,double height,
             double *mlat,double *mlon,double *azm) {

  double flat,flon,frho;
  double fx,fy,fz;

  double gx,gy,gz;
  double glat,glon;
  double gdlat,gdlon,gdrho;
  double gbx,gby,gbz; 
  double ghx,ghy,ghz;
  double bx,by,bz,b;
  double dummy,elv,azc;

  double r,tmp_ht;
  double xlat,xlon,nlat,nlon;

  
 

  gdlat=hdw->geo_lat;
  gdlon=hdw->geo_long;

  if (rx==0) rx=hdw->rec_rise;

  geographic(1,bm,rn,hdw,lagfr,smsep,rx,
             height,&frho,&flat,&flon);

  sphtocar(frho,flat,flon,&fx,&fy,&fz);       
  geodtgc(1,&gdlat,&gdlon,&gdrho,&glat,&glon,&dummy);
  sphtocar(gdrho,glat,glon,&gbx,&gby,&gbz);       
  gx=fx-gbx;
  gy=fy-gby;
  gz=fz-gbz;     
  norm_vec(&gx,&gy,&gz);
  glbthor(1,flat,flon,&gx,&gy,&gz,&ghx,&ghy,&ghz);
  norm_vec(&ghx,&ghy,&ghz);
            
  magcmp(year,frho,flat,flon,&bx,&by,&bz,&b);

  norm_vec(&bx,&by,&bz);
  ghz=-(bx*ghx+by*ghy)/bz;
  norm_vec(&ghx,&ghy,&ghz);
  elv=atan2d(ghz,sqrt(ghx*ghx+ghy*ghy));
  azc=atan2d(ghy,-ghx);
      
  r=smsep*0.15;
   
  geodtgc(-1,&gdlat,&gdlon,&gdrho,&flat,&flon,&dummy);
  tmp_ht=frho-gdrho;
   
  cnv_aacgm(flat,flon,tmp_ht,mlat,mlon,&dummy,0);
        
  fldpnt_sph(frho,flat,flon,azc,r,&xlat,&xlon);
      
  cnv_aacgm(xlat,xlon,tmp_ht,&nlat,&nlon,&dummy,0);
       
  if ((nlon-*mlon) > 180)  nlon=nlon-360;
  if ((nlon-*mlon) < -180) nlon=nlon+360;

  fldpnt_azm(*mlat,*mlon,nlat,nlon,azm);      
}



