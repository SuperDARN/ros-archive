/* cnvt_coord.c
   ============
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
 $Log: cnvt_coord.c,v $
 Revision 1.10  2001/06/27 20:49:59  barnes
 Added license tag

 Revision 1.9  2001/01/29 18:11:53  barnes
 Added Author Name

 Revision 1.8  1999/12/15 19:41:34  barnes
 Modifications to pass in receiver rise time variations.

 Revision 1.7  1999/08/10 20:08:25  barnes
 Changed the height paramter to a double.

 Revision 1.6  1999/03/18 15:21:32  barnes
 Adjusted ground scatter routines to have same form as other routines.

 Revision 1.5  1999/03/17 18:10:28  barnes
 Added ground scatter calculations.

 Revision 1.4  1999/02/24 16:29:04  barnes
 Fixed serious bug that affects the final conversion.

 Revision 1.3  1999/02/12 16:57:02  barnes
 Fixed bug that caused the beams to be slight offset.

 Revision 1.2  1998/06/11 15:14:16  barnes
 Changed the name of the radar hardware structure to radar_site.

 Revision 1.1  1998/06/03 18:57:12  root
 Initial revision

 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "math_constant.h"
#include "hardware.h"
#include "geo.h"
#include "aacgm_lib.h"

double slant_range(int lagfr,int smsep,
		   double rxris,double range_edge,
		   int range_gate) {

   return (lagfr-rxris+(range_gate-1)*smsep+range_edge)*0.15;
}


void geodtgc(int iopt,double *gdlat,double *gdlon,
             double *grho,double *glat,
			 double *glon,double *del) {

   double a=6378.16;
   double f=1.0/298.25;
   double b,e2;

   b=a*(1.0-f);
   e2=(a*a)/(b*b)-1;
   if (iopt>0) {
     *glat=atand( (b*b)/(a*a)*tand(*gdlat));
     *glon=*gdlon;
     if (*glon > 180) *glon=*glon-360;
   } else {
     *gdlat=atand( (a*a)/(b*b)*tand(*glat));
     *gdlon=*glon;
   }
   *grho=a/sqrt(1.0+e2*sind(*glat)*sind(*glat));
   *del=*gdlat-*glat;
}

void fldpnt(double rrho,double rlat,double rlon,double ral,
			double rel,double r,double *frho,double *flat,
                        double *flon) {

   double rx,ry,rz,sx,sy,sz,tx,ty,tz;
   double sinteta;
  
   /* convert from global spherical to global cartesian*/

   sinteta=sind(90.0-rlat);
   rx=rrho*sinteta*cosd(rlon);
   ry=rrho*sinteta*sind(rlon);
   rz=rrho*cosd(90.0-rlat);

   sx=-r*cosd(rel)*cosd(ral);
   sy=r*cosd(rel)*sind(ral);
   sz=r*sind(rel);

   tx  =  cosd(90.0-rlat)*sx + sind(90.0-rlat)*sz;
   ty  =  sy;
   tz  = -sind(90.0-rlat)*sx + cosd(90.0-rlat)*sz;
   sx  =  cosd(rlon)*tx - sind(rlon)*ty;
   sy  =  sind(rlon)*tx + cosd(rlon)*ty;
   sz  =  tz;

   tx=rx+sx;
   ty=ry+sy;
   tz=rz+sz;

   /* convert from cartesian back to global spherical*/
   *frho=sqrt((tx*tx)+(ty*ty)+(tz*tz));
   *flat=90.0-acosd(tz/(*frho));
   if ((tx==0) && (ty==0)) *flon=0;
   else *flon=atan2d(ty,tx);
}

void geocnvrt(double gdlat,double gdlon,
			  double xal,double xel,double *ral,double *rel) {

  double kxg,kyg,kzg,kxr,kyr,kzr;
  double rrad,rlat,rlon,del;

  kxg=cosd(xel)*sind(xal);
  kyg=cosd(xel)*cosd(xal);
  kzg=sind(xel);
  geodtgc(1,&gdlat,&gdlon,&rrad,&rlat,&rlon,&del);
  kxr=kxg;
  kyr=kyg*cosd(del)+kzg*sind(del);
  kzr=-kyg*sind(del)+kzg*cosd(del);

  *ral=atan2d(kxr,kyr);
  *rel=atand(kzr/sqrt((kxr*kxr)+(kyr*kyr)));
}

