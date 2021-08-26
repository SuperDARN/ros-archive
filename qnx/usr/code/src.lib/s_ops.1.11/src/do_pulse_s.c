/* do_pulse_s.c 
   ============ */

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
 $Log: do_pulse_s.c,v $
 Revision 1.2  2001/06/27 21:10:46  barnes
 Added license tag

 Revision 1.1  2000/03/19 20:33:19  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "a_d_drive.h"
#include "dio.h"

int do_pulse_s(int debug,int buffer,
 	       int num_words_A,int num_words_B,
               int num_channel_A,int num_channel_B,
	       int tsg_id_A,int tsg_id_B,
               int *delay,
	       pid_t dio_id,
               pid_t *ad_id,
               pid_t *reset_id) {

  unsigned char xflg[2]={0,0},sflg[2]={0x01,0x01};
  unsigned char seq[2];
  seq[0]=tsg_id_A; 
  seq[1]=tsg_id_B;

  if (do_scan(ad_id[0],buffer,num_words_A,debug,num_channel_A) !=0) return -1;
  if (do_scan(ad_id[1],buffer,num_words_B,debug,num_channel_B) !=0) return -1;

  if (send_tsg(dio_id,1,seq,xflg,sflg,delay,NULL) != 0) { 
    scan_reset(ad_id[0],reset_id[0]);
    scan_reset(ad_id[1],reset_id[1]);
    return -1;
  }
  return 0;
}


