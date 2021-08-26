/* tmseq_s.c
   ========= */

/*
 $Log: tmseq_s.c,v $
 Revision 1.1  2000/03/19 20:33:19  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "limit.h"
#include "message.h"
#include "dio.h"
#include "tsg.h"

#include "global_s.h"

int time_seq(int channel,pid_t dio_id,struct tsg_table *ptr,int *patn) {
  int status;
  struct tsg_parms prm;
  struct tsg_buf *buf=NULL;
  
  int i,id;
  int flag;

  if (channel==0) {
    prm.nrang=nrang_A;         
    prm.frang=frang_A;         
    prm.rsep=rsep_A;          
    prm.smsep=smsep_A;
    prm.txpl=txpl_A; 
    prm.mpinc=mpinc_A;
    prm.mppul=mppul_A; 
  } else {
    prm.nrang=nrang_B;         
    prm.frang=frang_B;         
    prm.rsep=rsep_B;          
    prm.smsep=smsep_B;
    prm.txpl=txpl_B; 
    prm.mpinc=mpinc_B;
    prm.mppul=mppul_B; 
  }
  prm.mlag=0;
  prm.nbaud=1;
  prm.code=NULL;
  prm.pat=malloc(sizeof(int)*prm.mppul);
  for (i=0;i<prm.mppul;i++) prm.pat[i]=patn[i];

  id=check_tsg(&prm,ptr);
  if ((id !=-1) && ((status=verify_id(dio_id,id)) !=0)) { 
     if (channel==0) {
        lagfr_A=ptr->buf[id].lagfr;
        smsep_A=ptr->buf[id].smsep;
        txpl_A=ptr->buf[id].txpl;
      } else {
        lagfr_B=ptr->buf[id].lagfr;
        smsep_B=ptr->buf[id].smsep;
        txpl_B=ptr->buf[id].txpl;
      }         
    free(prm.pat);
    return id;
  }
 

  /* new timing sequence so download it to radops_dio */ 
  buf=make_tsg(&prm,&flag);
  
  if (buf==NULL) {
     free(prm.pat);
     return -1;
  }

  if (id==-1) id=add_tsg(&prm,ptr); 
  if (id !=-1) {
    status=set_tsg(dio_id,id,buf->len,buf->code_byte,buf->rep_byte,NULL);
    if (status==0) { 
      if (channel==0) {
        lagfr_A=prm.lagfr;
        smsep_A=prm.smsep;
        txpl_A=prm.txpl;
      } else {
        lagfr_B=prm.lagfr;
        smsep_B=prm.smsep;
        txpl_B=prm.txpl;
      }
    } else {
     remove_tsg(ptr,id);
     id=status;
    }
  }


  free(prm.pat);
  free_tsg(buf);
 
  return id; 
}

 
