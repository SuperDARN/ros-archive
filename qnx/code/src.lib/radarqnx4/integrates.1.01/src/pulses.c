/* pulses.c
   ========
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "rtypes.h"
#include "ad.h"
#include "dio.h"

/*
 $Log: pulses.c,v $
 Revision 1.1  2006/03/29 13:27:33  barnes
 Initial revision

*/

int IntegrateSPulse(int buffer,
    	           int sampleA,int sampleB,
                   int channelA,int channelB,
	               int tsgidA,int tsgidB,
                   int *delay,
   	               ipcid_t dioid,
                   ipcid_t *adid,
                   ipcid_t *resetid) {

  unsigned char xflg[2]={0,0},sflg[2]={0x01,0x01};
  unsigned char seq[2];
  seq[0]=tsgidA; 
  seq[1]=tsgidB;

  if (ADDoScan(adid[0],buffer,2*sampleA*channelA,0,2*channelA) !=0) return -1;
  if (ADDoScan(adid[1],buffer,2*sampleA*channelA,0,2*channelA) !=0) return -1;

  if (DIOSendTSG(dioid,1,seq,xflg,sflg,delay,NULL) != 0) {
      ADScanReset(adid[0],resetid[0]);
      ADScanReset(adid[1],resetid[1]);
    return -1;
  }
  return 0;
}


