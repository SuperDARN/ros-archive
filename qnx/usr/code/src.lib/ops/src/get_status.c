/* radar.c
   ======= */

/*
 $Log: get_status.c,v $
 Revision 1.2  1999/08/09 01:53:31  barnes
 Fixed problems with the agc and low power status flags.

 Revision 1.1  1999/03/31 19:57:19  barnes
 Initial revision

 Revision 1.1  1999/03/30 19:47:19  barnes
 Initial revision

 Revision 1.2  1999/03/29 14:53:10  barnes
 Modified the syntax for the user call.

 Revision 1.1  1999/03/25 20:04:30  barnes
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

#include "radar.h"
#include "global.h"

extern pid_t dio_id;

void get_status(int clear) {
  get_dio_status(dio_id,clear,0,&lopwr_stat,&agc_stat);
} 

 