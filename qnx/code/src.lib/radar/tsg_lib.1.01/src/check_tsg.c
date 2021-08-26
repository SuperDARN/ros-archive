/* check_tsg.c
   ===========  */

#include <stdio.h>
#include <stdlib.h>

#include "tsg.h"

struct tsg_table *make_tsg_table(int max) {
  struct tsg_table *ptr=NULL;
  if (max==0) return 0;
  ptr=malloc(sizeof(struct tsg_table));
  if (ptr==NULL) return NULL;
  ptr->buf=malloc(sizeof(struct tsg_parms)*max);
  if (ptr->buf==NULL) {
    free(ptr);
    return NULL;
  }
  ptr->active=malloc(sizeof(int)*max);
  if (ptr->active==NULL) {
    free(ptr->buf);
    free(ptr);
    return NULL;
  }
  memset(ptr->active,0,sizeof(int)*max);

  ptr->num=0;
  ptr->max=max;
  return ptr;
}

void free_tsg_table(struct tsg_table *ptr) {
  if (ptr->buf !=NULL) free(ptr->buf);
  free(ptr);
} 

int comp_tsg(struct tsg_parms *p1, struct tsg_parms *p2) {
  int i;
  if (p1->nrang !=p2->nrang) return -1;
  if (p1->frang !=p2->frang) return -1;
  if (p1->rsep !=p2->rsep) return -1;
  if (p1->mppul != p2->mppul) return -1;
  if (p1->mpinc != p2->mpinc) return -1;
  if (p1->nbaud != p2->nbaud) return -1;
  for (i=0;(i<p1->mppul) && (p1->pat[i]==p2->pat[i]);i++);
  if (i<p1->mppul) return -1;
  return 0;  
}

int check_tsg(struct tsg_parms *tsg,struct tsg_table *tsgtab) {
  int i;
  if (tsgtab->num==0) return -1;
  for (i=tsgtab->num-1;i>=0;i--) {
    if ((tsgtab->active[i]==1) && (comp_tsg(tsg,&tsgtab->buf[i])==0)) break;
  }
  return i;
}

void remove_tsg(struct tsg_table *tsgtab,int id) {
  struct tsg_parms *ptr;
  if (tsgtab->num==0) return;
  if (id==tsgtab->num) tsgtab->num--;
  ptr=&tsgtab->buf[id];
  tsgtab->active[id]=0;
  if (ptr->code !=NULL) free(ptr->code);
  if (ptr->pat !=NULL) free(ptr->pat);
}

int add_tsg(struct tsg_parms *tsg,struct tsg_table *tsgtab) {
  struct tsg_parms *ptr;
  int i;
  if (tsgtab->num==tsgtab->max) return -1;
  for (i=0;(tsgtab->active[i]==1) && (i<tsgtab->num);i++); 
 
  tsgtab->active[i]=1;
  ptr=&tsgtab->buf[i];
  memcpy(ptr,tsg,sizeof(struct tsg_parms));
  if (ptr->mppul !=0) {
    ptr->pat=malloc(sizeof(int)*ptr->mppul);
    if (ptr->pat==NULL) return -1;
    memcpy(ptr->pat,tsg->pat,sizeof(int)*ptr->mppul);
  } 
  if (ptr->nbaud>1) {
    ptr->code=malloc(sizeof(int)*ptr->nbaud);
    if (ptr->code==NULL) {
      if (ptr->pat !=NULL) free(ptr->pat);
      return -1;
    }
    memcpy(ptr->code,tsg->code,sizeof(int)*ptr->nbaud);
  }
  if (i==tsgtab->num) tsgtab->num++; 
  return i; 
}
