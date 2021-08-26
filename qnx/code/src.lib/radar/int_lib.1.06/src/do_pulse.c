/* do_pulse.c 
   ========== */

/*
 $Log: do_pulse.c,v $
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


