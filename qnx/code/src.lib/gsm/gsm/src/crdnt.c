/* crdnt.c
   ======= */

/*
 $Log: crdnt.c,v $
 Revision 1.2  1999/08/08 22:36:20  barnes
 Fixed syntax error.

 Revision 1.1  1999/08/05 20:30:55  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "matcald.h"
#include "crdnt.h"




struct {
    double gst, sdec, srasn, oblecl;
} rgjdsb;

struct {
    double colat, xlong;
} erhkge;


struct {
    double trmtrx[9];
} wrkjvs;


struct {
    int imode;
} zkvejq;


/* Table of constant values */

static int c__9 = 9;
static int c__1 = 1;
static double c_b11 = 360.;

int sun(int iyear, int imnth, int iday, int ihour,
	int imin, double rsec) {

  
     int idt[100] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-1,-1,-1,0,
	    -1,-1,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2 };

     double a, d, f, g;
     int m;
     double q, u, x, y, pirat;
     int id;
     double al;
     int ih, im;
     double dt;
     int iy;
   
    pirat = atan(1.) * 4. / 180.;

    if (iyear < 1900 || iyear > 1999) {
      fprintf(stderr," *** in Subroutine Sun : Year is out of range ***");
      exit(1);
    }

    f = (float)45.;
    al = (float)45.;
    iy = iyear - 1900;
    m = imnth;
    id = iday;
    ih = ihour;
    im = imin;
    dt = (double) (iy - 30 + idt[iy - 1]) / 3600.;
    u = (double) ih + (double) im / 60. + rsec / 3600.;
    kpl(iy, m, id, u, f, al, dt, &x, &y, &a, &d, &g, &q);

    rgjdsb.gst = g * pirat;
    rgjdsb.sdec = d * pirat;
    rgjdsb.srasn = a * pirat;
    rgjdsb.oblecl = q * pirat;

    return 0;
} 

int kpl(int iy, int m, int id, double u, double f, double al,
		double dt, double *x, double *y, double *a, double *d,
		double *g, double *q) {

    /* Local variables */
     double c, e, h;
     int i, j;
     double s, t, v, w;
     int ia;
     double aj, fr;
   

  /* ** solving Kepler's equation to get solar position *** */
  /* ** u,dt (hours); f,al,x,y,a,d,g (degree) *** */

    i = (14 - m) / 12;
    j = iy - i + 6700;
    j = j * 1461 / 4 + (i * 12 + m - 2) * 367 / 12 - (j / 100 + 1) * 3 / 4;
    aj = (double) (j + id) - (float)2447095.5 + (u + dt) / (float)24.;
    t = aj / (float)36525.;
    w = t - (float) 1.;
    c = (float).016751 - t * (float)4.18e-5;
    *x = (t * (float)35999.04975 + (float)358.47583 - t * (float)1.5e-4 * t) *
	     .017453292519943278;
    e = *x;
    for (ia = 1; ia <= 4; ++ia)
		e = *x + c * sin(e);
    v = atan(sqrt((c + (float)1.) / ((float)1. - c)) * tan(e / (float)2.))
	     * (float)2.;
    *g = t * (float)1.71917 + (float)281.22083 + t * (float)4.5e-4 * t;
    s = w * (float)22519. - (float)8.;
    *x = cos((w * (float)32964. + (float)158.) * .017453292519943278) * (
	    float)20. + cos((w * (float)19. + (float)159.) * 
	    .017453292519943278) * (float)18. + cos((w * (float)445267. + (
	    float)208.) * .017453292519943278) * (float)18. + sin(s * (float)
	    2. * .017453292519943278) * (float)15. + cos(s * 
	    .017453292519943278) * (float)13.;
    w = ((float)259.18328 - t * (float)1934.14201 + t * (float).00208 * t) * 
	    .017453292519943278;
    *q = ((float)23.45229 - t * (float).01301 + cos(w) * (float).00256) * 
	    .017453292519943278;
    *x = (*x * (float)1e-4 + *g - sin(w) * (float).00479 - (float).00569 / ((
	    float)1. - c * cos(e))) * .017453292519943278 + v;
    *d = asin(sin(*q) * sin(*x));
    *a = asin(cos(*q) * sin(*x) / cos(*d));
    w = cos(*x) / cos(*d);
    if (w < (float)0.)
		*a = 3.14159265358979 - *a;
    *g = t * (float)36000.76893 + (float)99.69098 + u * (float)15.;
 

    *g = fmod(*g, c_b11);
    fr = f * .017453292519943278;
    h = (*g + al) * .017453292519943278 - *a;
    sky(h, *d, fr, x, y);
    *x /= .017453292519943278;
    *y /= .017453292519943278;
    *a /= .017453292519943278;
    *d /= .017453292519943278;
    *q /= .017453292519943278;
    return 0;
} 



