/* fclr_set.c
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
 $Log: fclr_set.c,v $
 Revision 1.4  2001/06/27 20:53:22  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:11:49  barnes
 Added Author Name

 Revision 1.2  2000/03/19 20:04:43  barnes
 Fixed bugs in checking for downloaded sequences.

 Revision 1.1  1999/03/25 21:04:05  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cnv_time.h"
#include "types.h"
#include "limit.h"
#include "message.h"

#include "tsg.h"
#include "dio.h"
#include "do_fclr.h"

int fclr_set(pid_t dio_id,struct tsg_table *ptr,int *tsg_id) {
  int status;
  struct tsg_parms prm;
  struct tsg_buf *buf=NULL;
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
  id=check_tsg(&prm,ptr);

  if ((id !=-1) && ((status=verify_id(dio_id,id)) !=0)) tsg_id[0]=id;  
  else {
    /* new timing sequence so download it to dio_driver */ 
    buf=make_tsg(&prm,&flag);
   
    if (buf==NULL) return -1;
  
   
    if (id==-1) id=add_tsg(&prm,ptr);
    
    if (id !=-1) {
     
      status=set_tsg(dio_id,id,buf->len,buf->code_byte,buf->rep_byte,NULL);
      
      if (status ==0) tsg_id[0]=id;
      else {
        remove_tsg(ptr,id);
        id=status;
      }
    }
    free(buf);
  }
  if (id==-1) return -1;
  prm.nrang=FCLR_NRANG_FINE;
  id=check_tsg(&prm,ptr);
  
  if ((id !=-1) && ((status=verify_id(dio_id,id)) !=0)) tsg_id[1]=id;  
  else {
    /* new timing sequence so download it to dio_driver */ 
    buf=make_tsg(&prm,&flag);
  
    if (buf==NULL) return -1;
 
    if (id==-1) id=add_tsg(&prm,ptr); 
    if (id !=-1) {
      
      status=set_tsg(dio_id,id,buf->len,buf->code_byte,buf->rep_byte,NULL);
      
      if (status ==0) tsg_id[1]=id;
      else {
        remove_tsg(ptr,id);
        id=status;
      }
    }
    free(buf);
  }
  return id; 
}