void fldpnth(double gdlat,double gdlon,double psi,double bore,
			 double fh,double r,double *frho,double *flat,
	                 double *flon) {

  double rrad,rlat,rlon,del;
  double tan_azi,azi,rel,xel,fhx,xal,rrho,ral,xh;
  double dum,dum1,dum2,dum3;
  double frad;  
 
  if (fh<=150) xh=fh;
  else {
    if (r<=600) xh=115;
    else if ((r>600) && (r<800)) xh=(r-600)/200*(fh-115)+115;
    else xh=fh;
  }

  if (r<150) xh=(r/150.0)*115.0;
  geodtgc(1,&gdlat,&gdlon,&rrad,&rlat,&rlon,&del);
  rrho=rrad;
  frad=rrad;
 

  do {
    *frho=frad+xh;
  
    rel=asind( ((*frho**frho) - (rrad*rrad) - (r*r)) / (2*rrad*r));
    xel=rel;
    if (((cosd(psi)*cosd(psi))-(sind(xel)*sind(xel)))<0) tan_azi=1e32;
      else tan_azi=sqrt( (sind(psi)*sind(psi))/
                ((cosd(psi)*cosd(psi))-(sind(xel)*sind(xel))));
    if (psi>0) azi=atand(tan_azi)*1.0;
      else azi=atand(tan_azi)*-1.0;
    xal=azi+bore;
    geocnvrt(gdlat,gdlon,xal,xel,&ral,&dum);

    fldpnt(rrho,rlat,rlon,ral,rel,r,frho,flat,flon);
    geodtgc(-1,&dum1,&dum2,&frad,flat,flon,&dum3);
    fhx=*frho-frad; 
  } while(fabs(fhx-xh) > 0.5);
} 


void fldpnth_gs(double gdlat,double gdlon,double psi,double bore,
			 double fh,double r,double *frho,double *flat,
	                 double *flon) {

  double rrad,rlat,rlon,del;
  double tan_azi,azi,rel,xel,fhx,xal,rrho,ral,xh;
  double dum,dum1,dum2,dum3;
  double frad;  
 
  if (fh<=150) xh=fh;
  else {
    if (r<=300) xh=115;
    else if ((r>300) && (r<500)) xh=(r-300)/200*(fh-115)+115;
    else xh=fh;
  }

  if (r<150) xh=(r/150.0)*115.0;
  geodtgc(1,&gdlat,&gdlon,&rrad,&rlat,&rlon,&del);
  rrho=rrad;
  frad=rrad;
 

  do {
    *frho=frad+xh;
  
    rel=asind( ((*frho**frho) - (rrad*rrad) - (r*r)) / (2*rrad*r));
    xel=rel;
    if (((cosd(psi)*cosd(psi))-(sind(xel)*sind(xel)))<0) tan_azi=1e32;
      else tan_azi=sqrt( (sind(psi)*sind(psi))/
                ((cosd(psi)*cosd(psi))-(sind(xel)*sind(xel))));
    if (psi>0) azi=atand(tan_azi)*1.0;
      else azi=atand(tan_azi)*-1.0;
    xal=azi+bore;
    geocnvrt(gdlat,gdlon,xal,xel,&ral,&dum);

    fldpnt(rrho,rlat,rlon,ral,rel,r,frho,flat,flon);
    geodtgc(-1,&dum1,&dum2,&frad,flat,flon,&dum3);
    fhx=*frho-frad; 
  } while(fabs(fhx-xh) > 0.5);
} 


void geographic(int center,int bcrd,int rcrd,
                struct radar_site *pos,
                int lagfr,int smsep,
                int rxrise,double height,
                double *rho,double *lat,double *lng) {

  double rx;
  double psi,d;
  double re=6356.779;

  double bm_edge=0;
  double range_edge=0;
  
  if (center==0) {
    bm_edge=-pos->beam_sep*0.5;
    range_edge=-0.5*smsep;
  }
  
  if (rxrise==0) rx=pos->rec_rise;
  else rx=rxrise;

  psi=pos->beam_sep*(bcrd-7.5)+bm_edge;
  d=slant_range(lagfr,smsep,rx,range_edge,rcrd+1);
  if (height < 90) height=-re+sqrt((re*re)+2*d*re*sind(height)+(d*d));
 
  fldpnth(pos->geo_lat,pos->geo_long,psi,pos->boresite,
		  height,d,rho,lat,lng); 
}


void magnetic(int center,int bcrd,int rcrd,
                struct radar_site *pos,
                int lagfr,int smsep,int rxrise,double height,
                double *rho,double *lat,double *lng) {

  double rx;
  double radius;
  double psi,d;
  double re=6356.779;

  double bm_edge=0;
  double range_edge=0;
  
  if (center==0) {
    bm_edge=-pos->beam_sep*0.5;
    range_edge=-0.5*smsep;
  }
  
  if (rxrise==0) rx=pos->rec_rise;
  else rx=rxrise;


  psi=pos->beam_sep*(bcrd-7.5)+bm_edge;
  d=slant_range(lagfr,smsep,rx,range_edge,rcrd+1);
  if (height < 90) height=-re+sqrt((re*re)+2*d*re*sind(height)+(d*d));
 
  fldpnth(pos->geo_lat,pos->geo_long,psi,pos->boresite,
		  height,d,rho,lat,lng); 
 
  cnv_aacgm(*lat,*lng,(double) height,lat,lng,&radius,0);
 
 

}


