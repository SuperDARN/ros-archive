/* do_pulse.c
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

/*
 $Log: do_pulse.c,v $
 Revision 1.5  2001/06/27 20:53:45  barnes
 Added license tag

 Revision 1.4  2001/01/29 18:11:50  barnes
 Added Author Name

 Revision 1.3  1998/11/08 22:14:05  barnes
 Changed calling sequence of send_tsg.

 Revision 1.2  1998/11/08 17:27:12  barnes
 Changed calling sequence of send_tsg to support stereo operations.

 Revision 1.1  1998/10/28 22:14:09  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "a_d_drive.h"
#include "dio.h"

int do_pulse(int debug,int buffer,
 	         int num_words,
             int num_channel,
		     int tsg_id,
	         pid_t dio_id,
             pid_t ad_id,
             pid_t reset_id) {

  unsigned char xflg=0,sflg=0x01;
  unsigned char seq;
  seq=tsg_id; 

  if (do_scan(ad_id,buffer,num_words,debug,num_channel) !=0) return -1;
  if (send_tsg(dio_id,0,&seq,&xflg,&sflg,NULL,NULL) != 0) { 
    scan_reset(ad_id,reset_id);
    return -1;
  }
  return 0;
}


