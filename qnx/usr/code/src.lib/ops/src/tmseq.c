/* tmseq.c
   ======= */

/*
 $Log: tmseq.c,v $
 Revision 1.2  2000/03/19 20:22:56  barnes
 Fixed bug that could mean the radar parameters would get screwed up.

 Revision 1.1  1999/03/31 19:59:42  barnes
 Initial revision

 Revision 1.1  1999/03/25 20:04:30  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "limit.h"
#include "message.h"
#include "dio.h"
#include "tsg.h"

#include "global.h"

int time_seq(pid_t dio_id,struct tsg_table *ptr,int *patn) {
  int status;
  struct tsg_parms prm;
  struct tsg_buf *buf=NULL;
  
  int i,id;
  int flag;

  prm.nrang=nrang;         
  prm.frang=frang;         
  prm.rsep=rsep;          
  prm.smsep=smsep;
  prm.txpl=txpl; 
  prm.mpinc=mpinc;
  prm.mppul=mppul; 
  prm.mlag=0;
  prm.nbaud=1;
  prm.code=NULL;
  prm.pat=malloc(sizeof(int)*prm.mppul);
  for (i=0;i<prm.mppul;i++) prm.pat[i]=patn[i];
  id=check_tsg(&prm,ptr);
 
  if ((id !=-1) && ((status=verify_id(dio_id,id)) !=0)) {  
    free(prm.pat);
    lagfr=ptr->buf[id].lagfr;
    smsep=ptr->buf[id].smsep;
    txpl=ptr->buf[id].txpl;
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
      lagfr=prm.lagfr;
      smsep=prm.smsep;
      txpl=prm.txpl;
    } else {
     remove_tsg(ptr,id);
     id=status;
    }
  }
   free(prm.pat);
   free_tsg(buf);
  return id; 
}

 
