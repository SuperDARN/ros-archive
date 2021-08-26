/* mktsg.c
   ======= 
   Author: R.J.Barnes
*/


/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
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
 $Log: mktsg.c,v $
 Revision 1.3  2007/05/24 13:55:50  code
 Fixed missing parameters.

 Revision 1.2  2006/04/12 18:48:30  barnes
 Changed 'o' to 'z' for consistency.

 Revision 1.1  2006/04/12 18:29:17  barnes
 Initial revision

*/


#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "rtypes.h"

#include "tsg.h"
#include "maketsg.h"
#include "tsgtable.h"


int main(int argc,char *argv[]) {

  int i=0;
  int nrang=75;
  int frang=180;
  int rsep=45;
  int smsep=300;
  int txpl=rsep*20/3;
  int mpinc=2400;
  int mppul=8;
  
  int flag;

  int patn[8]={0,14,22,24,27,31,42,43};

  struct TSGprm prm;
  struct TSGbuf *buf=NULL;

  prm.nrang=nrang;         
  prm.frang=frang;
  prm.rtoxmin=0;      
  prm.rsep=rsep;          
  prm.smsep=smsep;
  prm.txpl=txpl; 
  prm.mpinc=mpinc;
  prm.mppul=mppul; 
  prm.mlag=0;
  prm.smdelay=0;
  prm.stdelay=0;
  prm.gort=1;
  prm.nbaud=1;
  prm.code=NULL;
  prm.pat=malloc(sizeof(int)*prm.mppul);
  for (i=0;i<prm.mppul;i++) prm.pat[i]=patn[i];

  buf=TSGMake(&prm,&flag);
  if (buf==NULL) {
    fprintf(stderr,"Error:%d\n",flag);
    exit(-1);
  }
  for (i=0;i<buf->len;i++) {

    if (buf->code[i] & 0x80) fprintf(stdout,"*");
    else fprintf(stdout," ");
    if (buf->code[i] & 0x01) fprintf(stdout,"s");
    if (buf->code[i] & 0x02) fprintf(stdout,"r");
    if (buf->code[i] & 0x04) fprintf(stdout,"x");
    if (buf->code[i] & 0x08) fprintf(stdout,"a");
    if (buf->code[i] & 0x10) fprintf(stdout,"p");
    if ((buf->code[i] & 0x7f)==0) fprintf(stdout,"z");
    fprintf(stdout," %d\n",buf->rep[i]);
  }

  return 0;
}
