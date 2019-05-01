/* smrseek.c
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rtime.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"

#include "smrread.h"

/*
 $Log: smrseek.c,v $
 Revision 1.3  2004/05/05 15:52:13  barnes
 Added missing header.

 Revision 1.2  2004/05/04 18:35:57  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2003/09/13 22:39:29  barnes
 Initial revision

*/

int SmrSeek(FILE *fp,
	     int yr,int mo,int dy,int hr,int mt,int sc,double *atme) {
  struct RadarParm prm;
  struct FitData fit;
  int status=0,sjmp=0;
  int ctime=0;
  int tval;
  tval=TimeYMDHMSToYrsec(yr,mo,dy,hr,mt,sc);
  fseek(fp,0,SEEK_SET);
 
  do {
    sjmp=status;
    status=SmrFread(fp,&prm,&fit,0);
    if (status !=-1) ctime=TimeYMDHMSToYrsec(prm.time.yr,
				    prm.time.mo,
				    prm.time.dy,
				    prm.time.hr,
				    prm.time.mt,
				    prm.time.sc+prm.time.us/1.0e6);
  } while ((tval>=ctime) && (status !=-1));
  if (status==-1) return -1;
  fseek(fp,-(status+sjmp),SEEK_CUR);
  if (atme !=NULL) {
    status=SmrFread(fp,&prm,&fit,0);
    *atme=TimeYMDHMSToEpoch(prm.time.yr,
			    prm.time.mo,
			    prm.time.dy,
			    prm.time.hr,
			    prm.time.mt,
			    prm.time.sc+prm.time.us/1.0e6);
  } 
  fseek(fp,-status,SEEK_CUR);
  return 0;
}











