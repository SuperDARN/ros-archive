/* smr_control.c
   ============= */

#include <stdio.h>
#include <stdlib.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include "message.h"
#include "task_msg.h"

#define GET_SUMMARY 'g'
#define PUT_SUMMARY 'p'

int get_smr(struct task_id *ptr,int *blist,int *pwr) {

  char msg=GET_SUMMARY;
  int beamno;
  struct _mxfer_entry send_mx[1]; 
  struct _mxfer_entry reply_mx[3]; 

  _setmx(&send_mx[0],&msg,sizeof(char));
  _setmx(&reply_mx[0],pwr,sizeof(int));      
  _setmx(&reply_mx[1],&beamno,sizeof(int)); 
  _setmx(&reply_mx[2],blist,sizeof(int)*16); 
 
  if (ptr->id==-1) {
    if ( (ptr->id=qnx_name_locate(0,ptr->name,0,NULL)) == -1) return -1;
  }
  Sendmx(ptr->id,1,3,send_mx,reply_mx);
  return beamno;
}

int put_smr(struct task_id *ptr,int beamno,int *blist,int pwr) {

  char msg=PUT_SUMMARY;
  struct _mxfer_entry send_mx[4]; 
  struct _mxfer_entry reply_mx[1]; 

  _setmx(&send_mx[0],&msg,sizeof(char));
  _setmx(&send_mx[1],&pwr,sizeof(int));      
  _setmx(&send_mx[2],&beamno,sizeof(int)); 
  _setmx(&send_mx[3],blist,sizeof(int)*16); 
  _setmx(&reply_mx[0],&msg,sizeof(char));

  if (ptr->id==-1) {
    if ( (ptr->id=qnx_name_locate(0,ptr->name,0,NULL)) == -1) return -1;
  }
  Sendmx(ptr->id,4,1,send_mx,reply_mx);
  return (msg !=TASK_OK);
}

