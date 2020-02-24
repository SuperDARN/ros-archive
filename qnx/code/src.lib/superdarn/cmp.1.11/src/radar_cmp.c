/* radar_cmp.c
   ===========
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

/*
 $Log: radar_cmp.c,v $
 Revision 1.10  2002/02/02 21:42:38  barnes
 Fixed bug in passing the channel number into the scan structure.

 Revision 1.9  2002/02/02 21:23:12  barnes
 Fixed bug in setting the channel bits.

 Revision 1.8  2002/01/30 17:49:55  barnes
 Added channel to the cmp structure.

 Revision 1.7  2002/01/29 21:47:12  barnes
 Corrected bug in recording the program id.

 Revision 1.6  2002/01/29 21:42:58  barnes
 Added code to record the program ID in the data file.

 Revision 1.5  2001/06/27 20:46:56  barnes
 Added license tag

 Revision 1.4  2001/01/29 18:11:51  barnes
 Added Author Name

 Revision 1.3  1999/07/24 18:22:04  barnes
 Added version numbers to the file header.

 Revision 1.2  1999/03/18 15:20:28  barnes
 Modified the internal format of the cmp file to use epoch time.

 Revision 1.1  1999/03/16 19:26:46  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "cnv_time.h"
#include "rawdata.h"
#include "radar_scan.h"
#include "cmpdata.h"

void radar_cmphdr(double pmax,double vmax,double wmax,
				struct radardata *scan,struct cmphdr *ptr) {
  int yr,mo,dy,hr,mt,i;
  double sc;

  for (i=0;i<16;i++) if (scan->bmcnt[i] !=0) break;
  if (i==16) {
    ptr->frang=180;
    ptr->rsep=45;
    ptr->cp=0;
  } else {
    ptr->frang=scan->frang[i][0];
    ptr->rsep=scan->rsep[i][0];
    ptr->cp=scan->cpid[i][0];
    ptr->atten=scan->cpid[i][0];
  }
  ptr->pmax=pmax;
  ptr->vmax=vmax;
  ptr->wmax=wmax;
  ptr->st_id=scan->st_id;
  ptr->major_rev=MAJOR_REVISION;
  ptr->minor_rev=MINOR_REVISION;

}

int radar_cmp(int bmnum,int cnt,int store,double min_pwr,
	    double pmax,double vmax,double wmax,struct radardata *sptr,
	    struct cmpdata *ptr,int scan) {
  /* This is a high rate compression algorithm designed for graphics only */
  int loop,rng_num,i;
  int yr,mo,dy,hr,mt;
  double sc;


  memset(ptr->store,0,75*5);

  /* time stamp the record */

  ptr->time=sptr->time[bmnum][cnt];
 
  /* record  the beam number and a scan flag in the top bits of
     the dflag entry */

  ptr->noise=sptr->noise[bmnum][cnt];
  ptr->tfreq=sptr->freq[bmnum][cnt];
  ptr->channel=sptr->channel[bmnum][cnt];
  ptr->scan=0; 
  if (scan !=0) {
    ptr->scan=1;
    ptr->magic='z';
  } else ptr->magic='x';
  ptr->stored=store;
  ptr->bmnum=bmnum;
    
  /* work out which ranges to store */
  min_pwr=-10000;   
  rng_num=0;
  for (loop=0;loop<MAX_RANGE;loop++) {
    if (sptr->rng[bmnum][loop][cnt].gsct !=0) ptr->store[MAX_RANGE+loop]=1;
    if ((sptr->sct[bmnum][loop][cnt] !=0) && 
        (sptr->rng[bmnum][loop][cnt].p_l > min_pwr)) {
      ptr->store[loop]=1;
      rng_num++;     }    
  }
  if (rng_num>0) { 
    for (loop=0;loop<MAX_RANGE;loop++) {
      if (ptr->store[loop]  !=0) {
        int p_l_c=0,v_c=0,w_l_c=0;
        if ((store & 0x01) !=0) { 
          p_l_c=(sptr->rng[bmnum][loop][cnt].p_l/pmax)*256;
          p_l_c=(p_l_c >255) ? 255 : p_l_c;
          p_l_c=(p_l_c <0) ? 0 : p_l_c;
          ptr->store[2*MAX_RANGE+loop]=p_l_c;
        }
        if ((store & 0x02) !=0) {
          v_c=(1+(sptr->rng[bmnum][loop][cnt].v/vmax))*128;
          v_c=(v_c >255) ? 255 : v_c;
          v_c=(v_c < 0) ? 0 : v_c;
          ptr->store[3*MAX_RANGE+loop]=v_c;
        }
        if ((store & 0x04) !=0) {
          w_l_c=(sptr->rng[bmnum][loop][cnt].w_l/wmax)*256;
          w_l_c=(w_l_c >255) ? 255 : w_l_c;
          w_l_c=(w_l_c <0) ? 0 : w_l_c;
          ptr->store[4*MAX_RANGE+loop]=w_l_c; 
        }
      } 
    }   
  }
  return 0;
}
 


