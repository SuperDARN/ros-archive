/* build_raw_s.c
   ============= */

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
 $Log: build_raw_s.c,v $
 Revision 1.5  2002/02/07 16:58:29  barnes
 Added code to set the channel parameter in the parameter block.

 Revision 1.4  2001/06/27 21:10:46  barnes
 Added license tag

 Revision 1.3  2000/11/13 20:08:29  barnes
 Allowed for separate control program numbers for each channel.

 Revision 1.2  2000/09/18 16:10:28  barnes
 Updated code so that there are separate nave counters for each channel.

 Revision 1.1  2000/03/19 20:33:19  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cnv_time.h"
#include "types.h"
#include "limit.h"

#include "rawdata.h"
#include "global_s.h" 
#include "version.h"

void build_raw_s(int chn,struct rawdata *raw,int *ptab,int *lags) {
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
  raw->PARMS.INTT  = intt;
  

  if (chn==0) {
    raw->PARMS.CHANNEL=1;
    raw->PARMS.NAVE=nave_A;
    raw->PARMS.TXPOW = txpow_A;
    raw->PARMS.ATTEN = atten_A;
    raw->PARMS.LAGFR = lagfr_A;
    raw->PARMS.SMSEP = smsep_A;
    raw->PARMS.NBAUD = nbaud_A;
    raw->PARMS.NOISE=noise_A;
    raw->PARMS.NOISE_MEAN=noise_stat_A[0];
    raw->PARMS.RXRISE= rxrise_A;
    raw->PARMS.TXPL  = txpl_A;
    raw->PARMS.MPINC = mpinc_A;
    raw->PARMS.MPPUL = mppul_A;
    raw->PARMS.MPLGS = mplgs_A;
    raw->PARMS.NRANG = nrang_A;
    raw->PARMS.FRANG = frang_A;
    raw->PARMS.RSEP  = rsep_A;
    raw->PARMS.BMNUM = bmnum_A;
    raw->PARMS.XCF   = xcf_A;
    raw->PARMS.TFREQ = tfreq_A;
    raw->PARMS.SCAN  = scan_A;
    raw->PARMS.MXPWR = mxpwr_A;
    raw->PARMS.LVMAX = lvmax_A;
    raw->PARMS.usr_resL1=usr_resL1_A;
    raw->PARMS.usr_resL2=usr_resL2_A;
    raw->PARMS.usr_resS1=usr_resS1_A;
    raw->PARMS.usr_resS2=usr_resS2_A;
    raw->PARMS.usr_resS3=usr_resS3_A;
    raw->PARMS.CP=cp_A;
    raw->PARMS.AGC_STAT=agc_stat_A;
    raw->PARMS.LOPWR_STAT=lopwr_stat_A;
    if (backward) raw->PARMS.usr_resS1=-1;
    else raw->PARMS.usr_resS1=1;

 

    for (i=0;i<mppul_A;i++) raw->PULSE_PATTERN[i]=ptab[i];
    for (i=0;i<2;i++) {
      for (j=0;j<mplgs_A;j++) raw->LAG_TABLE[i][j]=lags[i*mplgs_A+j];
    } 

    for (i=0;i<nrang_A;i++) raw->pwr0[i]=pwr0_A[i];
    for (i=0;i<nrang_A;i++) {
      for (j=0;j<mplgs_A;j++) {
        raw->acfd[i][j][0]=acfd_A[(i*mplgs_A*2)+(2*j)];
        raw->acfd[i][j][1]=acfd_A[(i*mplgs_A*2)+(2*j)+1];
        raw->xcfd[i][j][0]=xcfd_A[(i*mplgs_A*2)+(2*j)];
        raw->xcfd[i][j][1]=xcfd_A[(i*mplgs_A*2)+(2*j)+1];
      }
    }
  } else {
    raw->PARMS.CHANNEL=2; 
    raw->PARMS.NAVE=nave_B;
    raw->PARMS.TXPOW = txpow_B;
    raw->PARMS.ATTEN = atten_B;
    raw->PARMS.LAGFR = lagfr_B;
    raw->PARMS.SMSEP = smsep_B;
    raw->PARMS.NBAUD = nbaud_B;
    raw->PARMS.NOISE=noise_B;
    raw->PARMS.NOISE_MEAN=noise_stat_B[0];
    raw->PARMS.RXRISE= rxrise_B;
    raw->PARMS.TXPL  = txpl_B;
    raw->PARMS.MPINC = mpinc_B;
    raw->PARMS.MPPUL = mppul_B;
    raw->PARMS.MPLGS = mplgs_B;
    raw->PARMS.NRANG = nrang_B;
    raw->PARMS.FRANG = frang_B;
    raw->PARMS.RSEP  = rsep_B;
    raw->PARMS.BMNUM = bmnum_B;
    raw->PARMS.XCF   = xcf_B;
    raw->PARMS.TFREQ = tfreq_B;
    raw->PARMS.SCAN  = scan_B;
    raw->PARMS.MXPWR = mxpwr_B;
    raw->PARMS.LVMAX = lvmax_B;
    raw->PARMS.usr_resL1=usr_resL1_B;
    raw->PARMS.usr_resL2=usr_resL2_B;
    raw->PARMS.usr_resS1=usr_resS1_B;
    raw->PARMS.usr_resS2=usr_resS2_B;
    raw->PARMS.usr_resS3=usr_resS3_B;
    raw->PARMS.CP= cp_B;
    raw->PARMS.AGC_STAT=agc_stat_B;
    raw->PARMS.LOPWR_STAT=lopwr_stat_B;
    if (backward) raw->PARMS.usr_resS1=-1;
    else raw->PARMS.usr_resS1=1;

 

    for (i=0;i<mppul_B;i++) raw->PULSE_PATTERN[i]=ptab[i];
    for (i=0;i<2;i++) {
      for (j=0;j<mplgs_B;j++) raw->LAG_TABLE[i][j]=lags[i*mplgs_B+j];
    } 

    for (i=0;i<nrang_B;i++) raw->pwr0[i]=pwr0_B[i];
    for (i=0;i<nrang_B;i++) {
      for (j=0;j<mplgs_B;j++) {
        raw->acfd[i][j][0]=acfd_B[(i*mplgs_B*2)+(2*j)];
        raw->acfd[i][j][1]=acfd_B[(i*mplgs_B*2)+(2*j)+1];
        raw->xcfd[i][j][0]=xcfd_B[(i*mplgs_B*2)+(2*j)];
        raw->xcfd[i][j][1]=xcfd_B[(i*mplgs_B*2)+(2*j)+1];
      }
    }
  }
  if (discretion) raw->PARMS.CP=-raw->PARMS.CP;




}
