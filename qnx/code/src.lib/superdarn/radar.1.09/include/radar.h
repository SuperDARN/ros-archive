/* radar.h
   =======
   Author: R.J.Barnes
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

/*
 $Log: radar.h,v $
 Revision 1.3  2006/03/13 19:10:27  barnes
 Added the ability to set the maximum number of beams in the hardware file

 Revision 1.2  2005/09/09 15:08:40  barnes
 Added the maxrange term to the hardware information to deal with Wallops.

 Revision 1.1  2004/07/12 22:24:49  barnes
 Initial revision

*/


#ifndef _RADAR_H
#define _RADAR_H

struct RadarSite { 
  double tval;
  double geolat,geolon,alt;
  double boresite,bmsep;
  double vdir;
  double atten;
  double tdiff;
  double phidiff;
  double interfer[3];
  double recrise;
  int maxatten;
  int maxrange;
  int maxbeam;
};

struct Radar {
  int id;
  int status;
  int cnum;
  char **code;
  char *name;
  char *operator;
  char *hdwfname;
  int snum;
  struct RadarSite *site;
};

struct RadarNetwork {
  int rnum;
  struct Radar *radar;
};

struct RadarSite *RadarEpochGetSite(struct Radar *ptr,double tval);
struct RadarSite *RadarYMDHMSGetSite(struct Radar *ptr,int yr,
				     int mo,int dy,int hr,int mt,
                                     int sc);


struct Radar *RadarGetRadar(struct RadarNetwork *ptr,int stid);
int RadarGetID(struct RadarNetwork *ptr,char *code);
int RadarGetCodeNum(struct RadarNetwork *ptr,int stid);
char *RadarGetCode(struct RadarNetwork *ptr,int stid,int cnum);
char *RadarGetName(struct RadarNetwork *ptr,int stid);
char *RadarGetOperator(struct RadarNetwork *ptr,int stid);
int RadarGetStatus(struct RadarNetwork *ptr,int stid);


void RadarFree(struct RadarNetwork *ptr);
struct RadarNetwork *RadarLoad(FILE *fp); 
int RadarLoadHardware(char *hdwpath,struct RadarNetwork *ptr); 
struct RadarSite *RadarGetSite(struct RadarNetwork *ptr,
                               int yr,int mo,int dy,int hr,int mt,int sc,
                               int stid);

#endif
