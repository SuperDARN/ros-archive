/* geo.h
   =====
   Author: R.J.Barnes
*/


#ifndef _GEO_H
#define _GEO_H

#ifndef _HARDWARE
#include "hardware.h"
#endif

struct rpos {
  double gdlat;
  double gdlon;
  double boresite;
  double bmwidth;
  double rxris;
};

void geographic(int center,int bcrd,int rcrd,
                struct radar_site *pos,
                int lagfr,int smsep,int rxrise,double height,
                double *rho,double *lat,double *lng);
void magnetic(int center,int bcrd,int rcrd,
                struct radar_site *pos,
                int lagfr,int smsep,int rxrise,double height,
                double *rho,double *lat,double *lng);

void cubic(int center,int bcrd,int rcrd,
                struct radar_site *pos,
                int lagfr,int smsep,int rxrise,double height,
                double *x,double *y,double *z);

void geographic_gs(int center,int bcrd,int rcrd,
                struct radar_site *pos,
                int lagfr,int smsep,int rxrise,double height,
                double *rho,double *lat,double *lng);
void magnetic_gs(int center,int bcrd,int rcrd,
                struct radar_site *pos,
                int lagfr,int smsep,int rxrise,double height,
                double *rho,double *lat,double *lng);

void cubic_gs(int center,int bcrd,int rcrd,
              struct radar_site *pos,
              int lagfr,int smsep,int rxrise,double height,
              double *x,double *y,double *z);



#endif
