/* do_pulse_s.c 
   ============ */

/*
 $Log: do_pulse_s.c,v $
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


