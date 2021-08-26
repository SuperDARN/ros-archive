/* build_raw.c
   =========== */

/*
 $Log: build_raw.c,v $
 Revision 1.5  2000/10/30 15:18:22  barnes
 Fixed bug that meant the comment buffer was not being added to the data
 structure.

 Revision 1.4  1999/10/08 17:30:49  barnes
 Passed the mean noise as determined by the clear frequency search.

 Revision 1.3  1999/09/16 18:37:01  barnes
 Added user variables.

 Revision 1.2  1999/08/09 01:53:31  barnes
 Fixed problems with the agc and low power status flags.

 Revision 1.1  1999/03/31 19:57:19  barnes
 Initial revision

 Revision 1.2  1999/03/30 19:47:19  barnes
 Added the get_status function.

 Revision 1.1  1999/03/25 20:04:30  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cnv_time.h"
#include "types.h"
#include "limit.h"

#include "rawdata.h"
#include "global.h" 
#include "version.h"

void build_raw(struct rawdata *raw,int *ptab,int *lags) {
  int i,j;
  raw->PARMS.REV.MAJOR = VMAJOR;
  raw->PARMS.REV.MINOR = VMINOR;
  raw->PARMS.NPARM = sizeof(struct radar_parms)/2;
  raw->PARMS.ST_ID = st_id;
  raw->PARMS.YEAR=year;
  raw->PARMS.MONTH=month;
  raw->PARMS.DAY=day;
  raw->PARMS.HOUR=hour;
  raw->PARMS.MINUT=minut;
  raw->PARMS.SEC=sec;
  raw->PARMS.TXPOW = txpow;
  raw->PARMS.NAVE=nave;
  raw->PARMS.ATTEN = atten;
  raw->PARMS.LAGFR = lagfr;
  raw->PARMS.SMSEP = smsep;
  raw->PARMS.NBAUD = nbaud;
  raw->PARMS.NOISE=noise;
  raw->PARMS.NOISE_MEAN=noise_stat[0];
  raw->PARMS.RXRISE= rxrise;
  raw->PARMS.INTT  = intt;
  raw->PARMS.TXPL  = txpl;
  raw->PARMS.MPINC = mpinc;
  raw->PARMS.MPPUL = mppul;
  raw->PARMS.MPLGS = mplgs;
  raw->PARMS.NRANG = nrang;
  raw->PARMS.FRANG = frang;
  raw->PARMS.RSEP  = rsep;
  raw->PARMS.BMNUM = bmnum;
  raw->PARMS.XCF   = xcf;
  raw->PARMS.TFREQ = tfreq;
  raw->PARMS.SCAN  = scan;
  raw->PARMS.MXPWR = mxpwr;
  raw->PARMS.LVMAX = lvmax;
  raw->PARMS.usr_resL1=usr_resL1;
  raw->PARMS.usr_resL2=usr_resL2;
  raw->PARMS.usr_resS1=usr_resS1;
  raw->PARMS.usr_resS2=usr_resS2;
  raw->PARMS.usr_resS3=usr_resS3;

  raw->PARMS.CP	= cp;
  raw->PARMS.AGC_STAT=agc_stat;
  raw->PARMS.LOPWR_STAT=lopwr_stat;
  if (backward) raw->PARMS.usr_resS1=-1;
  else raw->PARMS.usr_resS1=1;
  if (discretion) raw->PARMS.CP=-raw->PARMS.CP;


  for (i=0;i<mppul;i++) raw->PULSE_PATTERN[i]=ptab[i];
  for (i=0;i<2;i++) {
    for (j=0;j<mplgs;j++) raw->LAG_TABLE[i][j]=lags[i*mplgs+j];
  } 

  strncpy(raw->COMBF,combf,COMBF_SIZE);

  for (i=0;i<nrang;i++) raw->pwr0[i]=pwr0[i];
  for (i=0;i<nrang;i++) {
    for (j=0;j<mplgs;j++) {
      raw->acfd[i][j][0]=acfd[(i*mplgs*2)+(2*j)];
      raw->acfd[i][j][1]=acfd[(i*mplgs*2)+(2*j)+1];
      raw->xcfd[i][j][0]=xcfd[(i*mplgs*2)+(2*j)];
      raw->xcfd[i][j][1]=xcfd[(i*mplgs*2)+(2*j)+1];
    }
  }
}