void cubic(int center,int bcrd,int rcrd,
		   struct radar_site *pos,
		   int lagfr,int smsep,int rxrise,double height,
           double *x,double *y,double *z) {
   
    /* returns cartesian cubic co-ordinates */
  double rx;
  double psi,d;
  double rho,lat,lng;
  double re=6356.779;

  double bm_edge=0;
  double range_edge=0;

  if (center==0) {
    bm_edge=-pos->beam_sep*0.5;
    range_edge=-0.5*smsep;
  }

  if (rxrise==0) rx=pos->rec_rise;
  else rx=rxrise;


  psi=pos->beam_sep*(bcrd-7.5)+ bm_edge;
  d=slant_range(lagfr,smsep,rx,range_edge,rcrd+1);
  if (height < 90) height=-re+sqrt((re*re)+2*d*re*sind(height)+(d*d));
  fldpnth(pos->geo_lat,pos->geo_long,psi,pos->boresite,
	      height,d,&rho,&lat,&lng);  

  /* convert to x,y,z (normalized to the unit sphere) */

  lng=90-lng;
  *x=rho*cos(lng*PI/180.0)*cos(lat*PI/180.0)/re;
  *y=rho*sin(lat*PI/180.0)/re;
  *z=rho*sin(lng*PI/180.0)*cos(lat*PI/180.0)/re;

} 



void geographic_gs(int center,int bcrd,int rcrd,
                struct radar_site *pos,
                int lagfr,int smsep,int rxrise,double height,
                double *rho,double *lat,double *lng) {
 
  double rx;
  double psi,d;
  double re=6356.779;

  double bm_edge=0;
  double range_edge=0;
  
  if (center==0) {
    bm_edge=-pos->beam_sep*0.5;
    range_edge=-0.5*smsep;
  }
  
  if (rxrise==0) rx=pos->rec_rise;
  else rx=rxrise;


  psi=pos->beam_sep*(bcrd-7.5)+bm_edge;
  d=slant_range(lagfr,smsep,rx,range_edge,rcrd+1)/2;
  if (height < 90) height=-re+sqrt((re*re)+2*d*re*sind(height)+(d*d));
 
  fldpnth_gs(pos->geo_lat,pos->geo_long,psi,pos->boresite,
		  height,d,rho,lat,lng); 
}



void magnetic_gs(int center,int bcrd,int rcrd,
                struct radar_site *pos,
                int lagfr,int smsep,int rxrise,double height,
                double *rho,double *lat,double *lng) {
  double rx;
  double radius;
  double psi,d;
  double re=6356.779;

  double bm_edge=0;
  double range_edge=0;
  
  if (center==0) {
    bm_edge=-pos->beam_sep*0.5;
    range_edge=-0.5*smsep;
  }
  
  if (rxrise==0) rx=pos->rec_rise;
  else rx=rxrise;

  psi=pos->beam_sep*(bcrd-7.5)+bm_edge;
  d=slant_range(lagfr,smsep,rx,range_edge,rcrd+1)/2;
  if (height < 90) height=-re+sqrt((re*re)+2*d*re*sind(height)+(d*d));
 
  fldpnth_gs(pos->geo_lat,pos->geo_long,psi,pos->boresite,
		  height,d,rho,lat,lng); 
 
  cnv_aacgm(*lat,*lng,(double) height,lat,lng,&radius,0);
 
 

}


void cubic_gs(int center,int bcrd,int rcrd,
		   struct radar_site *pos,
		   int lagfr,int smsep,int rxrise,double height,
           double *x,double *y,double *z) {
   
    /* returns cartesian cubic co-ordinates */
  double rx;
  double psi,d; 
  double rho,lat,lng;
  double re=6356.779;

  double bm_edge=0;
  double range_edge=0;

  if (center==0) {
    bm_edge=-pos->beam_sep*0.5;
    range_edge=-0.5*smsep;
  }

  if (rxrise==0) rx=pos->rec_rise;
  else rx=rxrise;

  psi=pos->beam_sep*(bcrd-7.5)+ bm_edge;
  d=slant_range(lagfr,smsep,rx,range_edge,rcrd+1)/2;
  if (height < 90) height=-re+sqrt((re*re)+2*d*re*sind(height)+(d*d));
  fldpnth_gs(pos->geo_lat,pos->geo_long,psi,pos->boresite,
	      height,d,&rho,&lat,&lng);  

  /* convert to x,y,z (normalized to the unit sphere) */

  lng=90-lng;
  *x=rho*cos(lng*PI/180.0)*cos(lat*PI/180.0)/re;
  *y=rho*sin(lat*PI/180.0)/re;
  *z=rho*sin(lng*PI/180.0)*cos(lat*PI/180.0)/re;

} 

 



