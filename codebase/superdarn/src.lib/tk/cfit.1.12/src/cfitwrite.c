/* cfitwrite.c
   ==========
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <zlib.h>
#include "rtypes.h"
#include "rconvert.h"
#include "limit.h"
#include "cfitdata.h"
#include "cfitread.h"






int CFitWrite(int fp,struct CFitdata *ptr) {
  int i=0;
  if (ConvertWriteDouble(fp,ptr->time) !=0) return -1; 
  if (ConvertWriteShort(fp,ptr->stid) !=0) return -1; 
  if (ConvertWriteShort(fp,ptr->scan) !=0) return -1; 
  if (ConvertWriteShort(fp,ptr->cp) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->bmnum) !=0) return -1;
  if (ConvertWriteFloat(fp,ptr->bmazm) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->channel) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->intt.sc) !=0) return -1;
  if (ConvertWriteInt(fp,ptr->intt.us) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->frang) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->rsep) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->rxrise) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->tfreq) !=0) return -1;
  if (ConvertWriteInt(fp,ptr->noise) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->atten) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->nave) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->nrang) !=0) return -1;
  if (write(fp,&ptr->num,1) !=1) return -1;
  if (write(fp,ptr->rng,ptr->num) !=ptr->num) return -1;

  for (i=0;i<ptr->num;i++) {
    if (write(fp,&ptr->data[i].gsct,1) !=1) break;
    if (ConvertWriteFloat(fp,ptr->data[i].p_0) !=0) break;
    if (ConvertWriteFloat(fp,ptr->data[i].p_0_e) !=0) break;
    if (ConvertWriteFloat(fp,ptr->data[i].v) !=0) break;
    if (ConvertWriteFloat(fp,ptr->data[i].p_l) !=0) break;
    if (ConvertWriteFloat(fp,ptr->data[i].w_l) !=0) break;
    if (ConvertWriteFloat(fp,ptr->data[i].v_e) !=0) break;
    if (ConvertWriteFloat(fp,ptr->data[i].p_l_e) !=0) break;
    if (ConvertWriteFloat(fp,ptr->data[i].w_l_e) !=0) break;
     

  }
  if (i !=ptr->num) return -1;
  return 0;
}

int CFitFwrite(FILE *fp,struct CFitdata *ptr) {
  return CFitWrite(fileno(fp),ptr);
}
