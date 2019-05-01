/* cfitwrite.c
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


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "rtypes.h"
#include "rconvert.h"
#include "limit.h"
#include "cfitdata.h"
#include "cfitread.h"

/*
 $Log: cfitwrite.c,v $
 Revision 1.4  2004/06/22 17:36:33  barnes
 Changed references of Long to Int.

 Revision 1.3  2004/05/04 18:18:23  barnes
  Renamed the type header to avoid confusion.

 Revision 1.2  2003/09/13 22:36:31  barnes
 Modifcation to use the new data structures.

 Revision 1.1  2003/04/09 23:41:09  barnes
 Initial revision

*/




int CFitWrite(int fp,struct CFitdata *ptr) {
  int i=0;
  if (ConvertWriteDouble(fp,ptr->time) !=0) return -1; 
  if (ConvertWriteShort(fp,ptr->stid) !=0) return -1; 
  if (ConvertWriteShort(fp,ptr->scan) !=0) return -1; 
  if (ConvertWriteShort(fp,ptr->cp) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->bmnum) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->channel) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->intt.sc) !=0) return -1;
  if (ConvertWriteShort(fp,ptr->intt.us) !=0) return -1;
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
