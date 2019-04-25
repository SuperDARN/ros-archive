/* tmseq.c
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
 "Radar Operating System - Site Files" (hereinafter "the Program").
 
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "rtypes.h"
#include "limit.h"
#include "rprm.h"
#include "rawdata.h"
#include "fitblk.h"
#include "fitdata.h"
#include "radar.h"
#include "taskid.h"
#include "rmsg.h"
#include "radarshell.h"
#include "dio.h"
#include "tsg.h"
#include "maketsg.h"
#include "tsgtable.h"
#include "global.h"
#include "hdw.h"

/*
 $Log: tmseq.c,v $
 Revision 1.5  2008/03/21 15:34:09  code
 Modifications to support I&Q sample capture.

 Revision 1.4  2007/03/01 15:07:19  code
 Added stdelay to deal with ramp up of digitial receiver.

 Revision 1.3  2005/07/27 18:55:38  barnes
 Changed delay after TX to 360.

 Revision 1.2  2004/12/20 17:54:13  barnes
 Fixed bug in setting parameter block.

 Revision 1.1  2004/05/11 15:31:56  barnes
 Initial revision

*/

int SiteTimeSeq(int *patn) {
  int status;
  struct TSGprm prm;
  struct TSGbuf *buf=NULL;
  
  int i,id;
  int flag;
 
  memset(&prm,0,sizeof(struct TSGprm));
  prm.nrang=nrang;         
  prm.frang=frang;         
  prm.rsep=rsep;          
  prm.smsep=smsep;
  prm.txpl=txpl; 
  prm.mpinc=mpinc;
  prm.mppul=mppul; 
  prm.mlag=0;
  prm.nbaud=1;
  prm.rtoxmin=360;
  prm.stdelay=90;
  prm.code=NULL;
  prm.pat=malloc(sizeof(int)*prm.mppul);
  for (i=0;i<prm.mppul;i++) prm.pat[i]=patn[i];
  id=TSGCheck(&prm,tsgtable);
 
  if ((id !=-1) && ((status=DIOVerifyID(dioid,id)) !=0)) {  
    free(prm.pat);
    lagfr=tsgtable->buf[id].lagfr;
    smsep=tsgtable->buf[id].smsep;
    txpl=tsgtable->buf[id].txpl;
    return id;
  }

  /* new timing sequence so download it to radops_dio */ 
  buf=TSGMake(&prm,&flag);
  
  if (buf==NULL) {
     free(prm.pat);
     return -1;
  }
  if (id==-1) id=TSGAdd(&prm,tsgtable); 
  if (id !=-1) {
     status=DIOSetTSG(dioid,id,buf->len,buf->code,buf->rep,
                     NULL);
    if (status==0) { 
      lagfr=prm.lagfr;
      smsep=prm.smsep;
      txpl=prm.txpl;
    } else {
     TSGRemove(tsgtable,id);
     id=status;
    }
  }
   free(prm.pat);
   TSGFree(buf);
   return id; 
}



 
