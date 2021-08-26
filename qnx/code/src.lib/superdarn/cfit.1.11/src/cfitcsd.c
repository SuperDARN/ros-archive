/* cfitcsd.c
   ==========
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


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "rtypes.h"
#include "limit.h"
#include "cfitdata.h"
#include "cfitread.h"
#include "csddata.h"


/*
 $Log: cfitcsd.c,v $
 Revision 1.3  2004/05/04 18:18:23  barnes
  Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 21:37:01  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2004/03/11 22:10:36  barnes
 Initial revision

*/


int CFitToCSD(int store,double min_pwr,
	      double pmax,double vmax,double wmax,struct CFitdata *cfit,
	      struct CSDdata *ptr) {

  int loop,rng_num,rng;
 
  ptr->pmax=pmax;
  ptr->vmax=vmax;
  ptr->wmax=wmax;
  ptr->version.major=CSD_MAJOR_REVISION;
  ptr->version.minor=CSD_MINOR_REVISION;
  ptr->stored=store;

  memset(ptr->store,0,MAX_RANGE*5);

  /* time stamp the record */
  ptr->time=cfit->time;

  ptr->stid=cfit->stid;
  ptr->scan=cfit->scan;  
  ptr->cp=cfit->cp;
  ptr->bmnum=cfit->bmnum;
  ptr->channel=cfit->channel; 
  ptr->intt.sc=cfit->intt.sc;
  ptr->intt.us=cfit->intt.us;
  ptr->frang=cfit->frang;
  ptr->rsep=cfit->rsep;
  ptr->rxrise=cfit->rxrise;
  ptr->tfreq=cfit->tfreq;
  ptr->noise=cfit->noise;
  ptr->atten=cfit->atten;
  ptr->nave=cfit->nave;

  ptr->nrang=cfit->nrang;

  /* work out which ranges to store */
   
  rng_num=0;
  for (loop=0;loop<cfit->num;loop++) {
    rng=cfit->rng[loop];
    if (cfit->data[loop].gsct !=0) ptr->store[MAX_RANGE+rng]=1;
    if (cfit->data[loop].p_0 > min_pwr) {
      ptr->store[rng]=1;
      rng_num++;
    }   
  }
  if (rng_num>0) { 
    for (loop=0;loop<cfit->num;loop++) {
      rng=cfit->rng[loop];
      if (ptr->store[rng]  !=0) {
        int p_l_c=0,v_c=0,w_l_c=0;
        if ((store & 0x01) !=0) { 
          p_l_c=(cfit->data[loop].p_l/pmax)*256;
          p_l_c=(p_l_c >255) ? 255 : p_l_c;
          p_l_c=(p_l_c <0) ? 0 : p_l_c;
          ptr->store[2*MAX_RANGE+rng]=p_l_c;
        }
        if ((store & 0x02) !=0) {
          v_c=(1+(cfit->data[loop].v/vmax))*128;
          v_c=(v_c >255) ? 255 : v_c;
          v_c=(v_c < 0) ? 0 : v_c;
          ptr->store[3*MAX_RANGE+rng]=v_c;
        }
        if ((store & 0x04) !=0) {
          w_l_c=(cfit->data[loop].w_l/wmax)*256;
          w_l_c=(w_l_c >255) ? 255 : w_l_c;
          w_l_c=(w_l_c <0) ? 0 : w_l_c;
          ptr->store[4*MAX_RANGE+rng]=w_l_c; 
        }
      } 
    }
  }
  return 0;
}
 






