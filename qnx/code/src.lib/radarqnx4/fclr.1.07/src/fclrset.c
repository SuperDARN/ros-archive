/* fclrset.c
   =========
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
#include <time.h>
#include "rtypes.h"
#include "tsg.h"
#include "maketsg.h"
#include "tsgtable.h"
#include "dio.h"
#include "freq.h"
#include "fclr.h"

/*
 $Log: fclrset.c,v $
 Revision 1.2  2004/05/04 18:50:46  barnes
 Renamed the type header to avoid confusion.

 Revision 1.1  2004/05/03 22:29:12  barnes
 Initial revision

*/

int FCLRSet(ipcid_t dio_id,struct TSGtable *ptr,int *tsg_id) {
  int status;
  struct TSGprm prm;
  struct TSGbuf *buf=NULL;
  int flag;  
  int id;
  prm.nrang=FCLR_NRANG_BASE;         
  prm.frang=0;         
  prm.rsep=3*MIN_SMSEP_STEP/20;          
  prm.smsep=MIN_SMSEP_STEP;
  prm.txpl=0; 
  prm.mpinc=MIN_SMSEP_STEP;
  prm.mppul=0; 
  prm.mlag=0;
  prm.nbaud=1;
  prm.code=NULL;
  prm.pat=NULL;
  id=TSGCheck(&prm,ptr);

  if ((id !=-1) && ((status=DIOVerifyID(dio_id,id)) !=0)) tsg_id[0]=id;  
  else {
    /* new timing sequence so download it to dio_driver */ 
    buf=TSGMake(&prm,&flag);
   
    if (buf==NULL) return -1;
  
   
    if (id==-1) id=TSGAdd(&prm,ptr);
    
    if (id !=-1) {
     
      status=DIOSetTSG(dio_id,id,buf->len,buf->code,buf->rep,NULL);
      
      if (status ==0) tsg_id[0]=id;
      else {
        TSGRemove(ptr,id);
        id=status;
      }
    }
    free(buf);
  }
  if (id==-1) return -1;
  prm.nrang=FCLR_NRANG_FINE;
  id=TSGCheck(&prm,ptr);
  
  if ((id !=-1) && ((status=DIOVerifyID(dio_id,id)) !=0)) tsg_id[1]=id;  
  else {
    /* new timing sequence so download it to dio_driver */ 
    buf=TSGMake(&prm,&flag);
  
    if (buf==NULL) return -1;
 
    if (id==-1) id=TSGAdd(&prm,ptr); 
    if (id !=-1) {
      
      status=DIOSetTSG(dio_id,id,buf->len,buf->code,buf->rep,NULL);
      
      if (status ==0) tsg_id[1]=id;
      else {
        TSGRemove(ptr,id);
        id=status;
      }
    }
    free(buf);
  }
  return id; 
}