void add_cmp_scan(struct radardata *ptr,struct cmpdata *cmp,
                  struct cmphdr *hdr) {
  int bmnum,rng,cnt;

  bmnum=cmp->bmnum;
  cnt=ptr->bmcnt[bmnum];
  ptr->bmcnt[bmnum]++;
  ptr->st_id=hdr->st_id;
  if (cnt>0) {
    ptr->cpid[bmnum]=realloc(ptr->cpid[bmnum],(cnt+1)*sizeof(int));
    ptr->intt[bmnum]=realloc(ptr->intt[bmnum],(cnt+1)*sizeof(int));
    ptr->frang[bmnum]=realloc(ptr->frang[bmnum],(cnt+1)*sizeof(int));
    ptr->rsep[bmnum]=realloc(ptr->rsep[bmnum],(cnt+1)*sizeof(int));
    ptr->rxrise[bmnum]=realloc(ptr->rxrise[bmnum],(cnt+1)*sizeof(int));
    ptr->freq[bmnum]=realloc(ptr->freq[bmnum],(cnt+1)*sizeof(int));
    ptr->noise[bmnum]=realloc(ptr->noise[bmnum],(cnt+1)*sizeof(int));
    ptr->atten[bmnum]=realloc(ptr->atten[bmnum],(cnt+1)*sizeof(int));
    ptr->channel[bmnum]=realloc(ptr->channel[bmnum],(cnt+1)*sizeof(int));
    ptr->time[bmnum]=realloc(ptr->time[bmnum],(cnt+1)*sizeof(double));
  }
  
 

  ptr->cpid[bmnum][cnt]=cmp->cp;
  ptr->intt[bmnum][cnt]=7;
  ptr->frang[bmnum][cnt]=cmp->frang;
  ptr->rsep[bmnum][cnt]=cmp->rsep;
  ptr->rxrise[bmnum][cnt]=0;
  ptr->freq[bmnum][cnt]=cmp->tfreq;
  ptr->noise[bmnum][cnt]=cmp->noise;
  ptr->atten[bmnum][cnt]=cmp->atten;
  ptr->channel[bmnum][cnt]=cmp->channel;
  ptr->time[bmnum][cnt]=cmp->time;
  for (rng=0;rng<MAX_RANGE;rng++) {

    if (cnt>0) ptr->sct[bmnum][rng]=
      realloc(ptr->sct[bmnum][rng],(cnt+1)*sizeof(char));  
    ptr->sct[bmnum][rng][cnt]=cmp->store[rng];
    if (cnt>0)  ptr->rng[bmnum][rng]=
      realloc(ptr->rng[bmnum][rng],(cnt+1)*sizeof(struct datapoint));


    ptr->rng[bmnum][rng][cnt].gsct=cmp->store[rng+MAX_RANGE];
    ptr->rng[bmnum][rng][cnt].p_l=hdr->pmax*
                                    cmp->store[rng+2*MAX_RANGE]/256.0; 
    ptr->rng[bmnum][rng][cnt].w_l=hdr->wmax*
                                    cmp->store[rng+4*MAX_RANGE]/256.0; 
    ptr->rng[bmnum][rng][cnt].v=hdr->vmax*
                                    (cmp->store[rng+3*MAX_RANGE]-128.0)/128.0; 
    ptr->rng[bmnum][rng][cnt].v_e=50.0; 
  }
}