int sky(double h, double d, double f, double *x, double *y) {
   

     double pi = 3.14159265358979;

  
     double z;

   /* ** conversion from (H,D,lat) to (ZA,AA) *** */
   /* ** arguments (radian) *** */

    *x = acos(sin(f) * sin(d) + cos(f) * cos(d) * cos(h));
    *y = -asin(sin(h) * cos(d) / sin(*x));
    z = sin(d) - sin(f) * cos(*x);
    if (z < (float)0.)
		*y = pi - *y;
    if (*y < (float)0.)
		*y += pi * (float)2.;
    return 0;
} 

double vallt(double xlong) {

    /* Local variables */
     double pi, xlt;

    pi = atan(1.) * 4.;

    xlt = (xlong + rgjdsb.gst - rgjdsb.srasn) / pi * 12. + 12.;

	while( xlt < 0. || xlt >= 24.){
    	if (xlt < 0.)
			xlt += 24.;
    	if (xlt >= 24.)
			xlt += -24.;
	}

    return  xlt;
} 


int idleap(int iyear) {
    if (iyear % 100 == 0)
		return (iyear / 100 % 4 == 0);
   	else
		return (iyear % 4 == 0);
} 


int ttdym(int iyear, int imnth, int *itday) {
   

   int doy1[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
   int doy2[12] = { 31,29,31,30,31,30,31,31,30,31,30,31 };

   if (idleap(iyear) == 0)
		*itday = (idleap(iyear) == 0) ?
			doy1[imnth - 1] : (*itday = doy2[imnth - 1]);

    return 0;
} 

int daydoy(int iyear, int imnth, int iday, int *idoy) {
   

     int doy1[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
     int doy2[12] = { 31,29,31,30,31,30,31,31,30,31,30,31 };

     int ik;
     int usedoy[12];

      usedoy[0] = 0;
      if (idleap(iyear) == 0)
		for (ik = 2; ik <= 12; ++ik)
	    	usedoy[ik - 1] = usedoy[ik - 2] + doy1[ik - 2];
      else
		for (ik = 2; ik <= 12; ++ik)
	    	usedoy[ik - 1] = usedoy[ik - 2] + doy2[ik - 2];

      *idoy = usedoy[imnth - 1] + iday;
    return 0;
} 

int doyday(int iyear, int idoy, int *imnth, int *iday) {
    

     int doy1[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
     int doy2[12] = { 31,29,31,30,31,30,31,31,30,31,30,31 };

     int ia, ik;
     int usedoy[12];

    usedoy[0] = 0;
    if (idleap(iyear) == 0)
		for (ik = 2; ik <= 12; ++ik)
	    	usedoy[ik - 1] = usedoy[ik - 2] + doy1[ik - 2];
    else
		for (ik = 2; ik <= 12; ++ik)
	    	usedoy[ik - 1] = usedoy[ik - 2] + doy2[ik - 2];

    ia = 0;
	while(ia <= 12 && idoy > usedoy[ia - 1])
		ia++;

    *imnth = ia - 1;
    *iday = idoy - usedoy[*imnth - 1];

    return 0;
} 



int sodhms(double sod, int *ihour, int *imin, double *rsec) {
    *ihour = (int) (sod / 3600.);
    *imin = (int) ((sod - (double) (*ihour) * 3600.) / 60.);
    *rsec = sod - (double) (*ihour) * 3600. - (double) (*imin) * 60.;

    return 0;
} 


int hmssod(int ihour, int imin, double rsec, double *sod) {
    *sod = (double) (ihour) * 3600. + (double) (imin) * 60. + rsec;
    return 0;
} 

int soymdh(int iyear, double soy, int *imnth, int *iday, int *ihour,
		int *imin, double *rsec) {
     int idoy;
     double sod;

    idoy = (int) (soy / 86400.);
    sod = soy - (double) idoy * 86400.;
    ++idoy;
    doyday(iyear, idoy, imnth, iday);
    sodhms(sod, ihour, imin, rsec);

    return 0;
} 

int mdhsoy(int iyear, int imnth, int iday, int ihour, int imin,
		double rsec, double *soy) {
    int idoy;
   

    daydoy(iyear, imnth, iday, &idoy);
    *soy = (double) (idoy - 1) * 86400. + (double) (ihour) * 3600. + 
	    (double) (imin) * 60. + rsec;
    return 0;
} 



int dipole(int iyear, int imnth, int iday, int ihour, int imin,
		double rsec) {
    /* Initialized data */

     double year[11] = { 1945.,1950.,1955.,1960.,1965.,1970.,1975.,
	    1980.,1985.,1990.,1995. };
     double g0[11] = { -30594.,-30554.,-30500.,-30421.,-30334.,
	    -30220.,-30100.,-29992.,-29873.,-29775.,-29685. };
     double g1[11] = { -2285.,-2250.,-2215.,-2169.,-2119.,-2068.,
	    -2013.,-1956.,-1905.,-1851.,-1798. };
     double h1[11] = { 5810.,5815.,5820.,5791.,5776.,5737.,5675.,
	    5604.,5500.,5411.,5330.5 };

 
     int idoy;
     double diyr, calg0, calg1, calh1, atday, h0;
     int jk;
  
     double calyea;
     int isbscr;
     double totday;

    totday = 365 + idleap(iyear);

    daydoy(iyear, imnth, iday, &idoy);
    atday = (double) (idoy - 1) + (double) (ihour) / 24. + (
	    double) (imin) / 24. / 60. + rsec / 24. / 60. / 60.;

    calyea = (double) (iyear) + atday / totday;

    diyr = (double) (iyear);

    if (diyr < year[0])
		isbscr = 1;
    for (jk = 1; jk <= 10; ++jk)
		if (diyr >= year[jk - 1] && diyr < year[jk])
	    	isbscr = jk;
    if (diyr >= year[10])
		isbscr = 10;

    calg0 = g0[isbscr - 1] + (g0[isbscr] - g0[isbscr - 1]) / (year[isbscr] - 
	    year[isbscr - 1]) * (calyea - year[isbscr - 1]);
    calg1 = g1[isbscr - 1] + (g1[isbscr] - g1[isbscr - 1]) / (year[isbscr] - 
	    year[isbscr - 1]) * (calyea - year[isbscr - 1]);
    calh1 = h1[isbscr - 1] + (h1[isbscr] - h1[isbscr - 1]) / (year[isbscr] - 
	    year[isbscr - 1]) * (calyea - year[isbscr - 1]);

    if (calg0 < 0.) {
		calg0 = -calg0;
		calg1 = -calg1;
		calh1 = -calh1;
    }

    h0 = sqrt(calg0 * calg0 + calg1 * calg1 + calh1 * calh1);
    erhkge.colat = acos(calg0 / h0);
    erhkge.xlong = atan2(calh1, calg1);

    return 0;
} 

int setmtr(double *dmmtrx) {
    int ij, ik;
    dmmtrx -= 4;

    for (ij = 1; ij <= 3; ++ij)
		for (ik = 1; ik <= 3; ++ik)
	    	wrkjvs.trmtrx[ik + ij * 3 - 4] = dmmtrx[ik + ij * 3];

    return 0;
} 


int getmtr(double *dmmtrx) {
     int ij, ik;

    dmmtrx -= 4;

    for (ij = 1; ij <= 3; ++ij)
		for (ik = 1; ik <= 3; ++ik)
	    	dmmtrx[ik + ij * 3] = wrkjvs.trmtrx[ik + ij * 3 - 4];
    return 0;
} 

int invtrs() {
    double dmmtr[9];
    double trsmtr[9];

    getmtr(trsmtr);
    inver3(trsmtr, dmmtr);
    setmtr(dmmtr);

    return 0;
} 

int cnvrt(double *frmvct, double *tovct) {
  
    --tovct;
    --frmvct;

    mtrvct(wrkjvs.trmtrx, &frmvct[1], &tovct[1]);

    return 0;
} 




int geigeo() {
    
 
    double trmtrx[9];

    trmtrx[0] = cos(rgjdsb.gst);
    trmtrx[3] = sin(rgjdsb.gst);
    trmtrx[6] = 0.;
    trmtrx[1] = -sin(rgjdsb.gst);
    trmtrx[4] = cos(rgjdsb.gst);
    trmtrx[7] = 0.;
    trmtrx[2] = 0.;
    trmtrx[5] = 0.;
    trmtrx[8] = 1.;

    setmtr(trmtrx);

    return 0;
} 


int geogei() {
    
    double trmtrx[9];

    trmtrx[0] = cos(rgjdsb.gst);
    trmtrx[3] = -sin(rgjdsb.gst);
    trmtrx[6] = 0.;
    trmtrx[1] = sin(rgjdsb.gst);
    trmtrx[4] = cos(rgjdsb.gst);
    trmtrx[7] = 0.;
    trmtrx[2] = 0.;
    trmtrx[5] = 0.;
    trmtrx[8] = 1.;

    setmtr(trmtrx);

    return 0;
} 


int geigse() {
    double gsex[3], gsey[3], gsez[3], dumv[3], dmmtr[9];
    
    gsez[0] = 0.;
    gsez[1] = -sin(rgjdsb.oblecl);
    gsez[2] = cos(rgjdsb.oblecl);
    gsex[0] = cos(rgjdsb.sdec) * cos(rgjdsb.srasn);
    gsex[1] = cos(rgjdsb.sdec) * sin(rgjdsb.srasn);
    gsex[2] = sin(rgjdsb.sdec);

    prdout(gsez, gsex, dumv);
    unifyv(dumv, gsey);
    prdout(gsex, gsey, dumv);
    unifyv(dumv, gsez);
    asmunt(gsex, gsey, gsez, dmmtr);
    setmtr(dmmtr);

    return 0;
} 


int gsegei() {
  
    geigse();
  
    return 0;
} 


int geism() {
    
    double dumv[3], trmt1[9];
    double sungei[3], smzgei[3], smzgeo[3];
    double smx[3], smy[3];

    sungei[0] = cos(rgjdsb.sdec) * cos(rgjdsb.srasn);
    sungei[1] = cos(rgjdsb.sdec) * sin(rgjdsb.srasn);
    sungei[2] = sin(rgjdsb.sdec);
    smzgeo[0] = sin(erhkge.colat) * cos(erhkge.xlong);
    smzgeo[1] = sin(erhkge.colat) * sin(erhkge.xlong);
    smzgeo[2] = cos(erhkge.colat);
    geogei();
    cnvrt(smzgeo, smzgei);
    prdout(smzgei, sungei, dumv);
    unifyv(dumv, smy);
    prdout(smy, smzgei, dumv);
    unifyv(dumv, smx);
    asmunt(smx, smy, smzgei, trmt1);
    setmtr(trmt1);
    return 0;
}


int smgei() {
    
    geism();
    invtrs();

    return 0;
} 

int geomag() {
    
    double magx[3], magy[3], magz[3], geoz[3], dumv[3], dmmtr[9];
  
    magz[0] = sin(erhkge.colat) * cos(erhkge.xlong);
    magz[1] = sin(erhkge.colat) * sin(erhkge.xlong);
    magz[2] = cos(erhkge.colat);

    geoz[0] = 0.;
    geoz[1] = 0.;
    geoz[2] = 1.;

    prdout(geoz, magz, dumv);
    unifyv(dumv, magy);
    prdout(magy, magz, dumv);
    unifyv(dumv, magx);
    asmunt(magx, magy, magz, dmmtr);
    setmtr(dmmtr);

    return 0;
} 


int maggeo() {
  

    geomag();
    invtrs();

    return 0;
} 

int geosm() {
    
    double dumv[3], trmt1[9];
  
    double sungei[3], sungeo[3];
    double smx[3], smy[3], smz[3];

    sungei[0] = cos(rgjdsb.sdec) * cos(rgjdsb.srasn);
    sungei[1] = cos(rgjdsb.sdec) * sin(rgjdsb.srasn);
    sungei[2] = sin(rgjdsb.sdec);
    geigeo();
    cnvrt(sungei, sungeo);

    smz[0] = sin(erhkge.colat) * cos(erhkge.xlong);
    smz[1] = sin(erhkge.colat) * sin(erhkge.xlong);
    smz[2] = cos(erhkge.colat);

    prdout(smz, sungeo, dumv);
    unifyv(dumv, smy);
    prdout(smy, smz, dumv);
    unifyv(dumv, smx);
    asmunt(smx, smy, smz, trmt1);
    setmtr(trmt1);

    return 0;
} 



int smgeo() {
   

    geosm();
    invtrs();

    return 0;
}

int geogsm() {
   
    double dumv[3], gsmx[3], gsmy[3], gsmz[3], trmt1[9];
    double dplgeo[3], sungei[3], sungeo[3];
  

    sungei[0] = cos(rgjdsb.sdec) * cos(rgjdsb.srasn);
    sungei[1] = cos(rgjdsb.sdec) * sin(rgjdsb.srasn);
    sungei[2] = sin(rgjdsb.sdec);
    geigeo();
    cnvrt(sungei, sungeo);
    unifyv(sungeo, gsmx);
    dplgeo[0] = sin(erhkge.colat) * cos(erhkge.xlong);
    dplgeo[1] = sin(erhkge.colat) * sin(erhkge.xlong);
    dplgeo[2] = cos(erhkge.colat);

    prdout(dplgeo, gsmx, dumv);
    unifyv(dumv, gsmy);
    prdout(gsmx, gsmy, dumv);
    unifyv(dumv, gsmz);
    asmunt(gsmx, gsmy, gsmz, trmt1);
    setmtr(trmt1);

    return 0;
} 


int gsmgeo()
{
  

    geogsm();
    invtrs();

    return 0;
} 

int gsexyp(double *posgse, double *xypp) {
     double dumv[3], xypx[3], xypy[3], dmmtr[9], sungse[3];
     double rse;

    --xypp;
    --posgse;

    
    dumv[0] = xypp[1];
    dumv[1] = xypp[2];
    dumv[2] = xypp[3];
    unifyv(dumv, &xypp[1]);

    rse = 23460.;
    dumv[0] = 1. - posgse[1] / rse;
    dumv[1] = -posgse[2] / rse;
    dumv[2] = -posgse[3] / rse;
    unifyv(dumv, sungse);
    prdout(&xypp[1], sungse, dumv);
    unifyv(dumv, xypy);
    prdout(xypy, &xypp[1], dumv);
    unifyv(dumv, xypx);
    asmunt(xypx, xypy, &xypp[1], dmmtr);
    setmtr(dmmtr);

    return 0;
} 


int xypgse_(double *posgse, double *xypp) {

    --xypp;
    --posgse;

  
    gsexyp(&posgse[1], &xypp[1]);
    invtrs();

    return 0;
} 


int geogse() {
    double trmtr1[9], trmtr2[9],trmtr3[9];

    geogei();
    getmtr(trmtr1);
    geigse();
    getmtr(trmtr2);
    mtrprd(trmtr2, trmtr1, trmtr3);
    setmtr(trmtr3);

    return 0;
} 


int gsegeo() {
    double trmtr1[9], trmtr2[9],trmtr3[9];
   
    gsegei();
    getmtr(trmtr1);
    geigeo();
    getmtr(trmtr2);
    mtrprd(trmtr2, trmtr1, trmtr3);
    setmtr(trmtr3);

    return 0;
} 



int gsevdh(double *posgse) {
    
    double vdhx[3], vdhy[3], dumv[3], dmmtr[9];
    double dplgeo[3], dplgse[3];
   
    --posgse;

    /* Function Body */
    dplgeo[0] = sin(erhkge.colat) * cos(erhkge.xlong);
    dplgeo[1] = sin(erhkge.colat) * sin(erhkge.xlong);
    dplgeo[2] = cos(erhkge.colat);
    geogse();
    cnvrt(dplgeo, dplgse);
    prdout(dplgse, &posgse[1], dumv);
    unifyv(dumv, vdhy);
    prdout(vdhy, dplgse, dumv);
    unifyv(dumv, vdhx);
    asmunt(vdhx, vdhy, dplgse, dmmtr);
    setmtr(dmmtr);

    return 0;
} 

int vdhgse(double *posgse) {
  
    --posgse;

    
    gsevdh(&posgse[1]);
    invtrs();

    return 0;
} 

int geovdh(double *posgeo) {
   
     double vdhd[3], vdhh[3], vdhv[3], dumv[3], dmmtr[9];
 
    --posgeo;

    vdhh[0] = sin(erhkge.colat) * cos(erhkge.xlong);
    vdhh[1] = sin(erhkge.colat) * sin(erhkge.xlong);
    vdhh[2] = cos(erhkge.colat);

    prdout(vdhh, &posgeo[1], dumv);
    unifyv(dumv, vdhd);

    prdout(vdhd, vdhh, dumv);
    unifyv(dumv, vdhv);

    asmunt(vdhv, vdhd, vdhh, dmmtr);
    setmtr(dmmtr);

    return 0;
} 

int vdhgeo(double *posgeo) {
   
    --posgeo;

    geovdh(&posgeo[1]);
    invtrs();

    return 0;
} 

int geopen(double *posgeo) {
     double pene[3], penn[3], penp[3], dumv[3], dmmtr[9];
  
    --posgeo;

   
    penp[0] = 0.;
    penp[1] = 0.;
    penp[2] = 1.;
    prdout(&posgeo[1], penp, dumv);
    unifyv(dumv, penn);
    prdout(penn, penp, dumv);
    unifyv(dumv, pene);

    asmunt(penp, pene, penn, dmmtr);
    setmtr(dmmtr);

    return 0;
} 

int pengeo(double *posgeo) {
   

 
    --posgeo;

   
    geopen(&posgeo[1]);
    invtrs();

    return 0;
} 


int penvdh(double *posgeo) {
     double trmtr1[9],trmtr2[9],trmtr3[9];
  

  
    --posgeo;

    
    pengeo(&posgeo[1]);
    getmtr(trmtr1);
    geovdh(&posgeo[1]);
    getmtr(trmtr2);
    mtrprd(trmtr2, trmtr1, trmtr3);
    setmtr(trmtr3);

    return 0;
} 

int gsesm() {
  
    double trmtr1[9],trmtr2[9],trmtr3[9];
 
    gsegei();
    getmtr(trmtr1);
    geism();
    getmtr(trmtr2);
    mtrprd(trmtr2, trmtr1, trmtr3);
    setmtr(trmtr3);

    return 0;
} 


int smgse() {
 
    double trmtr1[9],trmtr2[9],trmtr3[9];
 
    smgei();
    getmtr(trmtr1);
    geigse();
    getmtr(trmtr2);
    mtrprd(trmtr2, trmtr1, trmtr3);
    setmtr(trmtr3);

    return 0;
} 



int gsmsm() {
   
    double trmtr1[9],trmtr2[9],trmtr3[9];
  
    gsmgeo();
    getmtr(trmtr1);
    geosm();
    getmtr(trmtr2);
    mtrprd(trmtr2, trmtr1, trmtr3);
    setmtr(trmtr3);

    return 0;
} 


int smgsm() {
    
    double trmtr1[9],trmtr2[9],trmtr3[9];
 

    smgeo();
    getmtr(trmtr1);
    geogsm();
    getmtr(trmtr2);
    mtrprd(trmtr2, trmtr1, trmtr3);
    setmtr(trmtr3);

    return 0;
} 


int gsegsm() {
    double trmtr1[9],trmtr2[9],trmtr3[9];
   

    gsegeo();
    getmtr(trmtr1);
    geogsm();
    getmtr(trmtr2);
    mtrprd(trmtr2, trmtr1, trmtr3);
    setmtr(trmtr3);

    return 0;
} 


int gsmgse() {
 

    gsegsm();
    invtrs();

    return 0;
} 



int gsmgsw() {
   
    double dmyv[3], gswx[3], gswy[3], gswz[3], geonp[3], dmmtr[9],
             gsmnp[3], gsesw[3];
    
    double pi, abrang;
  
    pi = atan(1.) * 4.;
    abrang = pi * .022222222222222223;

    gsesw[0] = cos(abrang);
    gsesw[1] = -sin(abrang);
    gsesw[2] = 0.;

    gsegsm();
    cnvrt(gsesw, gswx);

    geonp[0] = sin(erhkge.colat) * cos(erhkge.xlong);
    geonp[1] = sin(erhkge.colat) * sin(erhkge.xlong);
    geonp[2] = cos(erhkge.colat);

    geogsm();
    cnvrt(geonp, gsmnp);

    prdout(gsmnp, gswx, dmyv);
    unifyv(dmyv, gswy);
    prdout(gswx, gswy, dmyv);
    unifyv(dmyv, gswz);

    asmunt(gswx, gswy, gswz, dmmtr);
    setmtr(dmmtr);

    return 0;
} 



int gsmhng(double *posgsm, double *poshng) {
   
     double geonp[3], gsmnp[3];
  
     double gswnp[3];
     double rh;
     double posgsw[3], psi;

   
    --poshng;
    --posgsm;

    /* Function Body */
    rh = 7.064;
    gsmgsw();
    cnvrt(&posgsm[1], posgsw);

    geonp[0] = sin(erhkge.colat) * cos(erhkge.xlong);
    geonp[1] = sin(erhkge.colat) * sin(erhkge.xlong);
    geonp[2] = cos(erhkge.colat);

    geogsm();
    cnvrt(geonp, gsmnp);

    gsmgsw();
    cnvrt(gsmnp, gswnp);

    angl2d(gswnp[2], gswnp[0], &psi);

    if (posgsw[0] <= -rh * cos(psi))
		poshng[3] = posgsw[2] - rh * sin(psi);
    else
		poshng[3] = posgsw[2] + posgsw[0] * tan(psi);

    poshng[1] = posgsw[0];
    poshng[2] = posgsw[1];

    return 0;
} 



int gsegsw() {
    double trmtr1[9],trmtr2[9],trmtr3[9];
 
    gsegsm();
    getmtr(trmtr1);
    gsmgsw();
    getmtr(trmtr2);
    mtrprd(trmtr2, trmtr1, trmtr3);
    setmtr(trmtr3);

    return 0;
} 



int gswgse() {
 
    gsegsw();
    invtrs();
    return 0;
} 


int gsease() {
  
     double pi, abrang;
     double trmtrx[9];

    pi = atan(1.) * 4.;
    abrang = pi * .022222222222222223;

    trmtrx[0] = cos(abrang);
    trmtrx[3] = -sin(abrang);
    trmtrx[6] = 0.;
    trmtrx[1] = sin(abrang);
    trmtrx[4] = cos(abrang);
    trmtrx[7] = 0.;
    trmtrx[2] = 0.;
    trmtrx[5] = 0.;
    trmtrx[8] = 1.;

    setmtr(trmtrx);

    return 0;
} 


int asegse() {
   
     double pi, abrang;
     double trmtrx[9];

    pi = atan(1.) * 4.;
    abrang = pi * .022222222222222223;

    trmtrx[0] = cos(abrang);
    trmtrx[3] = sin(abrang);
    trmtrx[6] = 0.;
    trmtrx[1] = -sin(abrang);
    trmtrx[4] = cos(abrang);
    trmtrx[7] = 0.;
    trmtrx[2] = 0.;
    trmtrx[5] = 0.;
    trmtrx[8] = 1.;

    setmtr(trmtrx);

    return 0;
} 



int lmnmod(int jmode) {
    zkvejq.imode = jmode;

    return 0;
} 

int gselmn(double *posgse) {
    
     double d__1, d__2;

  
     double pprm, dmyv[3], x;
     double gmzgm[3], a0, b0, c0, dmmtr[9]	/* was [3][3] */;
 
     double gwzgw[3], smzsm[3];
    
     double elvase[3], emvase[3], envase[3], gmzase[3], elvgse[3], 
	    emvgse[3], envgse[3], posase[3];
     double gmzgse[3];
     double gwzase[3], smzase[3], gwzgse[3];
     double smzgse[3];
     double ryz;





    --posgse;

    /* Function Body */
    a0 = exp((float)-1.915);
    b0 = exp((float)2.922);
    c0 = -exp((float)5.413);

    gsease();
    cnvrt(&posgse[1], posase);

    x = posase[0];

/* Computing 2nd power */

    d__1 = posase[1];

/* Computing 2nd power */

    d__2 = posase[2];
    ryz = sqrt(d__1 * d__1 + d__2 * d__2);


    pprm = (-b0 * x - sqrt((b0 * b0 - a0 * 4. * c0) * x * x - c0 * 4. * ryz * 
	    ryz)) / 2. / c0;


    dmyv[0] = a0 * x + b0 * pprm / (float)2.;
    dmyv[1] = posase[1];
    dmyv[2] = posase[2];
    unifyv(dmyv, envase);

    if (zkvejq.imode == 0) {


	gmzgm[0] = 0.;
	gmzgm[1] = 0.;
	gmzgm[2] = 1.;

	gsmgse();
	cnvrt(gmzgm, gmzgse);
	gsease();
	cnvrt(gmzgse, gmzase);

	prdout(envase, gmzase, dmyv);
	unifyv(dmyv, emvase);
    } else if (zkvejq.imode == 1) {

	smzsm[0] = 0.;
	smzsm[1] = 0.;
	smzsm[2] = 1.;

	smgse();
	cnvrt(smzsm, smzgse);
	gsease();
	cnvrt(smzgse, smzase);

	prdout(envase, smzase, dmyv);
	unifyv(dmyv, emvase);

	gwzgw[0] = 0.;
	gwzgw[1] = 0.;
	gwzgw[2] = 1.;

	gswgse();
	cnvrt(gwzgw, gwzgse);
	gsease();
	cnvrt(gwzgse, gwzase);

	prdout(envase, gwzase, dmyv);
	unifyv(dmyv, emvase);
    }

    prdout(emvase, envase, dmyv);
    unifyv(dmyv, elvase);

    asegse();
    cnvrt(elvase, elvgse);
    cnvrt(emvase, emvgse);
    cnvrt(envase, envgse);

    asmunt(elvgse, emvgse, envgse, dmmtr);
    setmtr(dmmtr);

    return 0;
} 



int lmngse(double *posgse) {
   
    --posgse;

    gselmn(&posgse[1]);
    invtrs();

    return 0;
} 




int r_lat_lt(double *posv, double *posa) {
    /* System generated locals */
    double d__1, d__2, d__3;

    float picon;

    --posa;
    --posv;

  
    picon = (float)57.295780490442965;

/* Computing 2nd power */
    d__1 = posv[1];

/* Computing 2nd power */
    d__2 = posv[2];

/* Computing 2nd power */

    d__3 = posv[3];
    posa[1] = sqrt(d__1 * d__1 + d__2 * d__2 + d__3 * d__3);

    posa[2] = (float)90. - picon * acos(posv[3] / posa[1]);

    posa[3] = (picon * atan2(posv[2], posv[1]) + (float)180.) / (float)15.;

    if (posa[3] > (float)24.)
		posa[3] += (float)-24.;
    if (posa[3] < (float)0.)
		posa[3] += (float)24.;

    return 0;
} 

