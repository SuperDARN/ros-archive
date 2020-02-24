/* smr_control.c
   ============= */

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

