/* radar_s.c
   ========= */

/*
 $Log: get_status_s.c,v $
 Revision 1.1  2000/03/19 20:33:19  barnes
 Initial revision

 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cnv_time.h"
#include "types.h"
#include "limit.h"
#include "message.h"
#include "a_d_drive.h"
#include "dio.h"
#include "tsg.h"
#include "do_radar.h"

#include "default.h"

#include "radar_s.h"
#include "global_s.h"

extern pid_t dio_id;

void get_status_s(int chn,int clear) {
  if (chn==0) {
    set_channel(dio_id_s,0); 
    get_dio_status(dio_id_s,clear,0,&lopwr_stat_A,&agc_stat_A);
  } else {
    set_channel(dio_id_s,1);
    get_dio_status(dio_id_s,clear,0,&lopwr_stat_B,&agc_stat_B);
  }
} 

 
