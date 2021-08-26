/* decode_msg.c
   ============ */

/*
 $Log: decode_msg.c,v $
 Revision 1.2  1998/10/22 13:08:40  barnes
 Changed to integer types.

 Revision 1.1  1998/06/19 10:48:42  root
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>

#include "message.h"
#include "task_msg.h"


int time_cmp(void *a,void *b) {
  struct data_rec *x;
  struct data_rec *y;
  x=(struct data_rec *) a;
  y=(struct data_rec *) b;
  if (x->time<y->time) return -1;
  if (x->time==y->time) return 0;
  return 1;
}

int open_poll(struct task_decode *ptr,int i) {
  if (ptr->opentme==0) return 0;
  if (ptr->block.data[i].time<ptr->opentme) return 0;
  ptr->opentme=0;
  return 1;
} 

int close_poll(struct task_decode *ptr,int i) {
  if (ptr->closetme==0) return 0;
  if (ptr->block.data[i].time<ptr->closetme) return 0;
  ptr->closetme=0;
  return 1;
} 

char decode_data(pid_t tid,struct task_block *blk,
			     unsigned char **store) {
  int offset;
  int status;
  int i;
  char rmsg;
  unsigned char *dptr;

  offset=sizeof(char)+sizeof(int);
  blk->num=0;
  status=(Readmsg(tid,offset,&blk->num,
          sizeof(int)) !=sizeof(int));
  offset+=sizeof(int);
  if (status==0) status=(Readmsg(tid,offset,&blk->tsize,
  sizeof(int))!=sizeof(int));
  offset+=sizeof(int);
 
  if (status==0) status=(Readmsg(tid,offset,&blk->data,
  sizeof(struct data_rec)*blk->num) != 
        (sizeof(struct data_rec)*blk->num));
  offset+=sizeof(struct data_rec)*blk->num;
  if (*store !=NULL) {
    free(*store);
    *store=NULL;
  }  
  if ((status==0) && (blk->tsize !=0))
    status=((*store=malloc(blk->tsize))==NULL);
  if ((status==0) && (*store !=NULL))
    status=(Readmsg(tid,offset,*store,blk->tsize)
                    != blk->tsize);
  dptr=*store;
  if (status==0) for (i=0;i<blk->num;i++) {
    blk->ptr[i]=dptr;
    dptr+=blk->data[i].size;
  }
  if (status==0) rmsg=TASK_OK;   
  else rmsg=TASK_ERR;
  return rmsg;
}


char decode_msg(pid_t tid,char msg,struct task_decode *ptr) {
  int offset;
  int status;
  int i;
  char rmsg;
  unsigned char *dptr;

  switch (msg) {
    case TASK_DATA:
      rmsg=decode_data(tid,&ptr->block,&ptr->store);
      /* sort the data into ascending order */
      qsort(ptr->block.data,ptr->block.num,sizeof(struct data_rec),time_cmp);

      break;
    case TASK_OPEN:
      offset=sizeof(char)+sizeof(int);
      status=(Readmsg(tid,offset,&ptr->opentme,
             sizeof(double))!=sizeof(double));
      offset+=sizeof(double);
      if (status==0) status=(Readmsg(tid,offset,&ptr->open.size,
					 sizeof(int)) != sizeof(int));
      offset+=sizeof(int);
      if (ptr->open.ptr !=NULL) free(ptr->open.ptr);
      if ((status ==0) && (ptr->open.size !=0)) 
         ptr->open.ptr=malloc(ptr->open.size+1);
      if ((status ==0) && (ptr->open.ptr !=NULL)) 
        status=(Readmsg(tid,offset,ptr->open.ptr,ptr->open.size)
                       != ptr->open.size);
        ptr->open.ptr[ptr->open.size]=0;
      if (status !=0) {
        rmsg=TASK_ERR;
        ptr->opentme=0;
      }
      break;
    case TASK_CLOSE:
      offset=sizeof(char)+sizeof(int);
      status=(Readmsg(tid,offset,&ptr->closetme,
             sizeof(double))!=sizeof(double));
      if (status !=0) {
        rmsg=TASK_ERR;
        ptr->closetme=0;
      }
      break;
    case TASK_QUIT:
       rmsg=TASK_OK;
       break;
    case TASK_RESET:
	   rmsg=TASK_OK;
       break;
    default:
      rmsg=UNKNOWN_TYPE;
  }
  return rmsg;
}


 