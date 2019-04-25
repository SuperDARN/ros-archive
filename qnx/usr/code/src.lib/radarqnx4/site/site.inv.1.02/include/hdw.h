/* hdw.h
   =====
   Author: R.J.Barnes
*/

/*
 $Log: hdw.h,v $
 Revision 1.1  2008/03/21 18:33:19  code
 Initial revision

*/

/*
 $Log: hdw.h,v $
 Revision 1.1  2008/03/21 18:33:19  code
 Initial revision

*/

#define FREQ_LOCAL 2

extern pid_t dioid;
extern pid_t drid;

extern struct TSGtable *tsgtable;

extern int bufnum;
extern int bufsze;
extern void *bufadr[16];

extern int badrange;
extern struct timespec tock;

int SiteSetupHardware();
int SiteSetIntt(int sc,int us);
int SiteSetBeam(int bmnum);
int SiteFCLR(int sfrq,int efrq);
int SiteSetFreq(int tfreq);
int SiteIntegrate(int (*lags)[2]);
