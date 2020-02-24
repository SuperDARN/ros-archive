/* smr_seek.c
   ==========
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
 $Log: smr_seek.c,v $
 Revision 1.3  2001/06/27 20:50:13  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:54  barnes
 Added Author Name

 Revision 1.1  1999/03/16 20:09:54  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include "cnv_time.h"
#include "rawdata.h"
#include "fitdata.h"

int smr_seek(FILE *fp,
	     int yr,int mo,int dy,int hr,int mt,int sc,double *atme) {
  struct fitdata *fit=NULL;
  int status=0,sjmp=0;
  int ctime=0;
  int tval;
  tval=time_yrsec(yr,mo,dy,hr,mt,sc);
  fit=malloc(sizeof(struct fitdata));
  if (fit==NULL) return -1;
  fseek(fp,0,SEEK_SET);
 
  do {
    sjmp=status;
    status=smr_fread(fp,fit,0);
    if (status !=-1) ctime=time_yrsec(fit->prms.YEAR,
				    fit->prms.MONTH,
				    fit->prms.DAY,
				    fit->prms.HOUR,
				    fit->prms.MINUT,
				    fit->prms.SEC);
  } while ((tval>=ctime) && (status !=-1));
  free(fit);
  if (status==-1) return -1;
  fseek(fp,-(status+sjmp),SEEK_CUR);
  if (atme !=NULL) {
    status=smr_fread(fp,fit,0);
    *atme=time_epoch(fit->prms.YEAR,
		     fit->prms.MONTH,
		     fit->prms.DAY,
		     fit->prms.HOUR,
		     fit->prms.MINUT,
		     fit->prms.SEC);
    fseek(fp,-status,SEEK_CUR);
  }
  return 0;
}











