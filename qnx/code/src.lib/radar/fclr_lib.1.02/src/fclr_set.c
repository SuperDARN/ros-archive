/* fclr_set.c
   ========== */

/*
 $Log: fclr_set.c,v $
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
